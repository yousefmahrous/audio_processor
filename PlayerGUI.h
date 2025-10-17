#pragma once

#include <JuceHeader.h>
#include "PlayerAudio.h"

class PlayerGUI : public juce::Component,
    public juce::Button::Listener,
    public juce::Slider::Listener
{
public:
    PlayerGUI(PlayerAudio& audioPlayer);
    ~PlayerGUI() override = default;

    // Component methods
    void paint(juce::Graphics& g) override;
    void resized() override;

    // Event handlers
    void buttonClicked(juce::Button* button) override;
    void sliderValueChanged(juce::Slider* slider) override;

private:
    PlayerAudio& playerAudio;

    // GUI Controls
    juce::TextButton loadButton{ "Load File" };
    juce::TextButton playButton{ "Play" };
    juce::TextButton stopButton{ "Stop" };
    juce::TextButton loopButton{ "Loop" };
    juce::Slider volumeSlider;

    std::unique_ptr<juce::FileChooser> fileChooser;
    bool isLooping = false;  // Track loop state

    void loadAudioFile();
    void updateLoopButton();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerGUI)
};
