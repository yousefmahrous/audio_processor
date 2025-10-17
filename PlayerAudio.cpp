﻿#include "PlayerAudio.h"

PlayerAudio::PlayerAudio() {
	formatManager.registerBasicFormats();
}

PlayerAudio::~PlayerAudio() {
	releaseResources();
}
void PlayerAudio::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void PlayerAudio::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    transportSource.getNextAudioBlock(bufferToFill);
}

void PlayerAudio::releaseResources()
{
    transportSource.releaseResources();
}

bool PlayerAudio::LoadFile(const juce::File& file) {
    if (file.existsAsFile())
    {
        if (auto* reader = formatManager.createReaderFor(file))
        {
            // 🔑 Disconnect old source first
            transportSource.stop();
            transportSource.setSource(nullptr);
            readerSource.reset();

            // Create new reader source
            readerSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);

            // Attach safely
            transportSource.setSource(readerSource.get(),
                0,
                nullptr,
                reader->sampleRate);
            transportSource.start();
        }
    }
}
void PlayerAudio::play() {
    transportSource.start();
}
void PlayerAudio::stop() {
    transportSource.stop();
    transportSource.setPosition(0.0);
}
void PlayerAudio::setGain(float gain) {
    transportSource.setGain(gain);
}
void PlayerAudio::setPosition(double pos) {
    transportSource.setPosition(pos);
}
double PlayerAudio::getPosition() const {
    retrun transportSource.getCurrentPosition();
}
double PlayerAudio::getPosition() const {
    retrun transportSource.getLengthInSeconds();
}