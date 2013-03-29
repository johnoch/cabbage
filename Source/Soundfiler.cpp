/*
  Copyright (C) 2009 Rory Walsh

  Cabbage is free software; you can redistribute it
  and/or modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.   

  Cabbage is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with Csound; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
  02111-1307 USA
*/

#include "Soundfiler.h"

//=================================================================
// display a sound file as a waveform..
//=================================================================
class WaveformDisplay : public Component,
						public Timer
{
public:
	WaveformDisplay(AudioFormatManager& formatManager, BufferingAudioSource &source, int sr):
	thumbnailCache (5), 
	thumbnail (512, formatManager, thumbnailCache), 
	source(source), 
	sampleRate(sr),
	currentPlayPosition(0),
	mouseDownX(0),
	mouseUpX(0)
	{	
	setSize(100, 100);
	}
	
	~WaveformDisplay()
	{	
	}
	
	void resized()
	{
	repaint();	
	}
	
    void setFile (const File& file)
    {
		if(file.existsAsFile()){
			thumbnail.clear();
			thumbnail.setSource (new FileInputSource(file));
		}
		startTime = 0;
        endTime = thumbnail.getTotalLength();
		repaint();
    }

    void setZoomFactor (double amount)
    {
        if (thumbnail.getTotalLength() > 0)
        {
            const double newScale = jmax (0.001, thumbnail.getTotalLength() * (1.0 - jlimit (0.0, 0.99, amount)));
            const double timeAtCentre = xToTime (getWidth() / 2.0f);
            //startTime = timeAtCentre - newScale * 0.5;
            //endTime = timeAtCentre + newScale * 0.5;
            repaint();
        }
    }

    void paint (Graphics& g)
    {
        g.fillAll (Colours::black);
        g.setColour (Colours::lime);
        if (thumbnail.getTotalLength() > 0)
        {
            thumbnail.drawChannels (g, getLocalBounds(),
                                    startTime, endTime, 2.0f);
        }
        else
        {
            g.setFont (14.0f);
            g.drawFittedText ("(No audio file selected)", getLocalBounds(), Justification::centred, 2);
        }
		g.setColour(Colours::yellow.withAlpha(.6f));
		//g.fillRect(mouseDownX ,0 , mouseUpX - mouseDownX, getHeight());
		g.drawLine(timeToX(currentPlayPosition), 0, timeToX(currentPlayPosition), getHeight(), 2);
    }

    void timerCallback()
    {
		Viewport* const viewport = findParentComponentOfClass <Viewport> (); //Get the parent viewport
		if(viewport != nullptr) //Check for nullness
		viewport->setViewPosition(jmax(0.f, timeToX(currentPlayPosition)-100), 0);
		currentPlayPosition = source.getNextReadPosition()/sampleRate;		
		repaint();
    }

    void mouseDown (const MouseEvent& e)
    {
		if(e.mods.isCommandDown()){
		if(e.mods.isLeftButtonDown())
			this->setSize(jmin(1000, this->getWidth()+100), getHeight());
		else
			this->setSize(jmax(400, this->getWidth()-100), getHeight());
			Logger::writeToLog("command down");
		}
			
		source.setNextReadPosition (jmax (0.0, xToTime ((float) e.x)*sampleRate));
		currentPlayPosition = jmax (0.0, xToTime ((float) e.x));
		mouseDownX = e.x;
		repaint();
    }

    void mouseDrag(const MouseEvent& e)
    {
        mouseUpX = e.x;
        repaint();
    }
	
    AudioThumbnailCache thumbnailCache;
    AudioThumbnail thumbnail;
	
private:
    BufferingAudioSource& source;
	int mouseDownX, mouseUpX;
    double startTime, endTime;
	Rectangle<int> localBounds;
	float sampleRate;
	double currentPlayPosition;

    DrawableRectangle currentPositionMarker;

    float timeToX (const double time) const
    {
        return getWidth() * (float) ((time - startTime) / (endTime - startTime));
    }

    double xToTime (const float x) const
    {
        return (x / getWidth()) * (endTime - startTime) + startTime;
    }
};

//==============================================================================
// soundfiler component
//==============================================================================
Soundfiler::Soundfiler(CabbageAudioSource& _audioSource, String fileName, int sr)
:cabbageAudioSource(&_audioSource)
{
    AudioFormatManager formatManager;
    formatManager.registerBasicFormats();  
	viewport = new Viewport();
	waveformDisplay = new WaveformDisplay(formatManager, *cabbageAudioSource->audioSourceBuffer, sr);
	
	startStop = new TextButton("PlayButton");
	startStop->setButtonText("Play");
	loadFile = new TextButton("Open File");
	startStop->addListener(this);
	loadFile->addListener(this);

	addAndMakeVisible(startStop);
	addAndMakeVisible(loadFile);


	addAndMakeVisible(viewport);
    setSize (400, 300);

	waveformDisplay->setFile(File(fileName));

	viewport->setViewedComponent(waveformDisplay, false);
	viewport->setScrollBarsShown(true, true);
}

void Soundfiler::buttonClicked(Button *button)
{
	if(button->getName()=="PlayButton"){
		if(!cabbageAudioSource->isSourcePlaying){
			waveformDisplay->startTimer(200);
			startStop->setButtonText("Stop..");
		}
		else{
			waveformDisplay->stopTimer();
			startStop->setButtonText("Play");
		}
		cabbageAudioSource->isSourcePlaying=!cabbageAudioSource->isSourcePlaying;
	}
	else{
		FileChooser openFC(String("Open a Cabbage .csd file..."), File::nonexistent, String("*.wav;*.mp3"));
		if(openFC.browseForFileToOpen())
			cabbageAudioSource->setFile(openFC.getResult().getFullPathName(), 2);
					waveformDisplay->setFile(openFC.getResult());
		
	}
}
//==============================================================================
void Soundfiler::resized()
{
waveformDisplay->setSize(800, getHeight()-40);
viewport->setBounds(0, 0, getWidth(), getHeight()-20);
startStop->setBounds(0, getHeight()-20, 70, 20);
loadFile->setBounds(70, getHeight()-20, 70, 20);
}
//==============================================================================
void Soundfiler::paint (Graphics& g)
{
g.fillAll(Colours::black);
}

