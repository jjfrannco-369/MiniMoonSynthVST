#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <cmath>
#include <algorithm>

MiniJJMoogAudioProcessor::MiniJJMoogAudioProcessor()
    : AudioProcessor (BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
                      .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
#endif
                      .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
#endif
                      ),
      apvts (*this, nullptr, "Parameters", createParameterLayout())
{
    heldNotes.reserve (16);
}

juce::AudioProcessorValueTreeState::ParameterLayout MiniJJMoogAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    const juce::StringArray rangeChoices { "LO", "32'", "16'", "8'", "4'", "2'" };
    const juce::StringArray waveChoices  { "Tri", "Tri/Saw", "Saw", "Square", "Wide Pulse", "Narrow Pulse" };
    const juce::StringArray noiseChoices { "White", "Pink" };
    const juce::StringArray polyChoices  { "Mono", "Poly 4", "Poly 8", "Poly 16" };

    // CONTROLLERS
    layout.add (std::make_unique<juce::AudioParameterFloat> (
        "tune", "Tune", juce::NormalisableRange<float> (-12.0f, 12.0f, 0.05f), 0.0f));
    layout.add (std::make_unique<juce::AudioParameterFloat> (
        "glide", "Glide", juce::NormalisableRange<float> (0.0f, 2.5f, 0.005f, 0.45f), 0.15f));
    layout.add (std::make_unique<juce::AudioParameterFloat> (
        "pitch_bend_range", "PB Range", juce::NormalisableRange<float> (1.0f, 24.0f, 1.0f), 12.0f));
    layout.add (std::make_unique<juce::AudioParameterFloat> (
        "mod_mix", "Mod Mix", juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.35f));
    layout.add (std::make_unique<juce::AudioParameterChoice> (
        "voice_mode", "Poly Mode", polyChoices, 0));
    layout.add (std::make_unique<juce::AudioParameterBool> (
        "osc_mod_on", "Osc Mod", true));
    layout.add (std::make_unique<juce::AudioParameterBool> (
        "osc3_ctrl_on", "Osc 3 Ctrl", false));
    layout.add (std::make_unique<juce::AudioParameterBool> (
        "legato_on", "Legato", true));

    // OSCILLATOR BANK
    layout.add (std::make_unique<juce::AudioParameterChoice> (
        "osc1_range", "Osc 1 Range", rangeChoices, 3));
    layout.add (std::make_unique<juce::AudioParameterFloat> (
        "osc1_freq", "Osc 1 Freq", juce::NormalisableRange<float> (-7.0f, 7.0f, 0.01f), 0.0f));
    layout.add (std::make_unique<juce::AudioParameterChoice> (
        "osc1_wave", "Osc 1 Wave", waveChoices, 2));

    layout.add (std::make_unique<juce::AudioParameterChoice> (
        "osc2_range", "Osc 2 Range", rangeChoices, 3));
    layout.add (std::make_unique<juce::AudioParameterFloat> (
        "osc2_freq", "Osc 2 Freq", juce::NormalisableRange<float> (-7.0f, 7.0f, 0.01f), 0.0f));
    layout.add (std::make_unique<juce::AudioParameterChoice> (
        "osc2_wave", "Osc 2 Wave", waveChoices, 2));

    layout.add (std::make_unique<juce::AudioParameterChoice> (
        "osc3_range", "Osc 3 Range", rangeChoices, 3));
    layout.add (std::make_unique<juce::AudioParameterFloat> (
        "osc3_freq", "Osc 3 Freq", juce::NormalisableRange<float> (-7.0f, 7.0f, 0.01f), 0.0f));
    layout.add (std::make_unique<juce::AudioParameterChoice> (
        "osc3_wave", "Osc 3 Wave", waveChoices, 3));

    // MIXER
    layout.add (std::make_unique<juce::AudioParameterFloat> (
        "osc1_vol", "Osc 1 Vol", juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.80f));
    layout.add (std::make_unique<juce::AudioParameterFloat> (
        "osc2_vol", "Osc 2 Vol", juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.0f));
    layout.add (std::make_unique<juce::AudioParameterFloat> (
        "osc3_vol", "Osc 3 Vol", juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.0f));
    layout.add (std::make_unique<juce::AudioParameterFloat> (
        "noise_vol", "Noise Vol", juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.0f));
    layout.add (std::make_unique<juce::AudioParameterChoice> (
        "noise_color", "Noise Type", noiseChoices, 0));
    layout.add (std::make_unique<juce::AudioParameterFloat> (
        "ext_vol", "Ext Input Vol", juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.0f));
    layout.add (std::make_unique<juce::AudioParameterFloat> (
        "overload", "Overload", juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.15f));

    // MODIFIERS
    layout.add (std::make_unique<juce::AudioParameterBool> (
        "filter_mod_on", "Filter Mod", false));
    layout.add (std::make_unique<juce::AudioParameterBool> (
        "kb_ctrl_on", "Keyboard Control", true));
    layout.add (std::make_unique<juce::AudioParameterFloat> (
        "cutoff", "Cutoff", juce::NormalisableRange<float> (30.0f, 14000.0f, 1.0f, 0.32f), 3200.0f));
    layout.add (std::make_unique<juce::AudioParameterFloat> (
        "emphasis", "Emphasis", juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.25f));
    layout.add (std::make_unique<juce::AudioParameterFloat> (
        "filter_contour", "Filter Contour", juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.35f));

    layout.add (std::make_unique<juce::AudioParameterFloat> (
        "filter_attack", "Filter Attack", juce::NormalisableRange<float> (0.001f, 2.0f, 0.001f, 0.4f), 0.01f));
    layout.add (std::make_unique<juce::AudioParameterFloat> (
        "filter_decay", "Filter Decay", juce::NormalisableRange<float> (0.005f, 4.0f, 0.001f, 0.4f), 0.35f));
    layout.add (std::make_unique<juce::AudioParameterFloat> (
        "filter_sustain", "Filter Sustain", juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.40f));

    layout.add (std::make_unique<juce::AudioParameterFloat> (
        "loud_attack", "Loud Attack", juce::NormalisableRange<float> (0.001f, 2.0f, 0.001f, 0.4f), 0.005f));
    layout.add (std::make_unique<juce::AudioParameterFloat> (
        "loud_decay", "Loud Decay", juce::NormalisableRange<float> (0.005f, 4.0f, 0.001f, 0.4f), 0.35f));
    layout.add (std::make_unique<juce::AudioParameterFloat> (
        "loud_sustain", "Loud Sustain", juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.80f));

    // OUTPUT
    layout.add (std::make_unique<juce::AudioParameterFloat> (
        "master_vol", "Volume", juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.70f));
    layout.add (std::make_unique<juce::AudioParameterBool> (
        "main_out_on", "Main Output", true));

    return layout;
}

