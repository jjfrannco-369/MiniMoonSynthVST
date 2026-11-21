#include "PluginEditor.h"

void MoogKnobLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                                           float sliderPosProportional, float rotaryStartAngle,
                                           float rotaryEndAngle, juce::Slider&)
{
    auto radius = static_cast<float>(juce::jmin(width, height)) * 0.42f;
    auto centreX = static_cast<float>(x) + static_cast<float>(width) * 0.5f;
    auto centreY = static_cast<float>(y) + static_cast<float>(height) * 0.5f;
    auto angle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);

    g.setColour(juce::Colour(0xff141416));
    g.fillEllipse(centreX - radius, centreY - radius, radius * 2.0f, radius * 2.0f);

    auto capRadius = radius * 0.68f;
    g.setColour(juce::Colour(0xffc9ced4));
    g.fillEllipse(centreX - capRadius, centreY - capRadius, capRadius * 2.0f, capRadius * 2.0f);

    juce::Path pointer;
    auto pointerLength = radius * 0.88f;
    auto pointerThickness = 3.0f;
    pointer.addRectangle(-pointerThickness * 0.5f, -pointerLength, pointerThickness, pointerLength);
    pointer.applyTransform(juce::AffineTransform::rotation(angle).translated(centreX, centreY));

    g.setColour(juce::Colours::black);
    g.fillPath(pointer);
}

MiniJJMoogAudioProcessorEditor::MiniJJMoogAudioProcessorEditor(MiniJJMoogAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    setLookAndFeel(&moogLookAndFeel);
    setSize(980, 560);

    auto attachSlider = [&](juce::Slider& s, const juce::String& paramId, const juce::String& title) {
        setupKnob(s, title);
        addAndMakeVisible(s);
        sliderAttachments.push_back(std::make_unique<SliderAttachment>(audioProcessor.apvts, paramId, s));
    };

    auto attachToggle = [&](juce::ToggleButton& b, const juce::String& paramId, const juce::String& text, juce::Colour accent) {
        setupSwitch(b, text, accent);
        addAndMakeVisible(b);
        buttonAttachments.push_back(std::make_unique<ButtonAttachment>(audioProcessor.apvts, paramId, b));
    };

    auto attachCombo = [&](juce::ComboBox& c, const juce::String& paramId, const juce::StringArray& items) {
        c.addItemList(items, 1);
        addAndMakeVisible(c);
        comboAttachments.push_back(std::make_unique<ComboAttachment>(audioProcessor.apvts, paramId, c));
    };

    attachSlider(tuneKnob,   "tune",   "TUNE");
    attachSlider(glideKnob,  "glide",  "GLIDE");
    attachSlider(modMixKnob, "modMix", "MOD MIX");
    attachToggle(osc3CtrlSwitch, "osc3Ctrl", "OSC 3 CTRL", juce::Colour(0xffe06d1b));

    juce::StringArray ranges { "LO", "32'", "16'", "8'", "4'", "2'" };
    juce::StringArray waves  { "Tri", "TriSaw", "Saw", "Sqr", "Wide", "Narrow" };

    attachCombo(osc1Range, "osc1Range", ranges);
    attachCombo(osc1Wave,  "osc1Wave",  waves);
    attachSlider(osc1Freq, "osc1Freq",  "OSC 1 FREQ");

    attachCombo(osc2Range, "osc2Range", ranges);
    attachCombo(osc2Wave,  "osc2Wave",  waves);
    attachSlider(osc2Freq, "osc2Freq",  "OSC 2 FREQ");

    attachCombo(osc3Range, "osc3Range", ranges);
    attachCombo(osc3Wave,  "osc3Wave",  waves);
    attachSlider(osc3Freq, "osc3Freq",  "OSC 3 FREQ");

    attachSlider(vol1Knob,     "vol1",     "VOL 1");
    attachToggle(on1Switch,    "on1",      "ON 1", juce::Colour(0xff2c82c9));
    attachSlider(vol2Knob,     "vol2",     "VOL 2");
    attachToggle(on2Switch,    "on2",      "ON 2", juce::Colour(0xff2c82c9));
    attachSlider(vol3Knob,     "vol3",     "VOL 3");
    attachToggle(on3Switch,    "on3",      "ON 3", juce::Colour(0xff2c82c9));
    attachSlider(noiseVolKnob, "noiseVol", "NOISE");
    attachToggle(noiseOnSwitch,"noiseOn",  "N ON", juce::Colour(0xff2c82c9));
    attachToggle(pinkNoiseSwitch, "pinkNoise", "PINK", juce::Colour(0xffe06d1b));

    attachSlider(cutoffKnob,  "cutoff",     "CUTOFF");
    attachSlider(emphasisKnob,"emphasis",   "EMPHASIS");
    attachSlider(contourKnob, "contourAmt", "CONTOUR");
    attachSlider(filtAtkKnob, "filtAtk",    "F ATK");
    attachSlider(filtDecKnob, "filtDec",    "F DEC");
    attachSlider(filtSusKnob, "filtSus",    "F SUS");
    attachSlider(ampAtkKnob,  "ampAtk",     "A ATK");
    attachSlider(ampDecKnob,  "ampDec",     "A DEC");
    attachSlider(ampSusKnob,  "ampSus",     "A SUS");

    attachSlider(masterVolKnob, "masterVol", "VOLUME");

    startTimerHz(30);
}

