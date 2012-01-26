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

using namespace std; 

#include "CabbageUtils.h"

extern ApplicationProperties* appProperties;
class CabbagePluginAudioProcessor;

//==============================================================================
// This is the main host window. It gets instatiated in StandaloneFilterApp.cpp
//==============================================================================
class StandaloneFilterWindow    : public DocumentWindow,
									public Button::Listener,
									public ChangeListener,
									public CabbageUtils,
									public Timer
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
			Logger::writeToLog(T("Connection #") + String (ourNumber) + T(" - connection started"));
		}

		void connectionLost(){
			showMessage("Connection with WinXound has been lost, please restart Cabbage");
			Logger::writeToLog(T("Connection #") + String (ourNumber) + T(" - connection lost"));
		}

		void messageReceived (const MemoryBlock& message){
			if(message.toString().contains("UPDATE")){
				String filename = message.toString().trimCharactersAtStart(T("CABBAGE_UPDATE|"));
				//Logger::writeToLog(filename.trim());
				owner.csdFile = File(filename.trim());
				//owner.OPENEDFROMDISK = true;
				owner.resetFilter();
				//owner.toFront(true);
				Logger::writeToLog(T("Cabbage Updated"));
#ifdef Cabbage_GUI_Editor
				if(owner.isGuiEnabled())
				owner.sendMessageToWinXound(T("CABBAGE_SELECT_LINE"), owner.getCurrentLine()); 
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
    void resized();
	void changeListenerCallback(ChangeBroadcaster *source);
	int exportPlugin(String type);

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
	bool timerRunning;
	int setUniquePluginID(File inFile, File csdFile);
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
	File csdFile; 
	bool isGUIOn;
	int currentLine;
	bool pipeOpenedOk;
	
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StandaloneFilterWindow);
};

#endif   // __JUCE_STANDALONEFILTERWINDOW_JUCEHEADER__