int MiniJJMoogAudioProcessor::voiceModeToCount (int modeIndex)
{
    switch (modeIndex)
    {
        case 1: return 4;
        case 2: return 8;
        case 3: return 16;
        case 0:
        default: return 1;
    }
}

float MiniJJMoogAudioProcessor::rangeToOctaveMultiplier (int rangeIndex)
{
    switch (rangeIndex)
    {
        case 0: return 0.125f;
        case 1: return 0.25f;
        case 2: return 0.5f;
        case 3: return 1.0f;
        case 4: return 2.0f;
        case 5: return 4.0f;
        default: return 1.0f;
    }
}

float MiniJJMoogAudioProcessor::semitonesToRatio (float semitones)
{
    return std::pow (2.0f, semitones / 12.0f);
}

float MiniJJMoogAudioProcessor::renderWaveform (double normPhase, int waveIndex, float pulseWidth)
{
    const float p = static_cast<float> (normPhase - std::floor (normPhase));

    const float tri = 1.0f - 4.0f * std::abs (p - 0.5f);
    const float saw = 2.0f * p - 1.0f;
    const float sqr = p < 0.5f ? 1.0f : -1.0f;
    const float widePulse = p < 0.72f ? 1.0f : -1.0f;
    const float narrowPulse = p < pulseWidth ? 1.0f : -1.0f;

    switch (waveIndex)
    {
        case 0: return tri;
        case 1: return 0.5f * (tri + saw);
        case 2: return saw;
        case 3: return sqr;
        case 4: return widePulse;
        case 5: return narrowPulse;
        default: return saw;
    }
}

