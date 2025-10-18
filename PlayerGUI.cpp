#include "PlayerGUI.h"

PlayerGUI::PlayerGUI(PlayerAudio& audioPlayer)
    : playerAudio(audioPlayer)
{
    // Setup buttons
    for (auto* btn : { &loadButton, &playButton, &stopButton, &loopButton })
    {
        btn->addListener(this);
        addAndMakeVisible(btn);
    }

    // Setup volume slider
    volumeSlider.setRange(0.0, 1.0, 0.01);
    volumeSlider.setValue(0.5);
    volumeSlider.addListener(this);
    addAndMakeVisible(volumeSlider);

    // Setup position slider
    positionSlider.setRange(0.0, 1.0, 0.001);
    positionSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    positionSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    positionSlider.addListener(this);
    addAndMakeVisible(positionSlider);

    // Setup time labels
    currentTimeLabel.setText("0:00", juce::dontSendNotification);
    currentTimeLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(currentTimeLabel);
    durationLabel.setText("0:00", juce::dontSendNotification);
    durationLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(durationLabel);

    // Initialize loop button state
    updateLoopButton();

    // Start timer for real-time updates (30ms interval for smooth updates)
    startTimer(30);
}

void PlayerGUI::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey);
}

void PlayerGUI::resized()
{
    auto area = getLocalBounds().reduced(10);

    auto positionArea = area.removeFromTop(40);
    {
        currentTimeLabel.setBounds(positionArea.removeFromLeft(50));
        durationLabel.setBounds(positionArea.removeFromRight(50));
        positionSlider.setBounds(positionArea);
    }

    auto buttonArea = area.removeFromTop(50);
    int buttonWidth = 80;
    int buttonHeight = 40;
    int margin = 10;

    loadButton.setBounds(buttonArea.removeFromLeft(buttonWidth + 20));
    buttonArea.removeFromLeft(margin);
    playButton.setBounds(buttonArea.removeFromLeft(buttonWidth));
    buttonArea.removeFromLeft(margin);
    stopButton.setBounds(buttonArea.removeFromLeft(buttonWidth));
    buttonArea.removeFromLeft(margin);
    loopButton.setBounds(buttonArea.removeFromLeft(buttonWidth));

    volumeSlider.setBounds(area.reduced(0, 10));
}

void PlayerGUI::buttonClicked(juce::Button* button)
{
    if (button == &loadButton)
    {
        loadAudioFile();
    }
    else if (button == &playButton)
    {
        playerAudio.start();
    }
    else if (button == &stopButton)
    {
        playerAudio.stop();
        updateTimeDisplays();
    }
    else if (button == &loopButton)
    {
        // Toggle loop state
        isLooping = !isLooping;
        playerAudio.setLooping(isLooping);
        updateLoopButton();
    }
}

void PlayerGUI::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &volumeSlider)
    {
        playerAudio.setGain(static_cast<float>(slider->getValue()));
    }
    else if (slider == &positionSlider)
    {
        //Handle position slider changes
        if (!isDraggingPositionSlider)
        {
            double newPosition = positionSlider.getValue() * playerAudio.getLengthInSeconds();
            playerAudio.setPosition(newPosition);
            updateTimeDisplays();
        }
    }
}

void PlayerGUI::timerCallback()
{
    if (!isDraggingPositionSlider && playerAudio.getLengthInSeconds() > 0)
    {
        double currentPos = playerAudio.getCurrentPosition();
        double totalLength = playerAudio.getLengthInSeconds();
        double normalizedPos = currentPos / totalLength;

        positionSlider.setValue(normalizedPos, juce::dontSendNotification);
        updateTimeDisplays();
    }
}

void PlayerGUI::updateLoopButton()
{
    loopButton.setButtonText("Loop");
    if (isLooping)
    {
        loopButton.setColour(juce::TextButton::buttonColourId, juce::Colours::green);
    }
    else
    {
        loopButton.setColour(juce::TextButton::buttonColourId, juce::Colours::red);
    }
}

void PlayerGUI::loadAudioFile()
{
    fileChooser = std::make_unique<juce::FileChooser>(
        "Select an audio file...",
        juce::File{},
        "*.wav;*.mp3;*.aiff");

    fileChooser->launchAsync(
        juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
        [this](const juce::FileChooser& fc)
        {
            auto file = fc.getResult();
            if (file.existsAsFile())
            {
                playerAudio.loadFile(file);
                // Reset position slider when new file is loaded
                positionSlider.setValue(0.0, juce::dontSendNotification);
                updateTimeDisplays();
            }
        });
}

// Update time displays
void PlayerGUI::updateTimeDisplays()
{
    double currentTime = playerAudio.getCurrentPosition();
    double duration = playerAudio.getLengthInSeconds();

    currentTimeLabel.setText(formatTime(currentTime), juce::dontSendNotification);
    durationLabel.setText(formatTime(duration), juce::dontSendNotification);
}

// Format seconds to MM:SS string
juce::String PlayerGUI::formatTime(double seconds)
{
    if (seconds < 0) return "0:00";

    int totalSeconds = static_cast<int>(seconds);
    int minutes = totalSeconds / 60;
    int secs = totalSeconds % 60;

    return juce::String::formatted("%d:%02d", minutes, secs);
}