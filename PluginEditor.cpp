#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <algorithm>
#include <cmath>

void MiniMoonLookAndFeel::drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height,
                                              float sliderPosProportional, float rotaryStartAngle,
                                              float rotaryEndAngle, juce::Slider&)
{
    const float w = static_cast<float> (width);
    const float h = static_cast<float> (height);
    const float baseR = std::max (8.0f, std::min (w, h) * 0.40f);
    const float cx = static_cast<float> (x) + w * 0.5f;
    const float cy = static_cast<float> (y) + h * 0.5f;

    // Marcas de escala radiais em volta do knob
    const int numTicks = 11;
    for (int i = 0; i < numTicks; ++i)
    {
        const float t = static_cast<float> (i) / static_cast<float> (numTicks - 1);
        const float ang = rotaryStartAngle + t * (rotaryEndAngle - rotaryStartAngle) - juce::MathConstants<float>::halfPi;
        const float rTick1 = baseR + 2.0f;
        const float rTick2 = baseR + 5.5f;
        g.setColour (juce::Colour (0xffb2a792));
        g.drawLine (cx + std::cos (ang) * rTick1,
                    cy + std::sin (ang) * rTick1,
                    cx + std::cos (ang) * rTick2,
                    cy + std::sin (ang) * rTick2, 1.1f);
    }

    // Sombra projetada 3D
    g.setColour (juce::Colour (0x99050506));
    g.fillEllipse (cx - baseR + 2.0f, cy - baseR + 3.0f, baseR * 2.0f, baseR * 2.0f);

    // Saia / base escura com gradiente de relevo
    juce::ColourGradient skirtGrad (juce::Colour (0xff383530), cx, cy - baseR,
                                  juce::Colour (0xff161513), cx, cy + baseR, false);
    g.setGradientFill (skirtGrad);
    g.fillEllipse (cx - baseR, cy - baseR, baseR * 2.0f, baseR * 2.0f);

    g.setColour (juce::Colour (0xff7c7058));
    g.drawEllipse (cx - baseR, cy - baseR, baseR * 2.0f, baseR * 2.0f, 1.1f);

    // Corpo / tampa 3D do knob
    const float capR = baseR * 0.72f;
    juce::ColourGradient capGrad (juce::Colour (0xff2b2926), cx - capR * 0.3f, cy - capR * 0.4f,
                                juce::Colour (0xff0c0c0d), cx + capR * 0.3f, cy + capR * 0.4f, false);
    g.setGradientFill (capGrad);
    g.fillEllipse (cx - capR, cy - capR, capR * 2.0f, capR * 2.0f);

    g.setColour (juce::Colour (0xff5c5548));
    g.drawEllipse (cx - capR, cy - capR, capR * 2.0f, capR * 2.0f, 1.0f);

    // Ponteiro indicador clássico com realce 3D
    const float angle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle)
                        - juce::MathConstants<float>::halfPi;
    const float px = cx + std::cos (angle) * (capR * 0.90f);
    const float py = cy + std::sin (angle) * (capR * 0.90f);

    g.setColour (juce::Colour (0x88000000));
    g.drawLine (cx + 1.0f, cy + 1.0f, px + 1.0f, py + 1.0f, 2.5f);

    g.setColour (juce::Colour (0xfff7f4ed));
    g.drawLine (cx, cy, px, py, 2.2f);
}

