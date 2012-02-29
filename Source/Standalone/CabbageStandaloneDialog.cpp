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
#include "../../JuceLibraryCode/juce_PluginHeaders.h"
#include "../Plugin/CabbagePluginProcessor.h"

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
      optionsButton ("options"), isGUIOn(false), pipeOpenedOk(false), helpInfo(false)
{
	setTitleBarButtonsRequired (DocumentWindow::minimiseButton | DocumentWindow::closeButton, false);
    Component::addAndMakeVisible (&optionsButton);
    optionsButton.addListener (this);
	optionsButton.setTooltip("This is a test");
	timerRunning = false;
	yAxis = 0;
    optionsButton.setTriggeredOnMouseDown (true);
	setAlwaysOnTop(true);
	this->setResizable(false, false);

	lookAndFeel = new CabbageLookAndFeel();
	this->setLookAndFeel(lookAndFeel);

// MOD - Stefano Bonetti 
#ifdef Cabbage_Named_Pipe 
	ipConnection = new socketConnection(*this);
	pipeOpenedOk = ipConnection->createPipe(T("cabbage"));
	if(pipeOpenedOk) Logger::writeToLog(T("Namedpipe created ..."));
#endif 
// MOD - End

	cabbageCsoundEditor = new CabbageEditorWindow(lookAndFeel);
	cabbageCsoundEditor->setVisible(false);
	cabbageCsoundEditor->addActionListener(this);


	JUCE_TRY
    {
        filter = createCabbagePluginFilter("", false);
		filter->addChangeListener(this);
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
String text = messageType+T("|")+message;
MemoryBlock messageData (text.toUTF8(), text.getNumBytesAsUTF8());
pipeOpenedOk = ipConnection->sendMessage(messageData);
}
}

void StandaloneFilterWindow::sendMessageToWinXound(String messageType, int value)
{
if(pipeOpenedOk){
String text = messageType+T("|")+String(value);
MemoryBlock messageData (text.toUTF8(), text.getNumBytesAsUTF8());
pipeOpenedOk = ipConnection->sendMessage(messageData);
}
}

//==============================================================================
// insane Cabbage dancing....
//==============================================================================
void StandaloneFilterWindow::timerCallback()
{   
	float moveY = sin(yAxis*2*3.14*20/100); 
	float moveX = sin(yAxis*2*3.14*10/100); 
	yAxis+=1;
	this->setTopLeftPosition(this->getScreenX()+(moveX*5), this->getScreenY()+(moveY*10));
}

//==============================================================================
// action Callback - updates instrument according to changes in source code
//==============================================================================
void StandaloneFilterWindow::actionListenerCallback (const String& message){
	if(message.equalsIgnoreCase("fileSaved")){
		saveFile();
		//resetFilter();
		//cabbageCsoundEditor->toFront(true);
	}
	else if(message.contains("fileOpen")){
	/*	String file = message.substring(9);
		csdFile = File(file);
		resetFilter();
		cabbageCsoundEditor->repaint();*/
		openFile();
	}
	else if(message.contains("fileSaveAs")){
		saveFileAs();
	}
	else if(message.contains("fileExportSynth")){
	exportPlugin(T("VSTi"));
	}
	else if(message.contains("fileExportEffect")){
	exportPlugin(T("VST"));
	}
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
sendMessageToWinXound(T("CABBAGE_FILE_UPDATED"), csdFile.getFullPathName());
sendMessageToWinXound(T("CABBAGE_UPDATE"), "");
}
else if(filter->getChangeMessageType().containsIgnoreCase("GUI_lock")){
setGuiEnabled(false);
setCurrentLine(filter->getCurrentLine()+1);
sendMessageToWinXound(T("CABBAGE_FILE_UPDATED"), csdFile.getFullPathName());
sendMessageToWinXound(T("CABBAGE_UPDATE"), "");
if(getCurrentLine()>1)
sendMessageToWinXound(T("CABBAGE_SELECT_LINE"), getCurrentLine()); 
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
              sendMessageToWinXound(T("CABBAGE_DEBUG"), "Debug message string is empty?");
              break;
          }

      }

      filter->clearDebugMessageArray();
	  sendMessageToWinXound(T("CABBAGE_DEBUG"), text);

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
              //cabbageCsoundEditor->setCsoundOutputText(text);
              break;
          }

      }

      filter->clearDebugMessageArray();
	  cabbageCsoundEditor->setCsoundOutputText(text);
	  //sendMessageToWinXound(T("CABBAGE_DEBUG"), text);

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
//const MessageManagerLock mmLock; 
    deleteFilter();
	deviceManager->closeAudioDevice();
	filter = createCabbagePluginFilter(csdFile.getFullPathName(), isGuiEnabled());
	//filter->suspendProcessing(isGuiEnabled());
	filter->addChangeListener(this);
	filter->sendChangeMessage();
	filter->createGUI(csdFile.loadFileAsString());
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
	sendMessageToWinXound(T("CABBAGE_LOADED"), "");
