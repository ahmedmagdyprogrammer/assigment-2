 
#include "MainComponent.h"
#include "PlayerAudio.h"
MainComponent::MainComponent()
{
    addAndMakeVisible(player1);
    addAndMakeVisible(player2);

    
    mixer.addInputSource(player1.getAudioSource(), false);
    mixer.addInputSource(player2.getAudioSource(), false);

    setAudioChannels(0, 2);  
    setSize(900, 600);
}

MainComponent::~MainComponent()
{
    shutdownAudio();
    mixer.removeAllInputs();
}

void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    
    player1.prepareToPlay(samplesPerBlockExpected, sampleRate);
    player2.prepareToPlay(samplesPerBlockExpected, sampleRate);
 
    mixer.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    mixer.getNextAudioBlock(bufferToFill);
}

void MainComponent::releaseResources()
{
    mixer.releaseResources();
    player1.releaseResources();
    player2.releaseResources();
}

void MainComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkslategrey);
}

void MainComponent::resized()
{
    auto area = getLocalBounds().reduced(8);
    auto half = area.removeFromTop(area.getHeight() / 2);
    player1.setBounds(half.reduced(6));
    player2.setBounds(area.reduced(6));
}
 