void MiniMoonLookAndFeel::drawToggleButton (juce::Graphics& g, juce::ToggleButton& button,
                                            bool, bool)
{
    const bool isOn = button.getToggleState();
    auto bounds = button.getLocalBounds().toFloat();

    // Chave metalica vintage
    const float metalW = 24.0f;
    const float metalH = 16.0f;
    const float metalX = bounds.getX() + 2.0f;
    const float metalY = bounds.getCentreY() - metalH * 0.5f;

    juce::ColourGradient metalGrad (juce::Colour (0xffc9c3b8), metalX, metalY,
                                   juce::Colour (0xff58544e), metalX, metalY + metalH, false);
    g.setGradientFill (metalGrad);
    g.fillRoundedRectangle (metalX, metalY, metalW, metalH, 2.5f);
    g.setColour (juce::Colour (0xff2b2926));
    g.drawRoundedRectangle (metalX, metalY, metalW, metalH, 2.5f, 1.0f);

    const float tabX = isOn ? (metalX + metalW - 8.0f) : (metalX + 2.0f);
    g.setColour (juce::Colour (0xffeae6de));
    g.fillRoundedRectangle (tabX, metalY + 2.0f, 6.0f, metalH - 4.0f, 1.5f);

    // LED sob lente plastica anos 60/70 ao lado da chave
    const float ledR = 5.0f;
    const float ledCx = metalX + metalW + 10.0f;
    const float ledCy = bounds.getCentreY();

    // Tampa / bezel plastico da lente
    g.setColour (juce::Colour (0xff2c2924));
    g.fillEllipse (ledCx - ledR - 1.5f, ledCy - ledR - 1.5f, (ledR + 1.5f) * 2.0f, (ledR + 1.5f) * 2.0f);

    if (isOn)
    {
        g.setColour (juce::Colour (0xffffb838));
        g.fillEllipse (ledCx - ledR, ledCy - ledR, ledR * 2.0f, ledR * 2.0f);
        g.setColour (juce::Colour (0xffffeab8));
        g.fillEllipse (ledCx - ledR * 0.45f, ledCy - ledR * 0.55f, ledR * 0.5f, ledR * 0.45f);
    }
    else
    {
        g.setColour (juce::Colour (0xff3a2414));
        g.fillEllipse (ledCx - ledR, ledCy - ledR, ledR * 2.0f, ledR * 2.0f);
    }

    auto textRect = bounds.withTrimmedLeft (ledCx + ledR + 7.0f);
    g.setColour (isOn ? juce::Colour (0xfff4d68c) : juce::Colour (0xffd6cfc2));
    g.setFont (juce::Font (10.5f, juce::Font::bold));
    g.drawFittedText (button.getButtonText(), textRect.toNearestInt(), juce::Justification::centredLeft, 1);
}

