#include "PlayerAudio.h"

PlayerAudio::PlayerAudio()
{
    formatManager.registerBasicFormats();
    transportSource.setSource(nullptr);
}

PlayerAudio::~PlayerAudio()
{
    transportSource.setSource(nullptr);
}

void PlayerAudio::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void PlayerAudio::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    if (transportSource.isPlaying())
    {
        transportSource.getNextAudioBlock(bufferToFill);

        // Handle looping
        if (looping && transportSource.hasStreamFinished())
        {
            transportSource.setPosition(0.0);
        }
    }
    else
    {
        bufferToFill.clearActiveBufferRegion();
    }
}

void PlayerAudio::releaseResources()
{
    transportSource.releaseResources();
}

void PlayerAudio::loadFile(const juce::File& audioFile)
{
    transportSource.stop();
    transportSource.setSource(nullptr);
    readerSource.reset();

    if (audioFile.existsAsFile())
    {
        if (auto* reader = formatManager.createReaderFor(audioFile))
        {
            readerSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);
            transportSource.setSource(readerSource.get(),
                0,
                nullptr,
                reader->sampleRate);

            // Enable looping on the reader source
            if (readerSource != nullptr)
            {
                readerSource->setLooping(looping);
            }
        }
    }
}

void PlayerAudio::start()
{
    transportSource.start();
}

void PlayerAudio::stop()
{
    transportSource.stop();
    transportSource.setPosition(0.0);
}

void PlayerAudio::setPosition(double position)
{
    transportSource.setPosition(position);
}

void PlayerAudio::setGain(float gain)
{
    transportSource.setGain(gain);
}

void PlayerAudio::setLooping(bool shouldLoop)
{
    looping = shouldLoop;

    if (readerSource != nullptr)
    {
        readerSource->setLooping(looping);
    }
}