#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <algorithm>
#include <cmath>

void MiniMoonLookAndFeel::setSwitchAccent (juce::ToggleButton* btn, SwitchAccent accent)
{
    if (btn != nullptr)
        accents[btn] = accent;
}

void MiniMoonLookAndFeel::drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height,
                                              float sliderPosProportional, float rotaryStartAngle,
                                              float rotaryEndAngle, juce::Slider&)
{
    const float w = static_cast<float> (width);
    const float h = static_cast<float> (height);
    const float baseR = std::max (8.0f, std::min (w, h) * 0.40f);
    const float cx = static_cast<float> (x) + w * 0.5f;
    const float cy = static_cast<float> (y) + h * 0.5f;

    // Pontos/marcas de escala ao redor do knob
    const int numTicks = 11;
    for (int i = 0; i < numTicks; ++i)
    {
        const float t = static_cast<float> (i) / static_cast<float> (numTicks - 1);
        const float ang = rotaryStartAngle + t * (rotaryEndAngle - rotaryStartAngle) - juce::MathConstants<float>::halfPi;
        const float r1 = baseR + 2.2f;
        const float r2 = baseR + 5.2f;
        g.setColour (juce::Colour (0xffeae6de));
        g.drawLine (cx + std::cos (ang) * r1,
                    cy + std::sin (ang) * r1,
                    cx + std::cos (ang) * r2,
                    cy + std::sin (ang) * r2, 1.15f);
    }

    // Sombra projetada sob o knob
    g.setColour (juce::Colour (0xcc020203));
    g.fillEllipse (cx - baseR + 2.0f, cy - baseR + 3.0f, baseR * 2.0f, baseR * 2.0f);

    // Corpo preto do knob com gomos sutis em volta
    g.setColour (juce::Colour (0xff121316));
    g.fillEllipse (cx - baseR, cy - baseR, baseR * 2.0f, baseR * 2.0f);

    const int numLobes = 8;
    for (int l = 0; l < numLobes; ++l)
    {
        const float lobeAng = (static_cast<float> (l) / static_cast<float> (numLobes)) * juce::MathConstants<float>::twoPi;
        const float lx = cx + std::cos (lobeAng) * (baseR * 0.78f);
        const float ly = cy + std::sin (lobeAng) * (baseR * 0.78f);
        g.setColour (juce::Colour (0xff1f2126));
        g.fillEllipse (lx - baseR * 0.22f, ly - baseR * 0.22f, baseR * 0.44f, baseR * 0.44f);
    }

    g.setColour (juce::Colour (0xff585d64));
    g.drawEllipse (cx - baseR, cy - baseR, baseR * 2.0f, baseR * 2.0f, 1.0f);

    // Tampa metálica prateada com ranhuras concêntricas no estilo da foto
    const float capR = baseR * 0.65f;
    juce::ColourGradient silverCap (juce::Colour (0xfff4f7fa), cx, cy - capR,
                                    juce::Colour (0xff8c929b), cx, cy + capR, false);
    g.setGradientFill (silverCap);
    g.fillEllipse (cx - capR, cy - capR, capR * 2.0f, capR * 2.0f);

    for (float ringScale = 0.25f; ringScale <= 0.82f; ringScale += 0.19f)
    {
        const float rr = capR * ringScale;
        g.setColour (juce::Colour (0x36181a1f));
        g.drawEllipse (cx - rr, cy - rr, rr * 2.0f, rr * 2.0f, 0.8f);
    }

    g.setColour (juce::Colour (0xff23262b));
    g.drawEllipse (cx - capR, cy - capR, capR * 2.0f, capR * 2.0f, 1.1f);

    // Ponteiro escuro + ponto branco bem marcado na borda
    const float angle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle)
                        - juce::MathConstants<float>::halfPi;
    const float px = cx + std::cos (angle) * (capR * 0.84f);
    const float py = cy + std::sin (angle) * (capR * 0.84f);

    g.setColour (juce::Colour (0xff0c0d0f));
    g.drawLine (cx, cy, px, py, 2.2f);

    g.setColour (juce::Colour (0xffffffff));
    g.fillEllipse (px - 2.5f, py - 2.5f, 5.0f, 5.0f);
    g.setColour (juce::Colour (0xff101114));
    g.drawEllipse (px - 2.5f, py - 2.5f, 5.0f, 5.0f, 0.9f);
}