MiniJJMoogAudioProcessorEditor::MiniJJMoogAudioProcessorEditor (MiniJJMoogAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setLookAndFeel (&miniMoonLookAndFeel);

    const juce::StringArray rangeChoices { "LO", "32'", "16'", "8'", "4'", "2'" };
    const juce::StringArray waveChoices  { "Tri", "Tri/Saw", "Saw", "Square", "Wide Pulse", "Narrow Pulse" };
    const juce::StringArray noiseChoices { "White", "Pink" };

    // CONTROLLERS
    setupKnob   (tuneSlider, tuneLabel, "TUNE");
    setupKnob   (glideSlider, glideLabel, "GLIDE");
    setupKnob   (pbRangeSlider, pbRangeLabel, "PB RNG");
    setupKnob   (modMixSlider, modMixLabel, "MOD MIX");
    setupToggle (oscModBtn, "OSC MOD");
    setupToggle (osc3CtrlBtn, "OSC 3 CTRL");
    setupToggle (legatoBtn, "LEGATO");

    // OSCILLATOR BANK
    setupCombo (osc1RangeBox, osc1RangeLabel, "OSC 1 RNG", rangeChoices);
    setupKnob  (osc1FreqSlider, osc1FreqLabel, "OSC 1 FRQ");
    setupCombo (osc1WaveBox, osc1WaveLabel, "OSC 1 WAV", waveChoices);

    setupCombo (osc2RangeBox, osc2RangeLabel, "OSC 2 RNG", rangeChoices);
    setupKnob  (osc2FreqSlider, osc2FreqLabel, "OSC 2 FRQ");
    setupCombo (osc2WaveBox, osc2WaveLabel, "OSC 2 WAV", waveChoices);

    setupCombo (osc3RangeBox, osc3RangeLabel, "OSC 3 RNG", rangeChoices);
    setupKnob  (osc3FreqSlider, osc3FreqLabel, "OSC 3 FRQ");
    setupCombo (osc3WaveBox, osc3WaveLabel, "OSC 3 WAV", waveChoices);

    // MIXER
    setupKnob  (osc1VolSlider, osc1VolLabel, "OSC 1");
    setupKnob  (osc2VolSlider, osc2VolLabel, "OSC 2");
    setupKnob  (osc3VolSlider, osc3VolLabel, "OSC 3");
    setupKnob  (noiseVolSlider, noiseVolLabel, "NOISE");
    setupCombo (noiseColorBox, noiseColorLabel, "NOISE CLR", noiseChoices);
    setupKnob  (extVolSlider, extVolLabel, "EXT IN");
    setupKnob  (overloadSlider, overloadLabel, "OVERLOAD");

    // MODIFIERS
    setupToggle (filterModBtn, "FLT MOD");
    setupToggle (kbCtrlBtn, "KB CTRL");
    setupKnob   (cutoffSlider, cutoffLabel, "CUTOFF");
    setupKnob   (emphasisSlider, emphasisLabel, "EMPHASIS");
    setupKnob   (contourSlider, contourLabel, "CONTOUR");

    setupKnob (fAtkSlider, fAtkLabel, "F-ATK");
    setupKnob (fDecSlider, fDecLabel, "F-DEC");
    setupKnob (fSusSlider, fSusLabel, "F-SUS");

    setupKnob (lAtkSlider, lAtkLabel, "L-ATK");
    setupKnob (lDecSlider, lDecLabel, "L-DEC");
    setupKnob (lSusSlider, lSusLabel, "L-SUS");

    // OUTPUT
    setupKnob   (masterVolSlider, masterVolLabel, "VOLUME");
    setupToggle (mainOutBtn, "MAIN OUT");

    auto& tree = audioProcessor.apvts;
    tuneAtt       = std::make_unique<SliderAttachment> (tree, "tune", tuneSlider);
    glideAtt      = std::make_unique<SliderAttachment> (tree, "glide", glideSlider);
    pbRangeAtt    = std::make_unique<SliderAttachment> (tree, "pitch_bend_range", pbRangeSlider);
    modMixAtt     = std::make_unique<SliderAttachment> (tree, "mod_mix", modMixSlider);
    oscModAtt     = std::make_unique<ButtonAttachment> (tree, "osc_mod_on", oscModBtn);
    osc3CtrlAtt   = std::make_unique<ButtonAttachment> (tree, "osc3_ctrl_on", osc3CtrlBtn);
    legatoAtt     = std::make_unique<ButtonAttachment> (tree, "legato_on", legatoBtn);

    osc1RangeAtt  = std::make_unique<ComboAttachment>  (tree, "osc1_range", osc1RangeBox);
    osc1FreqAtt   = std::make_unique<SliderAttachment> (tree, "osc1_freq", osc1FreqSlider);
    osc1WaveAtt   = std::make_unique<ComboAttachment>  (tree, "osc1_wave", osc1WaveBox);

    osc2RangeAtt  = std::make_unique<ComboAttachment>  (tree, "osc2_range", osc2RangeBox);
    osc2FreqAtt   = std::make_unique<SliderAttachment> (tree, "osc2_freq", osc2FreqSlider);
    osc2WaveAtt   = std::make_unique<ComboAttachment>  (tree, "osc2_wave", osc2WaveBox);

    osc3RangeAtt  = std::make_unique<ComboAttachment>  (tree, "osc3_range", osc3RangeBox);
    osc3FreqAtt   = std::make_unique<SliderAttachment> (tree, "osc3_freq", osc3FreqSlider);
    osc3WaveAtt   = std::make_unique<ComboAttachment>  (tree, "osc3_wave", osc3WaveBox);

    osc1VolAtt    = std::make_unique<SliderAttachment> (tree, "osc1_vol", osc1VolSlider);
    osc2VolAtt    = std::make_unique<SliderAttachment> (tree, "osc2_vol", osc2VolSlider);
    osc3VolAtt    = std::make_unique<SliderAttachment> (tree, "osc3_vol", osc3VolSlider);
    noiseVolAtt   = std::make_unique<SliderAttachment> (tree, "noise_vol", noiseVolSlider);
    noiseColorAtt = std::make_unique<ComboAttachment>  (tree, "noise_color", noiseColorBox);
    extVolAtt     = std::make_unique<SliderAttachment> (tree, "ext_vol", extVolSlider);
    overloadAtt   = std::make_unique<SliderAttachment> (tree, "overload", overloadSlider);

    filterModAtt  = std::make_unique<ButtonAttachment> (tree, "filter_mod_on", filterModBtn);
    kbCtrlAtt     = std::make_unique<ButtonAttachment> (tree, "kb_ctrl_on", kbCtrlBtn);
    cutoffAtt     = std::make_unique<SliderAttachment> (tree, "cutoff", cutoffSlider);
    emphasisAtt   = std::make_unique<SliderAttachment> (tree, "emphasis", emphasisSlider);
    contourAtt    = std::make_unique<SliderAttachment> (tree, "filter_contour", contourSlider);

    fAtkAtt       = std::make_unique<SliderAttachment> (tree, "filter_attack", fAtkSlider);
    fDecAtt       = std::make_unique<SliderAttachment> (tree, "filter_decay", fDecSlider);
    fSusAtt       = std::make_unique<SliderAttachment> (tree, "filter_sustain", fSusSlider);

    lAtkAtt       = std::make_unique<SliderAttachment> (tree, "loud_attack", lAtkSlider);
    lDecAtt       = std::make_unique<SliderAttachment> (tree, "loud_decay", lDecSlider);
    lSusAtt       = std::make_unique<SliderAttachment> (tree, "loud_sustain", lSusSlider);

    masterVolAtt  = std::make_unique<SliderAttachment> (tree, "master_vol", masterVolSlider);
    mainOutAtt    = std::make_unique<ButtonAttachment> (tree, "main_out_on", mainOutBtn);

    setSize (1140, 430);
}

