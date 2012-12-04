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

#include "CabbageStandaloneDialog.h"


#define MAXBYTES 16777216

//==============================================================================
//  Somewhere in the codebase of your plugin, you need to implement this function
//  and make it create an instance of the filter subclass that you're building.
extern CabbagePluginAudioProcessor* JUCE_CALLTYPE createCabbagePluginFilter(String inputfile, bool guiOnOff);


//==============================================================================
StandaloneFilterWindow::StandaloneFilterWindow (const String& title,
                                                const Colour& backgroundColour)
    : DocumentWindow (title, backgroundColour,
                      DocumentWindow::minimiseButton
                       | DocumentWindow::closeButton),
      optionsButton ("options"), isGUIOn(false), pipeOpenedOk(false), AudioEnabled(true), isAFileOpen(false)
{
	
	
	consoleMessages = "";
	cabbageDance = 0;
	setTitleBarButtonsRequired (DocumentWindow::minimiseButton | DocumentWindow::closeButton, false);
    Component::addAndMakeVisible (&optionsButton);
    optionsButton.addListener (this);
	timerRunning = false;
	yAxis = 0;
	optionsButton.setTriggeredOnMouseDown (true);
	bool alwaysontop = appProperties->getUserSettings()->getValue("SetAlwaysOnTop", var(0)).getFloatValue();
	setAlwaysOnTop(alwaysontop);
	this->setResizable(false, false);

	lookAndFeel = new CabbageLookAndFeel();
	this->setLookAndFeel(lookAndFeel);

	oldLookAndFeel = new LookAndFeel();
// MOD - Stefano Bonetti 
#ifdef Cabbage_Named_Pipe 
	ipConnection = new socketConnection(*this);
	pipeOpenedOk = ipConnection->createPipe(String("cabbage"));
	if(pipeOpenedOk) Logger::writeToLog(String("Namedpipe created ..."));
#endif 
// MOD - End

	JUCE_TRY
    {
        filter = createCabbagePluginFilter("", false);
		filter->addChangeListener(this);
		filter->addActionListener(this);
		filter->sendChangeMessage();
		filter->createGUI("");		
    }
    JUCE_CATCH_ALL

    if (filter == nullptr)
    {
        jassertfalse    // Your filter didn't create correctly! In a standalone app that's not too great.
        JUCEApplication::quit();
    }

    filter->setPlayConfigDetails (JucePlugin_MaxNumInputChannels, 
                                          JucePlugin_MaxNumOutputChannels,
                                          44100, 512);

    PropertySet* const globalSettings = getGlobalSettings();

    deviceManager = new AudioDeviceManager();
    deviceManager->addAudioCallback (&player);
    deviceManager->addMidiInputCallback (String::empty, &player);


    player.setProcessor (filter);
	

    ScopedPointer<XmlElement> savedState;

    if (globalSettings != nullptr)
        savedState = globalSettings->getXmlValue ("audioSetup");

    deviceManager->initialise (filter->getNumInputChannels(),
                               filter->getNumOutputChannels(),
                               savedState,
                               true);

	deviceManager->closeAudioDevice();

    if (globalSettings != nullptr)
    {
        MemoryBlock data;

        if (data.fromBase64Encoding (globalSettings->getValue ("filterState"))
             && data.getSize() > 0)
        {
            filter->setStateInformation (data.getData(), data.getSize());
        }
    }

    setContentOwned (filter->createEditorIfNeeded(), true);

    const int x = globalSettings->getIntValue ("windowX", -100);
    const int y = globalSettings->getIntValue ("windowY", -100);

    if (x != -100 && y != -100)
        setBoundsConstrained (Rectangle<int> (x, y, getWidth(), getHeight()));
    else
		centreWithSize (getWidth(), getHeight());
		
	appProperties->getUserSettings()->setValue("AutoUpdate", var(0));
	//	int val = appProperties->getUserSettings()->getValue("AutoUpdate", var(0)).getFloatValue();
	//	if(val)
	//		startTimer(100);
}
//==============================================================================
// Destructor
//==============================================================================
StandaloneFilterWindow::~StandaloneFilterWindow()
{

#ifdef Cabbage_Named_Pipe 
// MOD - Stefano Bonetti
	if(ipConnection){ 
	ipConnection->disconnect();
	}
// MOD - End
#endif

    PropertySet* const globalSettings = getGlobalSettings();

    if (globalSettings != nullptr)
    {
        globalSettings->setValue ("windowX", getX());
        globalSettings->setValue ("windowY", getY());

        if (deviceManager != nullptr)
        {
            ScopedPointer<XmlElement> xml (deviceManager->createStateXml());
            globalSettings->setValue ("audioSetup", xml);
        }
    }

    deviceManager->removeMidiInputCallback (String::empty, &player);
    deviceManager->removeAudioCallback (&player);
    deviceManager = nullptr;

    if (globalSettings != nullptr && filter != nullptr)
    {
        MemoryBlock data;
        filter->getStateInformation (data);

        globalSettings->setValue ("filterState", data.toBase64Encoding());
    }

    deleteFilter();
}

//==============================================================================
// sends messages to WinXound
//==============================================================================
void StandaloneFilterWindow::sendMessageToWinXound(String messageType, String message)
{
if(pipeOpenedOk){
String text = messageType+String("|")+message;
MemoryBlock messageData (text.toUTF8(), text.getNumBytesAsUTF8());
pipeOpenedOk = ipConnection->sendMessage(messageData);
}
}

