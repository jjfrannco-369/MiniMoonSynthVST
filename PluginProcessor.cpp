#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <cmath>
#include <algorithm>

MiniJJMoogAudioProcessor::MiniJJMoogAudioProcessor()
    : AudioProcessor (BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
                      .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
#endif
                      .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
#endif
                      ),
      apvts (*this, nullptr, "Parameters", createParameterLayout())
{
}

juce::AudioProcessorValueTreeState::ParameterLayout MiniJJMoogAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    const juce::StringArray rangeChoices { "LO", "32'", "16'", "8'", "4'", "2'" };
    const juce::StringArray waveChoices  { "Tri", "Tri/Saw", "Saw", "Square", "Wide Pulse", "Narrow Pulse" };
    const juce::StringArray noiseChoices { "White", "Pink" };

    // CONTROLLERS
    layout.add (std::make_unique<juce::AudioParameterFloat> (
        "tune", "Tune", juce::NormalisableRange<float> (-12.0f, 12.0f, 0.05f), 0.0f));
    layout.add (std::make_unique<juce::AudioParameterFloat> (
        "glide", "Glide", juce::NormalisableRange<float> (0.0f, 2.0f, 0.005f), 0.04f));
    layout.add (std::make_unique<juce::AudioParameterFloat> (
        "mod_mix", "Mod Mix", juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.25f));
    layout.add (std::make_unique<juce::AudioParameterBool> (
        "osc_mod_on", "Osc Mod", false));
    layout.add (std::make_unique<juce::AudioParameterBool> (
        "osc3_ctrl_on", "Osc 3 Ctrl", false));

    // OSCILLATOR BANK (1, 2, 3)
    layout.add (std::make_unique<juce::AudioParameterChoice> (
        "osc1_range", "Osc 1 Range", rangeChoices, 2)); // 16'
    layout.add (std::make_unique<juce::AudioParameterFloat> (
        "osc1_freq", "Osc 1 Freq", juce::NormalisableRange<float> (-7.0f, 7.0f, 0.01f), 0.0f));
    layout.add (std::make_unique<juce::AudioParameterChoice> (
        "osc1_wave", "Osc 1 Wave", waveChoices, 2)); // Saw

    layout.add (std::make_unique<juce::AudioParameterChoice> (
        "osc2_range", "Osc 2 Range", rangeChoices, 2)); // 16'
    layout.add (std::make_unique<juce::AudioParameterFloat> (
        "osc2_freq", "Osc 2 Freq", juce::NormalisableRange<float> (-7.0f, 7.0f, 0.01f), -0.08f));
    layout.add (std::make_unique<juce::AudioParameterChoice> (
        "osc2_wave", "Osc 2 Wave", waveChoices, 2)); // Saw

    layout.add (std::make_unique<juce::AudioParameterChoice> (
        "osc3_range", "Osc 3 Range", rangeChoices, 1)); // 32'
    layout.add (std::make_unique<juce::AudioParameterFloat> (
        "osc3_freq", "Osc 3 Freq", juce::NormalisableRange<float> (-7.0f, 7.0f, 0.01f), 0.0f));
    layout.add (std::make_unique<juce::AudioParameterChoice> (
        "osc3_wave", "Osc 3 Wave", waveChoices, 3)); // Square

    // MIXER
    layout.add (std::make_unique<juce::AudioParameterFloat> (
        "osc1_vol", "Osc 1 Vol", juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.80f));
    layout.add (std::make_unique<juce::AudioParameterFloat> (
        "osc2_vol", "Osc 2 Vol", juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.65f));
    layout.add (std::make_unique<juce::AudioParameterFloat> (
        "osc3_vol", "Osc 3 Vol", juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.0f));
    layout.add (std::make_unique<juce::AudioParameterFloat> (
        "noise_vol", "Noise Vol", juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.0f));
    layout.add (std::make_unique<juce::AudioParameterChoice> (
        "noise_color", "Noise Type", noiseChoices, 0));
    layout.add (std::make_unique<juce::AudioParameterFloat> (
        "ext_vol", "Ext Input Vol", juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.0f));
    layout.add (std::make_unique<juce::AudioParameterFloat> (
        "overload", "Overload", juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.15f));

    // MODIFIERS (Filter + Loudness Contour)
    layout.add (std::make_unique<juce::AudioParameterBool> (
        "filter_mod_on", "Filter Mod", false));
    layout.add (std::make_unique<juce::AudioParameterBool> (
        "kb_ctrl_on", "Keyboard Control", true));
    layout.add (std::make_unique<juce::AudioParameterFloat> (
        "cutoff", "Cutoff", juce::NormalisableRange<float> (30.0f, 14000.0f, 1.0f, 0.32f), 2200.0f));
    layout.add (std::make_unique<juce::AudioParameterFloat> (
        "emphasis", "Emphasis", juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.25f));
    layout.add (std::make_unique<juce::AudioParameterFloat> (
        "filter_contour", "Filter Contour", juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.45f));

    layout.add (std::make_unique<juce::AudioParameterFloat> (
        "filter_attack", "Filter Attack", juce::NormalisableRange<float> (0.001f, 2.0f, 0.001f, 0.4f), 0.01f));
    layout.add (std::make_unique<juce::AudioParameterFloat> (
        "filter_decay", "Filter Decay", juce::NormalisableRange<float> (0.005f, 4.0f, 0.001f, 0.4f), 0.35f));
    layout.add (std::make_unique<juce::AudioParameterFloat> (
        "filter_sustain", "Filter Sustain", juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.30f));

    layout.add (std::make_unique<juce::AudioParameterFloat> (
        "loud_attack", "Loud Attack", juce::NormalisableRange<float> (0.001f, 2.0f, 0.001f, 0.4f), 0.008f));
    layout.add (std::make_unique<juce::AudioParameterFloat> (
        "loud_decay", "Loud Decay", juce::NormalisableRange<float> (0.005f, 4.0f, 0.001f, 0.4f), 0.40f));
    layout.add (std::make_unique<juce::AudioParameterFloat> (
        "loud_sustain", "Loud Sustain", juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.75f));

    // OUTPUT
    layout.add (std::make_unique<juce::AudioParameterFloat> (
        "master_vol", "Volume", juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.70f));
    layout.add (std::make_unique<juce::AudioParameterBool> (
        "main_out_on", "Main Output", true));

    return layout;
}

