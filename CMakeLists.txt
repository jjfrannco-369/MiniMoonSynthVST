cmake_minimum_required(VERSION 3.22)
project(MiniJJMoogVST VERSION 2.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT DEFINED JUCE_PATH)
    set(JUCE_PATH "C:/JUCE")
endif()

add_subdirectory("${JUCE_PATH}" JUCE)

juce_add_plugin(MiniJJMoogVST
    COMPANY_NAME "JJ Frannco Audio"
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
    COPY_PLUGIN_AFTER_BUILD TRUE
    PLUGIN_MANUFACTURER_CODE Jjfm
    PLUGIN_CODE Mjmv
    FORMATS VST3 Standalone
    PRODUCT_NAME "Mini JJ Moog v2"
)

target_sources(MiniJJMoogVST PRIVATE
    PluginProcessor.h
    PluginProcessor.cpp
    PluginEditor.h
    PluginEditor.cpp
)

target_compile_definitions(MiniJJMoogVST PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
    _WIN32_WINNT=0x0601
    WINVER=0x0601
)

target_link_libraries(MiniJJMoogVST PRIVATE
    juce::juce_audio_utils
    juce::juce_dsp
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags
)
