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

#ifndef __PLUGINEDITOR_H_F4EBBBA1__
#define __PLUGINEDITOR_H_F4EBBBA1__


#include "../JuceLibraryCode/JuceHeader.h"
#include "../JuceLibraryCode/JucePluginCharacteristics.h"
#include "CabbagePluginProcessor.h"

class CabbageMainPanel;
class ComponentLayoutEditor;
//==============================================================================
class CabbagePluginAudioProcessorEditor  : public AudioProcessorEditor,
								public CabbageUtils,
								public SliderListener,
								public ComboBoxListener,
								public ButtonListener,
								public Timer,
								public KeyListener,
								public ChangeBroadcaster,
								public ChangeListener
{
public:
    CabbagePluginAudioProcessorEditor (CabbagePluginAudioProcessor* ownerFilter);
    ~CabbagePluginAudioProcessorEditor();

    //==============================================================================
    // This is just a standard Juce paint method...
    void paint (Graphics& g);
	void resized();
private:
	void InsertGUIControls();
	bool keyPressed(const juce::KeyPress &,juce::Component *);
	void InsertGroupBox(CabbageGUIClass cAttr);
	void comboBoxChanged (ComboBox* combo);
	void InsertComboBox(CabbageGUIClass cAttr);
	void SetupWindow(CabbageGUIClass cAttr);
	void InsertSlider(CabbageGUIClass cAttr); 
    void sliderValueChanged (Slider*);
	void InsertButton(CabbageGUIClass cAttr);
	void InsertCheckBox(CabbageGUIClass cAttr);
	void InsertMIDIKeyboard(CabbageGUIClass cAttr);
	void InsertImage(CabbageGUIClass cAttr);
	void InsertLabel(CabbageGUIClass cAttr);
	void buttonClicked (Button*);
	void timerCallback();
	void mouseDown(const MouseEvent &e);
	bool LOCKED;
	int lineNumber;
	//CabbagePluginAudioProcessor* filter;
	CabbagePluginAudioProcessor* getFilter() const
    {
        return static_cast <CabbagePluginAudioProcessor*> (getAudioProcessor());
    }

	OwnedArray<Component> controls;
	OwnedArray<Component> layoutComps;
#ifdef Cabbage_GUI_Editor
	ScopedPointer<CabbageMainPanel> componentPanel;
	ScopedPointer<ComponentLayoutEditor> layoutEditor;
#else
	ScopedPointer<Component> componentPanel;
#endif
	ScopedPointer<MidiKeyboardComponent> midiKeyboard;
	ScopedPointer<LookAndFeel> feely;
	AudioPlayHead::CurrentPositionInfo hostInfo;
    void changeListenerCallback (ChangeBroadcaster *source);
	Colour formColour;
	String formPic;
	ScopedPointer<Label> debugLabel;
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CabbagePluginAudioProcessorEditor);
};


//==============================================================================
/**
*/
class imageComponent : public Component,
						public CabbageUtils
{
public:
	imageComponent(String name, String file, int top, int left, int width, int height, String outline, String fill, String shape, int line):
	picFile(file), file(file), top(top), left(left), width(width), height(height), fill(fill), outline(outline), shape(shape), line(line){
		widget = new Component(name);

		img = ImageCache::getFromFile (File (file));
	}
	~imageComponent(){
	}

private:
	ScopedPointer<Component> widget;
	Image img;
	int top, left, width, height, line;
	String fill, outline, shape, file;
	File picFile;

	
	

	void paint (Graphics& g){
		if(file.length()>5){
		g.drawImage(img, 0, 0, width, height, 0, 0, img.getWidth(), img.getHeight());
		}
		else{
			if(shape.contains("rounded")){
				g.setColour(Colours::findColourForName(outline, Colours::black));
				g.fillRoundedRectangle(0,0, width, height, width*.02);
				g.setColour(Colours::findColourForName(fill, Colours::black));
				g.fillRoundedRectangle(line,line, width-(line*2), height-(line*2), width*.02);				
			}
			else if(shape.contains("ellipse")){
				g.setColour(Colours::findColourForName(outline, Colours::black));
				g.fillEllipse(0,0, width, height);
				g.setColour(Colours::findColourForName(fill, Colours::black));
				g.fillEllipse(line,line, width-(line*2), height-(line*2));				
			}
			else if(shape.contains("sharp")){
				
				g.setColour(Colours::findColourForName(outline, Colours::black));
				g.fillRect(0,0, width, height);
				g.setColour(Colours::findColourForName(fill, Colours::black));
				g.fillRect(line,line, width-(line*2), height-(line*2));				
			}
		}

	}
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (imageComponent);
};


#endif  // __PLUGINEDITOR_H_F4EBBBA1__