void StandaloneFilterWindow::sendMessageToWinXound(String messageType, int value)
{
if(pipeOpenedOk){
String text = messageType+String("|")+String(value);
MemoryBlock messageData (text.toUTF8(), text.getNumBytesAsUTF8());
pipeOpenedOk = ipConnection->sendMessage(messageData);
}
}

//==============================================================================
// insane Cabbage dancing....
//==============================================================================
void StandaloneFilterWindow::timerCallback()
{   
	int64 diskTime = csdFile.getLastModificationTime().toMilliseconds();
	int64 tempTime = lastSaveTime.toMilliseconds();
	if(diskTime>tempTime){
		resetFilter();
		lastSaveTime = csdFile.getLastModificationTime();
	}
	//cout << csdFile.getLastModificationTime().toString(true, true, false, false);
	if(cabbageDance){
	float moveY = sin(yAxis*2*3.14*20/100); 
	float moveX = sin(yAxis*2*3.14*10/100); 
	yAxis+=1;
	this->setTopLeftPosition(this->getScreenX()+(moveX*5), this->getScreenY()+(moveY*10));
	}
}

//==============================================================================
// action Callback - updates instrument according to changes in source code
//==============================================================================
void StandaloneFilterWindow::actionListenerCallback (const String& message){
	if(message == "GUI Update"){
		if(cabbageCsoundEditor){
		cabbageCsoundEditor->setCsoundFile(csdFile);
		cabbageCsoundEditor->csoundEditor->highlightLine(filter->getCurrentLineText());
		cabbageCsoundEditor->csoundEditor->grabKeyboardFocus();
		}
	}

	else if(message == "GUI Updated, controls added")
	filter->createGUI(csdFile.loadFileAsString());
	setGuiEnabled(true);
	filter->setGuiEnabled(true);
	setCurrentLine(filter->getCurrentLine()+1);
	if(message.equalsIgnoreCase("fileSaved")){
	saveFile();
	}

	else if(message.contains("fileOpen")){
	openFile();
	}

	else if(message.contains("fileSaveAs")){
	saveFileAs();
	}

	else if(message.contains("fileExportSynth")){
	exportPlugin(String("VSTi"), false);
	}

	else if(message.contains("fileExportEffect")){
	exportPlugin(String("VST"), false);
	}

	else if(message.contains("fileUpdateGUI")){
		filter->createGUI(cabbageCsoundEditor->getCurrentText());
		csdFile.replaceWithText(cabbageCsoundEditor->getCurrentText()); 
	}

#ifdef Cabbage_Build_Standalone
	else if(message.contains("MENU COMMAND: manual update instrument"))
		resetFilter();

	else if(message.contains("MENU COMMAND: open instrument"))
		openFile();

	else if(message.contains("MENU COMMAND: manual update GUI"))
		filter->createGUI(csdFile.loadFileAsString());
	
	else if(message.contains("MENU COMMAND: suspend audio"))
        if(AudioEnabled){
			filter->suspendProcessing(true);
			AudioEnabled = false;
		}
		else{
			AudioEnabled = true;
			filter->suspendProcessing(false);
		}
else{}
#endif

}
//==============================================================================

//==============================================================================
// listener Callback - updates WinXound compiler output with Cabbage messages
//==============================================================================
void StandaloneFilterWindow::changeListenerCallback(juce::ChangeBroadcaster* /*source*/)
{
String text = "";
#ifdef Cabbage_Named_Pipe
#ifdef Cabbage_GUI_Editor
if(filter->isGuiEnabled()){
if(filter->getChangeMessageType().containsIgnoreCase("GUI_edit")){
setGuiEnabled(true);
setCurrentLine(filter->getCurrentLine()+1);
sendMessageToWinXound(String("CABBAGE_FILE_UPDATED"), csdFile.getFullPathName());
sendMessageToWinXound(String("CABBAGE_UPDATE"), "");
}
else if(filter->getChangeMessageType().containsIgnoreCase("GUI_lock")){
setGuiEnabled(false);
setCurrentLine(filter->getCurrentLine()+1);
sendMessageToWinXound(String("CABBAGE_FILE_UPDATED"), csdFile.getFullPathName());
sendMessageToWinXound(String("CABBAGE_UPDATE"), "");
if(getCurrentLine()>1)
sendMessageToWinXound(String("CABBAGE_SELECT_LINE"), getCurrentLine()); 
Logger::writeToLog(String(getCurrentLine()));
}
}
else
#endif
// MOD - Stefano Bonetti
  if(filter && ipConnection->isConnected()){
      for(int i=0;i<filter->getDebugMessageArray().size();i++)
      {
          if(filter->getDebugMessageArray().getReference(i).length()>0)
          {
              text += String(filter->getDebugMessageArray().getReference(i).toUTF8());
          }
          else 
          {
              sendMessageToWinXound(String("CABBAGE_DEBUG"), "Debug message string is empty?");
              break;
          }

      }

      filter->clearDebugMessageArray();
	  sendMessageToWinXound(String("CABBAGE_DEBUG"), text);

  }
#endif
  // MOD - End

for(int i=0;i<filter->getDebugMessageArray().size();i++)
      {
          if(filter->getDebugMessageArray().getReference(i).length()>0)
          {
              text += String(filter->getDebugMessageArray().getReference(i).toUTF8());
          }
          else 
          {
              cabbageCsoundEditor->setCsoundOutputText(text);
              break;
          }

      }

      filter->clearDebugMessageArray();
	  consoleMessages = consoleMessages+text+"\n"; 
	  if(cabbageCsoundEditor)
		  cabbageCsoundEditor->setCsoundOutputText(text+"\n");
	  //sendMessageToWinXound(String("CABBAGE_DEBUG"), text);

}
//==============================================================================
// Delete filter
//==============================================================================
void StandaloneFilterWindow::deleteFilter()
{
	player.setProcessor (nullptr);

    if (filter != nullptr && getContentComponent() != nullptr)
    {
        filter->editorBeingDeleted (dynamic_cast <AudioProcessorEditor*> (getContentComponent()));
        clearContentComponent();
    }

    filter = nullptr;
}