float MiniJJMoogAudioProcessor::secondsToCoeff (float seconds, double sampleRate)
{
    const double clamped = std::max (0.0005, static_cast<double> (seconds));
    return static_cast<float> (1.0 - std::exp (-1.0 / (clamped * sampleRate)));
}

void MiniJJMoogAudioProcessor::triggerPolyNoteOn (int noteNumber, float vel, int activeVoiceCount, bool legatoMode)
{
    int chosenIdx = -1;

    // Reutiliza voz ja tocando a mesma nota se houver
    for (int i = 0; i < activeVoiceCount; ++i)
    {
        if (voices[i].active && voices[i].midiNote == noteNumber)
        {
            chosenIdx = i;
            break;
        }
    }

    // Senao, pega voz inativa
    if (chosenIdx < 0)
    {
        for (int i = 0; i < activeVoiceCount; ++i)
        {
            if (! voices[i].active && voices[i].envLevel < 0.002f)
            {
                chosenIdx = i;
                break;
            }
        }
    }

    // Senao, pega a voz mais antiga (voice stealing)
    if (chosenIdx < 0)
    {
        uint64_t oldestAge = UINT64_MAX;
        for (int i = 0; i < activeVoiceCount; ++i)
        {
            if (voices[i].ageCounter < oldestAge)
            {
                oldestAge = voices[i].ageCounter;
                chosenIdx = i;
            }
        }
    }

    if (chosenIdx < 0)
        chosenIdx = 0;

    auto& v = voices[chosenIdx];
    const bool wasActive = v.active && v.envLevel > 0.02f;
    v.midiNote = noteNumber;
    v.velocity = std::max (0.15f, vel);
    v.targetPitch = static_cast<double> (noteNumber);
    if (! wasActive)
        v.currentPitch = v.targetPitch;

    v.active = true;
    v.ageCounter = ++voiceAgeTick;
    if (! (legatoMode && wasActive))
        v.envLevel = std::max (v.envLevel, 0.30f);

    currentMidiNote = noteNumber;
}

void MiniJJMoogAudioProcessor::triggerPolyNoteOff (int noteNumber)
{
    for (auto& v : voices)
    {
        if (v.active && v.midiNote == noteNumber)
            v.active = false;
    }
}

const juce::String MiniJJMoogAudioProcessor::getName() const { return "Mini Moon"; }
bool MiniJJMoogAudioProcessor::acceptsMidi() const { return true; }
bool MiniJJMoogAudioProcessor::producesMidi() const { return false; }
bool MiniJJMoogAudioProcessor::isMidiEffect() const { return false; }
double MiniJJMoogAudioProcessor::getTailLengthSeconds() const { return 0.0; }
int MiniJJMoogAudioProcessor::getNumPrograms() { return 1; }
int MiniJJMoogAudioProcessor::getCurrentProgram() { return 0; }
void MiniJJMoogAudioProcessor::setCurrentProgram (int) {}
const juce::String MiniJJMoogAudioProcessor::getProgramName (int) { return {}; }
void MiniJJMoogAudioProcessor::changeProgramName (int, const juce::String&) {}

void MiniJJMoogAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate > 0.0 ? sampleRate : 44100.0;

    juce::dsp::ProcessSpec spec;
    spec.sampleRate = currentSampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32> (samplesPerBlock > 0 ? samplesPerBlock : 512);
    spec.numChannels = static_cast<juce::uint32> (juce::jmax (1, getTotalNumOutputChannels()));

    moogFilter.prepare (spec);
    moogFilter.setType (juce::dsp::StateVariableTPTFilterType::lowpass);
    moogFilter.reset();

    oscPhases.fill (0.0);
    for (auto& v : voices)
    {
        v.active = false;
        v.midiNote = 60;
        v.velocity = 0.0f;
        v.targetPitch = 60.0;
        v.currentPitch = 60.0;
        v.envLevel = 0.0f;
        v.ageCounter = 0;
        v.phases.fill (0.0);
    }

    voiceAgeTick = 0;
    lfoPhase = 0.0;
    targetMidiPitch = 60.0;
    currentMidiPitch = 60.0;
    pitchBendNorm = 0.0f;
    smoothedBendNorm = 0.0f;
    modWheelAmount = 0.0f;
    heldNotes.clear();
    currentMidiNote = 60;
    gateHeld = false;
    filterEnvValue = 0.0f;
    loudnessEnvValue = 0.0f;
}

