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

#include  "../CabbageCustomWidgets.h"
#include "../../JuceLibraryCode/JuceHeader.h"
#include "CabbagePluginProcessor.h"
#include "../CabbageLookAndFeel.h"



class CabbageMainPanel;
class ComponentLayoutEditor;
//==============================================================================
class CabbagePlantWindow  : public DocumentWindow
{

public:
        CabbagePlantWindow(const String& title, const Colour& backgroundColour)
                : DocumentWindow (title, backgroundColour, DocumentWindow::closeButton){};
        ~CabbagePlantWindow(){
        };

        void closeButtonPressed(){
        setVisible(false);
        };

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CabbagePlantWindow);
};

class CabbagePluginAudioProcessorEditor  : public AudioProcessorEditor,
                                                                public CabbageUtils,
                                                                public SliderListener,
                                                                public ComboBoxListener,
                                                                public ButtonListener,
                                                                public KeyListener,
                                                                public ChangeBroadcaster,
                                                                public ChangeListener,
                                                                public ActionListener, 
																public Timer
																
{
public:
    CabbagePluginAudioProcessorEditor (CabbagePluginAudioProcessor* ownerFilter);
    ~CabbagePluginAudioProcessorEditor();

    //==============================================================================
    // This is just a standard Juce paint method...
    void paint (Graphics& g);
    void resized();
	void setEditMode(bool on);
	void InsertGUIControls();
	void ksmpsYieldCallback();

private:
        
        bool keyPressed(const juce::KeyPress &,Component *);
        void InsertGroupBox(CabbageGUIClass &cAttr);
        void comboBoxChanged (ComboBox* combo);
        void InsertComboBox(CabbageGUIClass &cAttr);
        void SetupWindow(CabbageGUIClass &cAttr);
        void InsertSlider(CabbageGUIClass &cAttr);
		void sliderValueChanged (Slider*);
        void InsertButton(CabbageGUIClass &cAttr);
        void InsertSourceButton(CabbageGUIClass &cAttr);
        void InsertVUMeter(CabbageGUIClass &cAttr);
        void InsertCheckBox(CabbageGUIClass &cAttr);
        void InsertCsoundOutput(CabbageGUIClass &cAttr);
        void InsertMIDIKeyboard(CabbageGUIClass &cAttr);
        void InsertXYPad(CabbageGUIClass &cAttr);
        void InsertImage(CabbageGUIClass &cAttr);
        void InsertLabel(CabbageGUIClass &cAttr);
        void InsertTable(CabbageGUIClass &cAttr);
        void InsertInfoButton(CabbageGUIClass &cAttr);
        void InsertLineSeparator(CabbageGUIClass &cAttr);
        void InsertPatternMatrix(CabbageGUIClass &cAttr);
        void InsertSnapshot(CabbageGUIClass &cAttr);
        void InsertPVSViewer(CabbageGUIClass &cAttr);
        void buttonClicked (Button*);
        void mouseDown(const MouseEvent &e);
		void timerCallback();
        bool LOCKED;
        void insertCabbageText(String text);
        int lineNumber;
        File SnapShotFile;
        String presetFileText;
        void actionListenerCallback (const String& message);
        int zero_dbfs;
		StringArray tempArray;


        //CabbagePluginAudioProcessor* filter;
        CabbagePluginAudioProcessor* getFilter() const
			{
				return static_cast <CabbagePluginAudioProcessor*> (getAudioProcessor());
			}

	    bool keyStateChanged(bool onoff){
	    keyIsPressed = onoff;
		return false;
	    }
		bool keyIsPressed;
        OwnedArray<Component> controls;
        OwnedArray<Component> layoutComps;

		void positionComponentWithinPlant(String type, int idx, float left, float top, float width, float height, Component *layout, Component *control);
        //ScopedPointer<CabbagePlantWindow> subPatch;
        OwnedArray<CabbageButton> plantButton;
        OwnedArray<CabbagePlantWindow> subPatch;
        ScopedPointer<InfoWindow> infoWindow;
#ifdef Cabbage_GUI_Editor
        ScopedPointer<CabbageMainPanel> componentPanel;
        ScopedPointer<ComponentLayoutEditor> layoutEditor;
#else
        ScopedPointer<Component> componentPanel;
#endif
        ScopedPointer<MidiKeyboardComponent> midiKeyboard;
        ScopedPointer<LookAndFeel> feely;
        ScopedPointer<OldSchoolLookAndFeel> oldSchoolLook;

        AudioPlayHead::CurrentPositionInfo hostInfo;
    void changeListenerCallback (ChangeBroadcaster *source);
        Colour formColour, fontColour;
        String authorText;
        String formPic;
        float inValue;
		int xyPadIndex;
        ScopedPointer<CabbageLookAndFeel> lookAndFeel;
        ScopedPointer<Label> debugLabel;
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CabbagePluginAudioProcessorEditor);

};


//==============================================================================
/**
*/

#endif  // __PLUGINEDITOR_H_F4EBBBA1__

