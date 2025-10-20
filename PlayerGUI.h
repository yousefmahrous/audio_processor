#pragma once

#include <JuceHeader.h>
#include "PlayerAudio.h"

class PlayerGUI : public juce::Component,
    public juce::Button::Listener,
    public juce::Slider::Listener,
    public juce::Timer
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

    // Timer callback for real-time updates
    void timerCallback() override;

private:
    PlayerAudio& playerAudio;

    // GUI Controls
    juce::TextButton loadButton{ "Load File" };
    juce::TextButton playButton{ "Play" };
    juce::TextButton stopButton{ "Stop" };
    juce::TextButton loopButton{ "Loop" };
    juce::TextButton muteButton{ "Mute" };

    // A-B Looping buttons
    juce::TextButton loopAButton{ "Set A" };
    juce::TextButton loopBButton{ "Set B" };
    juce::TextButton abLoopButton{ "A-B Loop" };
    juce::TextButton clearLoopButton{ "Clear" };
    // Loop points labels
    juce::Label loopStartLabel;
    juce::Label loopEndLabel;
    juce::Label loopStatusLabel;

    juce::Slider volumeSlider;

    // Position slider and time displays
    juce::Slider positionSlider;
    juce::Label currentTimeLabel;
    juce::Label durationLabel;

    std::unique_ptr<juce::FileChooser> fileChooser;
    bool isLooping = false;
    bool isDraggingPositionSlider = false;

    void loadAudioFile();
    void updateLoopButton();
    void updateABLoopButton();
    void updateLoopPointsDisplay();
    void updateTimeDisplays();
    juce::String formatTime(double seconds);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerGUI)
};