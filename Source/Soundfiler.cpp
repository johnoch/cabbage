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
			//		waveformDisplay->setFile(openFC.getResult());
		
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

