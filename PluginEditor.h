#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"

class MiniJJMoogAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit MiniJJMoogAudioProcessorEditor (MiniJJMoogAudioProcessor&);
    ~MiniJJMoogAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ComboAttachment  = juce::AudioProcessorValueTreeState::ComboBoxAttachment;
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;

    void setupKnob (juce::Slider& slider, juce::Label& label, const juce::String& text);
    void setupCombo (juce::ComboBox& box, juce::Label& label, const juce::String& text, const juce::StringArray& items);
    void setupToggle (juce::ToggleButton& btn, const juce::String& text);

    MiniJJMoogAudioProcessor& audioProcessor;

    // Controllers
    juce::Slider tuneSlider, glideSlider, modMixSlider;
    juce::Label tuneLabel, glideLabel, modMixLabel;
    juce::ToggleButton oscModBtn, osc3CtrlBtn;

    // Oscillator Bank
    juce::ComboBox osc1RangeBox, osc1WaveBox, osc2RangeBox, osc2WaveBox, osc3RangeBox, osc3WaveBox;
    juce::Label osc1RangeLabel, osc1WaveLabel, osc2RangeLabel, osc2WaveLabel, osc3RangeLabel, osc3WaveLabel;
    juce::Slider osc1FreqSlider, osc2FreqSlider, osc3FreqSlider;
    juce::Label osc1FreqLabel, osc2FreqLabel, osc3FreqLabel;

    // Mixer
    juce::Slider osc1VolSlider, osc2VolSlider, osc3VolSlider, noiseVolSlider, extVolSlider, overloadSlider;
    juce::Label osc1VolLabel, osc2VolLabel, osc3VolLabel, noiseVolLabel, extVolLabel, overloadLabel;
    juce::ComboBox noiseColorBox;
    juce::Label noiseColorLabel;

    // Modifiers
    juce::ToggleButton filterModBtn, kbCtrlBtn;
    juce::Slider cutoffSlider, emphasisSlider, contourSlider;
    juce::Label cutoffLabel, emphasisLabel, contourLabel;
    juce::Slider fAtkSlider, fDecSlider, fSusSlider;
    juce::Label fAtkLabel, fDecLabel, fSusLabel;
    juce::Slider lAtkSlider, lDecSlider, lSusSlider;
    juce::Label lAtkLabel, lDecLabel, lSusLabel;

    // Output
    juce::Slider masterVolSlider;
    juce::Label masterVolLabel;
    juce::ToggleButton mainOutBtn;

    // Attachments
    std::unique_ptr<SliderAttachment> tuneAtt, glideAtt, modMixAtt;
    std::unique_ptr<ButtonAttachment> oscModAtt, osc3CtrlAtt;

    std::unique_ptr<ComboAttachment> osc1RangeAtt, osc1WaveAtt, osc2RangeAtt, osc2WaveAtt, osc3RangeAtt, osc3WaveAtt;
    std::unique_ptr<SliderAttachment> osc1FreqAtt, osc2FreqAtt, osc3FreqAtt;

    std::unique_ptr<SliderAttachment> osc1VolAtt, osc2VolAtt, osc3VolAtt, noiseVolAtt, extVolAtt, overloadAtt;
    std::unique_ptr<ComboAttachment> noiseColorAtt;

    std::unique_ptr<ButtonAttachment> filterModAtt, kbCtrlAtt;
    std::unique_ptr<SliderAttachment> cutoffAtt, emphasisAtt, contourAtt;
    std::unique_ptr<SliderAttachment> fAtkAtt, fDecAtt, fSusAtt;
    std::unique_ptr<SliderAttachment> lAtkAtt, lDecAtt, lSusAtt;

    std::unique_ptr<SliderAttachment> masterVolAtt;
    std::unique_ptr<ButtonAttachment> mainOutAtt;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MiniJJMoogAudioProcessorEditor)
};
