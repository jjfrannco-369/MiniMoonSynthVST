#include "PluginProcessor.h"
#include "PluginEditor.h"

void MiniJJMoogAudioProcessorEditor::VintageKnobLookAndFeel::drawRotarySlider(
    juce::Graphics& g, int x, int y, int width, int height,
    float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
    juce::Slider&)
{
    auto bounds = juce::Rectangle<float>((float) x, (float) y, (float) width, (float) height).reduced(4.0f);
    float radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) * 0.5f;
    auto centre = bounds.getCentre();

    // Marca de escala ao redor do knob
    const int numTicks = 11;
    for (int i = 0; i < numTicks; ++i)
    {
        float norm = (float) i / (float) (numTicks - 1);
        float angle = rotaryStartAngle + norm * (rotaryEndAngle - rotaryStartAngle) - juce::MathConstants<float>::halfPi;
        float innerR = radius + 1.0f;
        float outerR = radius + 4.5f;
        g.setColour(juce::Colour(0xffd8d2c5).withAlpha(0.75f));
        g.drawLine(centre.x + std::cos(angle) * innerR,
                   centre.y + std::sin(angle) * innerR,
                   centre.x + std::cos(angle) * outerR,
                   centre.y + std::sin(angle) * outerR, 1.1f);
    }

    // Corpo preto fosco do knob
    juce::ColourGradient knobGrad(juce::Colour(0xff3a3a3a), centre.x, centre.y - radius,
                                juce::Colour(0xff141414), centre.x, centre.y + radius, false);
    g.setGradientFill(knobGrad);
    g.fillEllipse(centre.x - radius, centre.y - radius, radius * 2.0f, radius * 2.0f);

    g.setColour(juce::Colour(0xff080808));
    g.drawEllipse(centre.x - radius, centre.y - radius, radius * 2.0f, radius * 2.0f, 1.5f);

    // Anel interno
    float innerCapR = radius * 0.72f;
    g.setColour(juce::Colour(0xff232323));
    g.fillEllipse(centre.x - innerCapR, centre.y - innerCapR, innerCapR * 2.0f, innerCapR * 2.0f);

    // Indicador branco clássico
    float angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle) - juce::MathConstants<float>::halfPi;
    juce::Path pointer;
    pointer.startNewSubPath(centre.x + std::cos(angle) * (radius * 0.25f),
                            centre.y + std::sin(angle) * (radius * 0.25f));
    pointer.lineTo(centre.x + std::cos(angle) * (radius * 0.92f),
                   centre.y + std::sin(angle) * (radius * 0.92f));
    g.setColour(juce::Colour(0xfff5f2eb));
    g.strokePath(pointer, juce::PathStrokeType(2.4f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
}

MiniJJMoogAudioProcessorEditor::MiniJJMoogAudioProcessorEditor(MiniJJMoogAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    setSize(1040, 470);

    const juce::StringArray ranges { "LO", "32'", "16'", "8'", "4'", "2'" };
    const juce::StringArray waves  { "Tri", "TriSaw", "Saw", "Square", "WidePulse", "NarrowPulse" };

    // Controllers
    configureKnob(tuneSlider, "TUNE");
    configureKnob(glideSlider, "GLIDE");
    configureKnob(modMixSlider, "MOD MIX");
    configureToggle(osc3CtrlButton, "OSC 3 CTRL");

    // Osc 1
    configureCombo(osc1RangeCombo, ranges);
    configureKnob(osc1FreqSlider, "OSC 1 FREQ");
    configureCombo(osc1WaveCombo, waves);

    // Osc 2
    configureCombo(osc2RangeCombo, ranges);
    configureKnob(osc2FreqSlider, "OSC 2 FREQ");
    configureCombo(osc2WaveCombo, waves);

    // Osc 3
    configureCombo(osc3RangeCombo, ranges);
    configureKnob(osc3FreqSlider, "OSC 3 FREQ");
    configureCombo(osc3WaveCombo, waves);

    // Mixer
    configureKnob(vol1Slider, "OSC 1 VOL");
    configureToggle(on1Button, "ON 1");
    configureKnob(vol2Slider, "OSC 2 VOL");
    configureToggle(on2Button, "ON 2");
    configureKnob(vol3Slider, "OSC 3 VOL");
    configureToggle(on3Button, "ON 3");
    configureKnob(noiseVolSlider, "NOISE VOL");
    configureToggle(noiseOnButton, "NOISE ON");
    configureToggle(pinkNoiseButton, "PINK");

    // Modifiers
    configureKnob(cutoffSlider, "CUTOFF");
    configureKnob(emphasisSlider, "EMPHASIS");
    configureKnob(contourAmtSlider, "CONTOUR");

    configureKnob(filtAtkSlider, "F. ATK");
    configureKnob(filtDecSlider, "F. DEC");
    configureKnob(filtSusSlider, "F. SUS");

    configureKnob(ampAtkSlider, "A. ATK");
    configureKnob(ampDecSlider, "A. DEC");
    configureKnob(ampSusSlider, "A. SUS");

    // Output
    configureKnob(masterVolSlider, "MASTER");

    // Attachments
    auto& apvts = audioProcessor.apvts;
    tuneAtt   = std::make_unique<SliderAttachment>(apvts, "tune", tuneSlider);
    glideAtt  = std::make_unique<SliderAttachment>(apvts, "glide", glideSlider);
    modMixAtt = std::make_unique<SliderAttachment>(apvts, "modMix", modMixSlider);
    osc3CtrlAtt = std::make_unique<ButtonAttachment>(apvts, "osc3Ctrl", osc3CtrlButton);

    osc1RangeAtt = std::make_unique<ComboBoxAttachment>(apvts, "osc1Range", osc1RangeCombo);
    osc1FreqAtt  = std::make_unique<SliderAttachment>(apvts, "osc1Freq", osc1FreqSlider);
    osc1WaveAtt  = std::make_unique<ComboBoxAttachment>(apvts, "osc1Wave", osc1WaveCombo);

    osc2RangeAtt = std::make_unique<ComboBoxAttachment>(apvts, "osc2Range", osc2RangeCombo);
    osc2FreqAtt  = std::make_unique<SliderAttachment>(apvts, "osc2Freq", osc2FreqSlider);
    osc2WaveAtt  = std::make_unique<ComboBoxAttachment>(apvts, "osc2Wave", osc2WaveCombo);

    osc3RangeAtt = std::make_unique<ComboBoxAttachment>(apvts, "osc3Range", osc3RangeCombo);
    osc3FreqAtt  = std::make_unique<SliderAttachment>(apvts, "osc3Freq", osc3FreqSlider);
    osc3WaveAtt  = std::make_unique<ComboBoxAttachment>(apvts, "osc3Wave", osc3WaveCombo);

    vol1Att = std::make_unique<SliderAttachment>(apvts, "vol1", vol1Slider);
    on1Att  = std::make_unique<ButtonAttachment>(apvts, "on1", on1Button);
    vol2Att = std::make_unique<SliderAttachment>(apvts, "vol2", vol2Slider);
    on2Att  = std::make_unique<ButtonAttachment>(apvts, "on2", on2Button);
    vol3Att = std::make_unique<SliderAttachment>(apvts, "vol3", vol3Slider);
    on3Att  = std::make_unique<ButtonAttachment>(apvts, "on3", on3Button);

    noiseVolAtt   = std::make_unique<SliderAttachment>(apvts, "noiseVol", noiseVolSlider);
    noiseOnAtt    = std::make_unique<ButtonAttachment>(apvts, "noiseOn", noiseOnButton);
    pinkNoiseAtt  = std::make_unique<ButtonAttachment>(apvts, "pinkNoise", pinkNoiseButton);

    cutoffAtt     = std::make_unique<SliderAttachment>(apvts, "cutoff", cutoffSlider);
    emphasisAtt   = std::make_unique<SliderAttachment>(apvts, "emphasis", emphasisSlider);
    contourAmtAtt = std::make_unique<SliderAttachment>(apvts, "contourAmt", contourAmtSlider);

    filtAtkAtt = std::make_unique<SliderAttachment>(apvts, "filtAtk", filtAtkSlider);
    filtDecAtt = std::make_unique<SliderAttachment>(apvts, "filtDec", filtDecSlider);
    filtSusAtt = std::make_unique<SliderAttachment>(apvts, "filtSus", filtSusSlider);

    ampAtkAtt = std::make_unique<SliderAttachment>(apvts, "ampAtk", ampAtkSlider);
    ampDecAtt = std::make_unique<SliderAttachment>(apvts, "ampDec", ampDecSlider);
    ampSusAtt = std::make_unique<SliderAttachment>(apvts, "ampSus", ampSusSlider);

    masterVolAtt = std::make_unique<SliderAttachment>(apvts, "masterVol", masterVolSlider);
}

void MiniJJMoogAudioProcessorEditor::configureKnob(juce::Slider& slider, const juce::String& labelText)
{
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 54, 15);
    slider.setLookAndFeel(&vintageKnobLAF);
    slider.setColour(juce::Slider::textBoxTextColourId, juce::Colour(0xffe6dfd1));
    slider.setColour(juce::Slider::textBoxBackgroundColourId, juce::Colour(0xff141414));
    slider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colour(0xff4a453c));
    slider.setName(labelText);
    addAndMakeVisible(slider);
}

