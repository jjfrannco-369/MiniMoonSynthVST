cmake_minimum_required(VERSION 3.22)

project(MiniJJMoogVST VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Usa o JUCE em C:/JUCE no runner do GitHub Actions (ou ajuste via -DJUCE_DIR=...)
set(JUCE_DIR "C:/JUCE" CACHE PATH "Path to JUCE")
add_subdirectory("${JUCE_DIR}" JUCE_build)

juce_add_plugin(MiniJJMoogVST
    COMPANY_NAME "JJFrannco"
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
    PLUGIN_MANUFACTURER_CODE Jjfm
    PLUGIN_CODE Mjm1
    FORMATS VST3
    PRODUCT_NAME "Mini JJ Moog"
)

target_sources(MiniJJMoogVST
    PRIVATE
        PluginProcessor.h
        PluginProcessor.cpp
        PluginEditor.h
        PluginEditor.cpp
)

target_compile_definitions(MiniJJMoogVST
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(MiniJJMoogVST
    PRIVATE
        juce::juce_audio_utils
        juce::juce_audio_processors
        juce::juce_dsp
        juce::juce_gui_basics
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)
