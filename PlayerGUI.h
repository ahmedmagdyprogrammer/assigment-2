 
#pragma once
#include <JuceHeader.h>
#include "PlayerAudio.h"

class PlayerGUI : public juce::Component,
    public juce::Button::Listener,
    public juce::Slider::Listener,
    private juce::Timer,
    public juce::ListBoxModel
{
public:
    PlayerGUI();
    ~PlayerGUI() override;

    void resized() override;
    void paint(juce::Graphics& g) override;

     
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate);
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill);
    void releaseResources();
     
    juce::AudioSource* getAudioSource() { return &playerAudio; }
    int getNumRows() override;
    void paintListBoxItem(int rowNumber, juce::Graphics& g, int width, int height, bool rowIsSelected) override;
    void listBoxItemClicked(int row, const juce::MouseEvent&) override;
private:
    PlayerAudio playerAudio;
    juce::ListBox playlistBox;
    int VideoIndex = -1;
    
    juce::TextButton loadButton{ "Load File" };
    juce::TextButton restartButton{ "Restart" };
    juce::TextButton stopButton{ "Stop" };
    juce::TextButton playButton{ "Play" };
    juce::TextButton loopButton{ "Loop" };
    juce::TextButton goToStartButton{ "Go to Start" };
    juce::TextButton goToEndButton{ "Go to End" };
    juce::TextButton muteButton{ "Mute" };
    juce::TextButton setAButton{ "set A" };
    juce::TextButton setBButton{ "set B" };
    juce::TextButton clearABButton{ "Clear A-B" };

    juce::Slider volumeSlider;
    juce::Slider speedSlider;
    juce::Slider progressSlider; 
     
    juce::Label abLabel;
    juce::Label positionLabel;  
    juce::Label metadataLabel;

    std::unique_ptr<juce::FileChooser> fileChooser;

    bool muted = false;
    float prevVolume = 0.5f; 

    double loopStart = 0.0;
    double loopEnd = 0.0;
    bool isABLooping = false;

    bool isUserDraggingSlider = false; 
    void buttonClicked(juce::Button* button) override;
    void sliderValueChanged(juce::Slider* changedSlider) override;
    void sliderDragStarted(juce::Slider* changedSlider) override;
    void sliderDragEnded(juce::Slider* changedSlider) override;
    void timerCallback() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerGUI)
};