void MiniJJMoogAudioProcessorEditor::configureToggle(juce::ToggleButton& button, const juce::String& buttonText)
{
    button.setButtonText(buttonText);
    button.setColour(juce::ToggleButton::textColourId, juce::Colour(0xffeae4d7));
    button.setColour(juce::ToggleButton::tickColourId, juce::Colour(0xffe05a2b));
    button.setColour(juce::ToggleButton::tickDisabledColourId, juce::Colour(0xff555555));
    addAndMakeVisible(button);
}

void MiniJJMoogAudioProcessorEditor::configureCombo(juce::ComboBox& combo, const juce::StringArray& items)
{
    combo.addItemList(items, 1);
    combo.setSelectedId(1, juce::dontSendNotification);
    combo.setColour(juce::ComboBox::backgroundColourId, juce::Colour(0xff181818));
    combo.setColour(juce::ComboBox::textColourId, juce::Colour(0xfff0ebe1));
    combo.setColour(juce::ComboBox::outlineColourId, juce::Colour(0xff6a6355));
    combo.setColour(juce::ComboBox::arrowColourId, juce::Colour(0xffd8d2c5));
    addAndMakeVisible(combo);
}

void MiniJJMoogAudioProcessorEditor::drawSectionBox(juce::Graphics& g, juce::Rectangle<int> bounds, const juce::String& title)
{
    g.setColour(juce::Colour(0xff161618));
    g.fillRoundedRectangle(bounds.toFloat(), 4.0f);

    g.setColour(juce::Colour(0xff787060));
    g.drawRoundedRectangle(bounds.toFloat(), 4.0f, 1.2f);

    g.setColour(juce::Colour(0xfff3efe6));
    g.setFont(juce::Font(juce::FontOptions(12.5f).withStyle("Bold")));
    g.drawText(title, bounds.getX() + 8, bounds.getY() + 6, bounds.getWidth() - 16, 16,
               juce::Justification::centredLeft, true);
}