MiniJJMoogAudioProcessorEditor::~MiniJJMoogAudioProcessorEditor()
{
    setLookAndFeel (nullptr);
}

void MiniJJMoogAudioProcessorEditor::setupKnob (juce::Slider& slider, juce::Label& label, const juce::String& text)
{
    slider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 50, 14);
    slider.setColour (juce::Slider::textBoxTextColourId, juce::Colour (0xffe8e2d5));
    slider.setColour (juce::Slider::textBoxBackgroundColourId, juce::Colour (0xff1d1c1a));
    slider.setColour (juce::Slider::textBoxOutlineColourId, juce::Colour (0xff4d473c));
    addAndMakeVisible (slider);

    label.setText (text, juce::dontSendNotification);
    label.setJustificationType (juce::Justification::centred);
    label.setColour (juce::Label::textColourId, juce::Colour (0xffe8e2d5));
    label.setFont (juce::Font (10.0f, juce::Font::bold));
    addAndMakeVisible (label);
}

void MiniJJMoogAudioProcessorEditor::setupCombo (juce::ComboBox& box, juce::Label& label, const juce::String& text, const juce::StringArray& items)
{
    box.addItemList (items, 1);
    box.setColour (juce::ComboBox::backgroundColourId, juce::Colour (0xff1e1d1b));
    box.setColour (juce::ComboBox::textColourId, juce::Colour (0xffe8e2d5));
    box.setColour (juce::ComboBox::outlineColourId, juce::Colour (0xff6b6252));
    addAndMakeVisible (box);

    label.setText (text, juce::dontSendNotification);
    label.setJustificationType (juce::Justification::centred);
    label.setColour (juce::Label::textColourId, juce::Colour (0xffe8e2d5));
    label.setFont (juce::Font (9.5f, juce::Font::bold));
    addAndMakeVisible (label);
}

void MiniJJMoogAudioProcessorEditor::setupToggle (juce::ToggleButton& btn, const juce::String& text)
{
    btn.setButtonText (text);
    addAndMakeVisible (btn);
}