MiniJJMoogAudioProcessorEditor::~MiniJJMoogAudioProcessorEditor()
{
    stopTimer();
    setLookAndFeel(nullptr);
}

void MiniJJMoogAudioProcessorEditor::setupKnob(juce::Slider& s, const juce::String& label)
{
    s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    s.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    s.setName(label);
}

void MiniJJMoogAudioProcessorEditor::setupSwitch(juce::ToggleButton& b, const juce::String& text, juce::Colour accent)
{
    b.setButtonText(text);
    b.setColour(juce::ToggleButton::tickColourId, accent);
    b.setColour(juce::ToggleButton::textColourId, juce::Colours::white);
}

void MiniJJMoogAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.setColour(juce::Colour(0xff5c3317));
    g.fillRect(getLocalBounds());

    auto panel = getLocalBounds().reduced(18, 14).toFloat();
    g.setColour(juce::Colour(0xffb8924a));
    g.drawRoundedRectangle(panel, 6.0f, 2.0f);

    auto innerPanel = panel.reduced(2.0f);
    g.setColour(juce::Colour(0xff18191c));
    g.fillRoundedRectangle(innerPanel, 5.0f);

    g.setColour(juce::Colours::white.withAlpha(0.85f));
    g.setFont(juce::Font(13.0f, juce::Font::bold));
    g.drawText("CONTROLLERS",     32, 24, 150, 20, juce::Justification::centredLeft);
    g.drawText("OSCILLATOR BANK", 200, 24, 240, 20, juce::Justification::centredLeft);
    g.drawText("MIXER",           460, 24, 150, 20, juce::Justification::centredLeft);
    g.drawText("MODIFIERS",       630, 24, 210, 20, juce::Justification::centredLeft);
    g.drawText("OUTPUT",          855, 24, 100, 20, juce::Justification::centredLeft);

    g.setFont(juce::Font(9.5f, juce::Font::bold));
    g.setColour(juce::Colours::lightgrey);
    for (int i = 0; i < getNumChildComponents(); ++i)
    {
        if (auto* sl = dynamic_cast<juce::Slider*>(getChildComponent(i)))
        {
            auto b = sl->getBounds();
            g.drawText(sl->getName(), b.getX() - 6, b.getBottom() - 2, b.getWidth() + 12, 14,
                       juce::Justification::centred);
        }
    }

    auto lowerArea = juce::Rectangle<int>(32, 395, getWidth() - 64, 138);
    g.setColour(juce::Colour(0xff101113));
    g.fillRoundedRectangle(lowerArea.toFloat(), 4.0f);

    auto pitchWheelRect = juce::Rectangle<float>(48.0f, 412.0f, 22.0f, 104.0f);
    auto modWheelRect   = juce::Rectangle<float>(82.0f, 412.0f, 22.0f, 104.0f);
    g.setColour(juce::Colour(0xff2a2c30));
    g.fillRect(pitchWheelRect);
    g.fillRect(modWheelRect);

    float pitchY = pitchWheelRect.getCentreY() - (audioProcessor.pitchBendNorm * 40.0f);
    g.setColour(juce::Colours::silver);
    g.fillRect(pitchWheelRect.getX() - 2.0f, pitchY - 5.0f, pitchWheelRect.getWidth() + 4.0f, 10.0f);

    float modY = modWheelRect.getBottom() - 8.0f - (audioProcessor.modWheelNorm * 88.0f);
    g.fillRect(modWheelRect.getX() - 2.0f, modY - 5.0f, modWheelRect.getWidth() + 4.0f, 10.0f);

    auto keysArea = juce::Rectangle<float>(122.0f, 408.0f, static_cast<float>(lowerArea.getRight() - 134), 112.0f);
    const int numWhiteKeys = 14;
    const float whiteW = keysArea.getWidth() / static_cast<float>(numWhiteKeys);

    for (int i = 0; i < numWhiteKeys; ++i)
    {
        juce::Rectangle<float> wk(keysArea.getX() + i * whiteW, keysArea.getY(), whiteW - 1.5f, keysArea.getHeight());
        g.setColour(juce::Colour(0xfff4f1ea));
        g.fillRect(wk);
    }

    const int blackPattern[7] = { 1, 1, 0, 1, 1, 1, 0 };
    for (int i = 0; i < numWhiteKeys - 1; ++i)
    {
        if (blackPattern[i % 7] == 1)
        {
            juce::Rectangle<float> bk(keysArea.getX() + (i + 0.68f) * whiteW, keysArea.getY(),
                                      whiteW * 0.62f, keysArea.getHeight() * 0.62f);
            g.setColour(juce::Colour(0xff141518));
            g.fillRect(bk);
        }
    }
}