void MiniMoonLookAndFeel::drawToggleButton (juce::Graphics& g, juce::ToggleButton& button,
                                            bool, bool)
{
    const bool isOn = button.getToggleState();
    auto bounds = button.getLocalBounds().toFloat();
    const juce::String txt = button.getButtonText();

    SwitchAccent acc = SwitchAccent::yellow;
    const auto it = accents.find (&button);
    if (it != accents.end())
        acc = it->second;

    juce::Colour activeCol (0xffe6b800);
    juce::Colour inactiveCol (0xff6e5b12);
    if (acc == SwitchAccent::red)
    {
        activeCol = juce::Colour (0xffe02828);
        inactiveCol = juce::Colour (0xff631919);
    }
    else if (acc == SwitchAccent::blue)
    {
        activeCol = juce::Colour (0xff2678e8);
        inactiveCol = juce::Colour (0xff163869);
    }

    const bool isCompactTrackButton = (txt == "1/3" || txt == "2/3" || txt == "FULL");

    if (isCompactTrackButton)
    {
        const float btnW = std::min (24.0f, bounds.getWidth() - 6.0f);
        const float btnH = 13.0f;
        const float btnX = bounds.getCentreX() - btnW * 0.5f;
        const float btnY = bounds.getY() + 2.0f;

        g.setColour (juce::Colour (0xff070708));
        g.fillRoundedRectangle (btnX - 1.0f, btnY - 1.0f, btnW + 2.0f, btnH + 2.0f, 3.0f);

        g.setColour (juce::Colour (0xff0b0b0c));
        g.fillRoundedRectangle (btnX + 1.0f, btnY + 1.0f, btnW, btnH, 2.5f);

        g.setColour (isOn ? activeCol : inactiveCol);
        g.fillRoundedRectangle (btnX, btnY, btnW, btnH, 2.5f);
        g.setColour (juce::Colour (0x55ffffff));
        g.drawLine (btnX + 1.5f, btnY + 1.2f, btnX + btnW - 1.5f, btnY + 1.2f, 0.9f);

        g.setColour (juce::Colour (0xff141416));
        g.drawRoundedRectangle (btnX, btnY, btnW, btnH, 2.5f, 1.0f);

        const float ledR = 3.2f;
        const float ledY = btnY + btnH + 5.0f;
        const float ledX = bounds.getCentreX();

        g.setColour (isOn ? juce::Colour (0xffff2424) : juce::Colour (0xff381010));
        g.fillEllipse (ledX - ledR, ledY - ledR, ledR * 2.0f, ledR * 2.0f);

        auto labelArea = bounds.withTop (ledY + ledR + 2.0f);
        g.setColour (juce::Colour (0xfff5f2eb));
        g.setFont (juce::Font (9.0f, juce::Font::bold));
        g.drawFittedText (txt, labelArea.toNearestInt(), juce::Justification::centred, 1);
        return;
    }

    const float btnW = 24.0f;
    const float btnH = 14.0f;
    const float btnX = bounds.getX() + 2.0f;
    const float btnY = bounds.getCentreY() - btnH * 0.5f;

    g.setColour (juce::Colour (0xff070708));
    g.fillRoundedRectangle (btnX - 1.2f, btnY - 1.2f, btnW + 2.4f, btnH + 2.4f, 3.5f);

    g.setColour (juce::Colour (0xff0b0b0c));
    g.fillRoundedRectangle (btnX + 1.0f, btnY + 1.5f, btnW, btnH, 3.0f);

    g.setColour (isOn ? activeCol : inactiveCol);
    g.fillRoundedRectangle (btnX, btnY, btnW, btnH, 3.0f);

    g.setColour (juce::Colour (0x66ffffff));
    g.drawLine (btnX + 2.0f, btnY + 1.5f, btnX + btnW - 2.0f, btnY + 1.5f, 1.0f);

    g.setColour (juce::Colour (0xff141416));
    g.drawRoundedRectangle (btnX, btnY, btnW, btnH, 3.0f, 1.0f);

    const float ledR = 4.2f;
    const float ledCx = btnX + btnW + 8.5f;
    const float ledCy = bounds.getCentreY();

    g.setColour (juce::Colour (0xff3a352f));
    g.fillEllipse (ledCx - ledR - 1.5f, ledCy - ledR - 1.5f, (ledR + 1.5f) * 2.0f, (ledR + 1.5f) * 2.0f);

    if (isOn)
    {
        g.setColour (juce::Colour (0xffff2424));
        g.fillEllipse (ledCx - ledR, ledCy - ledR, ledR * 2.0f, ledR * 2.0f);
        g.setColour (juce::Colour (0xffff9e9e));
        g.fillEllipse (ledCx - ledR * 0.45f, ledCy - ledR * 0.55f, ledR * 0.48f, ledR * 0.45f);
    }
    else
    {
        g.setColour (juce::Colour (0xff381010));
        g.fillEllipse (ledCx - ledR, ledCy - ledR, ledR * 2.0f, ledR * 2.0f);
    }

    auto textRect = bounds.withTrimmedLeft (ledCx + ledR + 5.0f);
    g.setColour (juce::Colour (0xfff5f2eb));
    g.setFont (juce::Font (9.8f, juce::Font::bold));
    g.drawFittedText (txt, textRect.toNearestInt(), juce::Justification::centredLeft, 1);
}

