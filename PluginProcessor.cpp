#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <cmath>

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

    layout.add (std::make_unique<juce::AudioParameterFloat> (
        "cutoff", "Cutoff",
        juce::NormalisableRange<float> (40.0f, 12000.0f, 1.0f, 0.35f),
        1800.0f));

    layout.add (std::make_unique<juce::AudioParameterFloat> (
        "gain", "Gain",
        juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f),
        0.5f));

    return layout;
}

const juce::String MiniJJMoogAudioProcessor::getName() const
{
    return "Mini JJ Moog";
}

bool MiniJJMoogAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return true;
   #endif
}

bool MiniJJMoogAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool MiniJJMoogAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double MiniJJMoogAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int MiniJJMoogAudioProcessor::getNumPrograms()
{
    return 1;
}

int MiniJJMoogAudioProcessor::getCurrentProgram()
{
    return 0;
}

void MiniJJMoogAudioProcessor::setCurrentProgram (int)
{
}

const juce::String MiniJJMoogAudioProcessor::getProgramName (int)
{
    return {};
}

void MiniJJMoogAudioProcessor::changeProgramName (int, const juce::String&)
{
}

void MiniJJMoogAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate > 0.0 ? sampleRate : 44100.0;

    juce::dsp::ProcessSpec spec;
    spec.sampleRate = currentSampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32> (samplesPerBlock > 0 ? samplesPerBlock : 512);
    spec.numChannels = static_cast<juce::uint32> (juce::jmax (1, getTotalNumOutputChannels()));

    lowpassFilter.prepare (spec);
    lowpassFilter.setType (juce::dsp::StateVariableTPTFilterType::lowpass);
    lowpassFilter.reset();
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

    for (const auto metadata : midiMessages)
    {
        const auto msg = metadata.getMessage();

        if (msg.isNoteOn())
        {
            currentMidiNote = msg.getNoteNumber();
            currentVelocity = msg.getFloatVelocity();
            currentFreqHz = 440.0 * std::pow (2.0, (currentMidiNote - 69) / 12.0);
        }
        else if (msg.isNoteOff())
        {
            if (msg.getNoteNumber() == currentMidiNote)
                currentVelocity = 0.0f;
        }
        else if (msg.isAllNotesOff() || msg.isAllSoundOff())
        {
            currentVelocity = 0.0f;
        }
    }

    const auto cutoffHz = apvts.getRawParameterValue ("cutoff")->load();
    const auto gainVal  = apvts.getRawParameterValue ("gain")->load();
    lowpassFilter.setCutoffFrequency (cutoffHz);

    const int numSamples = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();

    for (int ch = 0; ch < numChannels; ++ch)
        buffer.clear (ch, 0, numSamples);

    if (numChannels <= 0 || numSamples <= 0)
        return;

    const double phaseDelta = (currentFreqHz / currentSampleRate) * 2.0 * juce::MathConstants<double>::pi;

    for (int sample = 0; sample < numSamples; ++sample)
    {
        float outSample = 0.0f;

        if (currentVelocity > 0.0001f)
        {
            // Onda dente de serra simples como base analógica inicial
            const float saw = static_cast<float> (1.0 - (phase / juce::MathConstants<double>::pi));
            outSample = saw * currentVelocity * gainVal * 0.25f;

            phase += phaseDelta;
            if (phase >= juce::MathConstants<double>::twoPi)
                phase -= juce::MathConstants<double>::twoPi;
        }

        for (int ch = 0; ch < numChannels; ++ch)
            buffer.setSample (ch, sample, outSample);
    }

    juce::dsp::AudioBlock<float> block (buffer);
    juce::dsp::ProcessContextReplacing<float> context (block);
    lowpassFilter.process (context);
}

bool MiniJJMoogAudioProcessor::hasEditor() const
{
    return true;
}

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