void MiniJJMoogAudioProcessorEditor::resized()
{
    tuneKnob.setBounds(34, 54, 54, 54);
    glideKnob.setBounds(102, 54, 54, 54);
    modMixKnob.setBounds(34, 134, 54, 54);
    osc3CtrlSwitch.setBounds(100, 142, 92, 26);

    osc1Range.setBounds(205, 56, 62, 24);
    osc1Freq.setBounds(274, 48, 52, 52);
    osc1Wave.setBounds(334, 56, 86, 24);

    osc2Range.setBounds(205, 128, 62, 24);
    osc2Freq.setBounds(274, 120, 52, 52);
    osc2Wave.setBounds(334, 128, 86, 24);

    osc3Range.setBounds(205, 200, 62, 24);
    osc3Freq.setBounds(274, 192, 52, 52);
    osc3Wave.setBounds(334, 200, 86, 24);

    vol1Knob.setBounds(455, 54, 48, 48);
    on1Switch.setBounds(510, 64, 62, 24);

    vol2Knob.setBounds(455, 122, 48, 48);
    on2Switch.setBounds(510, 132, 62, 24);

    vol3Knob.setBounds(455, 190, 48, 48);
    on3Switch.setBounds(510, 200, 62, 24);

    noiseVolKnob.setBounds(455, 258, 48, 48);
    noiseOnSwitch.setBounds(510, 262, 62, 22);
    pinkNoiseSwitch.setBounds(510, 286, 62, 22);

    cutoffKnob.setBounds(610, 54, 54, 54);
    emphasisKnob.setBounds(674, 54, 54, 54);
    contourKnob.setBounds(738, 54, 54, 54);

    filtAtkKnob.setBounds(610, 134, 50, 50);
    filtDecKnob.setBounds(674, 134, 50, 50);
    filtSusKnob.setBounds(738, 134, 50, 50);

    ampAtkKnob.setBounds(610, 214, 50, 50);
    ampDecKnob.setBounds(674, 214, 50, 50);
    ampSusKnob.setBounds(738, 214, 50, 50);

    masterVolKnob.setBounds(868, 72, 64, 64);
}

void MiniJJMoogAudioProcessorEditor::timerCallback()
{
    repaint();
}