MiniJJMoogAudioProcessorEditor::MiniJJMoogAudioProcessorEditor (MiniJJMoogAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setLookAndFeel (&miniMoonLookAndFeel);
    buildCachedTextures();

    const juce::StringArray rangeChoices { "LO", "32'", "16'", "8'", "4'", "2'" };
    const juce::StringArray waveChoices  { "Tri", "Tri/Saw", "Saw", "Square", "Wide Pulse", "Narrow Pulse" };
    const juce::StringArray noiseChoices { "White", "Pink" };
    const juce::StringArray polyChoices  { "Mono", "Poly 4", "Poly 8", "Poly 16" };

    // CONTROLLERS
    setupKnob   (tuneSlider, tuneLabel, "TUNE");
    setupKnob   (glideSlider, glideLabel, "GLIDE");
    setupKnob   (pbRangeSlider, pbRangeLabel, "PB RNG");
    setupKnob   (modMixSlider, modMixLabel, "MOD MIX");
    setupKnob   (lfoRateSlider, lfoRateLabel, "LFO RATE");
    setupCombo  (voiceModeBox, voiceModeLabel, "POLY MODE", polyChoices);
    setupToggle (oscModBtn, "OSC MOD", MiniMoonLookAndFeel::SwitchAccent::yellow);
    setupToggle (osc3CtrlBtn, "OSC 3 CTRL", MiniMoonLookAndFeel::SwitchAccent::blue);
    setupToggle (legatoBtn, "LEGATO", MiniMoonLookAndFeel::SwitchAccent::red);
    setupToggle (retriggerBtn, "RETRIG", MiniMoonLookAndFeel::SwitchAccent::yellow);

    // OSCILLATOR BANK
    setupCombo (osc1RangeBox, osc1RangeLabel, "RNG 1", rangeChoices);
    setupKnob  (osc1FreqSlider, osc1FreqLabel, "FREQ 1");
    setupCombo (osc1WaveBox, osc1WaveLabel, "WAVE 1", waveChoices);

    setupCombo (osc2RangeBox, osc2RangeLabel, "RNG 2", rangeChoices);
    setupKnob  (osc2FreqSlider, osc2FreqLabel, "FREQ 2");
    setupCombo (osc2WaveBox, osc2WaveLabel, "WAVE 2", waveChoices);

    setupCombo (osc3RangeBox, osc3RangeLabel, "RNG 3", rangeChoices);
    setupKnob  (osc3FreqSlider, osc3FreqLabel, "FREQ 3");
    setupCombo (osc3WaveBox, osc3WaveLabel, "WAVE 3", waveChoices);

    // MIXER
    setupKnob  (osc1VolSlider, osc1VolLabel, "OSC 1");
    setupKnob  (osc2VolSlider, osc2VolLabel, "OSC 2");
    setupKnob  (osc3VolSlider, osc3VolLabel, "OSC 3");
    setupKnob  (noiseVolSlider, noiseVolLabel, "NOISE");
    setupCombo (noiseColorBox, noiseColorLabel, "NOISE", noiseChoices);
    setupKnob  (extVolSlider, extVolLabel, "EXT IN");
    setupKnob  (overloadSlider, overloadLabel, "OVERLOAD");

    // MODIFIERS
    setupToggle (filterModBtn, "FLT MOD", MiniMoonLookAndFeel::SwitchAccent::yellow);

    kbTrackLabel.setText ("KB TRACKING", juce::dontSendNotification);
    kbTrackLabel.setJustificationType (juce::Justification::centred);
    kbTrackLabel.setColour (juce::Label::textColourId, juce::Colour (0xfff5f2eb));
    kbTrackLabel.setFont (juce::Font (9.2f, juce::Font::bold));
    addAndMakeVisible (kbTrackLabel);

    setupToggle (kbTrk1Btn, "1/3",  MiniMoonLookAndFeel::SwitchAccent::blue);
    setupToggle (kbTrk2Btn, "2/3",  MiniMoonLookAndFeel::SwitchAccent::blue);
    setupToggle (kbTrk3Btn, "FULL", MiniMoonLookAndFeel::SwitchAccent::red);

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
    setupToggle (mainOutBtn, "MAIN OUT", MiniMoonLookAndFeel::SwitchAccent::red);

    auto& tree = audioProcessor.apvts;
    tuneAtt       = std::make_unique<SliderAttachment> (tree, "tune", tuneSlider);
    glideAtt      = std::make_unique<SliderAttachment> (tree, "glide", glideSlider);
    pbRangeAtt    = std::make_unique<SliderAttachment> (tree, "pitch_bend_range", pbRangeSlider);
    modMixAtt     = std::make_unique<SliderAttachment> (tree, "mod_mix", modMixSlider);
    lfoRateAtt    = std::make_unique<SliderAttachment> (tree, "lfo_rate", lfoRateSlider);
    voiceModeAtt  = std::make_unique<ComboAttachment>  (tree, "voice_mode", voiceModeBox);
    oscModAtt     = std::make_unique<ButtonAttachment> (tree, "osc_mod_on", oscModBtn);
    osc3CtrlAtt   = std::make_unique<ButtonAttachment> (tree, "osc3_ctrl_on", osc3CtrlBtn);
    legatoAtt     = std::make_unique<ButtonAttachment> (tree, "legato_on", legatoBtn);
    retriggerAtt  = std::make_unique<ButtonAttachment> (tree, "retrigger_on", retriggerBtn);

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
    kbTrk1Att     = std::make_unique<ButtonAttachment> (tree, "kb_trk_1", kbTrk1Btn);
    kbTrk2Att     = std::make_unique<ButtonAttachment> (tree, "kb_trk_2", kbTrk2Btn);
    kbTrk3Att     = std::make_unique<ButtonAttachment> (tree, "kb_trk_3", kbTrk3Btn);

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

    setSize (1180, 440);
}

