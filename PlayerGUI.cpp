#include "PlayerGUI.h"

PlayerGUI : PlayerGUI() {
    for (auto* btn : { &loadButton, &restartButton , &stopButton })
    {
        btn->addListener(this);
        addAndMakeVisible(btn);
    }
    volumeSlider.setRange(0.0, 1.0, 0.01);
    volumeSlider.setValue(0.5);
    volumeSlider.addListener(this);
    addAndMakeVisible(volumeSlider);

}

PlayerGUI:~PlayerGUI(); {}

void PlayerGUI::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    playerAudio.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void PlayerGUI::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    playerAudio.getNextAudioBlock(bufferToFill);
}

void PlayerGUI::releaseResources()
{
    playerAudio.releaseResources();
}
void PlayerGUI::resized() {
    int y = 20;
    loadButton.setBounds(20, y, 100, 40);
    restartButton.setBounds(140, y, 80, 40);
    stopButton.setBounds(240, y, 80, 40);
    /*prevButton.setBounds(340, y, 80, 40);
    nextButton.setBounds(440, y, 80, 40);*/
    volumeSlider.setBounds(20, 100, getWidth() - 40, 30);
}

void PlayerGUI::buttonClicked(juce::Button* button)
{
    if (button == &loadButton)
    {
        juce::FileChooser chooser("Select audio files...",
            juce::File{},
            "*.wav;*.mp3");

        fileChooser = std::make_unique<juce::FileChooser>(
            "Select an audio file...",
            juce::File{},
            "*.wav;*.mp3");

        fileChooser->launchAsync(
            juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
            [this](const juce::FileChooser& fc)
            {
                auto file = fc.getResult();
                if (file.existsAsFile()) {
                    playerAudio.LoadFile(file);
                }
            });
    } else if (button == &playButton)
    {
        playerAudio.play();
    } else if (button == &stopButton)
    {
        playerAudio.stop();
        playerAudio.setPosition(0.0);
    }

}

void PlayerGUI::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey);
}

void PlayerGUI::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &volumeSlider)
        playerAudio.setGain((float)slider->getValue());
}