void MiniJJMoogAudioProcessor::releaseResources()
{
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool MiniJJMoogAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

#if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif

    return true;
}
#endif

void MiniJJMoogAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;

    const int numSamples = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();

    for (int ch = 0; ch < numChannels; ++ch)
        buffer.clear (ch, 0, numSamples);

    if (numChannels <= 0 || numSamples <= 0)
        return;

    const int voiceModeIdx = static_cast<int> (apvts.getRawParameterValue ("voice_mode")->load());
    const int activeVoiceCount = voiceModeToCount (voiceModeIdx);
    const bool isPoly = activeVoiceCount > 1;
    const bool legatoMode = apvts.getRawParameterValue ("legato_on")->load() > 0.5f;

    for (const auto metadata : midiMessages)
    {
        const auto msg = metadata.getMessage();
        if (msg.isNoteOn())
        {
            const int noteNum = msg.getNoteNumber();
            const float vel = std::max (0.15f, msg.getFloatVelocity());

            if (isPoly)
            {
                triggerPolyNoteOn (noteNum, vel, activeVoiceCount, legatoMode);
            }
            else
            {
                const bool hadHeldNotes = ! heldNotes.empty();
                heldNotes.erase (std::remove (heldNotes.begin(), heldNotes.end(), noteNum), heldNotes.end());
                heldNotes.push_back (noteNum);

                noteVelocity = vel;

                if (heldNotes.size() == 1)
                {
                    currentMidiNote = noteNum;
                    targetMidiPitch = static_cast<double> (currentMidiNote);
                    if (! hadHeldNotes && ! gateHeld)
                        currentMidiPitch = targetMidiPitch;
                }
                else
                {
                    const int highestNote = *std::max_element (heldNotes.begin(), heldNotes.end());
                    currentMidiNote = highestNote;
                    targetMidiPitch = static_cast<double> (currentMidiNote);
                }

                gateHeld = true;

                const bool isLegatoSlide = legatoMode && hadHeldNotes;
                if (! isLegatoSlide)
                {
                    filterEnvValue = std::max (filterEnvValue, 0.25f);
                    loudnessEnvValue = std::max (loudnessEnvValue, 0.25f);
                }
            }
        }
        else if (msg.isNoteOff())
        {
            const int noteNum = msg.getNoteNumber();
            if (isPoly)
            {
                triggerPolyNoteOff (noteNum);
            }
            else
            {
                heldNotes.erase (std::remove (heldNotes.begin(), heldNotes.end(), noteNum), heldNotes.end());

                if (! heldNotes.empty())
                {
                    const int lowestNote = *std::min_element (heldNotes.begin(), heldNotes.end());
                    currentMidiNote = lowestNote;
                    targetMidiPitch = static_cast<double> (currentMidiNote);
                    gateHeld = true;
                }
                else
                {
                    gateHeld = false;
                }
            }
        }
        else if (msg.isPitchWheel())
        {
            const double norm = (static_cast<double> (msg.getPitchWheelValue()) - 8192.0) / 8192.0;
            pitchBendNorm = static_cast<float> (juce::jlimit (-1.0, 1.0, norm));
        }
        else if (msg.isController() && msg.getControllerNumber() == 1)
        {
            modWheelAmount = static_cast<float> (msg.getControllerValue()) / 127.0f;
        }
        else if (msg.isAllNotesOff() || msg.isAllSoundOff())
        {
            heldNotes.clear();
            for (auto& v : voices)
                v.active = false;
            gateHeld = false;
            noteVelocity = 0.0f;
        }
    }

    const float tuneSemis     = apvts.getRawParameterValue ("tune")->load();
    const float glideSec      = apvts.getRawParameterValue ("glide")->load();
    const float pbRangeSemis  = apvts.getRawParameterValue ("pitch_bend_range")->load();
    const float modMix        = apvts.getRawParameterValue ("mod_mix")->load();
    const bool  oscModOn      = apvts.getRawParameterValue ("osc_mod_on")->load() > 0.5f;
    const bool  osc3CtrlOn    = apvts.getRawParameterValue ("osc3_ctrl_on")->load() > 0.5f;

    const int   r1            = static_cast<int> (apvts.getRawParameterValue ("osc1_range")->load());
    const float f1            = apvts.getRawParameterValue ("osc1_freq")->load();
    const int   w1            = static_cast<int> (apvts.getRawParameterValue ("osc1_wave")->load());

    const int   r2            = static_cast<int> (apvts.getRawParameterValue ("osc2_range")->load());
    const float f2            = apvts.getRawParameterValue ("osc2_freq")->load();
    const int   w2            = static_cast<int> (apvts.getRawParameterValue ("osc2_wave")->load());

    const int   r3            = static_cast<int> (apvts.getRawParameterValue ("osc3_range")->load());
    const float f3            = apvts.getRawParameterValue ("osc3_freq")->load();
    const int   w3            = static_cast<int> (apvts.getRawParameterValue ("osc3_wave")->load());

    const float v1            = apvts.getRawParameterValue ("osc1_vol")->load();
    const float v2            = apvts.getRawParameterValue ("osc2_vol")->load();
    const float v3            = apvts.getRawParameterValue ("osc3_vol")->load();
    const float noiseVol      = apvts.getRawParameterValue ("noise_vol")->load();
    const int   noiseType     = static_cast<int> (apvts.getRawParameterValue ("noise_color")->load());
    const float extVol        = apvts.getRawParameterValue ("ext_vol")->load();
    const float overloadAmt   = apvts.getRawParameterValue ("overload")->load();

    const bool  filterModOn   = apvts.getRawParameterValue ("filter_mod_on")->load() > 0.5f;
    const bool  kbCtrlOn      = apvts.getRawParameterValue ("kb_ctrl_on")->load() > 0.5f;
    const float baseCutoff    = apvts.getRawParameterValue ("cutoff")->load();
    const float emphasis      = apvts.getRawParameterValue ("emphasis")->load();
    const float filterContour = apvts.getRawParameterValue ("filter_contour")->load();

    const float fAtk          = apvts.getRawParameterValue ("filter_attack")->load();
    const float fDec          = apvts.getRawParameterValue ("filter_decay")->load();
    const float fSus          = apvts.getRawParameterValue ("filter_sustain")->load();

    const float lAtk          = apvts.getRawParameterValue ("loud_attack")->load();
    const float lDec          = apvts.getRawParameterValue ("loud_decay")->load();
    const float lSus          = apvts.getRawParameterValue ("loud_sustain")->load();

    const float masterVol     = apvts.getRawParameterValue ("master_vol")->load();
    const bool  mainOutOn     = apvts.getRawParameterValue ("main_out_on")->load() > 0.5f;

    const float fAtkCoeff     = secondsToCoeff (fAtk, currentSampleRate);
    const float fDecCoeff     = secondsToCoeff (fDec, currentSampleRate);
    const float lAtkCoeff     = secondsToCoeff (lAtk, currentSampleRate);
    const float lDecCoeff     = secondsToCoeff (lDec, currentSampleRate);

    const double glideTau = std::max (0.001, static_cast<double> (glideSec) / 3.0);
    const double expGlideStep = (glideSec <= 0.002f)
        ? 1.0
        : (1.0 - std::exp (-1.0 / (glideTau * currentSampleRate)));

    // Suavizacao gradativa (~18 ms) para evitar degraus mesmo em movimentos bruscos do Pitch Bend
    const float bendSmoothCoeff = secondsToCoeff (0.018f, currentSampleRate);

    moogFilter.setResonance (0.707f + emphasis * 6.5f);
    const float driveGain = 1.0f + overloadAmt * 4.5f;
    const float polyScale = isPoly ? (1.0f / std::sqrt (static_cast<float> (activeVoiceCount))) : 1.0f;

    const double lfoHz = 4.5;
    const float effectiveModDepth = juce::jlimit (0.0f, 1.5f, modMix + modWheelAmount * 0.65f);

    for (int sample = 0; sample < numSamples; ++sample)
    {
        smoothedBendNorm += bendSmoothCoeff * (pitchBendNorm - smoothedBendNorm);
        const float bendSemis = smoothedBendNorm * pbRangeSemis;

        const float lfoWave = static_cast<float> (std::sin (2.0 * juce::MathConstants<double>::pi * lfoPhase));
        lfoPhase += lfoHz / currentSampleRate;
        if (lfoPhase >= 1.0) lfoPhase -= 1.0;

        const float white = rng.nextFloat() * 2.0f - 1.0f;
        pinkB0 = 0.99886f * pinkB0 + white * 0.0555179f;
        pinkB1 = 0.99332f * pinkB1 + white * 0.0750759f;
        pinkB2 = 0.96900f * pinkB2 + white * 0.1538520f;
        pinkB3 = 0.86650f * pinkB3 + white * 0.3104856f;
        pinkB4 = 0.55000f * pinkB4 + white * 0.5329522f;
        pinkB5 = -0.7616f * pinkB5 - white * 0.0168980f;
        const float pink = (pinkB0 + pinkB1 + pinkB2 + pinkB3 + pinkB4 + pinkB5 + pinkB6 + white * 0.5362f) * 0.08f;
        pinkB6 = white * 0.115926f;
        const float noiseSample = (noiseType == 1) ? pink : white;

        float rawSynthSample = 0.0f;
        float activeEnvForFilter = 0.0f;

        if (! isPoly)
        {
            currentMidiPitch += expGlideStep * (targetMidiPitch - currentMidiPitch);
            const double glidedFreqHz = 440.0 * std::pow (2.0, (currentMidiPitch - 69.0) / 12.0);

            if (gateHeld)
            {
                filterEnvValue += fAtkCoeff * (1.0f - filterEnvValue);
                loudnessEnvValue += lAtkCoeff * (1.0f - loudnessEnvValue);
            }
            else
            {
                filterEnvValue += fDecCoeff * (0.0f - filterEnvValue);
                loudnessEnvValue += lDecCoeff * (0.0f - loudnessEnvValue);
            }

            activeEnvForFilter = gateHeld
                ? (filterEnvValue * (1.0f - fDecCoeff * 0.35f) + fSus * (fDecCoeff * 0.35f))
                : filterEnvValue;

            const float ampLevel = gateHeld
                ? (loudnessEnvValue * (1.0f - lDecCoeff * 0.25f) + lSus * (lDecCoeff * 0.25f))
                : loudnessEnvValue;

            const double freq3 = glidedFreqHz * rangeToOctaveMultiplier (r3) * semitonesToRatio (f3 + tuneSemis + bendSemis);
            const float out3 = renderWaveform (oscPhases[2], w3, 0.22f);
            oscPhases[2] += freq3 / currentSampleRate;
            if (oscPhases[2] >= 1.0) oscPhases[2] -= 1.0;

            const float modPitchSemis = oscModOn ? (lfoWave * effectiveModDepth * 2.5f) : 0.0f;
            const float totalSemisOffset = tuneSemis + bendSemis + modPitchSemis;

            const double freq1 = glidedFreqHz * rangeToOctaveMultiplier (r1) * semitonesToRatio (f1 + totalSemisOffset);
            const double freq2 = glidedFreqHz * rangeToOctaveMultiplier (r2) * semitonesToRatio (f2 + totalSemisOffset);

            const float out1 = renderWaveform (oscPhases[0], w1, 0.22f);
            const float out2 = renderWaveform (oscPhases[1], w2, 0.22f);

            oscPhases[0] += freq1 / currentSampleRate;
            if (oscPhases[0] >= 1.0) oscPhases[0] -= 1.0;

            oscPhases[1] += freq2 / currentSampleRate;
            if (oscPhases[1] >= 1.0) oscPhases[1] -= 1.0;

            const float osc3Mix = osc3CtrlOn ? 0.0f : (out3 * v3);
            rawSynthSample = (out1 * v1 + out2 * v2 + osc3Mix + noiseSample * noiseVol + extVol * 0.0f) * 0.45f * ampLevel * noteVelocity;
        }
        else
        {
            for (int i = 0; i < activeVoiceCount; ++i)
            {
                auto& v = voices[i];
                if (! v.active && v.envLevel < 0.0005f)
                    continue;

                v.currentPitch += expGlideStep * (v.targetPitch - v.currentPitch);
                const double voiceFreqHz = 440.0 * std::pow (2.0, (v.currentPitch - 69.0) / 12.0);

                if (v.active)
                    v.envLevel += lAtkCoeff * (1.0f - v.envLevel);
                else
                    v.envLevel += lDecCoeff * (0.0f - v.envLevel);

                const float voiceAmp = v.active
                    ? (v.envLevel * (1.0f - lDecCoeff * 0.20f) + lSus * (lDecCoeff * 0.20f))
                    : v.envLevel;

                activeEnvForFilter = std::max (activeEnvForFilter, voiceAmp);

                const float modPitchSemis = oscModOn ? (lfoWave * effectiveModDepth * 2.5f) : 0.0f;
                const float totalSemisOffset = tuneSemis + bendSemis + modPitchSemis;

                const double fOsc1 = voiceFreqHz * rangeToOctaveMultiplier (r1) * semitonesToRatio (f1 + totalSemisOffset);
                const double fOsc2 = voiceFreqHz * rangeToOctaveMultiplier (r2) * semitonesToRatio (f2 + totalSemisOffset);
                const double fOsc3 = voiceFreqHz * rangeToOctaveMultiplier (r3) * semitonesToRatio (f3 + totalSemisOffset);

                const float wOsc1 = renderWaveform (v.phases[0], w1, 0.22f);
                const float wOsc2 = renderWaveform (v.phases[1], w2, 0.22f);
                const float wOsc3 = renderWaveform (v.phases[2], w3, 0.22f);

                v.phases[0] += fOsc1 / currentSampleRate;
                if (v.phases[0] >= 1.0) v.phases[0] -= 1.0;

                v.phases[1] += fOsc2 / currentSampleRate;
                if (v.phases[1] >= 1.0) v.phases[1] -= 1.0;

                v.phases[2] += fOsc3 / currentSampleRate;
                if (v.phases[2] >= 1.0) v.phases[2] -= 1.0;

                const float osc3Mix = osc3CtrlOn ? 0.0f : (wOsc3 * v3);
                const float voiceMix = (wOsc1 * v1 + wOsc2 * v2 + osc3Mix + noiseSample * noiseVol * 0.25f) * 0.45f;
                rawSynthSample += voiceMix * voiceAmp * v.velocity * polyScale;
            }
        }

        const float saturated = std::tanh (rawSynthSample * driveGain);

        const float kbOffset = kbCtrlOn ? ((static_cast<float> (currentMidiNote) - 60.0f) * 24.0f) : 0.0f;
        const float contourHz = activeEnvForFilter * filterContour * 5500.0f;
        const float lfoCutoffHz = filterModOn ? (lfoWave * effectiveModDepth * 2600.0f) : 0.0f;
        const float finalCutoff = juce::jlimit (35.0f, 16000.0f, baseCutoff + kbOffset + contourHz + lfoCutoffHz);

        moogFilter.setCutoffFrequency (finalCutoff);

        const float outputGain = mainOutOn ? masterVol : 0.0f;

        for (int ch = 0; ch < numChannels; ++ch)
        {
            const float filtered = moogFilter.processSample (static_cast<size_t> (ch), saturated);
            buffer.setSample (ch, sample, filtered * outputGain);
        }
    }
}

bool MiniJJMoogAudioProcessor::hasEditor() const { return true; }

juce::AudioProcessorEditor* MiniJJMoogAudioProcessor::createEditor()
{
    return new MiniJJMoogAudioProcessorEditor (*this);
}

void MiniJJMoogAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    if (auto state = apvts.copyState(); state.isValid())
    {
        std::unique_ptr<juce::XmlElement> xml (state.createXml());
        if (xml != nullptr)
            copyXmlToBinary (*xml, destData);
    }
}

void MiniJJMoogAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    if (xmlState != nullptr && xmlState->hasTagName (apvts.state.getType()))
        apvts.replaceState (juce::ValueTree::fromXml (*xmlState));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MiniJJMoogAudioProcessor();
}
