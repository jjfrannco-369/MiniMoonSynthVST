#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class MiniJJMoogAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit MiniJJMoogAudioProcessorEditor(MiniJJMoogAudioProcessor&);
    ~MiniJJMoogAudioProcessorEditor() override = default;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    MiniJJMoogAudioProcessor& audioProcessor;

    struct VintageKnobLookAndFeel : public juce::LookAndFeel_V4
    {
        void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                              float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                              juce::Slider& slider) override;
    };

    VintageKnobLookAndFeel vintageKnobLAF;

    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;
    using ComboBoxAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;

    void configureKnob(juce::Slider& slider, const juce::String& labelText);
    void configureToggle(juce::ToggleButton& button, const juce::String& buttonText);
    void configureCombo(juce::ComboBox& combo, const juce::StringArray& items);
    void drawSectionBox(juce::Graphics& g, juce::Rectangle<int> bounds, const juce::String& title);

    // Controllers
    juce::Slider tuneSlider, glideSlider, modMixSlider;
    juce::ToggleButton osc3CtrlButton;

    // Oscillator Bank
    juce::ComboBox osc1RangeCombo, osc1WaveCombo;
    juce::Slider osc1FreqSlider;

    juce::ComboBox osc2RangeCombo, osc2WaveCombo;
    juce::Slider osc2FreqSlider;

    juce::ComboBox osc3RangeCombo, osc3WaveCombo;
    juce::Slider osc3FreqSlider;

    // Mixer
    juce::Slider vol1Slider, vol2Slider, vol3Slider, noiseVolSlider;
    juce::ToggleButton on1Button, on2Button, on3Button, noiseOnButton, pinkNoiseButton;

    // Modifiers (Filter + Envelopes)
    juce::Slider cutoffSlider, emphasisSlider, contourAmtSlider;
    juce::Slider filtAtkSlider, filtDecSlider, filtSusSlider;
    juce::Slider ampAtkSlider, ampDecSlider, ampSusSlider;

    // Output
    juce::Slider masterVolSlider;

    // Attachments
    std::unique_ptr<SliderAttachment> tuneAtt, glideAtt, modMixAtt;
    std::unique_ptr<ButtonAttachment> osc3CtrlAtt;

    std::unique_ptr<ComboBoxAttachment> osc1RangeAtt, osc1WaveAtt;
    std::unique_ptr<SliderAttachment> osc1FreqAtt;

    std::unique_ptr<ComboBoxAttachment> osc2RangeAtt, osc2WaveAtt;
    std::unique_ptr<SliderAttachment> osc2FreqAtt;

    std::unique_ptr<ComboBoxAttachment> osc3RangeAtt, osc3WaveAtt;
    std::unique_ptr<SliderAttachment> osc3FreqAtt;

    std::unique_ptr<SliderAttachment> vol1Att, vol2Att, vol3Att, noiseVolAtt;
    std::unique_ptr<ButtonAttachment> on1Att, on2Att, on3Att, noiseOnAtt, pinkNoiseAtt;

    std::unique_ptr<SliderAttachment> cutoffAtt, emphasisAtt, contourAmtAtt;
    std::unique_ptr<SliderAttachment> filtAtkAtt, filtDecAtt, filtSusAtt;
    std::unique_ptr<SliderAttachment> ampAtkAtt, ampDecAtt, ampSusAtt;

    std::unique_ptr<SliderAttachment> masterVolAtt;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MiniJJMoogAudioProcessorEditor)
};