#endif

	cabbageCsoundEditor->setName(csdFile.getFullPathName());
	if(cabbageCsoundEditor->isVisible()){
		//cabbageCsoundEditor->toFront(true);
		//this->toBehind(cabbageCsoundEditor);
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
	return appProperties->getUserSettings();
}

void StandaloneFilterWindow::showAudioSettingsDialog()
{
	const int numIns = filter->getNumInputChannels() <= 0 ? JucePlugin_MaxNumInputChannels : filter->getNumInputChannels();
    const int numOuts = filter->getNumOutputChannels() <= 0 ? JucePlugin_MaxNumOutputChannels : filter->getNumOutputChannels();

    AudioDeviceSelectorComponent selectorComp (*deviceManager,
                                               numIns, numIns, numOuts, numOuts,
                                               true, false, true, false);

    selectorComp.setSize (400, 350);
	setAlwaysOnTop(false);
    DialogWindow::showModalDialog(TRANS("Audio Settings"), &selectorComp, this, Colours::white, true, false, false);
	setAlwaysOnTop(true);
}
//==============================================================================
void StandaloneFilterWindow::closeButtonPressed()
{
	cabbageCsoundEditor->closeButtonPressed();
	JUCEApplication::quit();
}

void StandaloneFilterWindow::resized()
{
    DocumentWindow::resized();
    optionsButton.setBounds (8, 6, 60, getTitleBarHeight() - 8);
}