MiniJJMoogAudioProcessorEditor::~MiniJJMoogAudioProcessorEditor()
{
    setLookAndFeel (nullptr);
}

void MiniJJMoogAudioProcessorEditor::buildCachedTextures()
{
    const int texW = 256;
    const int texH = 256;

    // Moldura em madeira nogueira com veios
    cachedWoodTexture = juce::Image (juce::Image::RGB, texW, texH, true);
    juce::Graphics wg (cachedWoodTexture);
    juce::ColourGradient baseWood (juce::Colour (0xffb0682a), 0.0f, 0.0f,
                                   juce::Colour (0xff784215), 0.0f, static_cast<float> (texH), false);
    wg.setGradientFill (baseWood);
    wg.fillAll();

    juce::Random woodRng (42);
    for (int i = 0; i < 65; ++i)
    {
        const float y = woodRng.nextFloat() * static_cast<float> (texH);
        const float alpha = 0.06f + woodRng.nextFloat() * 0.10f;
        const bool darkGrain = woodRng.nextBool();
        wg.setColour ((darkGrain ? juce::Colour (0xff3d1e08) : juce::Colour (0xffe09850)).withAlpha (alpha));
        wg.drawLine (0.0f, y, static_cast<float> (texW), y + (woodRng.nextFloat() - 0.5f) * 3.0f, 1.1f);
    }

    // Painel preto clássico sólido
    cachedMetalTexture = juce::Image (juce::Image::RGB, texW, texH, true);
    juce::Graphics mg (cachedMetalTexture);
    mg.fillAll (juce::Colour (0xff101113));
}

