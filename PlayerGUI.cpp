#include "PlayerGUI.h"

PlayerGUI::PlayerGUI(PlayerAudio& audioPlayer)
    : playerAudio(audioPlayer)
{
    // Setup buttons
    juce::TextButton* buttons[] = {
        &loadButton, &playButton, &stopButton, &loopButton,
        &loopAButton, &loopBButton, &abLoopButton, &clearLoopButton,
        &muteButton
    };

    for (auto* btn : buttons)
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

    // Setup loop points labels
    loopStartLabel.setText("A: --:--", juce::dontSendNotification);
    loopStartLabel.setJustificationType(juce::Justification::centred);
    loopStartLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(loopStartLabel);

    loopEndLabel.setText("B: --:--", juce::dontSendNotification);
    loopEndLabel.setJustificationType(juce::Justification::centred);
    loopEndLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(loopEndLabel);

    // Setup loop status label
    loopStatusLabel.setText("Loop: Off", juce::dontSendNotification);
    loopStatusLabel.setJustificationType(juce::Justification::centred);
    loopStatusLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(loopStatusLabel);

    // Initialize button states
    updateLoopButton();
    updateABLoopButton();
    updateLoopPointsDisplay();

    // Start timer for real-time updates
    startTimer(30);
}

void PlayerGUI::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey);
}

void PlayerGUI::resized()
{
    auto area = getLocalBounds().reduced(10);

    // Position area with time labels
    auto positionArea = area.removeFromTop(40);
    {
        currentTimeLabel.setBounds(positionArea.removeFromLeft(50));
        durationLabel.setBounds(positionArea.removeFromRight(50));
        positionSlider.setBounds(positionArea);
    }

    // Loop points labels area
    auto loopLabelsArea = area.removeFromTop(20);
    {
        loopStartLabel.setBounds(loopLabelsArea.removeFromLeft(60));
        loopLabelsArea.removeFromLeft(10);
        loopEndLabel.setBounds(loopLabelsArea.removeFromLeft(60));
        loopLabelsArea.removeFromLeft(10);
        loopStatusLabel.setBounds(loopLabelsArea.removeFromLeft(100));
    }

    // Buttons area
    auto buttonArea = area.removeFromTop(60);
    int buttonWidth = 80;
    int margin = 10;

    // First row of buttons
    auto firstRow = buttonArea.removeFromTop(40);
    loadButton.setBounds(firstRow.removeFromLeft(buttonWidth + 10));
    firstRow.removeFromLeft(margin);
    playButton.setBounds(firstRow.removeFromLeft(buttonWidth));
    firstRow.removeFromLeft(margin);
    stopButton.setBounds(firstRow.removeFromLeft(buttonWidth));
    firstRow.removeFromLeft(margin);
    loopButton.setBounds(firstRow.removeFromLeft(buttonWidth));

    // Second row of buttons (A-B Looping)
    auto secondRow = buttonArea.removeFromTop(40);
    loopAButton.setBounds(secondRow.removeFromLeft(buttonWidth));
    secondRow.removeFromLeft(margin);
    loopBButton.setBounds(secondRow.removeFromLeft(buttonWidth));
    secondRow.removeFromLeft(margin);
    abLoopButton.setBounds(secondRow.removeFromLeft(buttonWidth));
    secondRow.removeFromLeft(margin);
    clearLoopButton.setBounds(secondRow.removeFromLeft(buttonWidth));

    auto soundArea = area.removeFromTop(40);
    volumeSlider.setBounds(soundArea.removeFromLeft(200));
    soundArea.removeFromLeft(10);
    muteButton.setBounds(soundArea.removeFromLeft(80));
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
        isLooping = !isLooping;
        playerAudio.setLooping(isLooping);
        updateLoopButton();
    }
    else if (button == &loopAButton)
    {
        playerAudio.setLoopPointA();
        updateLoopPointsDisplay();
        updateABLoopButton();
    }
    else if (button == &loopBButton)
    {
        playerAudio.setLoopPointB();
        updateLoopPointsDisplay();
        updateABLoopButton();
    }
    else if (button == &abLoopButton)
    {
        playerAudio.toggleABLooping();
        updateABLoopButton();
    }
    else if (button == &clearLoopButton)
    {
        playerAudio.clearLoopPoints();
        updateABLoopButton();
        updateLoopPointsDisplay();
    }
    else if (button == &muteButton)
    {
        playerAudio.Muted();
        muteButton.setButtonText(playerAudio.isMuted() ? "Unmute" : "Mute");
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

void PlayerGUI::updateABLoopButton()
{
    bool hasPoints = playerAudio.hasLoopPoints();
    bool isABLooping = playerAudio.isABLooping();

    if (isABLooping) {
        abLoopButton.setColour(juce::TextButton::buttonColourId, juce::Colours::green);
        loopStatusLabel.setText("Loop: ON", juce::dontSendNotification);
        loopStatusLabel.setColour(juce::Label::textColourId, juce::Colours::green);
    }
    else if (hasPoints) {
        abLoopButton.setColour(juce::TextButton::buttonColourId, juce::Colours::yellow);
        loopStatusLabel.setText("Loop: READY", juce::dontSendNotification);
        loopStatusLabel.setColour(juce::Label::textColourId, juce::Colours::yellow);
    }
    else {
        abLoopButton.setColour(juce::TextButton::buttonColourId, juce::Colours::black);
        loopStatusLabel.setText("Loop: OFF", juce::dontSendNotification);
        loopStatusLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    }
}

void PlayerGUI::updateLoopPointsDisplay()
{
    double loopStart = playerAudio.getLoopStart();
    double loopEnd = playerAudio.getLoopEnd();
    loopStartLabel.setText("A: " + formatTime(loopStart), juce::dontSendNotification);
    loopEndLabel.setText("B: " + formatTime(loopEnd), juce::dontSendNotification);
    updateABLoopButton();
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
                positionSlider.setValue(0.0, juce::dontSendNotification);
                updateTimeDisplays();
                updateLoopPointsDisplay();
                updateABLoopButton();
            }
        });
}

void PlayerGUI::updateTimeDisplays()
{
    double currentTime = playerAudio.getCurrentPosition();
    double duration = playerAudio.getLengthInSeconds();

    currentTimeLabel.setText(formatTime(currentTime), juce::dontSendNotification);
    durationLabel.setText(formatTime(duration), juce::dontSendNotification);
}

juce::String PlayerGUI::formatTime(double seconds)
{
    if (seconds < 0) return "--:--";

    int totalSeconds = static_cast<int>(seconds);
    int minutes = totalSeconds / 60;
    int secs = totalSeconds % 60;

    return juce::String::formatted("%d:%02d", minutes, secs);
}