//==============================================================================
// Reset filter
//==============================================================================
void StandaloneFilterWindow::resetFilter()
{
	deleteFilter();
	deviceManager->closeAudioDevice();
	filter = createCabbagePluginFilter(csdFile.getFullPathName(), false);
	filter->suspendProcessing(true);
	filter->addChangeListener(this);
	filter->addActionListener(this);
	filter->sendChangeMessage();
	//filter->suspendProcessing(true);
	filter->createGUI(csdFile.loadFileAsString());
	filter->suspendProcessing(false);
	String test = filter->getPluginName();
	setName(filter->getPluginName());

    if (filter != nullptr)
    {
        if (deviceManager != nullptr){
            player.setProcessor (filter);
			deviceManager->restartLastAudioDevice();
		}

        setContentOwned (filter->createEditorIfNeeded(), true);
    }


    PropertySet* const globalSettings = getGlobalSettings();

    if (globalSettings != nullptr)
        globalSettings->removeValue ("filterState");
	
#ifdef Cabbage_Named_Pipe
	//notify WinXound that Cabbage is set up and ready for action
	sendMessageToWinXound(String("CABBAGE_LOADED"), "");
#endif

	if(cabbageCsoundEditor){
	cabbageCsoundEditor->setName(csdFile.getFullPathName());
	if(cabbageCsoundEditor->isVisible())
		cabbageCsoundEditor->csoundEditor->textEditor->grabKeyboardFocus();
	}

}

//==============================================================================
void StandaloneFilterWindow::saveState()
{
    PropertySet* const globalSettings = getGlobalSettings();

    FileChooser fc (TRANS("Save current state"),
                    globalSettings != nullptr ? File (globalSettings->getValue ("lastStateFile"))
                                              : File::nonexistent);

    if (fc.browseForFileToSave (true))
    {
        MemoryBlock data;
        filter->getStateInformation (data);

        if (! fc.getResult().replaceWithData (data.getData(), data.getSize()))
        {
            AlertWindow::showMessageBox (AlertWindow::WarningIcon,
                                         TRANS("Error whilst saving"),
                                         TRANS("Couldn't write to the specified file!"));
        }
    }
}

void StandaloneFilterWindow::loadState()
{
    PropertySet* const globalSettings = getGlobalSettings();

    FileChooser fc (TRANS("Load a saved state"),
                    globalSettings != nullptr ? File (globalSettings->getValue ("lastStateFile"))
                                              : File::nonexistent);

    if (fc.browseForFileToOpen())
    {
        MemoryBlock data;

        if (fc.getResult().loadFileAsData (data))
        {
            filter->setStateInformation (data.getData(), data.getSize());
        }
        else
        {
            AlertWindow::showMessageBox (AlertWindow::WarningIcon,
                                         TRANS("Error whilst loading"),
                                         TRANS("Couldn't read from the specified file!"));
        }
    }
}

//==============================================================================
PropertySet* StandaloneFilterWindow::getGlobalSettings()
{
    /* If you want this class to store the plugin's settings, you can set up an
       ApplicationProperties object and use this method as it is, or override this
       method to return your own custom PropertySet.

       If using this method without changing it, you'll probably need to call
       ApplicationProperties::setStorageParameters() in your plugin's constructor to
       tell it where to save the file.
    */
	appProperties->getUserSettings()->setValue("htmlHelp", String("some directory"));
	return appProperties->getUserSettings();
}

void StandaloneFilterWindow::showAudioSettingsDialog()
{

const int numIns = filter->getNumInputChannels() <= 0 ? JucePlugin_MaxNumInputChannels : filter->getNumInputChannels();
const int numOuts = filter->getNumOutputChannels() <= 0 ? JucePlugin_MaxNumOutputChannels : filter->getNumOutputChannels();

    AudioDeviceSelectorComponent selectorComp (*deviceManager,
                                               numIns, numIns, numOuts, numOuts,
                                               true, false, true, false);

    selectorComp.setSize (400, 250);
	setAlwaysOnTop(false);
	selectorComp.setLookAndFeel(lookAndFeel);
	Colour col(44, 44, 44);
	DialogWindow::showModalDialog(TRANS("Audio Settings"), &selectorComp, this, col, true, false, false);
	bool alwaysontop = appProperties->getUserSettings()->getValue("SetAlwaysOnTop", var(0)).getFloatValue();
	setAlwaysOnTop(alwaysontop);

}
//==============================================================================
void StandaloneFilterWindow::closeButtonPressed()
{
	/*
if(cabbageCsoundEditor){
	cabbageCsoundEditor->removeAllActionListeners(); 
	cabbageCsoundEditor->removeFromDesktop();
	cabbageCsoundEditor->closeButtonPressed();
}
Time::waitForMillisecondCounter(1000);
*/
JUCEApplication::quit();
}

