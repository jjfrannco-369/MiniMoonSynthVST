#include "PluginProcessor.h"
#include "PluginEditor.h"

MiniJJMoogAudioProcessorEditor::MiniJJMoogAudioProcessorEditor (MiniJJMoogAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setSize (480, 280);
}

MiniJJMoogAudioProcessorEditor::~MiniJJMoogAudioProcessorEditor() = default;

void MiniJJMoogAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (0xff1b1a18));

    g.setColour (juce::Colour (0xffd6b36a));
    g.setFont (juce::Font (22.0f, juce::Font::bold));
    g.drawFittedText ("Mini JJ Moog VST3", getLocalBounds().removeFromTop (48),
                      juce::Justification::centred, 1);

    g.setColour (juce::Colours::lightgrey);
    g.setFont (juce::Font (14.0f));
    g.drawFittedText ("64-bit Windows VST3 Build", getLocalBounds(),
                      juce::Justification::centred, 1);
}

void MiniJJMoogAudioProcessorEditor::resized()
{
}
