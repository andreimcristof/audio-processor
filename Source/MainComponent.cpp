#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent() : btnRecord("Record")
{
    // Make sure you set the size of the component after
    // you add any child components.
    setSize(800, 600);
    // transportSource.addChangeListener(this);

    // Some platforms require permissions to open input channels so request that here
    if (RuntimePermissions::isRequired(RuntimePermissions::recordAudio) && !RuntimePermissions::isGranted(RuntimePermissions::recordAudio))
    {
        RuntimePermissions::request(RuntimePermissions::recordAudio,
                                    [&](bool granted) { if (granted)  setAudioChannels (2, 2); });
    }
    else
    {
        // Specify the number of input and output channels that we want to open
		int inputChannelCount = 2;
		int outputChannelCount = 2;
        setAudioChannels(inputChannelCount, outputChannelCount);
    }

    btnRecord.onClick = [this] { onBtnRecordClick(); };
    addAndMakeVisible(&btnRecord);

	sldrNoiseLevel.setRange(0.0, 0.25);
	sldrNoiseLevel.setTextBoxStyle(Slider::TextBoxRight, false, 100, 20);
	addAndMakeVisible(sldrNoiseLevel);

	lblNoiseLevel.setText("Noise Level", NotificationType::dontSendNotification);
	addAndMakeVisible(lblNoiseLevel);
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
}

// void MainComponent::changeListenerCallback(ChangeBroadcaster *source) override
// {
// }

//==============================================================================
void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    // This function will be called when the audio device is started, or when
    // its settings (i.e. sample rate, block size, etc) are changed.

    // You can use this function to initialise any resources you might need,
    // but be careful - it will be called on the audio thread, not the GUI thread.

    // For more details, see the help for AudioProcessor::prepareToPlay()
}


void MainComponent::getNextAudioBlock(const AudioSourceChannelInfo &bufferToFill)
{
	// Your audio-processing code goes here!
    // For more details, see the help for AudioProcessor::getNextAudioBlock()

    // Right now we are not producing any data, in which case we need to clear the buffer
    // (to prevent the output of random noise)

	auto* device = deviceManager.getCurrentAudioDevice();
	auto activeInputChannels = device->getActiveInputChannels();
	auto activeOutputChannels = device->getActiveOutputChannels();
	auto maxInputChannels = activeInputChannels.getHighestBit() + 1;
	auto maxOutputChannels = activeOutputChannels.getHighestBit() +1;

	auto level = (float)sldrNoiseLevel.getValue();

	for (auto channel = 0; channel < maxOutputChannels; ++channel) {
		if (!activeOutputChannels[channel] || maxInputChannels == 0)
			outputSilenceOnlyByZeroingOutputChannelBuffer(bufferToFill, channel);
		else {

		}

	}


    bufferToFill.clearActiveBufferRegion();
}

void MainComponent::releaseResources()
{
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.

    // For more details, see the help for AudioProcessor::releaseResources()
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
    // This is called when the MainContentComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
    btnRecord.setBounds(10, 10, getWidth() - 20, 30);
}

void MainComponent::onBtnRecordClick() {
	
}

void MainComponent::outputSilenceOnlyByZeroingOutputChannelBuffer(const AudioSourceChannelInfo& bufferToFill, int& channelIndexToClearSoundOn)
{
	bufferToFill.buffer->clear(channelIndexToClearSoundOn, bufferToFill.startSample, bufferToFill.numSamples);
}
