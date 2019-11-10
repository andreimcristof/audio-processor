#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent(): btnRecord("Record")
{
	for each (AudioIODeviceType * t in deviceManager.getAvailableDeviceTypes())
	{
		t->scanForDevices();
		auto lst = t->getDeviceNames();
	};

	btnRecord.onClick = [this] { onBtnRecordClick(); };
	addAndMakeVisible(&btnRecord);

	sldrNoiseLevel.setRange(0.0, 100.0, 0.5);
	sldrNoiseLevel.setTextBoxStyle(Slider::TextBoxRight, false, 100, 50);
	addAndMakeVisible(sldrNoiseLevel);

	lblNoiseLevel.setText("Noise Level", NotificationType::dontSendNotification);
	addAndMakeVisible(lblNoiseLevel);
	// Make sure you set the size of the component after
	// you add any child components.
	setSize(800, 600);

	// Some platforms require permissions to open input channels so request that here
    if (RuntimePermissions::isRequired(RuntimePermissions::recordAudio) && !RuntimePermissions::isGranted(RuntimePermissions::recordAudio))
    {
        RuntimePermissions::request(RuntimePermissions::recordAudio,
                                    [&](bool granted) { if (granted)  setAudioChannels (2, 2); });
    }
    else
    {
        setAudioChannels(2, 2);
    }


}

MainComponent::~MainComponent()
{
    shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
	auto t = deviceManager.getCurrentAudioDevice()->getName();
	String message;
	message << "Preparing to play audio..." << newLine;
	message << " samplesPerBlockExpected = " << samplesPerBlockExpected << newLine;
	message << " sampleRate = " << sampleRate;
	Logger::getCurrentLogger()->writeToLog(message);
}

void MainComponent::getNextAudioBlock(const AudioSourceChannelInfo &bufferToFill)
{
    auto *device = deviceManager.getCurrentAudioDevice();
    auto activeInputChannels = device->getActiveInputChannels();
    auto activeOutputChannels = device->getActiveOutputChannels();
    auto maxInputChannels = activeInputChannels.getHighestBit() +1;
    auto maxOutputChannels = activeOutputChannels.getHighestBit() +1;
	std::cout << "maxOutputChannels: " << maxOutputChannels;
    auto sldrLevel = (float)sldrNoiseLevel.getValue();

    for (auto channel = 0; channel < maxOutputChannels; ++channel)
    {
        if (!activeOutputChannels[channel] || maxInputChannels == 0)
            outputSilenceOnlyByZeroingOutputChannelBuffer(bufferToFill, channel);
        else
        {
            auto actualInputChannel = channel % maxInputChannels;
            if (!activeInputChannels[channel])
                outputSilenceOnlyByZeroingOutputChannelBuffer(bufferToFill, channel);
            else
            {
                auto *inputBuffer = bufferToFill.buffer->getReadPointer(actualInputChannel, bufferToFill.startSample);
                auto *outputBuffer = bufferToFill.buffer->getWritePointer(channel, bufferToFill.startSample);
                for (auto sample = 0; sample < bufferToFill.numSamples; ++sample)
                {
                    std::cout << "adding noise on channel" << channel << "\n";
                    outputBuffer[sample] = inputBuffer[sample] * random.nextFloat() * sldrLevel;
                }
            }
        }
    }
}

void MainComponent::releaseResources()
{
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.
}

//==============================================================================
void MainComponent::paint(Graphics &g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));

    // You can add your drawing code here!
}

void MainComponent::resized()
{
    btnRecord.setBounds(10, 10, getWidth() - 20, 30);
    sldrNoiseLevel.setBounds(10, 30, getWidth() - 60, 90);
}

void MainComponent::onBtnRecordClick()
{
}

void MainComponent::outputSilenceOnlyByZeroingOutputChannelBuffer(const AudioSourceChannelInfo &bufferToFill, int channelIndexToClearSoundOn)
{
    bufferToFill.buffer->clear(channelIndexToClearSoundOn, bufferToFill.startSample, bufferToFill.numSamples);
}
