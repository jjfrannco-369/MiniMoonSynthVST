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
    const float skirtR = std::max (10.0f, std::min (w, h) * 0.42f);
    const float cx = static_cast<float> (x) + w * 0.5f;
    const float cy = static_cast<float> (y) + h * 0.5f;

    // 1. Marcas de escala radiais em branco nítido
    const int numTicks = 11;
    for (int i = 0; i < numTicks; ++i)
    {
        const float t = static_cast<float> (i) / static_cast<float> (numTicks - 1);
        const float ang = rotaryStartAngle + t * (rotaryEndAngle - rotaryStartAngle) - juce::MathConstants<float>::halfPi;
        const float r1 = skirtR + 2.0f;
        const float r2 = skirtR + 5.2f;
        g.setColour (juce::Colour (0xfff2efe9));
        g.drawLine (cx + std::cos (ang) * r1,
                    cy + std::sin (ang) * r1,
                    cx + std::cos (ang) * r2,
                    cy + std::sin (ang) * r2, 1.15f);
    }

    // 2. Sombra projetada 3D
    g.setColour (juce::Colour (0xcc030304));
    g.fillEllipse (cx - skirtR + 2.5f, cy - skirtR + 3.5f, skirtR * 2.0f, skirtR * 2.0f);

    // 3. Saia circular preta brilhante (skirt)
    juce::ColourGradient skirtGrad (juce::Colour (0xff2b2d30), cx - skirtR * 0.3f, cy - skirtR * 0.4f,
                                    juce::Colour (0xff09090b), cx + skirtR * 0.4f, cy + skirtR * 0.5f, false);
    g.setGradientFill (skirtGrad);
    g.fillEllipse (cx - skirtR, cy - skirtR, skirtR * 2.0f, skirtR * 2.0f);

    // Reflexo especular fino na borda da saia
    g.setColour (juce::Colour (0xff555960));
    g.drawEllipse (cx - skirtR, cy - skirtR, skirtR * 2.0f, skirtR * 2.0f, 1.0f);

    const float angle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle)
                        - juce::MathConstants<float>::halfPi;

    // 4. Corpo canelado de 8 pontas em baquelite preta (gira junto com o knob)
    const float bodyOuterR = skirtR * 0.80f;
    const float bodyInnerR = skirtR * 0.72f;
    juce::Path flutedBody;
    const int numPoints = 64;
    for (int i = 0; i <= numPoints; ++i)
    {
        const float theta = angle + (static_cast<float> (i) / static_cast<float> (numPoints)) * juce::MathConstants<float>::twoPi;
        const float wave = 0.5f + 0.5f * std::cos (8.0f * (theta - angle));
        const float r = bodyInnerR + (bodyOuterR - bodyInnerR) * wave;
        const float px = cx + std::cos (theta) * r;
        const float py = cy + std::sin (theta) * r;
        if (i == 0)
            flutedBody.startNewSubPath (px, py);
        else
            flutedBody.lineTo (px, py);
    }
    flutedBody.closeSubPath();

    juce::ColourGradient bodyGrad (juce::Colour (0xff34373c), cx - bodyOuterR * 0.4f, cy - bodyOuterR * 0.5f,
                                   juce::Colour (0xff070708), cx + bodyOuterR * 0.4f, cy + bodyOuterR * 0.5f, false);
    g.setGradientFill (bodyGrad);
    g.fillPath (flutedBody);

    g.setColour (juce::Colour (0x55ffffff));
    g.strokePath (flutedBody, juce::PathStrokeType (0.9f));

    // 5. Disco central de alumínio torneado com sulcos concêntricos e reflexo cônico em ampulheta
    const float capR = skirtR * 0.58f;
    juce::ColourGradient baseMetal (juce::Colour (0xffb8bec6), cx - capR, cy - capR,
                                    juce::Colour (0xff7e848d), cx + capR, cy + capR, false);
    g.setGradientFill (baseMetal);
    g.fillEllipse (cx - capR, cy - capR, capR * 2.0f, capR * 2.0f);

    // Reflexo cônico (duas cunhas claras opostas e duas escuras, criando o efeito metálico da foto)
    g.saveState();
    juce::Path capClip;
    capClip.addEllipse (cx - capR, cy - capR, capR * 2.0f, capR * 2.0f);
    g.reduceClipRegion (capClip);

    for (int lobe = 0; lobe < 2; ++lobe)
    {
        const float baseAng = -0.65f + static_cast<float> (lobe) * juce::MathConstants<float>::pi;
        juce::Path lightCone;
        lightCone.startNewSubPath (cx, cy);
        lightCone.lineTo (cx + std::cos (baseAng - 0.38f) * capR * 1.4f, cy + std::sin (baseAng - 0.38f) * capR * 1.4f);
        lightCone.lineTo (cx + std::cos (baseAng + 0.38f) * capR * 1.4f, cy + std::sin (baseAng + 0.38f) * capR * 1.4f);
        lightCone.closeSubPath();
        g.setColour (juce::Colour (0x75ffffff));
        g.fillPath (lightCone);

        const float darkAng = baseAng + juce::MathConstants<float>::halfPi;
        juce::Path darkCone;
        darkCone.startNewSubPath (cx, cy);
        darkCone.lineTo (cx + std::cos (darkAng - 0.34f) * capR * 1.4f, cy + std::sin (darkAng - 0.34f) * capR * 1.4f);
        darkCone.lineTo (cx + std::cos (darkAng + 0.34f) * capR * 1.4f, cy + std::sin (darkAng + 0.34f) * capR * 1.4f);
        darkCone.closeSubPath();
        g.setColour (juce::Colour (0x55121417));
        g.fillPath (darkCone);
    }

    // Sulcos concêntricos finos (torneamento)
    for (float rRing = 2.2f; rRing < capR - 1.0f; rRing += 2.1f)
    {
        g.setColour (juce::Colour (0x26000000));
        g.drawEllipse (cx - rRing, cy - rRing, rRing * 2.0f, rRing * 2.0f, 0.65f);
    }
    g.restoreState();

    // Borda chanfrada escura ao redor do disco de alumínio
    g.setColour (juce::Colour (0xff141518));
    g.drawEllipse (cx - capR, cy - capR, capR * 2.0f, capR * 2.0f, 1.2f);

    // 6. Ponto indicador branco rebaixado na aba preta (igual ao knob Cosmo da foto)
    const float dotDist = skirtR * 0.68f;
    const float dotR = std::max (2.1f, skirtR * 0.105f);
    const float dx = cx + std::cos (angle) * dotDist;
    const float dy = cy + std::sin (angle) * dotDist;

    g.setColour (juce::Colour (0xff050506));
    g.fillEllipse (dx - dotR - 0.7f, dy - dotR - 0.7f, (dotR + 0.7f) * 2.0f, (dotR + 0.7f) * 2.0f);
    g.setColour (juce::Colour (0xfff7f6f2));
    g.fillEllipse (dx - dotR, dy - dotR, dotR * 2.0f, dotR * 2.0f);

    // Ponto menor na saia externa alinhado ao ponteiro
    const float skirtDotDist = skirtR * 0.89f;
    const float skirtDotR = std::max (1.4f, skirtR * 0.065f);
    const float sdx = cx + std::cos (angle) * skirtDotDist;
    const float sdy = cy + std::sin (angle) * skirtDotDist;
    g.setColour (juce::Colour (0xffe8e6e0));
    g.fillEllipse (sdx - skirtDotR, sdy - skirtDotR, skirtDotR * 2.0f, skirtDotR * 2.0f);
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

        g.setColour (juce::Colour (0xff050506));
        g.fillRoundedRectangle (btnX - 1.0f, btnY - 1.0f, btnW + 2.0f, btnH + 2.0f, 3.0f);

        g.setColour (isOn ? activeCol : inactiveCol);
        g.fillRoundedRectangle (btnX, btnY, btnW, btnH, 2.5f);
        g.setColour (juce::Colour (0x55ffffff));
        g.drawLine (btnX + 1.5f, btnY + 1.2f, btnX + btnW - 1.5f, btnY + 1.2f, 0.9f);

        g.setColour (juce::Colour (0xff101012));
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

    g.setColour (juce::Colour (0xff050506));
    g.fillRoundedRectangle (btnX - 1.2f, btnY - 1.2f, btnW + 2.4f, btnH + 2.4f, 3.5f);

    g.setColour (isOn ? activeCol : inactiveCol);
    g.fillRoundedRectangle (btnX, btnY, btnW, btnH, 3.0f);

    g.set