void StandaloneFilterWindow::showInfoWindow(String commandType)
{
	String helpText("");
	if(commandType==T("Open file"))
		helpText = T("This command opens a 'cabbaged' Csound text file. A Cabbage .csd file is the same as a regular Csound file only it has a special section marked <Cabbage> that includes information about how the Cabbage instrument will look and interact with Csound. Any Csound file can be loaded and played with Cabbage. The score will start playing back as soon as it opens, and you should probably replace the CsOptions with '-n -d'.\n\nTo continue working turn off help windows in the options menu.");
	else if(commandType==T("Save file"))
		helpText = T("This command will save your text into a Cabbage .csd file.\n\nTo continue working turn off help windows in the options menu.");
	else if(commandType==T("Save file as"))
		helpText = T("This command will save your current text into a new Cabbage .csd file.\n\nTo continue working turn off help windows in the options menu.");
	else if(commandType==T("Export plugin"))
		helpText = T("This command will export your Cabbage instrument as a plugin. When exporting plugins Cabbage will prompt you to save your plugin in a set location, under a specific name. Once Cabbage has created the plugin it will make a copy of the current .csd file and locate it in the same folder as the plugin. This new .csd file will have the same name as the plugin and should ALWAYS be in the same directory as the plugin. \n\nYou do not need to keep exporting instruments as plugins every time you modify them. You need only modify the associated source code. To simplify this task, Cabbage will automatically load the associated Cabbage file whenever you export as a plugin. You'll notice this from the main title bar of the editor.\n\nTo continue working turn off help windows in the options menu.");
	else if(commandType==T("Audio settings"))
		helpText = T("This command lets you change your Audio settings. Cabbage handles all audio and MIDI input and output so you don't need to call any devices in the CsOptions section of your source code.\n\nTo continue working turn off help windows in the options menu.");
	else if(commandType==T("New"))
		helpText = T("This command will help you create a new Cabbage instrument/effect. Cabbage instrument are synthesiser capable of creating sounds from scratch while effects process incoming audio. Effects can access the incoming audio by using the 'inch' opcodes. Instruments can access the incoming MIDI data in a host of different ways but thee easiest is to pipe the MIDI data directly to instrument p-fields using the MIDI inter-op command line flags. Examples can be found in the examples folder.\n\nTo continue working turn off help windows in the options menu.");	
	else if(commandType==T("View Text Editor"))
		helpText = T("This command will launch the integrated text editor. The text editor will always contain the text which corresponds to the instrument that is currently open.\n\nTo continue working turn off help windows in the options menu.");
	else if(commandType==T("Always on top"))
		helpText = T("This command lets you toggle 'Always on top' mode. By default it is turned on, which means your Cabbage instrument will always appear on top of any other applications that are currently open.\n\nTo continue working turn off help windows in the options menu.");
	else if(commandType==T("Update instrument"))
		helpText = T("This command lets you manually update instruments. This is useful if you wish to use a more advanced editor such as WinXound or Csound QT. You can load the file into both the editor and Cabbage. Any time you save the file in the editor can hit 'update instrument' in Cabbage to see the changes.\n\nTo continue working turn off help windows in the options menu.");
	else if(commandType==T("Batch convert"))
		helpText = T("This command will let you convert a selection of Cabbage .csd files into plugins so you don't have to manually open and export each one. Currently this only works on Windows.\n\nTo continue working turn off help windows in the options menu.");
	else if(commandType==T("Help info"))
		helpText = T("This command lets you toggle between displaying these little info windows or not.\n\nTo continue working turn off help windows in the options menu.");

	AlertWindow alert(commandType, helpText, AlertWindow::NoIcon, 0);

	alert.setLookAndFeel(lookAndFeel);
	alert.addButton("Ok", 1);
	alert.runModalLoop();
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
	PopupMenu newType;
	PopupMenu subExport;
	PopupMenu batchProc;
	m.setLookAndFeel(lookAndFeel);
	newType.setLookAndFeel(lookAndFeel);
	subExport.setLookAndFeel(lookAndFeel);
	batchProc.setLookAndFeel(lookAndFeel);

	m.addItem(1, T("Open Cabbage patch"));
	

	newType.addItem(30, T("Effect"));
	newType.addItem(31, T("Instrument"));
	m.addSubMenu(T("New Cabbage..."), newType);

	m.addItem(2, T("View Text Editor"));
    m.addItem(4, TRANS("Audio Settings..."));
    m.addSeparator();
	subExport.addItem(5, TRANS("Plugin Synth"));
	subExport.addItem(6, TRANS("Plugin Effect"));
	m.addSubMenu(TRANS("Export"), subExport);
	if(helpInfo)
	m.addItem(10, T("Enable help dialogue"), true, true);
	else
	m.addItem(10, T("Enable help dialogue"));

    m.addSeparator();
	if(isAlwaysOnTop())
	m.addItem(7, T("Always on Top"), true, true);
	else
	m.addItem(7, T("Always on Top"));
	m.addItem(8, T("Update instrument"));
	/*
	if(filter->getMidiDebug())
    m.addItem(9, TRANS("Show MIDI Debug Information"), true, true);
	else
	m.addItem(9, TRANS("Show MIDI Debug Information"));
	*/

	batchProc.addItem(11, TRANS("Effects"));
	batchProc.addItem(12, TRANS("Synths"));
	m.addSubMenu(TRANS("Batch Convert"), batchProc);
	m.addSeparator();
	if(timerRunning)
	m.addItem(99, T("Cabbage Dance"), true, true);
	else
	m.addItem(99, T("Cabbage Dance"));

	FileChooser saveFC(T("Save Cabbage file as..."), File::nonexistent, T("*.csd"));
	
	

	int options = m.showAt (&optionsButton);
 
	//----- open file ------
	if(options==1){
		if(helpInfo)showInfoWindow("Open file");
		else
		openFile();
	}
	//----- view text editor ------
	else if(options==2){
	if(helpInfo)showInfoWindow("View text editor");
	else{
	cabbageCsoundEditor->setCsoundFile(csdFile);
		this->toBehind(cabbageCsoundEditor);
		cabbageCsoundEditor->setVisible(true);
		cabbageCsoundEditor->toFront(true);
		cabbageCsoundEditor->csoundEditor->textEditor->grabKeyboardFocus();
	}
	}
	//----- new effect ------
	else if(options==30){
		if(helpInfo)showInfoWindow("New");
	else{
		cabbageCsoundEditor->setVisible(true);
		cabbageCsoundEditor->csoundEditor->newFile("effect");
		cabbageCsoundEditor->csoundEditor->textEditor->grabKeyboardFocus();
	}
	}
	//----- new instrument ------
	else if(options==31){
		if(helpInfo)showInfoWindow("New");
	else{
	cabbageCsoundEditor->setVisible(true);
	cabbageCsoundEditor->csoundEditor->newFile("instrument");
	cabbageCsoundEditor->csoundEditor->textEditor->grabKeyboardFocus();
	}
	}
	//----- audio settings ------
   	else if(options==4){
			if(helpInfo)showInfoWindow("Audio settings");
	else{
        showAudioSettingsDialog();
		resetFilter();
	}
	}

	//----- export  ------
	else if(options==5)
	if(helpInfo)showInfoWindow("Export plugin");
	else
	exportPlugin(T("VSTi"));
	
	else if(options==6)
	if(helpInfo)showInfoWindow("Export plugin");
	else
	exportPlugin(T("VST"));

	//----- always on top  ------
	else if(options==7)
	if(helpInfo)showInfoWindow("Always on top");
	else	
	if(isAlwaysOnTop())
		this->setAlwaysOnTop(false);
	else
		this->setAlwaysOnTop(true);
	
	//----- toggle help info  ------
	else if(options==10)
//	if(helpInfo)showInfoWindow("help info");
//	else
	if(helpInfo)
	helpInfo = false;
	else
	helpInfo = true;
	
	//----- update instrument  ------
    else if(options==8)
	if(helpInfo)showInfoWindow("Update instrument");
	else
        resetFilter();

	/*
   	else if(options==9){
		setAlwaysOnTop(false);

		//hint.showDialog("Cabbage", 0, 0, Colours::beige, true);

		hintDialog->toFront(true);
		if(filter->getMidiDebug())
			filter->setMidiDebug(false);
		else 
			filter->setMidiDebug(true);
        setAlwaysOnTop(true);
	}*/

	//----- batch process ------
	else if(options==11)
	if(helpInfo)showInfoWindow("Batch convert");
	else
		BatchProcess(T("VST"));

	else if(options==12)
	if(helpInfo)showInfoWindow("Batch convert");
	else
		BatchProcess(T("VSTi"));

	else if(options==99){
		if(!timerRunning){
		startTimer(20);
		timerRunning = true;
		}
		else{
		stopTimer();
		timerRunning = false;
		}
	}
		
	repaint();
}

