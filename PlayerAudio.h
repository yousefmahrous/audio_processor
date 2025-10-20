#pragma once

#include <JuceHeader.h>

class PlayerAudio : public juce::AudioSource
{
public:
    PlayerAudio();
    ~PlayerAudio() override;

    // AudioSource methods
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    // Transport control methods
    void loadFile(const juce::File& audioFile);
    void start();
    void stop();

    // Loop Features
    void setLooping(bool shouldLoop);
    bool isLooping() const { return looping; }

    // A-B Looping methods
    void setLoopPointA();
    void setLoopPointB();
    void toggleABLooping();
    bool isABLooping() const { return abLooping; }
    void clearLoopPoints();

    double getLoopStart() const { return loopStart; }
    double getLoopEnd() const { return loopEnd; }
    bool hasLoopPoints() const { return hasLoopStart && hasLoopEnd; }

    void setPosition(double position);
    void setGain(float gain);

    bool isPlaying() const { return transportSource.isPlaying(); }
    double getCurrentPosition() const { return transportSource.getCurrentPosition(); }
    double getLengthInSeconds() const { return transportSource.getLengthInSeconds(); }

   void Muted();
   bool isMuted() const;

private:
    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioTransportSource transportSource;
    bool looping = false;
    bool abLooping = false;
    double loopStart = 0.0;
    double loopEnd = 0.0;
    bool hasLoopStart = false;
    bool hasLoopEnd = false;


    bool muted = false;
    float previousGain = 1.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerAudio)

};