void MiniJJMoogAudioProcessorEditor::setupKnob (juce::Slider& slider, juce::Label& label, const juce::String& text)
{
    slider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 48, 14);
    slider.setColour (juce::Slider::textBoxTextColourId, juce::Colour (0xfff5f2eb));
    slider.setColour (juce::Slider::textBoxBackgroundColourId, juce::Colour (0xff191a1d));
    slider.setColour (juce::Slider::textBoxOutlineColourId, juce::Colour (0xff787e86));
    addAndMakeVisible (slider);

    label.setText (text, juce::dontSendNotification);
    label.setJustificationType (juce::Justification::centred);
    label.setColour (juce::Label::textColourId, juce::Colour (0xfff5f2eb));
    label.setFont (juce::Font (10.0f, juce::Font::bold));
    addAndMakeVisible (label);
}

void MiniJJMoogAudioProcessorEditor::setupCombo (juce::ComboBox& box, juce::Label& label, const juce::String& text, const juce::StringArray& items)
{
    box.addItemList (items, 1);
    box.setColour (juce::ComboBox::backgroundColourId, juce::Colour (0xff1a1b1e));
    box.setColour (juce::ComboBox::textColourId, juce::Colour (0xfff5f2eb));
    box.setColour (juce::ComboBox::outlineColourId, juce::Colour (0xff888d94));
    addAndMakeVisible (box);

    label.setText (text, juce::dontSendNotification);
    label.setJustificationType (juce::Justification::centred);
    label.setColour (juce::Label::textColourId, juce::Colour (0xfff5f2eb));
    label.setFont (juce::Font (10.0f, juce::Font::bold));
    addAndMakeVisible (label);
}

void MiniJJMoogAudioProcessorEditor::setupToggle (juce::ToggleButton& btn, const juce::String& text, MiniMoonLookAndFeel::SwitchAccent accent)
{
    btn.setButtonText (text);
    miniMoonLookAndFeel.setSwitchAccent (&btn, accent);
    addAndMakeVisible (btn);
}

