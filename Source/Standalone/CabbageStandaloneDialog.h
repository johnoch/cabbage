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
//==============================================================================
class StandaloneFilterWindow    : public DocumentWindow,
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
	//============ socket server class ================================
	//=================================================================
	class socketServer   : public InterprocessConnectionServer
	{
		StandaloneFilterWindow& owner;

	public:
		socketServer (StandaloneFilterWindow& owner_) : owner (owner_){}

		~socketServer(){}

		InterprocessConnection* createConnectionObject(){
			socketConnection* newConnection = new socketConnection(owner);
			owner.activeConnections.add (newConnection);
			return newConnection;
		}
	};

	 //==================== socket connection class ====================
	class socketConnection  : public InterprocessConnection,
								public CabbageUtils
	{
		StandaloneFilterWindow& owner;
		int ourNumber;

	public:
		socketConnection (StandaloneFilterWindow& owner_) : InterprocessConnection (true), owner (owner_){
			static int totalConnections = 0;
			ourNumber = ++totalConnections;
		}

		~socketConnection(){}

		void connectionMade(){
			Logger::writeToLog(String("Connection #") + String (ourNumber) + String(" - connection started"));
		}

		void connectionLost(){
			showMessage("Connection with WinXound has been lost, please restart Cabbage", nullptr);
			Logger::writeToLog(String("Connection #") + String (ourNumber) + String(" - connection lost"));
		}

		void messageReceived (const MemoryBlock& message){
			if(message.toString().contains("UPDATE")){
				String filename = message.toString().trimCharactersAtStart(String("CABBAGE_UPDATE|"));
				//Logger::writeToLog(filename.trim());
				owner.csdFile = File(filename.trim());
				//owner.OPENEDFROMDISK = true;
				owner.resetFilter();
				//owner.toFront(true);
				Logger::writeToLog(String("Cabbage Updated"));
#ifdef Cabbage_GUI_Editor
				if(owner.isGuiEnabled())
				owner.sendMessageToWinXound(String("CABBAGE_SELECT_LINE"), owner.getCurrentLine()); 
#endif
			}
		/*
			else if(message.toString().contains("CABBAGE_EXPORT_VSTI"))
				//owner.exportClicked("VSTi");
			else if(message.toString().contains("CABBAGE_EXPORT_VST"))
				//owner.exportClicked("VST");
			else if(message.toString().contains("CABBAGE_EXPORT_AU"))
				//owner.exportClicked("AU");
				*/
		}
	};

	//=================================================================
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
	ScopedPointer<socketServer> server;
	ScopedPointer<socketConnection> ipConnection;  //*** MOD:STEFANO - ADDED
	OwnedArray <socketConnection, CriticalSection> activeConnections;
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
	
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StandaloneFilterWindow);
};

#endif   // __JUCE_STANDALONEFILTERWINDOW_JUCEHEADER__


/*
class CabbageSettingsDialogue    : public DocumentWindow
{
public:
    //==============================================================================
	//custom property panel class
	class CabbagePropertyPanel : public Component
	{
	public:
	class propSlot: public Component,
					 public Button::Listener
		{
			
			ScopedPointer<TextButton> browseButton;
			ScopedPointer<ToggleButton> toggleButton;
			ScopedPointer<Label> propLabel;
			ScopedPointer<TextEditor> helpDirectory;
			Rectangle<int> dimensions;
			int x, y, width, height;
			String showComp;
			ScopedPointer<OldSchoolLookAndFeel> oldLook;

		public:
			propSlot(Rectangle<int> rect, String name, String comp):
							dimensions(rect), showComp(comp)
				{	
						propLabel = new Label(name);
						oldLook = new OldSchoolLookAndFeel();
						propLabel->setLookAndFeel(oldLook);
						propLabel->setColour(Label::textColourId, Colours::white);
						//propLabel->setColour(Label::ColourIds::outlineColourId, Colours::cornflowerblue);
						//propLabel->setColour(Label::backgroundColourId, Colours::cornflowerblue);
						propLabel->setText(name, false);
						addAndMakeVisible(propLabel);
	
						helpDirectory = new TextEditor();
						//helpDirectory->setLookAndFeel(oldLook);
						addAndMakeVisible(helpDirectory);
						helpDirectory->setScrollbarsShown(false);
						helpDirectory->setColour(TextEditor::textColourId, Colours::white);
						helpDirectory->setColour(TextEditor::backgroundColourId, Colours::grey);
						helpDirectory->setColour(TextEditor::ColourIds::outlineColourId, Colours::white);
						helpDirectory->setColour(TextEditor::ColourIds::shadowColourId, Colours::pink); 
						helpDirectory->setText("C:\\MyDocuments\\SourceCode\\Csound5\\csoundInstall\\Csound\\doc\\manual");
						helpDirectory->setCaretPosition(0);
						if(showComp=="browse"){
						browseButton = new TextButton("Browse");
						addAndMakeVisible(browseButton);
						}
						else if(showComp=="toggle"){
						toggleButton = new ToggleButton("Browse");
						addAndMakeVisible(toggleButton);
						}
				}

			~propSlot(){}

			void paint(Graphics &g){
						g.fillAll(Colours::black);
				}

			void resized(){
						propLabel->setBounds(0, 0, 120, dimensions.getHeight());
						if(showComp=="browse"){
						helpDirectory->setBounds(125, 0, dimensions.getWidth()-225, dimensions.getHeight());
						browseButton->setBounds(dimensions.getWidth()-100, 0, 100, dimensions.getHeight());
					}
				}

			void buttonClicked (Button*){}
	
	};

	//Property panel constructor	
	CabbagePropertyPanel(Rectangle<int> rect){
	//CabbageUtils::showMessage(String(rect.getWidth()));
	props.add(new propSlot(rect, "Csound Help Manual", "browse"));
	//props.add(new propSlot(rect, "Home .csd directory", "browse"));

	//props.getUnchecked(0)->setBounds(rect);
	for(int i=0;i<props.size();i++)
	addAndMakeVisible(props.getUnchecked(i));
	}

	~CabbagePropertyPanel(){

	}

	void resized(){
		props.getUnchecked(0)->setBounds(0, 0, this->getWidth(), this->getHeight()); 
	}

	private:
		OwnedArray<propSlot> props; 
	};
	//end of custom property panel

	CabbageSettingsDialogue (const String& title,
                            const Colour& backgroundColour, int width, int height): DocumentWindow (title, backgroundColour,
								DocumentWindow::closeButton)
	{	
	Rectangle<int> rect(width, 25);
	setAlwaysOnTop(true);
	Rectangle<int> screen = getParentMonitorArea();
	setTopLeftPosition((screen.getWidth()/2)-this->getWidth()/2, (screen.getHeight()/2)-this->getHeight()/2);  
	propPanel = new CabbagePropertyPanel(rect);
	propPanel->setSize(width, height);
	setContentOwned(propPanel, true);
	}

	~CabbageSettingsDialogue(){};

	void closeButtonPressed(){
		setVisible(false);
	}
	ScopedPointer<CabbagePropertyPanel> propPanel;
};
*/
