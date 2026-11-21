#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include <cmath>

class MiniJJMoogAudioProcessor : public juce::AudioProcessor
{
public:
    MiniJJMoogAudioProcessor();
    ~MiniJJMoogAudioProcessor() override = default;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return JucePlugin_Name; }
    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.5; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return "Init Classic"; }
    void changeProgramName(int, const juce::String&) override {}

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState apvts;
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    float pitchBendNorm = 0.0f;
    float modWheelNorm  = 0.0f;
    int activeNoteNumber = -1;

private:
    double currentSampleRate = 44100.0;

    double phase1 = 0.0;
    double phase2 = 0.0;
    double phase3 = 0.0;
    double lfoPhase = 0.0;
    double currentFreq = 110.0;
    double targetFreq  = 110.0;

    juce::ADSR filterEnv;
    juce::ADSR ampEnv;
    juce::ADSR::Parameters filterParams { 0.01f, 0.30f, 0.30f, 0.30f };
    juce::ADSR::Parameters ampParams    { 0.01f, 0.35f, 0.80f, 0.35f };

    juce::dsp::LadderFilter<float> ladderFilter;
    juce::Random rng;

    float renderWave(double phase, int waveType) const noexcept;
    void handleMidiEvent(const juce::MidiMessage& msg);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MiniJJMoogAudioProcessor)
};