void MiniJJMoogAudioProcessorEditor::paint (juce::Graphics& g)
{
    // Madeira rica com veios sutis e relevo de gabinete
    juce::ColourGradient woodGrad (juce::Colour (0xff5c3518), 0.0f, 0.0f,
                                   juce::Colour (0xff361d0b), 0.0f, static_cast<float> (getHeight()), false);
    g.setGradientFill (woodGrad);
    g.fillAll();

    g.setColour (juce::Colour (0xff7a4722));
    g.fillRect (0, 0, getWidth(), 5);
    g.setColour (juce::Colour (0xff241206));
    g.fillRect (0, getHeight() - 5, getWidth(), 5);

    auto panel = getLocalBounds().reduced (14, 12).toFloat();
    g.setColour (juce::Colour (0xff121214));
    g.fillRoundedRectangle (panel, 5.0f);

    g.setColour (juce::Colour (0xff8c7d5b));
    g.drawRoundedRectangle (panel, 5.0f, 1.5f);

    const float screwX[2] = { panel.getX() + 9.0f, panel.getRight() - 9.0f };
    const float screwY[2] = { panel.getY() + 9.0f, panel.getBottom() - 9.0f };
    for (float sx : screwX)
    {
        for (float sy : screwY)
        {
            g.setColour (juce::Colour (0xff666157));
            g.fillEllipse (sx - 3.5f, sy - 3.5f, 7.0f, 7.0f);
            g.setColour (juce::Colour (0xff191816));
            g.drawLine (sx - 2.0f, sy, sx + 2.0f, sy, 1.0f);
        }
    }

    // Cabecalho sem caracteres especiais quebrados
    g.setColour (juce::Colour (0xffd6b36a));
    g.setFont (juce::Font (15.5f, juce::Font::bold));
    g.drawText ("MINI MOON - ANALOG SYNTHESIZER",
                juce::roundToInt (panel.getX() + 20.0f),
                juce::roundToInt (panel.getY() + 3.0f),
                360, 20, juce::Justification::centredLeft);

    const float secStartY = panel.getY() + 24.0f;
    const float secHeight = panel.getHeight() - 28.0f;
    const float secW = panel.getWidth() / 5.0f;
    const juce::StringArray secTitles { "CONTROLLERS", "OSCILLATOR BANK", "MIXER", "MODIFIERS", "OUTPUT" };

    for (int i = 0; i < 5; ++i)
    {
        const float colX = panel.getX() + static_cast<float> (i) * secW;
        if (i > 0)
        {
            g.setColour (juce::Colour (0xff383632));
            g.drawLine (colX, secStartY + 2.0f, colX, secStartY + secHeight - 4.0f, 1.0f);
        }
        g.setColour (juce::Colour (0xffb8a472));
        g.setFont (juce::Font (10.5f, juce::Font::bold));
        g.drawText (secTitles[i],
                    juce::roundToInt (colX + 4.0f),
                    juce::roundToInt (panel.getY() + 3.0f),
                    juce::roundToInt (secW - 8.0f),
                    16,
                    juce::Justification::centred);
    }
}