void StandaloneFilterWindow::resized()
{
    DocumentWindow::resized();
    optionsButton.setBounds (8, 6, 60, getTitleBarHeight() - 8);
}


//==============================================================================
// Button clicked method
//==============================================================================
void StandaloneFilterWindow::buttonClicked (Button*)
{
    if (filter == nullptr)
        return;

	String test;
    PopupMenu m;
	PopupMenu subMenu;
	m.setLookAndFeel(lookAndFeel);
	subMenu.setLookAndFeel(lookAndFeel);

	m.addItem(1, String("Open Cabbage Instrument | Ctrl+o"));
	

	subMenu.addItem(30, String("Effect"));
	subMenu.addItem(31, String("Instrument"));
	m.addSubMenu(String("New Cabbage..."), subMenu);

	m.addItem(2, String("View Source Editor"));
	m.addSeparator();
	if(AudioEnabled)
	m.addItem(400, "Audio Enabled | Ctrl+m", true, true); 
	else
	m.addItem(400, "Audio Enabled | Ctrl+m", true, false); 
    m.addItem(4, TRANS("Audio Settings..."));
    m.addSeparator();
	m.addItem(100, String("Toggle Edit-mode"));
	m.addSeparator();
	subMenu.clear();
	subMenu.addItem(15, TRANS("Plugin Synth"));
	subMenu.addItem(16, TRANS("Plugin Effect"));
	m.addSubMenu(TRANS("Export..."), subMenu);
#ifndef MACOSX
	subMenu.clear();
	subMenu.addItem(5, TRANS("Plugin Synth"));
	subMenu.addItem(6, TRANS("Plugin Effect"));
	m.addSubMenu(TRANS("Export Plugin As..."), subMenu);
#endif
    m.addSeparator();
	int alwaysontop = appProperties->getUserSettings()->getValue("SetAlwaysOnTop", var(0)).getFloatValue(); 
	if(alwaysontop)
	m.addItem(7, String("Always on Top"), true, true);
	else
	m.addItem(7, String("Always on Top"), true, false);
	m.addItem(8, String("Rebuild Instrument | Ctrl+b"));
	m.addItem(9, String("Rebuild GUI | Ctrl+u"));
	/*
	if(filter->getMidiDebug())
    m.addItem(9, TRANS("Show MIDI Debug Information"), true, true);
	else
	m.addItem(9, TRANS("Show MIDI Debug Information"));
	*/
	subMenu.clear();
	subMenu.addItem(11, TRANS("Effects"));
	subMenu.addItem(12, TRANS("Synths"));
	m.addSubMenu(TRANS("Batch Convert"), subMenu);
	m.addSeparator();

	int autoUpdate = appProperties->getUserSettings()->getValue("AutoUpdate", var(0)).getFloatValue();
	if(!autoUpdate)
	m.addItem(299, String("Auto-update"), true, false);
	else
	m.addItem(299, String("Auto-update"), true, true);
/*
	m.addSeparator();
	if(cabbageDance)
	m.addItem(99, String("Cabbage Dance"), true, true);
	else
	m.addItem(99, String("Cabbage Dance"));
*/



	subMenu.clear();

	int pluginInfo = appProperties->getUserSettings()->getValue("DisablePluginInfo", var(0)).getFloatValue();
	subMenu.addItem(203, "Set Cabbage Plant Directory");
	subMenu.addItem(200, "Set Csound Manual Directory");
	if(!pluginInfo)
	subMenu.addItem(201, String("Disable Export Plugin Info"), true, false);
	else
	subMenu.addItem(201, String("Disable Export Plugin Info"), true, true);

	int disableGUIEditWarning = appProperties->getUserSettings()->getValue("DisableGUIEditModeWarning", var(0)).getFloatValue();
	if(!disableGUIEditWarning)
	subMenu.addItem(202, String("Disable GUI Edit Mode warning"), true, true);
	else
	subMenu.addItem(202, String("Disable GUI Edit Mode warning"), true, false);


	m.addSubMenu("Preferences", subMenu);
	
	
	
	

	int options = m.showAt (&optionsButton);
 
	//----- open file ------
	if(options==1){
		openFile();
	}
	//----- view text editor ------
	else if(options==2){
	if(!cabbageCsoundEditor){
	cabbageCsoundEditor = new CabbageEditorWindow(lookAndFeel);
	cabbageCsoundEditor->setVisible(false);
	cabbageCsoundEditor->setFullScreen(true);
	cabbageCsoundEditor->addActionListener(this);
	}
	cabbageCsoundEditor->setCsoundFile(csdFile);
	this->toBehind(cabbageCsoundEditor);
	cabbageCsoundEditor->setVisible(true);
	cabbageCsoundEditor->setFullScreen(true);
	cabbageCsoundEditor->toFront(true);
	cabbageCsoundEditor->setCsoundOutputText(consoleMessages);
	cabbageCsoundEditor->csoundEditor->textEditor->grabKeyboardFocus();
	}
	//----- new effect ------
	else if(options==30){
	if(!cabbageCsoundEditor){
		cabbageCsoundEditor = new CabbageEditorWindow(lookAndFeel);
		cabbageCsoundEditor->addActionListener(this);
		}
		cabbageCsoundEditor->setVisible(true);
		cabbageCsoundEditor->csoundEditor->newFile("effect");
		saveFileAs();
		cabbageCsoundEditor->csoundEditor->textEditor->grabKeyboardFocus();
	}
	//----- new instrument ------
	else if(options==31){
	if(!cabbageCsoundEditor){
		cabbageCsoundEditor = new CabbageEditorWindow(lookAndFeel);
		cabbageCsoundEditor->addActionListener(this);
		}
	cabbageCsoundEditor->setVisible(true);
	cabbageCsoundEditor->csoundEditor->newFile("instrument");
	saveFileAs();
	cabbageCsoundEditor->csoundEditor->textEditor->grabKeyboardFocus();
	isAFileOpen = true;
	}
	//----- audio settings ------
   	else if(options==4){
        showAudioSettingsDialog();
		resetFilter();
	}

	//suspend audio
   	else if(options==400){
        if(AudioEnabled){
			filter->suspendProcessing(true);
			AudioEnabled = false;
		}
		else{
			AudioEnabled = true;
			filter->suspendProcessing(false);
		}
	}

	//----- export ------
	else if(options==15)
	exportPlugin(String("VSTi"), false);
	
	else if(options==16)
	exportPlugin(String("VST"), false);


	//----- export as ------
	else if(options==5)
	exportPlugin(String("VSTi"), true);
	
	else if(options==6)
	exportPlugin(String("VST"), true);

	//----- always on top  ------
	else if(options==7)

	if(appProperties->getUserSettings()->getValue("SetAlwaysOnTop", var(0)).getFloatValue()){
		this->setAlwaysOnTop(false);
		appProperties->getUserSettings()->setValue("SetAlwaysOnTop", var(0));
	}
	else{
		this->setAlwaysOnTop(true);
		appProperties->getUserSettings()->setValue("SetAlwaysOnTop", var(1));
	}
	
	//----- update instrument  ------
    else if(options==8){
        resetFilter();
	}
	//----- update GUI only -----
	else if(options==9)
	filter->createGUI(csdFile.loadFileAsString());

	//----- batch process ------
	else if(options==11)
		BatchProcess(String("VST"));

	else if(options==12)
		BatchProcess(String("VSTi"));

	//----- auto-update file when saved remotely ------
	else if(options==299){
		int val = appProperties->getUserSettings()->getValue("AutoUpdate", var(0)).getFloatValue();
		if(val==0){
			appProperties->getUserSettings()->setValue("AutoUpdate", var(1));
			startTimer(100);	
			}
		else{
			appProperties->getUserSettings()->setValue("AutoUpdate", var(0));
			stopTimer();
		}
		}
/*
	//------- cabbage dance ------
	else if(options==99){
		if(!cabbageDance){
		startTimer(20);
		cabbageDance = true;
		}
		else{
		stopTimer();
		cabbageDance = false;
		}
	}
	*/
	//------- preference Csound manual dir ------
	else if(options==200){
		String dir = appProperties->getUserSettings()->getValue("CsoundHelpDir", "");
		FileChooser browser(String("Please select the Csound manual directory..."), File(dir), String("*.csd"));
		if(browser.browseForDirectory()){
			appProperties->getUserSettings()->setValue("CsoundHelpDir", browser.getResult().getFullPathName());
		}	
	}
	//------- preference Csound manual dir ------
	else if(options==203){
		String dir = appProperties->getUserSettings()->getValue("PlantFileDir", "");
		FileChooser browser(String("Please select your Plant file directory..."), File(dir), String("*.csd"));
		if(browser.browseForDirectory()){
			appProperties->getUserSettings()->setValue("PlantFileDir", browser.getResult().getFullPathName());
		}	
	}
	//------- preference plugin info ------
	else if(options==201){
		int val = appProperties->getUserSettings()->getValue("DisablePluginInfo", var(0)).getFloatValue();
		if(val==0)
			appProperties->getUserSettings()->setValue("DisablePluginInfo", var(1));
		else
			appProperties->getUserSettings()->setValue("DisablePluginInfo", var(0));
	}
	//------- preference disable gui edit warning ------
	else if(options==202){
		int val = appProperties->getUserSettings()->getValue("DisableGUIEditModeWarning", var(0)).getFloatValue();
		if(val==0) 
			appProperties->getUserSettings()->setValue("DisableGUIEditModeWarning", var(1));
		else
			appProperties->getUserSettings()->setValue("DisableGUIEditModeWarning", var(0));
	}
	//------- enable GUI edito mode------
	else if(options==100){
		int val = appProperties->getUserSettings()->getValue("DisableGUIEditModeWarning", var(0)).getFloatValue();
		if(val)
			showMessage("Warning!! This feature is bleeding edge! (that's programmer speak for totally untested and likely to crash hard!). If you like to live on the edge, disable this warning under the 'Preferences' menu command and try 'Edit Mode' again, otherwise just let it be...", oldLookAndFeel);
		else{
	if(isAFileOpen == true)
		if(filter->isGuiEnabled()){
		((CabbagePluginAudioProcessorEditor*)filter->getActiveEditor())->setEditMode(true);
		filter->setGuiEnabled(true);
		}
		else{
		((CabbagePluginAudioProcessorEditor*)filter->getActiveEditor())->setEditMode(false);
		filter->setGuiEnabled(false);
		stopTimer();
		appProperties->getUserSettings()->setValue("AutoUpdate", var(0));
		}
	else showMessage("Open or create a file first", &getLookAndFeel());
		}
	}
	repaint();
}