void MiniJJMoogAudioProcessorEditor::paint(juce::Graphics& g)
{
    // Gabinete de madeira clássico ao redor
    juce::ColourGradient woodGrad(juce::Colour(0xff9e5b2b), 0.0f, 0.0f,
                                juce::Colour(0xff6c3814), 0.0f, (float) getHeight(), false);
    g.setGradientFill(woodGrad);
    g.fillAll();

    // Borda externa escura
    g.setColour(juce::Colour(0xff2a1406));
    g.drawRect(getLocalBounds(), 3);

    // Painel frontal metálico escuro
    auto panel = getLocalBounds().reduced(18, 14);
    g.setColour(juce::Colour(0xff1e1e21));
    g.fillRoundedRectangle(panel.toFloat(), 6.0f);

    g.setColour(juce::Colour(0xff524c42));
    g.drawRoundedRectangle(panel.toFloat(), 6.0f, 1.5f);

    // Faixa inferior clássica estilo badge do Minimoog
    auto badgeArea = juce::Rectangle<int>(panel.getRight() - 190, panel.getBottom() - 44, 175, 34);
    g.setColour(juce::Colour(0xfff2efe9));
    g.fillRoundedRectangle(badgeArea.toFloat(), 3.0f);
    g.setColour(juce::Colour(0xff1c1c1c));
    g.setFont(juce::Font(juce::FontOptions(15.0f).withStyle("Bold")));
    g.drawText("MINI JJ MOOG v2", badgeArea, juce::Justification::centred, false);

    // Caixas de seção
    auto content = panel.reduced(10, 10);
    int secY = content.getY();
    int secH = content.getHeight() - 48;

    drawSectionBox(g, { content.getX(), secY, 120, secH }, "CONTROLLERS");
    drawSectionBox(g, { content.getX() + 128, secY, 270, secH }, "OSCILLATOR BANK");
    drawSectionBox(g, { content.getX() + 406, secY, 225, secH }, "MIXER");
    drawSectionBox(g, { content.getX() + 639, secY, 232, secH }, "MODIFIERS");
    drawSectionBox(g, { content.getX() + 879, secY, content.getRight() - (content.getX() + 879), secH }, "OUTPUT");

    // Rótulos pequenos acima dos controles principais
    g.setColour(juce::Colour(0xffcfc8b8));
    g.setFont(juce::Font(juce::FontOptions(10.5f).withStyle("Bold")));

    auto drawSmallLabel = [&](juce::Component& comp, const juce::String& text)
    {
        auto b = comp.getBounds();
        g.drawText(text, b.getX() - 4, b.getY() - 14, b.getWidth() + 8, 13,
                   juce::Justification::centred, false);
    };

    drawSmallLabel(tuneSlider, "TUNE");
    drawSmallLabel(glideSlider, "GLIDE");
    drawSmallLabel(modMixSlider, "MOD MIX");

    drawSmallLabel(osc1RangeCombo, "RANGE");
    drawSmallLabel(osc1FreqSlider, "FREQ");
    drawSmallLabel(osc1WaveCombo, "WAVE");

    drawSmallLabel(osc2RangeCombo, "RANGE");
    drawSmallLabel(osc2FreqSlider, "FREQ");
    drawSmallLabel(osc2WaveCombo, "WAVE");

    drawSmallLabel(osc3RangeCombo, "RANGE");
    drawSmallLabel(osc3FreqSlider, "FREQ");
    drawSmallLabel(osc3WaveCombo, "WAVE");

    drawSmallLabel(vol1Slider, "OSC 1");
    drawSmallLabel(vol2Slider, "OSC 2");
    drawSmallLabel(vol3Slider, "OSC 3");
    drawSmallLabel(noiseVolSlider, "NOISE");

    drawSmallLabel(cutoffSlider, "CUTOFF");
    drawSmallLabel(emphasisSlider, "EMPHASIS");
    drawSmallLabel(contourAmtSlider, "CONTOUR");

    drawSmallLabel(filtAtkSlider, "F. ATK");
    drawSmallLabel(filtDecSlider, "F. DEC");
    drawSmallLabel(filtSusSlider, "F. SUS");

    drawSmallLabel(ampAtkSlider, "A. ATK");
    drawSmallLabel(ampDecSlider, "A. DEC");
    drawSmallLabel(ampSusSlider, "A. SUS");

    drawSmallLabel(masterVolSlider, "VOLUME");
}