float MiniJJMoogAudioProcessor::rangeToOctaveMultiplier (int rangeIndex)
{
    switch (rangeIndex)
    {
        case 0: return 0.125f; // LO (sub-grave / LFO-like)
        case 1: return 0.25f;  // 32'
        case 2: return 0.5f;   // 16'
        case 3: return 1.0f;   // 8'
        case 4: return 2.0f;   // 4'
        case 5: return 4.0f;   // 2'
        default: return 0.5f;
    }
}

float MiniJJMoogAudioProcessor::semitonesToRatio (float semitones)
{
    return std::pow (2.0f, semitones / 12.0f);
}

float MiniJJMoogAudioProcessor::renderWaveform (double normPhase, int waveIndex, float pulseWidth)
{
    const float p = static_cast<float> (normPhase - std::floor (normPhase));

    const float tri = 1.0f - 4.0f * std::abs (p - 0.5f);
    const float saw = 2.0f * p - 1.0f;
    const float sqr = p < 0.5f ? 1.0f : -1.0f;
    const float widePulse = p < 0.72f ? 1.0f : -1.0f;
    const float narrowPulse = p < pulseWidth ? 1.0f : -1.0f;

    switch (waveIndex)
    {
        case 0: return tri;
        case 1: return 0.5f * (tri + saw);
        case 2: return saw;
        case 3: return sqr;
        case 4: return widePulse;
        case 5: return narrowPulse;
        default: return saw;
    }
}