void MiniJJMoogAudioProcessorEditor::resized()
{
    const auto panel = getLocalBounds().reduced (14, 12);
    const int topBar = 24;
    const int secW = panel.getWidth() / 5;
    const int secH = panel.getHeight() - topBar - 4;

    auto placeKnob = [] (juce::Slider& s, juce::Label& l, int x, int y, int w, int h)
    {
        l.setBounds (x + 2, y, std::max (16, w - 4), 14);
        s.setBounds (x + 2, y + 14, std::max (16, w - 4), std::max (24, h - 14));
    };

    auto placeCombo = [] (juce::ComboBox& b, juce::Label& l, int x, int y, int w, int h)
    {
        juce::ignoreUnused (h);
        l.setBounds (x + 2, y, std::max (16, w - 4), 14);
        b.setBounds (x + 4, y + 15, std::max (28, w - 8), 22);
    };

    // 1. CONTROLLERS
    {
        const int x0 = panel.getX();
        const int y0 = panel.getY() + topBar;
        const int kw = secW / 2;
        const int kh = 80;
        placeKnob   (tuneSlider, tuneLabel, x0, y0, kw, kh);
        placeKnob   (glideSlider, glideLabel, x0 + kw, y0, kw, kh);
        placeKnob   (pbRangeSlider, pbRangeLabel, x0, y0 + kh, kw, kh);
        placeKnob   (modMixSlider, modMixLabel, x0 + kw, y0 + kh, kw, kh);

        const int by = y0 + kh * 2 + 6;
        oscModBtn.setBounds   (x0 + 8, by, kw - 10, 22);
        osc3CtrlBtn.setBounds (x0 + kw + 4, by, kw - 10, 22);
        legatoBtn.setBounds   (x0 + 8, by + 26, kw - 10, 22);
    }

    // 2. OSCILLATOR BANK (Osc 1, 2 e 3 empilhados um abaixo do outro)
    {
        const int x0 = panel.getX() + secW;
        const int y0 = panel.getY() + topBar;
        const int rowH = secH / 3;
        const int c1 = static_cast<int> (static_cast<float> (secW) * 0.28f);
        const int c2 = static_cast<int> (static_cast<float> (secW) * 0.32f);
        const int c3 = secW - c1 - c2;

        // Linha 1: OSC 1
        placeCombo (osc1RangeBox, osc1RangeLabel, x0, y0, c1, rowH);
        placeKnob  (osc1FreqSlider, osc1FreqLabel, x0 + c1, y0, c2, rowH);
        placeCombo (osc1WaveBox, osc1WaveLabel, x0 + c1 + c2, y0, c3, rowH);

        // Linha 2: OSC 2
        placeCombo (osc2RangeBox, osc2RangeLabel, x0, y0 + rowH, c1, rowH);
        placeKnob  (osc2FreqSlider, osc2FreqLabel, x0 + c1, y0 + rowH, c2, rowH);
        placeCombo (osc2WaveBox, osc2WaveLabel, x0 + c1 + c2, y0 + rowH, c3, rowH);

        // Linha 3: OSC 3
        placeCombo (osc3RangeBox, osc3RangeLabel, x0, y0 + rowH * 2, c1, rowH);
        placeKnob  (osc3FreqSlider, osc3FreqLabel, x0 + c1, y0 + rowH * 2, c2, rowH);
        placeCombo (osc3WaveBox, osc3WaveLabel, x0 + c1 + c2, y0 + rowH * 2, c3, rowH);
    }

    // 3. MIXER
    {
        const int x0 = panel.getX() + secW * 2;
        const int y0 = panel.getY() + topBar;
        const int kw = secW / 2;
        const int kh = 84;
        placeKnob  (osc1VolSlider, osc1VolLabel, x0, y0, kw, kh);
        placeKnob  (osc2VolSlider, osc2VolLabel, x0 + kw, y0, kw, kh);
        placeKnob  (osc3VolSlider, osc3VolLabel, x0, y0 + kh, kw, kh);
        placeKnob  (noiseVolSlider, noiseVolLabel, x0 + kw, y0 + kh, kw, kh);
        placeCombo (noiseColorBox, noiseColorLabel, x0, y0 + kh * 2, kw, kh);
        placeKnob  (extVolSlider, extVolLabel, x0 + kw, y0 + kh * 2, kw, kh);
        placeKnob  (overloadSlider, overloadLabel, x0, y0 + kh * 3, kw, kh);
    }

    // 4. MODIFIERS
    {
        const int x0 = panel.getX() + secW * 3;
        const int y0 = panel.getY() + topBar;
        const int kw = secW / 3;
        const int kh = 84;

        filterModBtn.setBounds (x0 + 6, y0 + 10, kw - 8, 22);
        kbCtrlBtn.setBounds    (x0 + 6, y0 + 36, kw - 8, 22);
        placeKnob (contourSlider, contourLabel, x0 + kw, y0, kw, kh);
        placeKnob (cutoffSlider, cutoffLabel, x0 + kw * 2, y0, kw, kh);

        placeKnob (fAtkSlider, fAtkLabel, x0, y0 + kh, kw, kh);
        placeKnob (fDecSlider, fDecLabel, x0 + kw, y0 + kh, kw, kh);
        placeKnob (fSusSlider, fSusLabel, x0 + kw * 2, y0 + kh, kw, kh);

        placeKnob (lAtkSlider, lAtkLabel, x0, y0 + kh * 2, kw, kh);
        placeKnob (lDecSlider, lDecLabel, x0 + kw, y0 + kh * 2, kw, kh);
        placeKnob (lSusSlider, lSusLabel, x0 + kw * 2, y0 + kh * 2, kw, kh);

        placeKnob (emphasisSlider, emphasisLabel, x0, y0 + kh * 3, kw, kh);
    }

    // 5. OUTPUT
    {
        const int x0 = panel.getX() + secW * 4;
        const int y0 = panel.getY() + topBar;
        placeKnob   (masterVolSlider, masterVolLabel, x0 + 16, y0 + 14, secW - 32, 110);
        mainOutBtn.setBounds (x0 + 28, y0 + 138, secW - 56, 26);
    }
}
