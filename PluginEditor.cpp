#include "PluginProcessor.h"
#include "PluginEditor.h"

MiniJJMoogAudioProcessorEditor::MiniJJMoogAudioProcessorEditor (MiniJJMoogAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    const juce::StringArray rangeChoices { "LO", "32'", "16'", "8'", "4'", "2'" };
    const juce::StringArray waveChoices  { "Tri", "Tri/Saw", "Saw", "Square", "Wide Pulse", "Narrow Pulse" };
    const juce::StringArray noiseChoices { "White", "Pink" };

    // CONTROLLERS
    setupKnob (tuneSlider, tuneLabel, "TUNE");
    setupKnob (glideSlider, glideLabel, "GLIDE");
    setupKnob (modMixSlider, modMixLabel, "MOD MIX");
    setupToggle (oscModBtn, "OSC MOD");
    setupToggle (osc3CtrlBtn, "OSC 3 CTRL");

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

    // Attachments
    auto& tree = audioProcessor.apvts;
    tuneAtt       = std::make_unique<SliderAttachment> (tree, "tune", tuneSlider);
    glideAtt      = std::make_unique<SliderAttachment> (tree, "glide", glideSlider);
    modMixAtt     = std::make_unique<SliderAttachment> (tree, "mod_mix", modMixSlider);
    oscModAtt     = std::make_unique<ButtonAttachment> (tree, "osc_mod_on", oscModBtn);
    osc3CtrlAtt   = std::make_unique<ButtonAttachment> (tree, "osc3_ctrl_on", osc3CtrlBtn);

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

    setSize (980, 440);
}

MiniJJMoogAudioProcessorEditor::~MiniJJMoogAudioProcessorEditor() = default;

void MiniJJMoogAudioProcessorEditor::setupKnob (juce::Slider& slider, juce::Label& label, const juce::String& text)
{
    slider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 52, 14);
    slider.setColour (juce::Slider::rotarySliderFillColourId, juce::Colour (0xffd6b36a));
    slider.setColour (juce::Slider::rotarySliderOutlineColourId, juce::Colour (0xff4a4742));
    slider.setColour (juce::Slider::thumbColourId, juce::Colours::white);
    slider.setColour (juce::Slider::textBoxTextColourId, juce::Colour (0xffe8e2d5));
    slider.setColour (juce::Slider::textBoxBackgroundColourId, juce::Colour (0xff232220));
    addAndMakeVisible (slider);

    label.setText (text, juce::dontSendNotification);
    label.setJustificationType (juce::Justification::centred);
    label.setColour (juce::Label::textColourId, juce::Colour (0xffe8e2d5));
    label.setFont (juce::Font (10.5f, juce::Font::bold));
    addAndMakeVisible (label);
}

void MiniJJMoogAudioProcessorEditor::setupCombo (juce::ComboBox& box, juce::Label& label, const juce::String& text, const juce::StringArray& items)
{
    box.addItemList (items, 1);
    box.setColour (juce::ComboBox::backgroundColourId, juce::Colour (0xff262522));
    box.setColour (juce::ComboBox::textColourId, juce::Colour (0xffe8e2d5));
    box.setColour (juce::ComboBox::outlineColourId, juce::Colour (0xff7a7468));
    addAndMakeVisible (box);

    label.setText (text, juce::dontSendNotification);
    label.setJustificationType (juce::Justification::centred);
    label.setColour (juce::Label::textColourId, juce::Colour (0xffe8e2d5));
    label.setFont (juce::Font (10.0f, juce::Font::bold));
    addAndMakeVisible (label);
}

void MiniJJMoogAudioProcessorEditor::setupToggle (juce::ToggleButton& btn, const juce::String& text)
{
    btn.setButtonText (text);
    btn.setColour (juce::ToggleButton::textColourId, juce::Colour (0xffd6b36a));
    btn.setColour (juce::ToggleButton::tickColourId, juce::Colour (0xffd6b36a));
    addAndMakeVisible (btn);
}

