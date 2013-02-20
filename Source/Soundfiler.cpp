#include "Soundfiler.h"


Soundfiler::Soundfiler() : 
zoomSlider(0),
thread ("audio file preview"),
soundfileWaveform(0)
{
formatManager.registerBasicFormats();
thread.startThread (3);
zoomSlider = new Slider (String::empty);
soundfileWaveform = new SoundfileWaveform (formatManager, transportSource, *zoomSlider);
addAndMakeVisible(soundfileWaveform);

audioSourcePlayer.setSource (&transportSource);
transportSource.start();
}

Soundfiler::~Soundfiler()
{
    transportSource.setSource (nullptr);
    audioSourcePlayer.setSource (nullptr);	
}


void Soundfiler::paint (Graphics& g)
{
    g.fillAll (Colours::white);
}

void Soundfiler::resized()
{
    soundfileWaveform->setBounds (0 , 0, getWidth(), getHeight());
}

void Soundfiler::loadFileIntoTransport (const File& audioFile)
{
    // unload the previous file source and delete it..
    transportSource.stop();
    transportSource.setSource (nullptr);
    currentAudioFileSource = nullptr;

    AudioFormatReader* reader = formatManager.createReaderFor (audioFile);

    if (reader != nullptr)
    {
        currentAudioFileSource = new AudioFormatReaderSource (reader, true);

        // ..and plug it into our transport source
        transportSource.setSource (currentAudioFileSource,
                                   32768, // tells it to buffer this many samples ahead
                                   &thread, // this is the background thread to use for reading-ahead
                                   reader->sampleRate);
    }
}

void Soundfiler::showFile (const File& file)
{
	jassert(File(file).exists());
    loadFileIntoTransport (file);

    zoomSlider->setValue (0, dontSendNotification);
    soundfileWaveform->setFile (file);
	
}
