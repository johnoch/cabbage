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

#ifndef SOUNDFILEWAVEFORM_H
#define SOUNDFILEWAVEFORM_H

#include "../JuceLibraryCode/JuceHeader.h" 
#include "CabbageUtils.h"

class SoundfileWaveform :  public Component,
                           public ChangeListener,
                           public FileDragAndDropTarget,
                           private Timer
{
public:
    SoundfileWaveform (AudioFormatManager& formatManager,
                       AudioTransportSource& transportSource_,
                       Slider& zoomSlider_)
        : transportSource (transportSource_),
          zoomSlider (zoomSlider_),
          thumbnailCache (5),
          thumbnail (512, formatManager, thumbnailCache)
    {
        startTime = endTime = 0;
        thumbnail.addChangeListener (this);

        currentPositionMarker.setFill (Colours::purple.withAlpha (0.7f));
        addAndMakeVisible (&currentPositionMarker);
    }

    ~SoundfileWaveform()
    {
        thumbnail.removeChangeListener (this);
    }

    void setFile (const File& file)
    {
        thumbnail.setSource (new FileInputSource (file));
        startTime = 0;
        endTime = thumbnail.getTotalLength();
        startTimer (1000 / 40);
    }

    void setZoomFactor (double amount)
    {
        if (thumbnail.getTotalLength() > 0)
        {
            const double newScale = jmax (0.001, thumbnail.getTotalLength() * (1.0 - jlimit (0.0, 0.99, amount)));
            const double timeAtCentre = xToTime (getWidth() / 2.0f);
            startTime = timeAtCentre - newScale * 0.5;
            endTime = timeAtCentre + newScale * 0.5;
            repaint();
        }
    }

    void mouseWheelMove (const MouseEvent&, const MouseWheelDetails& wheel)
    {
        if (thumbnail.getTotalLength() > 0)
        {
            double newStart = startTime - wheel.deltaX * (endTime - startTime) / 10.0;
            newStart = jlimit (0.0, jmax (0.0, thumbnail.getTotalLength() - (endTime - startTime)), newStart);
            endTime = newStart + (endTime - startTime);
            startTime = newStart;

            if (wheel.deltaY != 0)
                zoomSlider.setValue (zoomSlider.getValue() - wheel.deltaY);

            repaint();
        }
    }

    void paint (Graphics& g)
    {
        g.fillAll (Colours::black);
        g.setColour (Colours::lime);

        if (thumbnail.getTotalLength() > 0)
        {
            thumbnail.drawChannels (g, getLocalBounds().reduced (2),
                                    startTime, endTime, 1.0f);
        }
        else
        {
            g.setFont (14.0f);
            g.drawFittedText ("(No audio file selected)", getLocalBounds(), Justification::centred, 2);
        }
    }

    void changeListenerCallback (ChangeBroadcaster*)
    {
        // this method is called by the thumbnail when it has changed, so we should repaint it..
        repaint();
    }

    bool isInterestedInFileDrag (const StringArray& /*files*/)
    {
        return true;
    }

    void filesDropped (const StringArray& files, int, int )
    {
 /*       AudioDemoPlaybackPage* demoPage = findParentComponentOfClass<AudioDemoPlaybackPage>();

        if (demoPage != nullptr)
            demoPage->showFile (File (files[0]));
			 */
    }

    void mouseDown (const MouseEvent& e)
    {
        mouseDrag (e);
    }

    void mouseDrag (const MouseEvent& e)
    {
        transportSource.setPosition (jmax (0.0, xToTime ((float) e.x)));
    }

    void mouseUp (const MouseEvent&)
    {
        transportSource.start();
    }

    void timerCallback()
    {
        currentPositionMarker.setVisible (transportSource.isPlaying() || isMouseButtonDown());

        double currentPlayPosition = transportSource.getCurrentPosition();

        currentPositionMarker.setRectangle (Rectangle<float> (timeToX (currentPlayPosition) - 0.75f, 0,
                                                              1.5f, (float) getHeight()));
    }
	 AudioTransportSource& transportSource;

private:
   
    Slider& zoomSlider;
    AudioThumbnailCache thumbnailCache;
    AudioThumbnail thumbnail;
    double startTime, endTime;

    DrawableRectangle currentPositionMarker;

    float timeToX (const double time) const
    {
        return getWidth() * (float) ((time - startTime) / (endTime - startTime));
    }

    double xToTime (const float x) const
    {
        return (x / getWidth()) * (endTime - startTime) + startTime;
    }
	
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SoundfileWaveform);
	
};


class Soundfiler : public Component
{
public:
	Soundfiler();
	~Soundfiler();
	
    void selectionChanged(){};
    void fileClicked (const File& file, const MouseEvent& e){};
    void fileDoubleClicked (const File& file){};
    void browserRootChanged (const File&) {}
    void showFile (const File& file);
    //[/UserMethods]

    void paint (Graphics& g);
    void resized();
    void sliderValueChanged (Slider* sliderThatWasMoved){};
    void buttonClicked (Button* buttonThatWasClicked){};	
	AudioTransportSource transportSource;
	
private:
//    AudioDeviceManager& deviceManager;
    AudioFormatManager formatManager;
    TimeSliceThread thread;
    //DirectoryContentsList directoryList;
	ScopedPointer<Slider> zoomSlider;
    AudioSourcePlayer audioSourcePlayer;
    
    ScopedPointer<AudioFormatReaderSource> currentAudioFileSource;

    void loadFileIntoTransport (const File& audioFile);
	ScopedPointer<SoundfileWaveform> soundfileWaveform;
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Soundfiler);

};

#endif // SOUNDFILEWAVEFORM_H