//==============================================================================
// open/save/save as methods
//==============================================================================
void StandaloneFilterWindow::openFile()
{
	FileChooser openFC(T("Open a Cabbage .csd file..."), File::nonexistent, T("*.csd"));

	if(openFC.browseForFileToOpen()){
		csdFile = openFC.getResult();
		resetFilter();
		cabbageCsoundEditor->setCsoundFile(csdFile);
	}

}

void StandaloneFilterWindow::saveFile()
{
csdFile.replaceWithText(cabbageCsoundEditor->getCurrentText());
resetFilter();
}

void StandaloneFilterWindow::saveFileAs()
{
FileChooser saveFC(T("Save Cabbage file as..."), File::nonexistent, T("*.csd"));
	if(saveFC.browseForFileToOpen()){
		csdFile = saveFC.getResult().withFileExtension(T(".csd"));
		csdFile.replaceWithText(cabbageCsoundEditor->getCurrentText());
		cabbageCsoundEditor->setCsoundFile(csdFile);
		resetFilter();
	}

}
//==============================================================================
// Export plugin method
//==============================================================================
int StandaloneFilterWindow::exportPlugin(String type){
File thisFile(File::getSpecialLocation(File::currentApplicationFile));
#ifdef LINUX
FileChooser saveFC(T("Save as..."), File::nonexistent, T(""));
String VST;
	if (saveFC.browseForFileToSave(true)){
		if(type.contains("VSTi"))
			VST = thisFile.getParentDirectory().getFullPathName() + T("/CabbagePluginSynth.so");
		else if(type.contains(T("VSTfx")))
			VST = thisFile.getParentDirectory().getFullPathName() + T("CabbagePluginEffect.so");
		else if(type.contains(T("AU"))){
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
FileChooser saveFC(T("Save plugin as..."), File::nonexistent, T(""));
String VST;
	if (saveFC.browseForFileToSave(true)){
		if(type.contains("VSTi"))
			VST = thisFile.getParentDirectory().getFullPathName() + T("\\CabbagePluginSynth.dat");
		else if(type.contains(T("VST")))
			VST = thisFile.getParentDirectory().getFullPathName() + T("\\CabbagePluginEffect.dat");
		else if(type.contains(T("AU"))){
			showMessage("This feature only works on computers running OSX");
			}
	//showMessage(VST);
	File VSTData(VST);
	if(!VSTData.exists())showMessage("problem with plugin lib");
	else{
	File dll(saveFC.getResult().withFileExtension(".dll").getFullPathName());
	//showMessage(dll.getFullPathName());
	if(!VSTData.copyFileTo(dll))	showMessage("problem moving plugin lib, make sure it's not currently open in your plugin host!");


	
	File loc_csdFile(saveFC.getResult().withFileExtension(".csd").getFullPathName());
	loc_csdFile.replaceWithText(csdFile.loadFileAsString());
	setUniquePluginID(dll, loc_csdFile);
	
#ifdef Cabbage_Named_Pipe
	sendMessageToWinXound("CABBAGE_PLUGIN_FILE_UPDATE", csdFile.getFullPathName()+T("|")+loc_csdFile.getFullPathName());
	csdFile = loc_csdFile;	
	cabbageCsoundEditor->setName(csdFile.getFullPathName());
	sendMessageToWinXound("CABBAGE_SHOW_MESSAGE|Info", "WinXound has been updated\nyour .csd file");
#endif
	}
	}//end of open save dialog
#endif

#if MACOSX
	
	FileChooser saveFC(T("Save as..."), File::nonexistent, T(""));
	String VST;
	if (saveFC.browseForFileToSave(true)){
		if(type.contains("VSTi"))
			VST = thisFile.getParentDirectory().getFullPathName() + T("//CabbagePluginSynth.dat");
		else if(type.contains(T("VST")))
			VST = thisFile.getParentDirectory().getFullPathName() + T("//CabbagePluginEffect.dat");
		else if(type.contains(T("AU"))){
			showMessage("this feature is coming soon");
			//VST = thisFile.getParentDirectory().getFullPathName() + T("\\CabbageVSTfx.component");
		}
		showMessage(String("Cabbage VST binary:")+VST);
		String plist  = T("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
		plist.append(T("<!DOCTYPE plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">\n"), MAXBYTES);
		plist.append(T("<plist version=\"1.0\">\n"), MAXBYTES);
		plist.append(T("<dict>\n"), MAXBYTES);
		plist.append(T("<key>CFBundleExecutable</key>\n"), MAXBYTES);
		plist.append(T("<string>")+String(saveFC.getResult().getFileNameWithoutExtension())+T("</string>\n"), MAXBYTES);
		plist.append(T("<key>CFBundleIdentifier</key>\n"), MAXBYTES);
		plist.append(T("<string>com.Cabbage.CabbagePlugin</string>\n"), MAXBYTES);
		plist.append(T("<key>CFBundleName</key>\n"), MAXBYTES);
		plist.append(T("<string>CabbagePlugin</string>\n"), MAXBYTES);
		plist.append(T("<key>CFBundlePackageType</key>\n"), MAXBYTES);
		plist.append(T("<string>BNDL</string>\n"), MAXBYTES);
		plist.append(T("<key>CFBundleShortVersionString</key>\n"), MAXBYTES);
		plist.append(T("<string>1.0.0</string>\n"), MAXBYTES);
		plist.append(T("<key>CFBundleSignature</key>\n"), MAXBYTES);
		plist.append(T("<string>PTul</string>\n"), MAXBYTES);
		plist.append(T("<key>CFBundleVersion</key>\n"), MAXBYTES);
		plist.append(T("<string>1.0.0</string>\n"), MAXBYTES);
		plist.append(T("</dict>\n"), MAXBYTES);
		plist.append(T("</plist>\n"), MAXBYTES);
		
		//create a copy of the data package and write it to the new location given by user
		File VSTData(VST);
		if(VSTData.exists())showMessage("lib exists");
		else{
			showMessage("non existo!");	
		}
		
		
		String plugType;
		if(type.contains(T("AU")))
			plugType = T(".component");
		else plugType = T(".vst");
		
		File dll(saveFC.getResult().withFileExtension(plugType).getFullPathName());
		
		if(VSTData.copyFileTo(dll));
		showMessage("copied");
		
		
		
		File pl(dll.getFullPathName()+T("/Contents/Info.plist"));
		showMessage(pl.getFullPathName());
		pl.replaceWithText(plist);
		
		//showMessage(VST+T("/Contents/MacOS/")+saveFC.getResult().getFileNameWithoutExtension());
		File bin(dll.getFullPathName()+T("/Contents/MacOS/CabbagePlugin"));
		
		bin.moveFileTo(dll.getFullPathName()+T("/Contents/MacOS/")+saveFC.getResult().getFileNameWithoutExtension());
		
		
		File loc_csdFile(dll.getFullPathName()+T("/Contents/")+saveFC.getResult().getFileNameWithoutExtension()+T(".csd"));
		
		loc_csdFile.replaceWithText(csdFile.loadFileAsString());
		
		showMessage(loc_csdFile.getFullPathName());
		showMessage(csdFile.loadFileAsString());
		setUniquePluginID(bin, loc_csdFile);
		csdFile = loc_csdFile;
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
	if(tokes.getReference(0).equalsIgnoreCase(T("form"))){
			CabbageGUIClass cAttr(csdText[i].trimEnd(), 0);		
			if(cAttr.getStringProp("pluginID").length()!=4){
			showMessage(T("Your plugin ID is not the right size. It should be 4 characters. Some hosts may not be able to load your plugin"));
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
const char *pluginName = "CabbageEffectNam";


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
		showMessage(T("Internel Cabbage Error: The pluginID was not found"));
	else {
		mFile.seekg (loc, ios::beg);	
		mFile.write(newID.toUTF8(), 4);	
	}

#ifdef WIN32
	//set plugin name based on .csd file
	String plugLibName = csdFile.getFileNameWithoutExtension();
	if(plugLibName.length()<16)
		for(int y=plugLibName.length();y<16;y++)
			plugLibName.append(T(" "), 1);
	
	mFile.seekg (0, ios::beg);
	buffer = (unsigned char*)malloc(sizeof(unsigned char)*file_size);
  	mFile.read((char*)&buffer[0], file_size);
	loc = cabbageFindPluginID(buffer, file_size, pluginName);
	if (loc < 0)
		showMessage(T("Plugin name could not be set?!?"));
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
FileChooser saveFC(T("Select files..."), File::nonexistent, T(""));
String VST;
	if (saveFC.browseForMultipleFilesToOpen()){
		if(type.contains("VSTi"))
			VST = thisFile.getParentDirectory().getFullPathName() + T("\\CabbagePluginSynth.dat");
		else if(type.contains(T("VST")))
			VST = thisFile.getParentDirectory().getFullPathName() + T("\\CabbagePluginEffect.dat");
		else if(type.contains(T("AU"))){
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