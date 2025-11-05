#include "PlayerGUI.h"
#include "PlayerAudio.h"

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

void PlayerGUI::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour::fromRGB(20, 30, 60));
}

PlayerGUI::PlayerGUI()
{
    for (auto* btn : { &loadButton, &restartButton, &stopButton, &playButton, &loopButton,
                       &goToStartButton, &goToEndButton, &muteButton, &setAButton, &setBButton, &clearABButton })
    {
        btn->addListener(this);
        addAndMakeVisible(btn);
    }
    playlistBox.setModel(this);
    addAndMakeVisible(playlistBox);
    volumeSlider.setRange(0.0, 1.0, 0.01);
    volumeSlider.setValue(prevVolume);
    volumeSlider.addListener(this);
    addAndMakeVisible(volumeSlider);
    speedSlider.setRange(0.5, 2.0, 0.01);
    speedSlider.setValue(1.0);
    speedSlider.addListener(this);
    speedSlider.setSliderStyle(juce::Slider::Rotary);
    speedSlider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::orange);
    speedSlider.setColour(juce::Slider::thumbColourId, juce::Colours::red);
    speedSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    addAndMakeVisible(speedSlider);




    addAndMakeVisible(progressSlider);
    progressSlider.setRange(0.0, 1.0);
    progressSlider.setValue(0.0);
    progressSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    progressSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    progressSlider.setChangeNotificationOnlyOnRelease(false);
    progressSlider.addListener(this);


    positionLabel.setText("0:00 / 0:00", juce::dontSendNotification);
    addAndMakeVisible(positionLabel);



    addAndMakeVisible(abLabel);
    abLabel.setText("A: -- B: -- ", juce::dontSendNotification);
    abLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    abLabel.setJustificationType(juce::Justification::centred);

    addAndMakeVisible(metadataLabel);
    metadataLabel.setText("No file loaded", juce::dontSendNotification);
    metadataLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    metadataLabel.setJustificationType(juce::Justification::centredLeft);


    startTimerHz(25);
    setSize(900, 260);

    loopButton.setButtonText("Loop:OFF");
    stopButton.setColour(juce::TextButton::buttonColourId, juce::Colours::red);
    playButton.setColour(juce::TextButton::buttonColourId, juce::Colours::green);
}

void PlayerGUI::resized()
{
    int margin = 10;
    int y = margin;
    int buttonW = 90;
    int buttonH = 28;


    loadButton.setBounds(margin, y, buttonW, buttonH);
    restartButton.setBounds(margin + 100, y, buttonW, buttonH);
    stopButton.setBounds(margin + 200, y, buttonW, buttonH);
    playButton.setBounds(margin + 300, y, buttonW, buttonH);
    loopButton.setBounds(margin + 400, y, buttonW + 20, buttonH);
    goToStartButton.setBounds(margin + 520, y, buttonW + 20, buttonH);
    goToEndButton.setBounds(margin + 640, y, buttonW + 20, buttonH);
    muteButton.setBounds(margin + 760, y, buttonW, buttonH);


    y += buttonH + 12;
    volumeSlider.setBounds(margin, y, getWidth() - 2 * margin - 320, 20);


    y += 30;
    metadataLabel.setBounds(margin, y, getWidth() - 2 * margin - 320, 24);
    y += 30;
    progressSlider.setBounds(margin, y, getWidth() - 2 * margin - 320, 20);
    positionLabel.setBounds(margin, y + 25, 120, 20);


    y += 50;

    setAButton.setBounds(margin + 180, y - 3, 70, 26);
    setBButton.setBounds(margin + 260, y - 3, 70, 26);
    clearABButton.setBounds(margin + 340, y - 3, 90, 26);
    abLabel.setBounds(margin + 450, y - 3, 140, 26);
    speedSlider.setBounds(margin, y, 150, 150);

    playlistBox.setBounds(getWidth() - 310, margin + 10, 300, getHeight() - 40);
}
PlayerGUI::~PlayerGUI()
{
    stopTimer();
}
int PlayerGUI::getNumRows() {
    return (int)playerAudio.playlistFiles.size();
}
void PlayerGUI::paintListBoxItem(int rowNumber, juce::Graphics& g, int width, int height, bool rowIsSelected) {
    if (rowIsSelected)
        g.fillAll(juce::Colours::blue);
    else
        g.fillAll(juce::Colours::white);
    if (rowNumber < (int)playerAudio.playlistFiles.size()) {
        g.setColour(juce::Colours::black);
        g.drawText(playerAudio.playlistFiles[rowNumber].getFileName(), 5, 0, width, height, juce::Justification::centredLeft);
    }
}
void PlayerGUI::listBoxItemClicked(int row, const juce::MouseEvent&)
{
    if (row >= 0 && row < playerAudio.playlistSources.size()) {
        VideoIndex = row;
        playerAudio.playsound(row);
    }
}

