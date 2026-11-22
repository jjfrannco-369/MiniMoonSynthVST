#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include <array>
#include <vector>

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
    static constexpr int maxPolyVoices = 16;

    struct SynthVoice
    {
        bool active = false;
        bool noteKeyHeld = false;
        int midiNote = 60;
        float velocity = 0.0f;
        double targetPitch = 60.0;
        double currentPitch = 60.0;
        float envLevel = 0.0f;
        uint64_t ageCounter = 0;
        std::array<double, 3> phases { 0.0, 0.0, 0.0 };
    };

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    static int voiceModeToCount (int modeIndex);
    static float rangeToOctaveMultiplier (int rangeIndex);
    static float semitonesToRatio (float semitones);
    static float renderWaveform (double normPhase, int waveIndex, float pulseWidth);
    static float secondsToCoeff (float seconds, double sampleRate);

    void triggerPolyNoteOn (int noteNumber, float vel, int activeVoiceCount, bool legatoMode);
    void triggerPolyNoteOff (int noteNumber);

    double currentSampleRate = 44100.0;
    uint64_t voiceAgeTick = 0;

    std::array<SynthVoice, maxPolyVoices> voices;
    std::array<double, 3> oscPhases { 0.0, 0.0, 0.0 };
    double lfoPhase = 0.0;

    double targetMidiPitch = 60.0;
    double currentMidiPitch = 60.0;
    float pitchBendNorm = 0.0f;
    float smoothedBendNorm = 0.0f;
    float modWheelAmount = 0.0f;
    bool sustainPedalDown = false;

    std::vector<int> heldNotes;
    float noteVelocity = 0.0f;
    int currentMidiNote = 60;
    bool gateHeld = false;

    float filterEnvValue = 0.0f;
    float loudnessEnvValue = 0.0f;

    float pinkB0 = 0.0f, pinkB1 = 0.0f, pinkB2 = 0.0f, pinkB3 = 0.0f, pinkB4 = 0.0f, pinkB5 = 0.0f, pinkB6 = 0.0f;
    juce::Random rng;

    juce::dsp::StateVariableTPTFilter<float> moogFilter;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MiniJJMoogAudioProcessor)
};