void MiniJJMoogAudioProcessorEditor::paint (juce::Graphics& g)
{
    if (cachedWoodTexture.isValid())
    {
        for (int ty = 0; ty < getHeight(); ty += cachedWoodTexture.getHeight())
            for (int tx = 0; tx < getWidth(); tx += cachedWoodTexture.getWidth())
                g.drawImageAt (cachedWoodTexture, tx, ty);
    }
    else
    {
        g.fillAll (juce::Colour (0xff9b5922));
    }

    auto panel = getLocalBounds().reduced (14, 12).toFloat();

    g.setColour (juce::Colour (0xff101113));
    g.fillRoundedRectangle (panel, 5.0f);

    g.setColour (juce::Colour (0xffb2b7be));
    g.drawRoundedRectangle (panel, 5.0f, 1.5f);

    const float screwX[2] = { panel.getX() + 9.0f, panel.getRight() - 9.0f };
    const float screwY[2] = { panel.getY() + 9.0f, panel.getBottom() - 9.0f };
    for (float sx : screwX)
    {
        for (float sy : screwY)
        {
            g.setColour (juce::Colour (0xffaeb4bc));
            g.fillEllipse (sx - 3.5f, sy - 3.5f, 7.0f, 7.0f);
            g.setColour (juce::Colour (0xff1b1b1d));
            g.drawLine (sx - 2.0f, sy, sx + 2.0f, sy, 1.0f);
        }
    }

    const float weights[5] = { 0.17f, 0.21f, 0.20f, 0.27f, 0.15f };
    float colXArr[5];
    float colWArr[5];
    float accX = panel.getX();
    for (int i = 0; i < 5; ++i)
    {
        colXArr[i] = accX;
        colWArr[i] = panel.getWidth() * weights[i];
        accX += colWArr[i];
    }

    const float secStartY = panel.getY() + 24.0f;
    const float secHeight = panel.getHeight() - 28.0f;
    const juce::StringArray secTitles { "CONTROLLERS", "OSCILLATOR BANK", "MIXER", "MODIFIERS", "OUTPUT" };

    for (int i = 0; i < 5; ++i)
    {
        if (i > 0)
        {
            g.setColour (juce::Colour (0xff484a4f));
            g.drawLine (colXArr[i], secStartY + 2.0f, colXArr[i], secStartY + secHeight - 4.0f, 1.0f);
        }
        g.setColour (juce::Colour (0xfff5f2eb));
        g.setFont (juce::Font (10.5f, juce::Font::bold));
        g.drawText (secTitles[i],
                    juce::roundToInt (colXArr[i] + 4.0f),
                    juce::roundToInt (panel.getY() + 4.0f),
                    juce::roundToInt (colWArr[i] - 8.0f),
                    16,
                    juce::Justification::centred);
    }

    const float outX = colXArr[4];
    const float outW = colWArr[4];
    const float badgeW = std::max (96.0f, outW - 22.0f);
    const float badgeH = 54.0f;
    const float badgeX = outX + (outW - badgeW) * 0.5f;
    const float badgeY = panel.getBottom() - badgeH - 12.0f;

    juce::ColourGradient brushedGrad (juce::Colour (0xffe6eaee), badgeX, badgeY,
                                      juce::Colour (0xff9da3ab), badgeX, badgeY + badgeH, false);
    g.setGradientFill (brushedGrad);
    g.fillRect (badgeX, badgeY, badgeW, badgeH);

    g.setColour (juce::Colour (0x35ffffff));
    for (float yLine = badgeY + 3.0f; yLine < badgeY + badgeH - 2.0f; yLine += 3.0f)
        g.drawLine (badgeX + 2.0f, yLine, badgeX + badgeW - 2.0f, yLine, 0.6f);

    g.setColour (juce::Colour (0xff141416));
    g.drawRect (badgeX, badgeY, badgeW, badgeH, 1.2f);

    g.setColour (juce::Colour (0xff0e0e10));
    g.setFont (juce::Font (17.5f, juce::Font::plain));
    const float halfBadgeH = badgeH * 0.5f;
    g.drawText ("Mini",
                juce::roundToInt (badgeX),
                juce::roundToInt (badgeY + 1.0f),
                juce::roundToInt (badgeW),
                juce::roundToInt (halfBadgeH),
                juce::Justification::centred);
    g.drawText ("Moon",
                juce::roundToInt (badgeX),
                juce::roundToInt (badgeY + halfBadgeH - 1.0f),
                juce::roundToInt (badgeW),
                juce::roundToInt (halfBadgeH),
                juce::Justification::centred);
}

