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
      optionsButton ("options"), isGUIOn(false), pipeOpenedOk(false)
{
	setTitleBarButtonsRequired (DocumentWindow::minimiseButton | DocumentWindow::closeButton, false);
    Component::addAndMakeVisible (&optionsButton);
    optionsButton.addListener (this);
	timerRunning = false;
	yAxis = 0;
    optionsButton.setTriggeredOnMouseDown (true);
	setAlwaysOnTop(true);
	this->setResizable(false, false);

// MOD - Stefano Bonetti 
#ifdef Cabbage_Named_Pipe 
	ipConnection = new socketConnection(*this);
	pipeOpenedOk = ipConnection->createPipe(T("cabbage"));
	if(pipeOpenedOk) Logger::writeToLog(T("Namedpipe created ..."));
#endif 
// MOD - End

	cabbageCsoundEditor = new CabbageEditorWindow();
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
		resetFilter();
		//cabbageCsoundEditor->toFront(true);
	}
	else if(message.contains("fileOpen|")){
		String file = message.substring(9);
		csdFile = File(file);
		resetFilter();
		cabbageCsoundEditor->repaint();
	}
	else if(message.contains("fileSaveAs|")){
		String file = message.substring(11);
		csdFile = File(file);
		resetFilter();
		cabbageCsoundEditor->repaint();
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
	m.addItem(1, T("Open Cabbage patch"));
	

	newType.addItem(30, T("Effect"));
	newType.addItem(31, T("Instrument"));
	m.addSubMenu(T("New Cabbage..."), newType);

	m.addItem(2, T("View Source"));
    m.addItem(4, TRANS("Audio Settings..."));
    m.addSeparator();
	subExport.addItem(5, TRANS("Plugin Synth"));
	subExport.addItem(6, TRANS("Plugin Effect"));
	m.addSubMenu(TRANS("Export"), subExport);
    m.addSeparator();
	if(isAlwaysOnTop())
	m.addItem(7, T("Always on Top"), true, true);
	else
	m.addItem(7, T("Always on Top"));
	m.addItem(8, T("Update instrument"));
	if(filter->getMidiDebug())
    m.addItem(9, TRANS("Show MIDI Debug Information"), true, true);
	else
	m.addItem(9, TRANS("Show MIDI Debug Information"));
    m.addSeparator();
	batchProc.addItem(11, TRANS("Effects"));
	batchProc.addItem(12, TRANS("Synths"));
	m.addSubMenu(TRANS("Batch Convert"), batchProc);
	if(timerRunning)
	m.addItem(99, T("Cabbage Dance"), true, true);
	else
	m.addItem(99, T("Cabbage Dance"));



	FileChooser openFC(T("Open a Cabbage .csd file..."), File::nonexistent, T("*.csd"));
	FileChooser saveFC(T("Save Cabbage file as..."), File::nonexistent, T("*.csd"));
	

    switch (m.showAt (&optionsButton))
    {
	case 1:
		if(openFC.browseForFileToOpen()){
		csdFile = openFC.getResult();
		resetFilter();
		cabbageCsoundEditor->openCsoundFile(csdFile);
        break;
		}
	case 2:
		cabbageCsoundEditor->openCsoundFile(csdFile);
		this->toBehind(cabbageCsoundEditor);
		cabbageCsoundEditor->setVisible(true);
		//cabbageCsoundEditor->setFullScreen(true);
		cabbageCsoundEditor->csoundEditor->textEditor->grabKeyboardFocus();
        break;

	case 30:
		cabbageCsoundEditor->setVisible(true);
		cabbageCsoundEditor->csoundEditor->newFile("effect");
		cabbageCsoundEditor->csoundEditor->textEditor->grabKeyboardFocus();
        break;

	case 31:
		cabbageCsoundEditor->setVisible(true);
		cabbageCsoundEditor->csoundEditor->newFile("instrument");
		cabbageCsoundEditor->csoundEditor->textEditor->grabKeyboardFocus();
        break;

    case 4:
        showAudioSettingsDialog();
		resetFilter();
        break;

	case 5: 
		exportPlugin(T("VSTi"));
		break;

	case 6:
		exportPlugin(T("VST"));
		break;

	case 7:
		if(isAlwaysOnTop())
			this->setAlwaysOnTop(false);
		else
			this->setAlwaysOnTop(true);
		break;

    case 8:
        resetFilter();
        break;

    case 9:
		if(filter->getMidiDebug())
			filter->setMidiDebug(false);
		else 
			filter->setMidiDebug(true);
        break;

	case 10:		
		break;

	case 11:
		BatchProcess(T("VST"));
		break;

	case 12:
		BatchProcess(T("VSTi"));
		break;

	case 99:
		if(!timerRunning){
		startTimer(20);
		timerRunning = true;
		}
		else{
		stopTimer();
		timerRunning = false;
		}
		break;

    default:
        break;
    }
		
	repaint();
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
FileChooser saveFC(T("Save as..."), File::nonexistent, T(""));
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
	sendMessageToWinXound("CABBAGE_SHOW_MESSAGE|Info", "WinXound has been updated\nyour .csd file");
#endif
	}
	}//end of open save dialog
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
				showMessage(cAttr.getStringProp("pluginID"));
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
	mFile.seekg (0, ios::beg);
	unsigned char* buffer = (unsigned char*)malloc(sizeof(unsigned char)*file_size);
  	mFile.read((char*)&buffer[0], file_size);
	loc = cabbageFindPluginID(buffer, file_size, pluginID);
	if (loc < 0)
		showMessage(T("Internel Cabbage Error: The pluginID was not found"));
	else {
		showMessage(T("The pluginID was found"));
		mFile.seekg (loc, ios::beg);	
		mFile.write(newID.toUTF8(), 4);	
	}
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