//==============================================================================
// open/save/save as methods
//==============================================================================
void StandaloneFilterWindow::openFile()
{
#ifdef MACOSX
	FileChooser openFC(String("Open a Cabbage .csd file..."), File::nonexistent, String("*.csd;*.vst"));
	if(openFC.browseForFileToOpen()){
		csdFile = openFC.getResult();
		originalCsdFile = openFC.getResult();
		lastSaveTime = csdFile.getLastModificationTime();
		csdFile.setAsCurrentWorkingDirectory();
		if(csdFile.getFileExtension()==(".vst")){
			String csd = csdFile.getFullPathName();
			csd << "/Contents/" << csdFile.getFileNameWithoutExtension() << ".csd";
			csdFile = File(csd);
		}
		resetFilter();
		//cabbageCsoundEditor->setCsoundFile(csdFile);
	}	
	
#else
	FileChooser openFC(String("Open a Cabbage .csd file..."), File::nonexistent, String("*.csd"));
	this->setAlwaysOnTop(false);
	if(openFC.browseForFileToOpen()){
		csdFile = openFC.getResult();
		csdFile.getParentDirectory().setAsCurrentWorkingDirectory();
		lastSaveTime = csdFile.getLastModificationTime();
		resetFilter();
		if(cabbageCsoundEditor)
		cabbageCsoundEditor->setCsoundFile(csdFile);
		isAFileOpen = true;
	}
	if(appProperties->getUserSettings()->getValue("SetAlwaysOnTop", var(0)).getFloatValue())
		setAlwaysOnTop((true));
	else
		setAlwaysOnTop(false);
#endif
}

