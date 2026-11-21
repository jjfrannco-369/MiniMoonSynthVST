#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <cmath>

MiniJJMoogAudioProcessor::MiniJJMoogAudioProcessor()
    : AudioProcessor(BusesProperties().withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "PARAMS", createParameterLayout())
{
}

juce::AudioProcessorValueTreeState::ParameterLayout MiniJJMoogAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<juce::AudioParameterFloat>("tune", "Tune", -12.0f, 12.0f, 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("glide", "Glide", 0.001f, 1.5f, 0.04f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("modMix", "Modulation Mix", 0.0f, 1.0f, 0.25f));
    params.push_back(std::make_unique<juce::AudioParameterBool>("osc3Ctrl", "Osc 3 Control", true));

    for (int i = 1; i <= 3; ++i)
    {
        auto id = juce::String(i);
        params.push_back(std::make_unique<juce::AudioParameterChoice>("osc" + id + "Range", "Osc " + id + " Range",
            juce::StringArray { "LO", "32'", "16'", "8'", "4'", "2'" }, 3));
        params.push_back(std::make_unique<juce::AudioParameterFloat>("osc" + id + "Freq", "Osc " + id + " Freq", -7.0f, 7.0f, 0.0f));
        params.push_back(std::make_unique<juce::AudioParameterChoice>("osc" + id + "Wave", "Osc " + id + " Wave",
            juce::StringArray { "Tri", "TriSaw", "Saw", "Square", "WidePulse", "NarrowPulse" }, 2));
    }

    params.push_back(std::make_unique<juce::AudioParameterFloat>("vol1", "Osc 1 Volume", 0.0f, 1.0f, 0.80f));
    params.push_back(std::make_unique<juce::AudioParameterBool>("on1", "Osc 1 On", true));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("vol2", "Osc 2 Volume", 0.0f, 1.0f, 0.55f));
    params.push_back(std::make_unique<juce::AudioParameterBool>("on2", "Osc 2 On", true));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("vol3", "Osc 3 Volume", 0.0f, 1.0f, 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterBool>("on3", "Osc 3 On", false));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("noiseVol", "Noise Volume", 0.0f, 1.0f, 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterBool>("noiseOn", "Noise On", false));
    params.push_back(std::make_unique<juce::AudioParameterBool>("pinkNoise", "Pink Noise", false));

    params.push_back(std::make_unique<juce::AudioParameterFloat>("cutoff", "Cutoff Frequency", 30.0f, 16000.0f, 1400.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("emphasis", "Filter Emphasis", 0.0f, 0.92f, 0.42f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("contourAmt", "Amount of Contour", 0.0f, 9000.0f, 3200.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>("filtAtk", "Filter Attack", 0.002f, 3.0f, 0.01f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("filtDec", "Filter Decay", 0.01f, 3.0f, 0.30f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("filtSus", "Filter Sustain", 0.0f, 1.0f, 0.30f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>("ampAtk", "Loudness Attack", 0.002f, 3.0f, 0.01f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("ampDec", "Loudness Decay", 0.01f, 3.0f, 0.35f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("ampSus", "Loudness Sustain", 0.0f, 1.0f, 0.80f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>("masterVol", "Master Volume", 0.0f, 1.0f, 0.70f));

    return { params.begin(), params.end() };
}

void MiniJJMoogAudioProcessor::prepareToPlay(double sampleRate, int /*samplesPerBlock*/)
{
    currentSampleRate = sampleRate > 0.0 ? sampleRate : 44100.0;
    filterEnv.setSampleRate(currentSampleRate);
    ampEnv.setSampleRate(currentSampleRate);
    filterStage1 = filterStage2 = filterStage3 = filterStage4 = 0.0f;
}

bool MiniJJMoogAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    return layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}

float MiniJJMoogAudioProcessor::renderWave(double phase, int waveType) const noexcept
{
    double p = phase - std::floor(phase);
    switch (waveType)
    {
        case 0: return static_cast<float>(4.0 * std::abs(p - 0.5) - 1.0);
        case 1:
        {
            float tri = static_cast<float>(4.0 * std::abs(p - 0.5) - 1.0);
            float saw = static_cast<float>(2.0 * p - 1.0);
            return 0.5f * (tri + saw);
        }
        case 2: return static_cast<float>(2.0 * p - 1.0);
        case 3: return p < 0.5 ? 1.0f : -1.0f;
        case 4: return p < 0.75 ? 1.0f : -1.0f;
        case 5:
        default: return p < 0.18 ? 1.0f : -1.0f;
    }
}

void MiniJJMoogAudioProcessor::handleMidiEvent(const juce::MidiMessage& msg)
{
    if (msg.isNoteOn() && msg.getVelocity() > 0)
    {
        activeNoteNumber = msg.getNoteNumber();
        targetFreq = juce::MidiMessage::getMidiNoteInHertz(activeNoteNumber);
        filterEnv.noteOn();
        ampEnv.noteOn();
    }
    else if (msg.isNoteOff() || (msg.isNoteOn() && msg.getVelocity() == 0))
    {
        if (msg.getNoteNumber() == activeNoteNumber)
        {
            activeNoteNumber = -1;
            filterEnv.noteOff();
            ampEnv.noteOff();
        }
    }
    else if (msg.isPitchWheel())
    {
        pitchBendNorm = static_cast<float>((msg.getPitchWheelValue() - 8192) / 8192.0);
    }
    else if (msg.isController() && msg.getControllerNumber() == 1)
    {
        modWheelNorm = static_cast<float>(msg.getControllerValue() / 127.0);
    }
}

void MiniJJMoogAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    const int numSamples = buffer.getNumSamples();

    filterParams.attack  = apvts.getRawParameterValue("filtAtk")->load();
    filterParams.decay   = apvts.getRawParameterValue("filtDec")->load();
    filterParams.sustain = apvts.getRawParameterValue("filtSus")->load();
    filterParams.release = filterParams.decay;
    filterEnv.setParameters(filterParams);

    ampParams.attack  = apvts.getRawParameterValue("ampAtk")->load();
    ampParams.decay   = apvts.getRawParameterValue("ampDec")->load();
    ampParams.sustain = apvts.getRawParameterValue("ampSus")->load();
    ampParams.release = ampParams.decay;
    ampEnv.setParameters(ampParams);

    const float tuneSemis  = apvts.getRawParameterValue("tune")->load();
    const float glideSec   = std::max(0.001f, apvts.getRawParameterValue("glide")->load());
    const float modMix     = apvts.getRawParameterValue("modMix")->load();
    const bool osc3Ctrl    = apvts.getRawParameterValue("osc3Ctrl")->load() > 0.5f;

    const int rangeIdx[3] = {
        static_cast<int>(apvts.getRawParameterValue("osc1Range")->load()),
        static_cast<int>(apvts.getRawParameterValue("osc2Range")->load()),
        static_cast<int>(apvts.getRawParameterValue("osc3Range")->load())
    };
    const float detuneSemi[3] = {
        apvts.getRawParameterValue("osc1Freq")->load(),
        apvts.getRawParameterValue("osc2Freq")->load(),
        apvts.getRawParameterValue("osc3Freq")->load()
    };
    const int waveIdx[3] = {
        static_cast<int>(apvts.getRawParameterValue("osc1Wave")->load()),
        static_cast<int>(apvts.getRawParameterValue("osc2Wave")->load()),
        static_cast<int>(apvts.getRawParameterValue("osc3Wave")->load())
    };

    const float vols[3] = {
        (apvts.getRawParameterValue("on1")->load() > 0.5f) ? apvts.getRawParameterValue("vol1")->load() : 0.0f,
        (apvts.getRawParameterValue("on2")->load() > 0.5f) ? apvts.getRawParameterValue("vol2")->load() : 0.0f,
        (apvts.getRawParameterValue("on3")->load() > 0.5f) ? apvts.getRawParameterValue("vol3")->load() : 0.0f
    };

    const float noiseVol = (apvts.getRawParameterValue("noiseOn")->load() > 0.5f)
                         ? apvts.getRawParameterValue("noiseVol")->load() : 0.0f;
    const bool pinkNoise = apvts.getRawParameterValue("pinkNoise")->load() > 0.5f;

    const float cutoffBase = apvts.getRawParameterValue("cutoff")->load();
    const float emphasis   = apvts.getRawParameterValue("emphasis")->load();
    const float contourAmt = apvts.getRawParameterValue("contourAmt")->load();
    const float masterVol  = apvts.getRawParameterValue("masterVol")->load();

    auto rangeToOctaveShift = [](int idx) -> float {
        switch (idx) {
            case 0: return -3.0f;
            case 1: return -2.0f;
            case 2: return -1.0f;
            case 3: return  0.0f;
            case 4: return  1.0f;
            case 5: return  2.0f;
            default: return 0.0f;
        }
    };

    const double glideCoeff = 1.0 - std::exp(-1.0 / (glideSec * currentSampleRate));
    float pinkState = 0.0f;

    auto midiIt = midiMessages.cbegin();
    auto midiEnd = midiMessages.cend();

    buffer.clear();
    auto* leftOut  = buffer.getWritePointer(0);
    auto* rightOut = buffer.getNumChannels() > 1 ? buffer.getWritePointer(1) : nullptr;

    for (int sample = 0; sample < numSamples; ++sample)
    {
        while (midiIt != midiEnd && (*midiIt).samplePosition <= sample)
        {
            handleMidiEvent((*midiIt).getMessage());
            ++midiIt;
        }

        currentFreq += (targetFreq - currentFreq) * glideCoeff;

        const double lfoFreq = osc3Ctrl ? (currentFreq * std::pow(2.0, (rangeToOctaveShift(rangeIdx[2]) + detuneSemi[2]) / 1.0))
                                      : (1.5 + std::pow(2.0, rangeToOctaveShift(2) + detuneSemi[2] * 0.2));
        lfoPhase += lfoFreq / currentSampleRate;
        if (lfoPhase >= 1.0) lfoPhase -= 1.0;
        const float lfoVal = static_cast<float>(std::sin(2.0 * juce::MathConstants<double>::pi * lfoPhase));

        const float whiteVal = rng.nextFloat() * 2.0f - 1.0f;
        pinkState = 0.90f * pinkState + 0.10f * whiteVal;
        const float noiseVal = pinkNoise ? pinkState * 2.5f : whiteVal;

        const float modSource = (1.0f - modMix) * lfoVal + modMix * noiseVal;
        const float pitchModSemis = modWheelNorm * modSource * 2.0f + (pitchBendNorm * 2.0f);
        const double basePitchFreq = currentFreq * std::pow(2.0, (tuneSemis + pitchModSemis) / 12.0);

        const double f1 = basePitchFreq * std::pow(2.0, rangeToOctaveShift(rangeIdx[0]) + detuneSemi[0] / 12.0);
        const double f2 = basePitchFreq * std::pow(2.0, rangeToOctaveShift(rangeIdx[1]) + detuneSemi[1] / 12.0);
        const double f3 = osc3Ctrl
            ? basePitchFreq * std::pow(2.0, rangeToOctaveShift(rangeIdx[2]) + detuneSemi[2] / 12.0)
            : lfoFreq;

        phase1 += f1 / currentSampleRate; if (phase1 >= 1.0) phase1 -= 1.0;
        phase2 += f2 / currentSampleRate; if (phase2 >= 1.0) phase2 -= 1.0;
        phase3 += f3 / currentSampleRate; if (phase3 >= 1.0) phase3 -= 1.0;

        const float s1 = renderWave(phase1, waveIdx[0]) * vols[0];
        const float s2 = renderWave(phase2, waveIdx[1]) * vols[1];
        const float s3 = renderWave(phase3, waveIdx[2]) * vols[2];
        const float ns = noiseVal * noiseVol;

        const float mixSat = std::tanh((s1 + s2 + s3 + ns) * 1.35f);

        const float fEnv = filterEnv.getNextSample();
        const float aEnv = ampEnv.getNextSample();

        const float cutoffHz = juce::jlimit(30.0f, 18000.0f,
            cutoffBase + (fEnv * contourAmt) + (modWheelNorm * modSource * 1800.0f));

        const float fc = juce::jlimit(0.001f, 0.45f, static_cast<float>(cutoffHz / currentSampleRate));
        const float g = 1.0f - std::exp(-2.0f * juce::MathConstants<float>::pi * fc);
        const float k = emphasis * 3.85f;
        const float filterInput = std::tanh(mixSat * 1.25f - k * filterStage4);

        filterStage1 += g * (std::tanh(filterInput) - std::tanh(filterStage1));
        filterStage2 += g * (std::tanh(filterStage1) - std::tanh(filterStage2));
        filterStage3 += g * (std::tanh(filterStage2) - std::tanh(filterStage3));
        filterStage4 += g * (std::tanh(filterStage3) - std::tanh(filterStage4));

        const float outSample = filterStage4 * aEnv * masterVol;
        leftOut[sample] = outSample;
        if (rightOut != nullptr)
            rightOut[sample] = outSample;
    }
}

juce::AudioProcessorEditor* MiniJJMoogAudioProcessor::createEditor()
{
    return new MiniJJMoogAudioProcessorEditor(*this);
}

void MiniJJMoogAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void MiniJJMoogAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState != nullptr && xmlState->hasTagName(apvts.state.getType()))
        apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MiniJJMoogAudioProcessor();
}
