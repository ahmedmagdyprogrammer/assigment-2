#include "PlayerAudio.h"
PlayerAudio::PlayerAudio()
{
    formatManager.registerBasicFormats();

    resamplingSource = std::make_unique<juce::ResamplingAudioSource>(&transportSource, false, 2);
}

PlayerAudio::~PlayerAudio()
{
    resamplingSource.reset();
    transportSource.setSource(nullptr);
    readerSource.reset();
}

void PlayerAudio::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    if (resamplingSource)
        resamplingSource->prepareToPlay(samplesPerBlockExpected, sampleRate);
    else
        transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void PlayerAudio::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    if (resamplingSource)
        resamplingSource->getNextAudioBlock(bufferToFill);
    else
        transportSource.getNextAudioBlock(bufferToFill);


    if (looping && VideoIndex >= 0)
    {
        double currentPos = transportSource.getCurrentPosition();
        double length = transportSource.getLengthInSeconds();
        if (length > 0 && currentPos >= (length - 0.05))
        {
            transportSource.setPosition(0.0);
            transportSource.start();
        }
    }
}

void PlayerAudio::releaseResources()
{
    if (resamplingSource)
        resamplingSource->releaseResources();
    transportSource.releaseResources();
}
void PlayerAudio::addFileToPlaylist(const juce::File& file)
{
    if (!file.existsAsFile())
        return;

    if (auto* reader = formatManager.createReaderFor(file))
    {
        auto source = std::make_unique<juce::AudioFormatReaderSource>(reader, true);
        playlistSources.push_back(std::move(source));
        playlistFiles.push_back(file);
    }


    if (VideoIndex == -1)
        playsound(0);
}

void PlayerAudio::playsound(int index)
{
    if (index < 0 || index >= (int)playlistSources.size())
        return;

    transportSource.stop();
    transportSource.setSource(nullptr);

    auto* readerSource = playlistSources[index].get();
    transportSource.setSource(readerSource, 0, nullptr, readerSource->getAudioFormatReader()->sampleRate);

    transportSource.start();
    VideoIndex = index;
}


void PlayerAudio::play() { transportSource.start(); }
void PlayerAudio::stop() { transportSource.stop(); }
void PlayerAudio::setLooping(bool shouldLoop) { looping = shouldLoop; }
bool PlayerAudio::isLooping() const { return looping; }
void PlayerAudio::setGain(float gain) { transportSource.setGain(gain); }
void PlayerAudio::setPosition(double pos) { transportSource.setPosition(pos); }
double PlayerAudio::getPosition() const { return transportSource.getCurrentPosition(); }
double PlayerAudio::getLength() const { return transportSource.getLengthInSeconds(); }
void PlayerAudio::setSpeed(double newSpeed)
{
    playbackSpeed = newSpeed;
    if (resamplingSource)
        resamplingSource->setResamplingRatio(playbackSpeed);
}
int PlayerAudio::getNumFiles() const
{
    return (int)playlistFiles.size();
}

const juce::File& PlayerAudio::getSoundFile(int index) const
{
    jassert(index >= 0 && index < (int)playlistFiles.size());
    return playlistFiles[index];
}