void MiniJJMoogAudioProcessorEditor::paint (juce::Graphics& g)
{
    // Gabinete estilo madeira clássica nas bordas
    g.fillAll (juce::Colour (0xff5c361a));

    auto panel = getLocalBounds().reduced (14, 14).toFloat();
    g.setColour (juce::Colour (0xff151517));
    g.fillRoundedRectangle (panel, 6.0f);

    g.setColour (juce::Colour (0xff8c7d5b));
    g.drawRoundedRectangle (panel, 6.0f, 1.5f);

    // Cabeçalho clássico
    auto header = panel.removeFromTop (28.0f);
    g.setColour (juce::Colour (0xffd6b36a));
    g.setFont (juce::Font (16.0f, juce::Font::bold));
    g.drawText ("MINI JJ MOOG  —  ANALOG SYNTHESIZER", header.reduced (12, 0), juce::Justification::centredLeft);

    // Divisórias de seções
    const float secW = panel.getWidth() / 5.0f;
    const juce::StringArray secTitles { "CONTROLLERS", "OSCILLATOR BANK", "MIXER", "MODIFIERS", "OUTPUT" };

    for (int i = 0; i < 5; ++i)
    {
        juce::Rectangle<float> col (panel.getX() + i * secW, panel.getY(), secW, panel.getHeight());
        if (i > 0)
        {
            g.setColour (juce::Colour (0xff3c3a36));
            g.drawLine (col.getX(), col.getY() + 4.0f, col.getX(), col.getBottom() - 4.0f, 1.0f);
        }
        g.setColour (juce::Colour (0xffb8a472));
        g.setFont (juce::Font (11.0f, juce::Font::bold));
        g.drawText (secTitles[i], col.getX() + 4.0f, col.getY() + 2.0f, secW - 8.0f, 16.0f, juce::Justification::centred);
    }
}

void MiniJJMoogAudioProcessorEditor::resized()
{
    auto panel = getLocalBounds().reduced (14, 14);
    const int topBar = 22;
    const int secW = panel.getWidth() / 5;
    const int secH = panel.getHeight() - topBar;

    auto placeKnob = [] (juce::Slider& s, juce::Label& l, int x, int y, int w, int h)
    {
        l.setBounds (x, y, w, 14);
        s.setBounds (x, y + 14, w, h - 14);
    };

    auto placeCombo = [] (juce::ComboBox& b, juce::Label& l, int x, int y, int w, int h)
    {
        l.setBounds (x, y, w, 14);
        b.setBounds (x + 4, y + 14, w - 8, 22);
    };

    // 1. CONTROLLERS
    {
        int x0 = panel.getX();
        int y0 = panel.getY() + topBar;
        int kw = secW / 2;
        int kh = 92;
        placeKnob (tuneSlider, tuneLabel, x0, y0, kw, kh);
        placeKnob (glideSlider, glideLabel, x0 + kw, y0, kw, kh);
        placeKnob (modMixSlider, modMixLabel, x0, y0 + kh, kw, kh);
        oscModBtn.setBounds (x0 + kw + 8, y0 + kh + 18, kw - 12, 24);
        osc3CtrlBtn.setBounds (x0 + kw + 8, y0 + kh + 48, kw - 12, 24);
    }

    // 2. OSCILLATOR BANK (3 linhas: Osc 1, Osc 2, Osc 3)
    {
        int x0 = panel.getX() + secW;
        int y0 = panel.getY() + topBar;
        int rowH = secH / 3;
        int c1 = static_cast<int> (secW * 0.30f);
        int c2 = static_cast<int> (secW * 0.32f);
        int c3 = secW - c1 - c2;

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
        int x0 = panel.getX() + secW * 2;
        int y0 = panel.getY() + topBar;
        int kw = secW / 2;
        int kh = 88;
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
        int x0 = panel.getX() + secW * 3;
        int y0 = panel.getY() + topBar;
        int kw = secW / 3;
        int kh = 88;

        filterModBtn.setBounds (x0 + 6, y0 + 10, kw - 8, 24);
        kbCtrlBtn.setBounds (x0 + 6, y0 + 38, kw - 8, 24);
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
        int x0 = panel.getX() + secW * 4;
        int y0 = panel.getY() + topBar;
        placeKnob   (masterVolSlider, masterVolLabel, x0 + 16, y0 + 12, secW - 32, 110);
        mainOutBtn.setBounds (x0 + 24, y0 + 135, secW - 48, 28);
    }
}