void MiniJJMoogAudioProcessorEditor::resized()
{
    const auto panel = getLocalBounds().reduced (14, 12);
    const int topBar = 24;
    const int secH = panel.getHeight() - topBar - 4;

    const float weights[5] = { 0.17f, 0.21f, 0.20f, 0.27f, 0.15f };
    int colX[5];
    int colW[5];
    float accX = static_cast<float> (panel.getX());
    for (int i = 0; i < 5; ++i)
    {
        colX[i] = juce::roundToInt (accX);
        colW[i] = juce::roundToInt (static_cast<float> (panel.getWidth()) * weights[i]);
        accX += static_cast<float> (panel.getWidth()) * weights[i];
    }

    auto placeKnob = [] (juce::Slider& s, juce::Label& l, int x, int y, int w, int h)
    {
        l.setBounds (x + 2, y, std::max (16, w - 4), 14);
        s.setBounds (x + 2, y + 14, std::max (16, w - 4), std::max (24, h - 14));
    };

    auto placeCombo = [] (juce::ComboBox& b, juce::Label& l, int x, int y, int w, int h)
    {
        juce::ignoreUnused (h);
        l.setBounds (x + 2, y, std::max (16, w - 4), 14);
        b.setBounds (x + 4, y + 15, std::max (30, w - 8), 22);
    };

    // 1. CONTROLLERS
    {
        const int x0 = colX[0];
        const int w0 = colW[0];
        const int y0 = panel.getY() + topBar + 2;
        const int kw = w0 / 2;
        const int kh = 68;

        placeKnob  (tuneSlider, tuneLabel, x0, y0, kw, kh);
        placeKnob  (glideSlider, glideLabel, x0 + kw, y0, kw, kh);
        placeKnob  (pbRangeSlider, pbRangeLabel, x0, y0 + kh, kw, kh);
        placeKnob  (modMixSlider, modMixLabel, x0 + kw, y0 + kh, kw, kh);
        placeKnob  (lfoRateSlider, lfoRateLabel, x0, y0 + kh * 2, kw, kh);
        placeCombo (voiceModeBox, voiceModeLabel, x0 + kw, y0 + kh * 2 + 4, kw, 38);

        const int by = y0 + kh * 3 + 4;
        oscModBtn.setBounds    (x0 + 6, by, kw - 6, 19);
        osc3CtrlBtn.setBounds  (x0 + kw + 2, by, kw - 4, 19);
        legatoBtn.setBounds    (x0 + 6, by + 20, kw - 6, 19);
        retriggerBtn.setBounds (x0 + kw + 2, by + 20, kw - 4, 19);
    }

    // 2. OSCILLATOR BANK
    {
        const int x0 = colX[1];
        const int w0 = colW[1];
        const int y0 = panel.getY() + topBar;
        const int rowH = secH / 3;
        const int c1 = static_cast<int> (static_cast<float> (w0) * 0.25f);
        const int c2 = static_cast<int> (static_cast<float> (w0) * 0.32f);
        const int c3 = w0 - c1 - c2;

        placeCombo (osc1RangeBox, osc1RangeLabel, x0, y0, c1, rowH);
        placeKnob  (osc1FreqSlider, osc1FreqLabel, x0 + c1, y0, c2, rowH);
        placeCombo (osc1WaveBox, osc1WaveLabel, x0 + c1 + c2, y0, c3, rowH);

        placeCombo (osc2RangeBox, osc2RangeLabel, x0, y0 + rowH, c1, rowH);
        placeKnob  (osc2FreqSlider, osc2FreqLabel, x0 + c1, y0 + rowH, c2, rowH);
        placeCombo (osc2WaveBox, osc2WaveLabel, x0 + c1 + c2, y0 + rowH, c3, rowH);

        placeCombo (osc3RangeBox, osc3RangeLabel, x0, y0 + rowH * 2, c1, rowH);
        placeKnob  (osc3FreqSlider, osc3FreqLabel, x0 + c1, y0 + rowH * 2, c2, rowH);
        placeCombo (osc3WaveBox, osc3WaveLabel, x0 + c1 + c2, y0 + rowH * 2, c3, rowH);
    }

    // 3. MIXER
    {
        const int x0 = colX[2];
        const int w0 = colW[2];
        const int y0 = panel.getY() + topBar;
        const int kw = w0 / 2;
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
        const int x0 = colX[3];
        const int w0 = colW[3];
        const int y0 = panel.getY() + topBar;
        const int kw = w0 / 3;
        const int kh = 84;

        filterModBtn.setBounds (x0 + 6, y0 + 4, kw - 6, 19);
        kbTrackLabel.setBounds (x0 + 2, y0 + 25, kw - 4, 12);

        const int subW = std::max (20, (kw - 6) / 3);
        const int subH = 34;
        kbTrk1Btn.setBounds (x0 + 3,            y0 + 38, subW, subH);
        kbTrk2Btn.setBounds (x0 + 3 + subW,     y0 + 38, subW, subH);
        kbTrk3Btn.setBounds (x0 + 3 + subW * 2, y0 + 38, subW, subH);

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
        const int x0 = colX[4];
        const int w0 = colW[4];
        const int y0 = panel.getY() + topBar;

        placeKnob   (masterVolSlider, masterVolLabel, x0 + 6, y0 + 8, w0 - 12, 98);
        mainOutBtn.setBounds (x0 + 8, y0 + 114, w0 - 12, 24);
    }
}
