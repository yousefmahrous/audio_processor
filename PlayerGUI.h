#pragma once
#include <JuceHeader.h>
#include "PlayerAudio.h"

class PlayerGUI : public juce::AudioAppComponent,
    public juce::Button::Listener,
    public juce::Slider::Listener {
public :
    PlayerGUI();
    ~PlayerGUI() override;

    void paint(juce::Graphics& g) override;
    void resized() override;
    void buttonClicked(juce::Button* button) override;
    void sliderValueChanged(juce::Slider* slider) override;
private :
    juce::TextButton loadButton{ "Load Files" };
    juce::TextButton restartButton{ "Restart" };
    juce::TextButton stopButton{ "Stop" };
    juce::Slider volumeSlider;

    std::unique_ptr<juce::FileChooser> fileChooser;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerGUI)
};