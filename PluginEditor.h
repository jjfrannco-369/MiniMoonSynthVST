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
    MiniJJMoogAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MiniJJMoogAudioProcessorEditor)
};