float MiniJJMoogAudioProcessor::secondsToCoeff (float seconds, double sampleRate)
{
    const double clamped = std::max (0.0005, static_cast<double> (seconds));
    return static_cast<float> (1.0 - std::exp (-1.0 / (clamped * sampleRate)));
}

const juce::String MiniJJMoogAudioProcessor::getName() const
{
    return "Mini JJ Moog";
}

bool MiniJJMoogAudioProcessor::acceptsMidi() const { return true; }
bool MiniJJMoogAudioProcessor::producesMidi() const { return false; }
bool MiniJJMoogAudioProcessor::isMidiEffect() const { return false; }
double MiniJJMoogAudioProcessor::getTailLengthSeconds() const { return 0.0; }

int MiniJJMoogAudioProcessor::getNumPrograms() { return 1; }
int MiniJJMoogAudioProcessor::getCurrentProgram() { return 0; }
void MiniJJMoogAudioProcessor::setCurrentProgram (int) {}
const juce::String MiniJJMoogAudioProcessor::getProgramName (int) { return {}; }
void MiniJJMoogAudioProcessor::changeProgramName (int, const juce::String&) {}

void MiniJJMoogAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate > 0.0 ? sampleRate : 44100.0;

    juce::dsp::ProcessSpec spec;
    spec.sampleRate = currentSampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32> (samplesPerBlock > 0 ? samplesPerBlock : 512);
    spec.numChannels = static_cast<juce::uint32> (juce::jmax (1, getTotalNumOutputChannels()));

    moogFilter.prepare (spec);
    moogFilter.setType (juce::dsp::StateVariableTPTFilterType::lowpass);
    moogFilter.reset();

    oscPhases.fill (0.0);
    lfoPhase = 0.0;
    filterEnvValue = 0.0f;
    loudnessEnvValue = 0.0f;
}

void MiniJJMoogAudioProcessor::releaseResources()
{
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool MiniJJMoogAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

#if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif

    return true;
}
#endif

void MiniJJMoogAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;

    const int numSamples = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();

    for (int ch = 0; ch < numChannels; ++ch)
        buffer.clear (ch, 0, numSamples);

    if (numChannels <= 0 || numSamples <= 0)
        return;

    for (const auto metadata : midiMessages)
    {
        const auto msg = metadata.getMessage();
        if (msg.isNoteOn())
        {
            currentMidiNote = msg.getNoteNumber();
            noteVelocity = std::max (0.1f, msg.getFloatVelocity());
            noteFreqHz = 440.0 * std::pow (2.0, (currentMidiNote - 69) / 12.0);
            gateHeld = true;
            filterEnvValue = std::max (filterEnvValue, 0.25f);
            loudnessEnvValue = std::max (loudnessEnvValue, 0.25f);
        }
        else if (msg.isNoteOff())
        {
            if (msg.getNoteNumber() == currentMidiNote)
                gateHeld = false;
        }
        else if (msg.isAllNotesOff() || msg.isAllSoundOff())
        {
            gateHeld = false;
            noteVelocity = 0.0f;
        }
    }

    const float tuneSemis      = apvts.getRawParameterValue ("tune")->load();
    const float glideSec       = apvts.getRawParameterValue ("glide")->load();
    const float modMix         = apvts.getRawParameterValue ("mod_mix")->load();
    const bool  oscModOn       = apvts.getRawParameterValue ("osc_mod_on")->load() > 0.5f;
    const bool  osc3CtrlOn     = apvts.getRawParameterValue ("osc3_ctrl_on")->load() > 0.5f;

    const int   r1             = static_cast<int> (apvts.getRawParameterValue ("osc1_range")->load());
    const float f1             = apvts.getRawParameterValue ("osc1_freq")->load();
    const int   w1             = static_cast<int> (apvts.getRawParameterValue ("osc1_wave")->load());

    const int   r2             = static_cast<int> (apvts.getRawParameterValue ("osc2_range")->load());
    const float f2             = apvts.getRawParameterValue ("osc2_freq")->load();
    const int   w2             = static_cast<int> (apvts.getRawParameterValue ("osc2_wave")->load());

    const int   r3             = static_cast<int> (apvts.getRawParameterValue ("osc3_range")->load());
    const float f3             = apvts.getRawParameterValue ("osc3_freq")->load();
    const int   w3             = static_cast<int> (apvts.getRawParameterValue ("osc3_wave")->load());

    const float v1             = apvts.getRawParameterValue ("osc1_vol")->load();
    const float v2             = apvts.getRawParameterValue ("osc2_vol")->load();
    const float v3             = apvts.getRawParameterValue ("osc3_vol")->load();
    const float noiseVol       = apvts.getRawParameterValue ("noise_vol")->load();
    const int   noiseType      = static_cast<int> (apvts.getRawParameterValue ("noise_color")->load());
    const float extVol         = apvts.getRawParameterValue ("ext_vol")->load();
    const float overloadAmt    = apvts.getRawParameterValue ("overload")->load();

    const bool  filterModOn    = apvts.getRawParameterValue ("filter_mod_on")->load() > 0.5f;
    const bool  kbCtrlOn       = apvts.getRawParameterValue ("kb_ctrl_on")->load() > 0.5f;
    const float baseCutoff     = apvts.getRawParameterValue ("cutoff")->load();
    const float emphasis       = apvts.getRawParameterValue ("emphasis")->load();
    const float filterContour  = apvts.getRawParameterValue ("filter_contour")->load();

    const float fAtk           = apvts.getRawParameterValue ("filter_attack")->load();
    const float fDec           = apvts.getRawParameterValue ("filter_decay")->load();
    const float fSus           = apvts.getRawParameterValue ("filter_sustain")->load();

    const float lAtk           = apvts.getRawParameterValue ("loud_attack")->load();
    const float lDec           = apvts.getRawParameterValue ("loud_decay")->load();
    const float lSus           = apvts.getRawParameterValue ("loud_sustain")->load();

    const float masterVol      = apvts.getRawParameterValue ("master_vol")->load();
    const bool  mainOutOn      = apvts.getRawParameterValue ("main_out_on")->load() > 0.5f;

    const float fAtkCoeff = secondsToCoeff (fAtk, currentSampleRate);
    const float fDecCoeff = secondsToCoeff (fDec, currentSampleRate);
    const float lAtkCoeff = secondsToCoeff (lAtk, currentSampleRate);
    const float lDecCoeff = secondsToCoeff (lDec, currentSampleRate);
    const float glideCoeff = glideSec > 0.001f ? secondsToCoeff (glideSec, currentSampleRate) : 1.0f;

    const float qVal = 0.707f + emphasis * 6.5f;
    moogFilter.setResonance (qVal);

    const float driveGain = 1.0f + overloadAmt * 4.5f;

    for (int sample = 0; sample < numSamples; ++sample)
    {
        // Envelopes (Filter & Loudness Contour)
        if (gateHeld)
        {
            filterEnvValue += fAtkCoeff * (1.0f - filterEnvValue);
            loudnessEnvValue += lAtkCoeff * (1.0f - loudnessEnvValue);
        }
        else
        {
            filterEnvValue += fDecCoeff * (fSus * 0.0f - filterEnvValue);
            loudnessEnvValue += lDecCoeff * (lSus * 0.0f - loudnessEnvValue);
        }

        const float envModLevel = gateHeld
            ? (filterEnvValue * (1.0f - fDecCoeff * 0.35f) + fSus * (fDecCoeff * 0.35f))
            : filterEnvValue;

        const float ampLevel = gateHeld
            ? (loudnessEnvValue * (1.0f - lDecCoeff * 0.25f) + lSus * (lDecCoeff * 0.25f))
            : loudnessEnvValue;

        // Oscilador 3 (pode atuar também como modulador LFO)
        const double freq3 = noteFreqHz * rangeToOctaveMultiplier (r3) * semitonesToRatio (f3 + tuneSemis);
        const float out3 = renderWaveform (oscPhases[2], w3, 0.22f);
        oscPhases[2] += freq3 / currentSampleRate;
        if (oscPhases[2] >= 1.0) oscPhases[2] -= 1.0;

        // Oscilador 1 & 2
        const float modPitchSemis = oscModOn ? (out3 * modMix * 3.0f) : 0.0f;
        const double freq1 = noteFreqHz * rangeToOctaveMultiplier (r1) * semitonesToRatio (f1 + tuneSemis + modPitchSemis);
        const double freq2 = noteFreqHz * rangeToOctaveMultiplier (r2) * semitonesToRatio (f2 + tuneSemis + modPitchSemis);

        const float out1 = renderWaveform (oscPhases[0], w1, 0.22f);
        const float out2 = renderWaveform (oscPhases[1], w2, 0.22f);

        oscPhases[0] += freq1 / currentSampleRate;
        if (oscPhases[0] >= 1.0) oscPhases[0] -= 1.0;

        oscPhases[1] += freq2 / currentSampleRate;
        if (oscPhases[1] >= 1.0) oscPhases[1] -= 1.0;

        // Noise (White / Pink)
        const float white = rng.nextFloat() * 2.0f - 1.0f;
        pinkB0 = 0.99886f * pinkB0 + white * 0.0555179f;
        pinkB1 = 0.99332f * pinkB1 + white * 0.0750759f;
        pinkB2 = 0.96900f * pinkB2 + white * 0.1538520f;
        pinkB3 = 0.86650f * pinkB3 + white * 0.3104856f;
        pinkB4 = 0.55000f * pinkB4 + white * 0.5329522f;
        pinkB5 = -0.7616f * pinkB5 - white * 0.0168980f;
        const float pink = (pinkB0 + pinkB1 + pinkB2 + pinkB3 + pinkB4 + pinkB5 + pinkB6 + white * 0.5362f) * 0.08f;
        pinkB6 = white * 0.115926f;
        const float noiseSample = (noiseType == 1) ? pink : white;

        const float osc3AudioMix = osc3CtrlOn ? 0.0f : v3;
        float mixed = (out1 * v1 + out2 * v2 + out3AudioMix * v3 + noiseSample * noiseVol) * 0.45f;

        // Overload / saturação analógica antes do filtro
        mixed = std::tanh (mixed * driveGain);

        // Cutoff dinâmico com Filter Contour, Keyboard Control e Filter Mod
        const float kbOffset = kbCtrlOn ? ((static_cast<float> (currentMidiNote) - 60.0f) * 28.0f) : 0.0f;
        const float contourHz = envModLevel * filterContour * 6500.0f;
        const float lfoCutoffHz = filterModOn ? (out3 * modMix * 2400.0f) : 0.0f;
        const float finalCutoff = juce::jlimit (35.0f, 16000.0f, baseCutoff + kbOffset + contourHz + lfoCutoffHz);

        moogFilter.setCutoffFrequency (finalCutoff);

        const float shaped = mixed * ampLevel * noteVelocity;
        const float outputGain = mainOutOn ? masterVol : 0.0f;

        for (int ch = 0; ch < numChannels; ++ch)
        {
            const float filtered = moogFilter.processSample (static_cast<size_t> (ch), shaped);
            buffer.setSample (ch, sample, filtered * outputGain);
        }

        (void) glideCoeff;
    }
}

bool MiniJJMoogAudioProcessor::hasEditor() const { return true; }

juce::AudioProcessorEditor* MiniJJMoogAudioProcessor::createEditor()
{
    return new MiniJJMoogAudioProcessorEditor (*this);
}

void MiniJJMoogAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    if (auto state = apvts.copyState(); state.isValid())
    {
        std::unique_ptr<juce::XmlElement> xml (state.createXml());
        if (xml != nullptr)
            copyXmlToBinary (*xml, destData);
    }
}

void MiniJJMoogAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    if (xmlState != nullptr && xmlState->hasTagName (apvts.state.getType()))
        apvts.replaceState (juce::ValueTree::fromXml (*xmlState));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MiniJJMoogAudioProcessor();
}