void MiniJJMoogAudioProcessorEditor::resized()
{
    auto panel = getLocalBounds().reduced(18, 14);
    auto content = panel.reduced(10, 10);
    int secY = content.getY();
    int secH = content.getHeight() - 48;

    // 1. CONTROLLERS
    auto ctrlBox = juce::Rectangle<int>(content.getX(), secY, 120, secH).reduced(10, 28);
    tuneSlider.setBounds(ctrlBox.getX() + 14, ctrlBox.getY() + 10, 72, 82);
    glideSlider.setBounds(ctrlBox.getX() + 14, ctrlBox.getY() + 104, 72, 82);
    modMixSlider.setBounds(ctrlBox.getX() + 14, ctrlBox.getY() + 198, 72, 82);
    osc3CtrlButton.setBounds(ctrlBox.getX() + 4, ctrlBox.getY() + 292, 92, 24);

    // 2. OSCILLATOR BANK (3 linhas: Osc 1, Osc 2, Osc 3)
    auto oscBox = juce::Rectangle<int>(content.getX() + 128, secY, 270, secH).reduced(10, 28);
    int rowH = oscBox.getHeight() / 3;

    auto layoutOscRow = [&](int rowIdx, juce::ComboBox& rangeC, juce::Slider& freqS, juce::ComboBox& waveC)
    {
        int y = oscBox.getY() + rowIdx * rowH + 14;
        rangeC.setBounds(oscBox.getX() + 2, y + 18, 66, 26);
        freqS.setBounds(oscBox.getX() + 74, y - 4, 82, 76);
        waveC.setBounds(oscBox.getX() + 162, y + 18, 84, 26);
    };

    layoutOscRow(0, osc1RangeCombo, osc1FreqSlider, osc1WaveCombo);
    layoutOscRow(1, osc2RangeCombo, osc2FreqSlider, osc2WaveCombo);
    layoutOscRow(2, osc3RangeCombo, osc3FreqSlider, osc3WaveCombo);

    // 3. MIXER
    auto mixBox = juce::Rectangle<int>(content.getX() + 406, secY, 225, secH).reduced(10, 28);
    int colW = mixBox.getWidth() / 2;

    vol1Slider.setBounds(mixBox.getX() + 12, mixBox.getY() + 14, 72, 76);
    on1Button.setBounds(mixBox.getX() + 16, mixBox.getY() + 92, 66, 22);

    vol2Slider.setBounds(mixBox.getX() + 12, mixBox.getY() + 122, 72, 76);
    on2Button.setBounds(mixBox.getX() + 16, mixBox.getY() + 200, 66, 22);

    vol3Slider.setBounds(mixBox.getX() + colW + 14, mixBox.getY() + 14, 72, 76);
    on3Button.setBounds(mixBox.getX() + colW + 18, mixBox.getY() + 92, 66, 22);

    noiseVolSlider.setBounds(mixBox.getX() + colW + 14, mixBox.getY() + 122, 72, 76);
    noiseOnButton.setBounds(mixBox.getX() + colW + 10, mixBox.getY() + 200, 86, 22);
    pinkNoiseButton.setBounds(mixBox.getX() + colW + 10, mixBox.getY() + 226, 86, 22);

    // 4. MODIFIERS
    auto modBox = juce::Rectangle<int>(content.getX() + 639, secY, 232, secH).reduced(10, 28);

    cutoffSlider.setBounds(modBox.getX() + 4, modBox.getY() + 14, 66, 76);
    emphasisSlider.setBounds(modBox.getX() + 74, modBox.getY() + 14, 66, 76);
    contourAmtSlider.setBounds(modBox.getX() + 144, modBox.getY() + 14, 66, 76);

    filtAtkSlider.setBounds(modBox.getX() + 4, modBox.getY() + 114, 66, 76);
    filtDecSlider.setBounds(modBox.getX() + 74, modBox.getY() + 114, 66, 76);
    filtSusSlider.setBounds(modBox.getX() + 144, modBox.getY() + 114, 66, 76);

    ampAtkSlider.setBounds(modBox.getX() + 4, modBox.getY() + 214, 66, 76);
    ampDecSlider.setBounds(modBox.getX() + 74, modBox.getY() + 214, 66, 76);
    ampSusSlider.setBounds(modBox.getX() + 144, modBox.getY() + 214, 66, 76);

    // 5. OUTPUT
    auto outBox = juce::Rectangle<int>(content.getX() + 879, secY, content.getRight() - (content.getX() + 879) - 10, secH).reduced(8, 28);
    masterVolSlider.setBounds(outBox.getX() + 14, outBox.getY() + 20, 82, 86);

    // Rodapé com dica rápida de uso no Reason
    auto infoArea = juce::Rectangle<int>(content.getX() + 8, panel.getBottom() - 40, panel.getWidth() - 210, 28);
    juce::ignoreUnused(infoArea);
}
