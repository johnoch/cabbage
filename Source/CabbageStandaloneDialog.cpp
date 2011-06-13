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
#include "../JuceLibraryCode/juce_PluginHeaders.h"
#include "CabbagePluginProcessor.h"

//==============================================================================
//  Somewhere in the codebase of your plugin, you need to implement this function
//  and make it create an instance of the filter subclass that you're building.
extern CabbagePluginAudioProcessor* JUCE_CALLTYPE createCabbagePluginFilter(String inputfile);

//==============================================================================
StandaloneFilterWindow::StandaloneFilterWindow (const String& title,
                                                const Colour& backgroundColour)
    : DocumentWindow (title, backgroundColour,
                      DocumentWindow::minimiseButton
                       | DocumentWindow::closeButton),
      optionsButton ("options")
{
	setTitleBarButtonsRequired (DocumentWindow::minimiseButton | DocumentWindow::closeButton, false);
    Component::addAndMakeVisible (&optionsButton);
    optionsButton.addListener (this);
    optionsButton.setTriggeredOnMouseDown (true);
	setAlwaysOnTop(true);

// MOD - Stefano Bonetti 
#ifdef Cabbage_Named_Pipe 
	ipConnection = new socketConnection(*this);
	bool openedOk = ipConnection->createPipe(T("cabbage"));
	if(openedOk) Logger::writeToLog(T("Namedpipe created ..."));
#endif 
// MOD - End


	JUCE_TRY
    {
        filter = createCabbagePluginFilter("");
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
	if(ipConnection) 
		ipConnection->disconnect();
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
// listener Callback - updates WinXound compiler output with Cabbage messages
//==============================================================================
void StandaloneFilterWindow::changeListenerCallback(juce::ChangeBroadcaster* /*source*/)
{
// MOD - Stefano Bonetti
#ifdef Cabbage_Named_Pipe
  if(filter && ipConnection->isConnected()){
      String text = "";
      for(int i=0;i<filter->getDebugMessageArray().size();i++)
      {
          if(filter->getDebugMessageArray().getReference(i).length()>0)
          {
              text += String(filter->getDebugMessageArray().getReference(i).toUTF8());
          }
          else 
          {
              text = (T("CABBAGE_DEBUG|")+String("Debug message string is empty?"));
              MemoryBlock messageData (text.toUTF8(), text.getNumBytesAsUTF8());
              ipConnection->sendMessage(messageData);
              break;
          }

      }

      filter->clearDebugMessageArray();
      String messageToSend = "CABBAGE_DEBUG|" + text;
      MemoryBlock messageData (messageToSend.toUTF8(), messageToSend.getNumBytesAsUTF8());
      ipConnection->sendMessage(messageData);

  }
#endif
  // MOD - End
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

	filter = createCabbagePluginFilter(csdFile.getFullPathName());
	filter->addChangeListener(this);
	filter->sendChangeMessage();
	filter->createGUI(csdFile.loadFileAsString());
	this->setName(filter->getPluginName());

    if (filter != nullptr)
    {
        if (deviceManager != nullptr)
            player.setProcessor (filter);

        setContentOwned (filter->createEditorIfNeeded(), true);
    }

    PropertySet* const globalSettings = getGlobalSettings();

    if (globalSettings != nullptr)
        globalSettings->removeValue ("filterState");
	
#ifdef Cabbage_Named_Pipe
	//notify WinXOund that Cabbage is set up and ready for action
	String messageToSend = "CABBAGE_LOADED";
	MemoryBlock messageData(messageToSend.toUTF8(), messageToSend.getNumBytesAsUTF8());
	ipConnection->sendMessage(messageData);
#endif


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
    return ApplicationProperties::getInstance()->getUserSettings();
}

void StandaloneFilterWindow::showAudioSettingsDialog()
{
    AudioDeviceSelectorComponent selectorComp (*deviceManager,
                                               filter->getNumInputChannels(),
                                               filter->getNumInputChannels(),
                                               filter->getNumOutputChannels(),
                                               filter->getNumOutputChannels(),
                                               true, false, true, false);

    selectorComp.setSize (400, 350);
	setAlwaysOnTop(false);
    DialogWindow::showModalDialog(TRANS("Audio Settings"), &selectorComp, this, Colours::white, true, false, false);
	setAlwaysOnTop(true);
}
//==============================================================================
void StandaloneFilterWindow::closeButtonPressed()
{
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

	String messageToSend = "CABBAGE_SELECT_LINES|4,10";
	MemoryBlock messageData(messageToSend.toUTF8(), messageToSend.getNumBytesAsUTF8());

    if (filter == nullptr)
        return;

    PopupMenu m;
	m.addItem(1, T("Open Cabbage patch"));
    m.addItem(4, TRANS("Audio Settings..."));
    m.addSeparator();
    m.addItem(5, TRANS("Export Plugin Synth"));
    m.addItem(6, TRANS("Export Plugin Effect"));
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
    m.addItem(10, TRANS("Select line"));


	FileChooser openFC(T("Open a Cabbage .csd file..."), File::nonexistent, T("*.csd"));

    switch (m.showAt (&optionsButton))
    {
	case 1:
		if(openFC.browseForFileToOpen()){
		csdFile = openFC.getResult();
		resetFilter();
		}
        break;
    case 4:
        showAudioSettingsDialog();
        break;

	case 5: 
		exportPlugin("VSTi");
		break;

	case 6:
		exportPlugin("VST");
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
		ipConnection->sendMessage(messageData);
		break;

    default:
        break;
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
	File dll(saveFC.getResult().withFileExtension(".so").getFullPathName());
	showMessage(dll.getFullPathName());
	if(VSTData.copyFileTo(dll))	showMessage("moved");
	File loc_csdFile(saveFC.getResult().withFileExtension(".csd").getFullPathName());
	loc_csdFile.replaceWithText(csdFile.loadFileAsString());
	}
#elif WIN32
FileChooser saveFC(T("Save as..."), File::nonexistent, T(""));
String VST;
	if (saveFC.browseForFileToSave(true)){
		if(type.contains("VSTi"))
			VST = thisFile.getParentDirectory().getFullPathName() + T("\\CabbagePlugin.dll");
		else if(type.contains(T("VSTfx")))
			VST = thisFile.getParentDirectory().getFullPathName() + T("\\CabbagePluginFX.dll");
		else if(type.contains(T("AU"))){
			showMessage("This feature only works on computers running OSX");
			}
	//showMessage(VST);
	File VSTData(VST);
	if(VSTData.exists())showMessage("lib exists");
	File dll(saveFC.getResult().withFileExtension(".dll").getFullPathName());
	//showMessage(dll.getFullPathName());
	if(VSTData.copyFileTo(dll))	showMessage("moved");
	File loc_csdFile(saveFC.getResult().withFileExtension(".csd").getFullPathName());
	loc_csdFile.replaceWithText(csdFile.loadFileAsString());
	
	
#ifdef Cabbage_Named_Pipe
	String messageToSend = "CABBAGE_PLUGIN_FILE_UPDATED|" + csdFile.getFullPathName()+T("|")+loc_csdFile.getFullPathName();
	MemoryBlock messageData(messageToSend.toUTF8(), messageToSend.getNumBytesAsUTF8());
	ipConnection->sendMessage(messageData);
	csdFile = loc_csdFile;	
	messageData.removeSection(0, messageToSend.getNumBytesAsUTF8());
	messageToSend = "CABBAGE_SHOW_MESSAGE|Info|Cabbage has been updated";
	messageData.append(messageToSend.toUTF8(),messageToSend.getNumBytesAsUTF8()); 
	ipConnection->sendMessage(messageData);
#endif

	}
#endif


return 0;	
}
