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

    // Initialize loop button state
    updateLoopButton();
}

void PlayerGUI::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey);
}

void PlayerGUI::resized()
{
    int y = 20;
    int buttonWidth = 80;
    int buttonHeight = 40;
    int margin = 20;

    loadButton.setBounds(margin, y, buttonWidth + 20, buttonHeight);
    playButton.setBounds(margin + buttonWidth + 40, y, buttonWidth, buttonHeight);
    stopButton.setBounds(margin + (buttonWidth + 40) * 2, y, buttonWidth, buttonHeight);
    loopButton.setBounds(margin + (buttonWidth + 40) * 3, y, buttonWidth, buttonHeight);
    volumeSlider.setBounds(margin, 100, getWidth() - 2 * margin, 30);
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
    }
    else if (button == &loopButton)
    {
        // Toggle loop state
        isLooping = !isLooping;
        playerAudio.setLooping(isLooping);
        updateLoopButton();
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

void PlayerGUI::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &volumeSlider)
    {
        playerAudio.setGain(static_cast<float>(slider->getValue()));
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
            }
        });
}