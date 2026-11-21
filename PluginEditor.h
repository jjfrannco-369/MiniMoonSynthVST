#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"

class MoogKnobLookAndFeel : public juce::LookAndFeel_V4
{
public:
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                          float sliderPosProportional, float rotaryStartAngle,
                          float rotaryEndAngle, juce::Slider& slider) override;
};

class MiniJJMoogAudioProcessorEditor : public juce::AudioProcessorEditor,
                                      public juce::Timer
{
public:
    explicit MiniJJMoogAudioProcessorEditor(MiniJJMoogAudioProcessor&);
    ~MiniJJMoogAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;

private:
    MiniJJMoogAudioProcessor& audioProcessor;
    MoogKnobLookAndFeel moogLookAndFeel;

    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;
    using ComboAttachment  = juce::AudioProcessorValueTreeState::ComboBoxAttachment;

    void setupKnob(juce::Slider& s, const juce::String& label);
    void setupSwitch(juce::ToggleButton& b, const juce::String& text, juce::Colour accent);

    juce::Slider tuneKnob, glideKnob, modMixKnob;
    juce::ToggleButton osc3CtrlSwitch;

    juce::ComboBox osc1Range, osc1Wave, osc2Range, osc2Wave, osc3Range, osc3Wave;
    juce::Slider osc1Freq, osc2Freq, osc3Freq;

    juce::Slider vol1Knob, vol2Knob, vol3Knob, noiseVolKnob;
    juce::ToggleButton on1Switch, on2Switch, on3Switch, noiseOnSwitch, pinkNoiseSwitch;

    juce::Slider cutoffKnob, emphasisKnob, contourKnob;
    juce::Slider filtAtkKnob, filtDecKnob, filtSusKnob;
    juce::Slider ampAtkKnob, ampDecKnob, ampSusKnob;
    juce::Slider masterVolKnob;

    std::vector<std::unique_ptr<SliderAttachment>> sliderAttachments;
    std::vector<std::unique_ptr<ButtonAttachment>> buttonAttachments;
    std::vector<std::unique_ptr<ComboAttachment>>  comboAttachments;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MiniJJMoogAudioProcessorEditor)
};