void PlayerGUI::buttonClicked(juce::Button* button)
{
    if (button == &loadButton)
    {
        fileChooser = std::make_unique<juce::FileChooser>("Select audio files...", juce::File{}, "*.wav;*.mp3");

        fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectMultipleItems,
            [this](const juce::FileChooser& chooser)
            {
                auto files = chooser.getResults();
                for (auto& f : files) {
                    playerAudio.addFileToPlaylist(f);
                    if (files.size() > 0)
                    {
                        auto file = files[0];
                        juce::AudioFormatManager manager;
                        manager.registerBasicFormats();

                        if (auto* reader = manager.createReaderFor(file))
                        {
                            double duration = reader->lengthInSamples / reader->sampleRate;
                            metadataLabel.setText("File: " + file.getFileName() +
                                " | Duration: " + juce::String(duration, 2) + " sec",
                                juce::dontSendNotification);
                            delete reader;
                        }
                        else
                        {
                            metadataLabel.setText("File: " + file.getFileName(), juce::dontSendNotification);
                        }
                    }

                    playlistBox.updateContent();
                    playlistBox.repaint();
                }
            });
    }

    else if (button == &restartButton)
    {
        playerAudio.stop();
        playerAudio.setPosition(0.0);
        playerAudio.play();
    }
    else if (button == &stopButton)
    {
        playerAudio.stop();
    }
    else if (button == &playButton)
    {
        playerAudio.play();
    }
    else if (button == &loopButton)
    {
        bool newstate = !playerAudio.isLooping();
        playerAudio.setLooping(newstate);
        if (newstate)
        {
            loopButton.setButtonText("Loop: ON");
            loopButton.setColour(juce::TextButton::buttonColourId, juce::Colours::green);
        }
        else
        {
            loopButton.setButtonText("Loop: OFF");
            loopButton.setColour(juce::TextButton::buttonColourId, juce::Colours::red);
        }
    }
    else if (button == &goToStartButton)
    {
        playerAudio.setPosition(0.0);
    }
    else if (button == &goToEndButton)
    {
        double length = playerAudio.getLength();
        if (length > 0)
        {
            playerAudio.setPosition(length - 1.0);
            playerAudio.play();
        }
    }
    else if (button == &muteButton)
    {
        if (!muted)
        {
            prevVolume = (float)volumeSlider.getValue();
            playerAudio.setGain(0.0f);
            volumeSlider.setValue(0.0);
            muteButton.setButtonText("UnMute");
            muteButton.setColour(juce::TextButton::buttonColourId, juce::Colours::red);
            muted = true;
        }
        else
        {
            volumeSlider.setValue(prevVolume);
            playerAudio.setGain(prevVolume);
            muteButton.setButtonText("Mute");
            muteButton.setColour(juce::TextButton::buttonColourId, juce::Colours::green);
            muted = false;
        }
    }
    else if (button == &setAButton)
    {
        loopStart = playerAudio.getPosition();
        isABLooping = false;

        int aMins = int(loopStart) / 60;
        int aSecs = int(loopStart) % 60;

        abLabel.setText("A: " + juce::String(aMins).paddedLeft('0', 2) + ":" + juce::String(aSecs).paddedLeft('0', 2) + " B: --", juce::dontSendNotification);
    }
    else if (button == &setBButton)
    {
        loopEnd = playerAudio.getPosition();
        if (loopEnd > loopStart)
        {
            isABLooping = true;

            int aMins = int(loopStart) / 60;
            int aSecs = int(loopStart) % 60;
            int bMins = int(loopEnd) / 60;
            int bSecs = int(loopEnd) % 60;

            abLabel.setText("A: " + juce::String(aMins).paddedLeft('0', 2) + ":" + juce::String(aSecs).paddedLeft('0', 2) +
                " B: " +
                juce::String(bMins).paddedLeft('0', 2) + ":" + juce::String(bSecs).paddedLeft('0', 2),
                juce::dontSendNotification);
        }
        else
        {
            juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon, "Error", "B must be after A!");
        }
    }
    else if (button == &clearABButton)
    {
        loopStart = 0.0;
        loopEnd = 0.0;
        isABLooping = false;
        abLabel.setText("A: -- B: --", juce::dontSendNotification);
        setAButton.setColour(juce::TextButton::buttonColourId, juce::Colours::lightgrey);
        setBButton.setColour(juce::TextButton::buttonColourId, juce::Colours::lightgrey);
    }
}

void PlayerGUI::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &volumeSlider)
    {
        playerAudio.setGain((float)slider->getValue());
    }
    else if (slider == &speedSlider)
    {
        playerAudio.setSpeed((double)speedSlider.getValue());
    }
    else if (slider == &progressSlider && !isUserDraggingSlider)
    {
        double length = playerAudio.getLength();
        if (length > 0)
        {
            double newPosition = progressSlider.getValue() * length;
            playerAudio.setPosition(newPosition);
        }
    }
    else if (slider == &progressSlider)
    {
        double length = playerAudio.getLength();
        if (length > 0)
        {
            playerAudio.setPosition(length * progressSlider.getValue());
        }
    }
}

void PlayerGUI::sliderDragStarted(juce::Slider* changedSlider)
{
    if (changedSlider == &progressSlider)
        isUserDraggingSlider = true;
}

void PlayerGUI::sliderDragEnded(juce::Slider* changedSlider)
{
    if (changedSlider == &progressSlider)
    {
        isUserDraggingSlider = false;

        double length = playerAudio.getLength();
        if (length > 0)
        {
            double newPosition = progressSlider.getValue() * length;
            playerAudio.setPosition(newPosition);
        }
    }
}

void PlayerGUI::timerCallback()
{
    double pos = playerAudio.getPosition();
    double length = playerAudio.getLength();
    if (length > 0)
    {
        if (!isUserDraggingSlider) {
            progressSlider.setValue(pos / length, juce::dontSendNotification);

            int currentMins = int(pos) / 60;
            int currentSecs = int(pos) % 60;
            int totalMins = int(length) / 60;
            int totalSecs = int(length) % 60;

            positionLabel.setText(juce::String(currentMins) + ":" + juce::String(currentSecs).paddedLeft('0', 2) +
                " / " + juce::String(totalMins) + ":" + juce::String(totalSecs).paddedLeft('0', 2),
                juce::dontSendNotification);
        }

    }

    if (isABLooping)
    {
        double cur = playerAudio.getPosition();
        if (cur >= loopEnd)
            playerAudio.setPosition(loopStart);
    }
}
