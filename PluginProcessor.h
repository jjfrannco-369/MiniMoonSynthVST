#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

class MiniJJMoogAudioProcessor : public juce::AudioProcessor
{
public:
    MiniJJMoogAudioProcessor();
    ~MiniJJMoogAudioProcessor() override = default;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState apvts;

private:
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    double currentSampleRate = 44100.0;
    juce::dsp::StateVariableTPTFilter<float> lowpassFilter;
    double phase = 0.0;
    double currentFreqHz = 110.0;
    float currentVelocity = 0.0f;
    int currentMidiNote = -1;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MiniJJMoogAudioProcessor)
};
