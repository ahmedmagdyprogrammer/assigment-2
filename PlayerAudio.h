#pragma once
#include <JuceHeader.h>
#include <vector>
class PlayerAudio : public juce::AudioSource
{
public:
    PlayerAudio();
    ~PlayerAudio() override;
    std::vector<std::unique_ptr<juce::AudioFormatReaderSource>> playlistSources;
    std::vector<juce::File> playlistFiles;
    // AudioSource overrides
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    // Controls
    bool loadFile(const juce::File& file);
    void addFileToPlaylist(const juce::File& file);
    void playsound(int index);
    void play();
    void stop();
    void setLooping(bool shouldLoop);
    bool isLooping() const;
    void setGain(float gain);
    void setPosition(double pos);
    double getPosition() const;
    double getLength() const;
    void setSpeed(double newSpeed);
    int getNumFiles() const;
    int VideoIndex = -1;
    const juce::File& getSoundFile(int index) const;
private:
    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioTransportSource transportSource;
    std::unique_ptr<juce::ResamplingAudioSource> resamplingSource;
    std::unique_ptr<juce::AudioFormatReaderSource> currentReaderSource;
    bool looping = false;
    double playbackSpeed = 1.0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerAudio)
};