void StandaloneFilterWindow::saveFile()
{
if(csdFile.hasWriteAccess())
csdFile.replaceWithText(cabbageCsoundEditor->getCurrentText());
resetFilter();
}

void StandaloneFilterWindow::saveFileAs()
{
FileChooser saveFC(String("Save Cabbage file as..."), File::nonexistent, String("*.csd"));
	this->setAlwaysOnTop(false);
	if(saveFC.browseForFileToSave(true)){
		csdFile = saveFC.getResult().withFileExtension(String(".csd"));
		csdFile.replaceWithText(cabbageCsoundEditor->getCurrentText());
		cabbageCsoundEditor->setCsoundFile(csdFile);
		resetFilter();
	}
	if(appProperties->getUserSettings()->getValue("SetAlwaysOnTop", var(0)).getFloatValue())
		setAlwaysOnTop(true);
	else
		setAlwaysOnTop(false);

}
//==============================================================================
// Export plugin method
//==============================================================================
int StandaloneFilterWindow::exportPlugin(String type, bool saveAs)
{
File dll;
File loc_csdFile;
File thisFile(File::getSpecialLocation(File::currentApplicationFile));

if(!csdFile.exists()){
					showMessage("You need to open a Cabbage instrument before you can export one as a plugin!", lookAndFeel);
					return 0;
				}
#ifdef LINUX
	FileChooser saveFC(String("Save as..."), File::nonexistent, String(""));
	String VST;
	if (saveFC.browseForFileToSave(true)){
		if(type.contains("VSTi"))
			VST = thisFile.getParentDirectory().getFullPathName() + String("/CabbagePluginSynth.so");
		else if(type.contains(String("VSTfx")))
			VST = thisFile.getParentDirectory().getFullPathName() + String("CabbagePluginEffect.so");
		else if(type.contains(String("AU"))){
			showMessage("This feature only works on computers running OSX");
		}
		showMessage(VST);
		File VSTData(VST);
		if(VSTData.exists())showMessage("lib exists");
		else{
			File dll(saveFC.getResult().withFileExtension(".so").getFullPathName());
			showMessage(dll.getFullPathName());
			if(VSTData.copyFileTo(dll))	showMessage("moved");
			File loc_csdFile(saveFC.getResult().withFileExtension(".csd").getFullPathName());
			loc_csdFile.replaceWithText(csdFile.loadFileAsString());
		}
	}
#elif WIN32
	FileChooser saveFC(String("Save plugin as..."), File::nonexistent, String("*.dll"));
	String VST;
		if(type.contains("VSTi"))
			VST = thisFile.getParentDirectory().getFullPathName() + String("\\CabbagePluginSynth.dat");
		else if(type.contains(String("VST")))
			VST = thisFile.getParentDirectory().getFullPathName() + String("\\CabbagePluginEffect.dat");

		File VSTData(VST);

		if(!VSTData.exists()){
			showMessage("Cabbage cannot find the plugin libraries. Make sure that Cabbage is situated in the same directory as CabbagePluginSynth.dat and CabbagePluginEffect.dat", lookAndFeel);
			return 0;
		}
		else{
			if(saveAs){
			if (saveFC.browseForFileToSave(true)){
			dll = saveFC.getResult().withFileExtension(".dll").getFullPathName();
			loc_csdFile = saveFC.getResult().withFileExtension(".csd").getFullPathName();
			}
			else
				return 0;
			}
			else{
			dll = csdFile.withFileExtension(".dll").getFullPathName();
			loc_csdFile = csdFile.withFileExtension(".csd").getFullPathName();
			
			}
			//showMessage(dll.getFullPathName());
			if(!VSTData.copyFileTo(dll))	
				showMessage("Problem moving plugin lib, make sure it's not currently open in your plugin host!", lookAndFeel);
			
			loc_csdFile.replaceWithText(csdFile.loadFileAsString());
			setUniquePluginID(dll, loc_csdFile);
			String info;
			info = String("Your plugin has been created. It's called:\n\n")+dll.getFullPathName()+String("\n\nIn order to modify this plugin you only have to edit the associated .csd file. You do not need to export every time you make changes.\n\nTo turn off this notice visit 'Preferences' in the main 'options' menu");
			
			int val = appProperties->getUserSettings()->getValue("DisablePluginInfo", var(0)).getFloatValue();
			if(!val)
			showMessage(info, lookAndFeel);
			
#ifdef Cabbage_Named_Pipe
			sendMessageToWinXound("CABBAGE_PLUGIN_FILE_UPDATE", csdFile.getFullPathName()+String("|")+loc_csdFile.getFullPathName());
			csdFile = loc_csdFile;	
			cabbageCsoundEditor->setName(csdFile.getFullPathName());
			sendMessageToWinXound("CABBAGE_SHOW_MESSAGE|Info", "WinXound has been updated\nyour .csd file");
#endif
		}

#endif
	
#if MACOSX
	
	FileChooser saveFC(String("Save as..."), File::nonexistent, String(".vst"));
	String VST;
	if (saveFC.browseForFileToSave(true)){
		//showMessage("name of file is:");
		//showMessage(saveFC.getResult().withFileExtension(".vst").getFullPathName());
				
		if(type.contains("VSTi"))
			//VST = thisFile.getParentDirectory().getFullPathName() + String("//CabbagePluginSynth.dat");
			VST = thisFile.getFullPathName()+"/Contents/CabbagePluginSynth.dat";
		else if(type.contains(String("VST")))
			//VST = thisFile.getParentDirectory().getFullPathName() + String("//CabbagePluginEffect.dat");
			VST = thisFile.getFullPathName()+"/Contents/CabbagePluginEffect.dat";
		else if(type.contains(String("AU"))){
			showMessage("this feature is coming soon");
			//VST = thisFile.getParentDirectory().getFullPathName() + String("\\CabbageVSTfx.component");
		}
		//showMessage(thisFile.getFullPathName()+"/Contents/CabbagePluginSynth.dat");
		
		String plist  = String("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
		plist.append(String("<!DOCTYPE plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">\n"), MAXBYTES);
		plist.append(String("<plist version=\"1.0\">\n"), MAXBYTES);
		plist.append(String("<dict>\n"), MAXBYTES);
		plist.append(String("<key>CFBundleExecutable</key>\n"), MAXBYTES);
		plist.append(String("<string>")+String(saveFC.getResult().getFileNameWithoutExtension())+String("</string>\n"), MAXBYTES);
		plist.append(String("<key>CFBundleIdentifier</key>\n"), MAXBYTES);
		plist.append(String("<string>com.Cabbage.CabbagePlugin</string>\n"), MAXBYTES);
		plist.append(String("<key>CFBundleName</key>\n"), MAXBYTES);
		plist.append(String("<string>CabbagePlugin</string>\n"), MAXBYTES);
		plist.append(String("<key>CFBundlePackageType</key>\n"), MAXBYTES);
		plist.append(String("<string>BNDL</string>\n"), MAXBYTES);
		plist.append(String("<key>CFBundleShortVersionString</key>\n"), MAXBYTES);
		plist.append(String("<string>1.0.0</string>\n"), MAXBYTES);
		plist.append(String("<key>CFBundleSignature</key>\n"), MAXBYTES);
		plist.append(String("<string>PTul</string>\n"), MAXBYTES);
		plist.append(String("<key>CFBundleVersion</key>\n"), MAXBYTES);
		plist.append(String("<string>1.0.0</string>\n"), MAXBYTES);
		plist.append(String("</dict>\n"), MAXBYTES);
		plist.append(String("</plist>\n"), MAXBYTES);
		
		//create a copy of the data package and write it to the new location given by user
		File VSTData(VST);
		if(!VSTData.exists()){
			showMessage("Cabbage cannot find the plugin libraries. Make sure that Cabbage is situated in the same directory as CabbagePluginSynth.dat and CabbagePluginEffect.dat", lookAndFeel);
			return 0;
		}
		
		
		String plugType;
		if(type.contains(String("AU")))
			plugType = String(".component");
		else plugType = String(".vst");
		
		File dll(saveFC.getResult().withFileExtension(plugType).getFullPathName());
		
		VSTData.copyFileTo(dll);
		//showMessage("copied");
		
		
		
		File pl(dll.getFullPathName()+String("/Contents/Info.plist"));
		//showMessage(pl.getFullPathName());
		pl.replaceWithText(plist);

		
		
		File loc_csdFile(dll.getFullPathName()+String("/Contents/")+saveFC.getResult().getFileNameWithoutExtension()+String(".csd"));		
		loc_csdFile.replaceWithText(csdFile.loadFileAsString());		
		//showMessage(loc_csdFile.getFullPathName());
		//showMessage(csdFile.loadFileAsString());
		csdFile = loc_csdFile;		
		
		//showMessage(VST+String("/Contents/MacOS/")+saveFC.getResult().getFileNameWithoutExtension());
		//if plugin already exists there is no point in rewriting the binaries
		if(!File(saveFC.getResult().withFileExtension(".vst").getFullPathName()+("/Contents/MacOS/")+saveFC.getResult().getFileNameWithoutExtension()).exists()){		
		File bin(dll.getFullPathName()+String("/Contents/MacOS/CabbagePlugin"));
		bin.moveFileTo(dll.getFullPathName()+String("/Contents/MacOS/")+saveFC.getResult().getFileNameWithoutExtension());
		setUniquePluginID(bin, loc_csdFile);
		}
		
		String info;
		info = String("Your plugin has been created. It's called:\n\n")+dll.getFullPathName()+String("\n\nIn order to modify this plugin you only have to edit the current .csd file. You do not need to export every time you make changes. To open the current csd file with Cabbage in another session, go to 'Open Cabbage Instrument' and select the .vst file. Cabbage will the load the associated .csd file. \n\nTo turn off this notice visit 'Preferences' in the main 'options' menu");
		
		int val = appProperties->getUserSettings()->getValue("DisablePluginInfo", var(0)).getFloatValue();
		if(!val)
			showMessage(info, lookAndFeel);		
		

	}
	
#endif		
	
	return 0;	
}

//==============================================================================
// Set unique plugin ID for each plugin based on the file name 
//==============================================================================
int StandaloneFilterWindow::setUniquePluginID(File binFile, File csdFile){
String newID;
StringArray csdText;
csdText.addLines(csdFile.loadFileAsString());
//read contents of csd file to find pluginID
for(int i=0;i<csdText.size();i++)
    {
	StringArray tokes;
	tokes.addTokens(csdText[i].trimEnd(), ", ", "\"");
	if(tokes.getReference(0).equalsIgnoreCase(String("form"))){
			CabbageGUIClass cAttr(csdText[i].trimEnd(), 0);		
			if(cAttr.getStringProp("pluginID").length()!=4){
			showMessage(String("Your plugin ID is not the right size. It MUST be 4 characters long. Some hosts may not be able to load your plugin"), lookAndFeel);
			return 0;
			}
			else{
				newID = cAttr.getStringProp("pluginID");
				i = csdText.size();
			}			
		}
	}

size_t file_size;
const char *pluginID = "YROR";

long loc;
fstream mFile(binFile.getFullPathName().toUTF8(), ios_base::in | ios_base::out | ios_base::binary);
if(mFile.is_open())
  {
	mFile.seekg (0, ios::end);
	file_size = mFile.tellg();
	//set plugin ID
	mFile.seekg (0, ios::beg);
	unsigned char* buffer = (unsigned char*)malloc(sizeof(unsigned char)*file_size);
  	mFile.read((char*)&buffer[0], file_size);
	loc = cabbageFindPluginID(buffer, file_size, pluginID);
	if (loc < 0)
		showMessage(String("Internel Cabbage Error: The pluginID was not found"));
	else {
		mFile.seekg (loc, ios::beg);	
		mFile.write(newID.toUTF8(), 4);	
	}

#ifdef WIN32
	//set plugin name based on .csd file
	const char *pluginName = "CabbageEffectNam";
	String plugLibName = csdFile.getFileNameWithoutExtension();
	if(plugLibName.length()<16)
		for(int y=plugLibName.length();y<16;y++)
			plugLibName.append(String(" "), 1);
	
	mFile.seekg (0, ios::beg);
	buffer = (unsigned char*)malloc(sizeof(unsigned char)*file_size);
  	mFile.read((char*)&buffer[0], file_size);
	loc = cabbageFindPluginID(buffer, file_size, pluginName);
	if (loc < 0)
		showMessage(String("Plugin name could not be set?!?"));
	else {
		//showMessage("plugin name set!");
		mFile.seekg (loc, ios::beg);	
		mFile.write(csdFile.getFileNameWithoutExtension().toUTF8(), 16);	
	}
#endif

}
mFile.close();
return 1;
}

//==============================================================================
// Batch process multiple csd files to convert them to plugins libs. 
//==============================================================================
void StandaloneFilterWindow::BatchProcess(String type){
File thisFile(File::getSpecialLocation(File::currentApplicationFile));
#ifdef WIN32  
FileChooser saveFC(String("Select files..."), File::nonexistent, String(""));
String VST;
	if (saveFC.browseForMultipleFilesToOpen()){
		if(type.contains("VSTi"))
			VST = thisFile.getParentDirectory().getFullPathName() + String("\\CabbagePluginSynth.dat");
		else if(type.contains(String("VST")))
			VST = thisFile.getParentDirectory().getFullPathName() + String("\\CabbagePluginEffect.dat");
		else if(type.contains(String("AU"))){
			showMessage("This feature only works on computers running OSX");
			}
	//showMessage(VST);
	File VSTData(VST);
	if(!VSTData.exists())  
		showMessage("problem with plugin lib");
	else{
		for(int i=0;i<saveFC.getResults().size();i++){
			File dll(saveFC.getResults().getReference(i).withFileExtension(".dll").getFullPathName());
		//showMessage(dll.getFullPathName());
		if(!VSTData.copyFileTo(dll))	
			showMessage("problem moving plugin lib");
		else{
		//File loc_csdFile(saveFC.getResults().getReference(i).withFileExtension(".csd").getFullPathName());
		//loc_csdFile.replaceWithText(csdFile.loadFileAsString());
		}
		}
		showMessage("Batch Convertion Complete");
	}
	}
#endif
}
