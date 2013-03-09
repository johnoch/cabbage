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

#ifndef __JUCE_STANDALONEFILTERWINDOW_JUCEHEADER__
#define __JUCE_STANDALONEFILTERWINDOW_JUCEHEADER__

#include "../Editor/CabbageEditorWindow.h"
#include "../CabbageUtils.h"
#include "../CabbageLookAndFeel.h"
#include "../Plugin/CabbagePluginProcessor.h"
#include "../Plugin/CabbagePluginEditor.h"

extern ApplicationProperties* appProperties;
extern PropertySet* defaultPropSet;
extern String currentApplicationDirectory;

//==============================================================================
// This is the main host window. It gets instatiated in StandaloneFilterApp.cpp
class CsoundMessageConsole : public DocumentWindow
{
ScopedPointer<TextEditor> textEditor;
public:
	CsoundMessageConsole(String title, Colour bgcolour, int top, int left): DocumentWindow(title, bgcolour, DocumentWindow::allButtons, true){
	textEditor = new TextEditor();
	textEditor->setColour(TextEditor::backgroundColourId, Colours::black);
	textEditor->setColour(TextEditor::textColourId, Colours::white);
	textEditor->setMultiLine(true);
	setSize(700, 200);
	setTopLeftPosition(top, left);
	setAlwaysOnTop(true);
	setResizable(true, false);
	this->setTitleBarHeight(20);
	textEditor->setBounds(0, 30, getWidth(), getHeight());
	setContentOwned(textEditor, true);	
	};
	
	~CsoundMessageConsole(){};
	
	void closeButtonPressed(){
		setVisible(false);
	}

	void setText(String text){
		textEditor->setText(text);
		textEditor->setCaretPosition(textEditor->getText().length());
	}
};  

//==============================================================================
// This is the main host window. It gets instatiated in StandaloneFilterApp.cpp
//==============================================================================
class StandaloneFilterWindow    :   public DocumentWindow,
									public Button::Listener,
									public ChangeListener,
									public CabbageUtils,
									public Timer,
									public ActionListener
{
public:
    //==============================================================================
    StandaloneFilterWindow (const String& title,
                            const Colour& backgroundColour);

    ~StandaloneFilterWindow();
	//=================================================================
    void resetFilter();
    void saveState();
    void loadState();
    virtual void showAudioSettingsDialog();
    virtual PropertySet* getGlobalSettings();

    //==============================================================================
    void closeButtonPressed();
    void buttonClicked (Button*);
    void showInfoWindow(String text);
	void resized();
	void changeListenerCallback(ChangeBroadcaster *source);
	void actionListenerCallback (const String& message);
	int exportPlugin(String type, bool saveAs);
	void openFile(String _csdfile);
	void saveFile();
	void saveFileAs();

	bool isGuiEnabled(){
		return isGUIOn;
	}

	void setGuiEnabled(bool onoff){
		isGUIOn = onoff;
	}
	
	void setCurrentLine(int line){
		currentLine=line;
	}

	int getCurrentLine(){
		return currentLine;
	}

private:
	bool updateEditorOutputConsole;
	Time lastSaveTime;
	bool standaloneMode;
	bool cabbageDance;
	bool timerRunning;
	bool AudioEnabled;
	bool isAFileOpen;
	int setUniquePluginID(File inFile, File csdFile, bool AU);
	float yAxis;
	void timerCallback();
    ScopedPointer<CabbagePluginAudioProcessor> filter;
    ScopedPointer<AudioDeviceManager> deviceManager;
	void sendMessageToWinXound(String messageType, String message);
	void sendMessageToWinXound(String messageType, int value);
	void BatchProcess(String pluginType);
	AudioProcessorPlayer player;
    TextButton optionsButton;
    void deleteFilter();
	File csdFile, originalCsdFile; 
	bool isGUIOn;
	int currentLine;
	bool pipeOpenedOk;
	ScopedPointer<CabbageLookAndFeel> lookAndFeel;
	ScopedPointer<LookAndFeel> oldLookAndFeel;
	ScopedPointer<CabbageEditorWindow> cabbageCsoundEditor;
	String consoleMessages;
	ScopedPointer<CsoundMessageConsole> outputConsole;
	StringArray previousScoreEvents;
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StandaloneFilterWindow);
};

#endif   // __JUCE_STANDALONEFILTERWINDOW_JUCEHEADER__
