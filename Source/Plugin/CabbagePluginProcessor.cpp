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

#include "CabbagePluginProcessor.h"   
#include "CabbagePluginEditor.h"

#define CABBAGE_VERSION "Cabbage v0.04.00 BETA"
#define MAX_BUFFER_SIZE 1024

#define LOGGER 1

//these two lines need to be copied to top part of csound.h
//#define int32 int
//#define uint32 unsigned int

//==============================================================================
// There are two different CabbagePluginAudioProcessor constructors. One for the
// standalone application and the other for the plugin library
//==============================================================================
#ifdef Cabbage_Build_Standalone
//===========================================================
// STANDALONE - CONSTRUCTOR 
//===========================================================
CabbagePluginAudioProcessor::CabbagePluginAudioProcessor(String inputfile, bool guiOnOff)
:csoundStatus(false), 
csdFile(File(inputfile)), 
showMIDI(false), 
csCompileResult(1), 
changeMessageType(""), 
guiON(false),
currentLine(-99),
noSteps(0),
noPatterns(0),
timeCounter(0),
beat(0),
bpm(120),
patMatrixActive(0),
masterCounter(0),
xyAutosCreated(false),
updateTable(false),
yieldCallbackBool(false),
yieldCounter(10),
isNativeThreadRunning(false),
soundFileIndex(0),
scoreEvents()
{
//set logger
#ifdef Cabbage_Logger
logFile = File((appProperties->getCommonSettings(true)->getFile().getParentDirectory().getFullPathName()+"/CabbageLog.txt"));
fileLogger = new FileLogger(logFile, String("Cabbage Log.."));
Logger::setCurrentLogger(fileLogger);
#endif
//reset patMatrix. If this has more than one we know that
//pattern matrix object is being used
patStepMatrix.clear();
patPfieldMatrix.clear();
setPlayConfigDetails(2, 2, 44100, 512); 
#ifndef Cabbage_No_Csound
//String localCsoundDirectory = File(inputfile).getParentDirectory().getFullPathName()+"/csound";
//if(File(localCsoundDirectory).exists())
//	if(csoundSetGlobalEnv(String("OPCODEDIR64").toUTF8(), localCsoundDirectory.toUTF8()))
//		Logger::writeToLog("couldn't write environment variables");


//don't start of run Csound in edit mode
csound = new Csound();

//Logger::writeToLog(csound->GetEnv("OPCODEDIR64"));
csound->PreCompile();
csound->SetHostData(this);

csound->SetMessageCallback(CabbagePluginAudioProcessor::messageCallback);
//for host midi to get sent to Csound, don't need this for standalone
//but might use it in the future foir midi mapping to controls
csound->SetExternalMidiInOpenCallback(OpenMidiInputDevice);
csound->SetExternalMidiReadCallback(ReadMidiData); 
//csound->SetExternalMidiOutOpenCallback(OpenMidiOutputDevice);
//csound->SetExternalMidiWriteCallback(WriteMidiData);


csoundPerfThread = new CsoundPerformanceThread(csound);
csoundPerfThread->SetProcessCallback(CabbagePluginAudioProcessor::YieldCallback, (void*)this);

csoundChanList = NULL;
numCsoundChannels = 0;
csndIndex = 32;

//set up PVS struct
dataout = new PVSDATEXT;

if(!inputfile.isEmpty()){
File(inputfile).setAsCurrentWorkingDirectory();
csCompileResult = csound->Compile(const_cast<char*>(inputfile.toUTF8().getAddress()));

if(csCompileResult==0){

        //simple hack to allow tables to be set up correctly. 
        csound->PerformKsmps();
        csound->SetScoreOffsetSeconds(0);
        csound->RewindScore();
        Logger::writeToLog("Csound compiled your file");
        
		//csound->SetYieldCallback(CabbagePluginAudioProcessor::yieldCallback);
        if(csound->GetSpout()==nullptr);
        CSspout = csound->GetSpout();
        CSspin  = csound->GetSpin();
        numCsoundChannels = csoundListChannels(csound->GetCsound(), &csoundChanList);
        csndIndex = csound->GetKsmps();
		csdKsmps = csound->GetKsmps();
		soundFilerTempVector = new MYFLT[csdKsmps];
        cs_scale = csound->Get0dBFS();
        csoundStatus = true;
        debugMessageArray.add(CABBAGE_VERSION);
        debugMessageArray.add(String("\n"));
}
else{
        Logger::writeToLog("Csound couldn't compile your file");
        csoundStatus=false;
        //debugMessage = "Csound did not compile correctly. Check for snytax errors by compiling with WinXound";
}
}
else
Logger::writeToLog("Welcome to Cabbage");

#endif

#ifdef Cabbage_Plugin_Host
createGUI(csdFile.loadFileAsString());
#endif


}
#else

//===========================================================
// PLUGIN - CONSTRUCTOR 
//===========================================================
CabbagePluginAudioProcessor::CabbagePluginAudioProcessor():
csoundStatus(false), 
showMIDI(false), 
csCompileResult(1), 
changeMessageType(""), 
guiON(false),
currentLine(-99),
noSteps(0),
noPatterns(0),
timeCounter(0),
beat(0),
bpm(120),
patMatrixActive(0),
masterCounter(0),
xyAutosCreated(false),
updateTable(false),
yieldCallbackBool(false),
yieldCounter(10),
soundFileIndex(0)
{
//Cabbage plugins always try to load a csd file with the same name as the plugin library.
//Therefore we need to find the name of the library and append a '.csd' to it. 
#ifdef MACOSX
String osxCSD = File::getSpecialLocation(File::currentApplicationFile).getFullPathName()+String("/Contents/")+File::getSpecialLocation(File::currentApplicationFile).getFileName();
File thisFile(osxCSD); 
Logger::writeToLog("MACOSX defined OK");
#else  
File thisFile(File::getSpecialLocation(File::currentExecutableFile)); 
#endif
csdFile = thisFile.withFileExtension(String(".csd")).getFullPathName();

Logger::writeToLog(File::getSpecialLocation(File::currentExecutableFile).getFullPathName());
        
if(csdFile.exists())
Logger::writeToLog("File exists:"+String(csdFile.getFullPathName()));
else
Logger::writeToLog("File doesn't exist"+String(csdFile.getFullPathName()));


File(csdFile.getFullPathName()).setAsCurrentWorkingDirectory();

//set logger
#ifdef Cabbage_Logger
logFile = File(File(csdFile.getFullPathName()).getParentDirectory()+"/CabbageLog.txt"));
fileLogger = new FileLogger(logFile, String("Cabbage Log.."));
Logger::setCurrentLogger(fileLogger);
#endif

#ifndef Cabbage_No_Csound
csound = new Csound();
csound->PreCompile();
csound->SetHostData(this);
//for host midi to get sent to Csound, don't need this for standalone
//but might use it in the future for midi mapping to controls
csound->SetMessageCallback(CabbagePluginAudioProcessor::messageCallback);
csound->SetExternalMidiInOpenCallback(OpenMidiInputDevice);
csound->SetExternalMidiReadCallback(ReadMidiData); 
csound->SetExternalMidiOutOpenCallback(OpenMidiOutputDevice);
csound->SetExternalMidiWriteCallback(WriteMidiData);


patStepMatrix.clear();
patternNames.clear();
patPfieldMatrix.clear();

csoundChanList = NULL;
numCsoundChannels = 0;
csndIndex = 32;
startTimer(15);

csCompileResult = csound->Compile(const_cast<char*>(csdFile.getFullPathName().toUTF8().getAddress()));
csdFile.setAsCurrentWorkingDirectory();
if(csCompileResult==0){
	Logger::writeToLog("compiled Ok");
        //simple hack to allow tables to be set up correctly. 
        csound->PerformKsmps();
        csound->SetScoreOffsetSeconds(0);
        csound->RewindScore();
        //set up PVS struct
		dataout = new PVSDATEXT;
        
        CSspout = csound->GetSpout();
        CSspin  = csound->GetSpin();
        cs_scale = csound->Get0dBFS();
        numCsoundChannels = csoundListChannels(csound->GetCsound(), &csoundChanList);
        csndIndex = csound->GetKsmps();
		csdKsmps = csound->GetKsmps();
		//soundFilerVector = new MYFLT[csdKsmps];
        csoundStatus = true;
        debugMessageArray.add(VERSION);
        debugMessageArray.add(String("\n"));
}
else{
        Logger::writeToLog("Csound couldn't compile your file");
        csoundStatus=false;
} 
#endif
createGUI(csdFile.loadFileAsString());
}
#endif

//===========================================================
// PLUGIN - DESTRUCTOR 
//===========================================================
CabbagePluginAudioProcessor::~CabbagePluginAudioProcessor()
{
Logger::setCurrentLogger (nullptr);
#ifndef Cabbage_No_Csound
patStepMatrix.clear();
patternNames.clear();
patPfieldMatrix.clear();

        const MessageManagerLock mmLock;
        if(csound){
			if(csoundPerfThread){
				csoundPerfThread->Stop();
				csoundPerfThread = nullptr;
			}
                csound->DeleteChannelList(csoundChanList);
                Logger::writeToLog("about to cleanup Csound");
                csound->Cleanup();
                csound->Reset();
                csound = nullptr;
                Logger::writeToLog("Csound cleaned up");
				for(int i=0;i<audioSourcesArray.size();i++){
					audioSourcesArray[i]->sourceChannelInfo.buffer = nullptr;
					audioSourcesArray[i]->audioSourceBuffer = nullptr;
				}
				audioSourcesArray.clear();
        }
		
		soundFilerTempVector = nullptr;

#endif
}

int CabbagePluginAudioProcessor::performEntireScore(){

	if(!isNativeThreadRunning){
	csoundPerfThread->Play();
	isNativeThreadRunning = true;
	}
	return 1;
}

//this callback will be employed when users run with Csound audio IO rather than Cabbage
void CabbagePluginAudioProcessor::YieldCallback(void* data){
	CabbagePluginAudioProcessor *cabbage = (CabbagePluginAudioProcessor *)data;
	cabbage->sendOutgoingMessagesToCsound();
	cabbage->updateCabbageControls();	
}


//===========================================================
// PARSE CSD FILE AND FILL GUI/GUI-LAYOUT VECTORs.
// NO JUCE WIDGETS GET CREATED IN THIS CLASS. ALL
// GUI OBJECTS ARE CREATED ON THE FLY IN THE CABBAGE PLUGIN 
// EDITOR FROM INFORMATION HELD IN THE GUICONTROLS VECTOR
//===========================================================
void CabbagePluginAudioProcessor::createGUI(String source)
{
guiLayoutCtrls.clear();       
guiCtrls.clear();
int checkGUI = isGuiEnabled();
setGuiEnabled((false));
int guiID=0;
StringArray csdText;
int lines=1;
String csdLine("");
csdText.addLines(source);
bool multiComment = false;
bool multiLine = false;
        //reset the size of filter's guiCtrls as the constructor can be called many times..
    for(int i=0;i<csdText.size();i++)
        {
                if(csdText[i].indexOfWholeWordIgnoreCase(String("</Cabbage>"))==-1)
						{
                                if(!csdText[i].contains("multitab "))//we don't enter for multitab, plants need to be created first
                                        if(csdText[i].trim().isNotEmpty()){
                                                
                                                if(csdText[i].contains("), \\")||
                                                                                        csdText[i].contains("),\\")||
                                                                                        csdText[i].contains(") \\")){
                                                        multiLine = true;
                                                        csdLine="";
                                                        lines=0;
                                                        while(multiLine){
                                                        if(csdText[i+lines].contains("), \\")||
                                                                                        csdText[i+lines].contains("),\\")||
                                                                                        csdText[i+lines].contains(") \\"))
                                                                lines++;
                                                        else multiLine=false;
                                                        }
                                                        for(int y=0;y<=lines;y++)
                                                        csdLine = csdLine + " "+ csdText[i+y].trim()+" ";
                                                        i=i+lines;
                                                }
                                                else
                                                csdLine = csdText[i]; 
                                                
                                                //tidy up string
                                                csdLine = csdLine.trimStart();
                                                //csdLine = csdLine.removeCharacters(" \\");
                                                //csdLine = csdLine.removeCharacters(",\\");
                                                //Logger::writeToLog(csdLine);
                                                StringArray tokes;
                                                tokes.addTokens(csdLine.trimEnd(), ", ", "\"");
                                                
                                                if(tokes.getReference(0).containsIgnoreCase(String("/*"))){
                                                multiComment = true;
                                                }
                                                if(tokes.getReference(0).containsIgnoreCase(String("*\\"))){
                                                multiComment = false;
                                                }

                                                if(tokes.getReference(0).containsIgnoreCase(String(";"))){
                                                //allows for single line comments
                                                }
                                                else if(tokes.getReference(0).containsIgnoreCase(String("}"))){
                                                plantFlag = ""; //reset plantFlag when a closing bracket is found
                                                presetFlag = "";
                                                }

                                                if(!multiComment)
                                                //populate the guiLayoutCtrls vector with non-interactive widgets
                                                //the host widgets aren't GUI based but they can be added to this
                                                //vector too, as can the editor button. 
                                                if(tokes.getReference(0).equalsIgnoreCase(String("form"))
                                                                ||tokes.getReference(0).equalsIgnoreCase(String("image"))
                                                                ||tokes.getReference(0).equalsIgnoreCase(String("keyboard"))
                                                                ||tokes.getReference(0).equalsIgnoreCase(String("csoundoutput"))
                                                                ||tokes.getReference(0).equalsIgnoreCase(String("line"))
                                                                ||tokes.getReference(0).equalsIgnoreCase(String("label"))
                                                                ||tokes.getReference(0).equalsIgnoreCase(String("hostbpm"))
                                                                ||tokes.getReference(0).equalsIgnoreCase(String("hosttime"))
                                                                ||tokes.getReference(0).equalsIgnoreCase(String("hostplaying"))
                                                                ||tokes.getReference(0).equalsIgnoreCase(String("hostppqpos"))
                                                                ||tokes.getReference(0).equalsIgnoreCase(String("vumeter"))
                                                                ||tokes.getReference(0).equalsIgnoreCase(String("patmatrix"))
                                                                ||tokes.getReference(0).equalsIgnoreCase(String("source"))
																||tokes.getReference(0).equalsIgnoreCase(String("multitab"))
                                                                ||tokes.getReference(0).equalsIgnoreCase(String("infobutton"))
																||tokes.getReference(0).equalsIgnoreCase(String("filebutton"))
																||tokes.getReference(0).equalsIgnoreCase(String("soundfiler"))
                                                                ||tokes.getReference(0).equalsIgnoreCase(String("snapshot"))
                                                                ||tokes.getReference(0).equalsIgnoreCase(String("table"))
                                                                ||tokes.getReference(0).equalsIgnoreCase(String("pvsview"))
                                                                ||tokes.getReference(0).equalsIgnoreCase(String("hostrecording"))
																||tokes.getReference(0).equalsIgnoreCase(String("directorylist"))
                                                                ||tokes.getReference(0).equalsIgnoreCase(String("groupbox")))
                                                {
                                                        CabbageGUIClass cAttr(csdLine.trimEnd(), guiID);
                                                        //showMessage(cAttr.getStringProp("type"));
                                                        csdLine = "";
														
														//add soundfiler buffering sources
														if(tokes.getReference(0).equalsIgnoreCase(String("soundfiler"))){
															addSoundfilerSource(cAttr.getStringProp(("file")), cAttr.getChannels());
															Logger::writeToLog(String(audioSourcesArray.size()-1));
															cAttr.setNumProp("soundfilerIndex", audioSourcesArray.size()-1);
														}
														
														
                                                        //set up plant flag if needed for other widgets
                                                        if(cAttr.getStringProp(String("plant")).isNotEmpty()){
                                                                plantFlag = cAttr.getStringProp(String("plant"));
                                                                presetFlag = cAttr.getStringProp(String("preset"));
                                                        }
                                                        else if(cAttr.getStringProp(String("relToPlant")).equalsIgnoreCase(String("")))
                                                                cAttr.setStringProp(String("relToPlant"), plantFlag);
                                                        guiLayoutCtrls.add(cAttr);
                                                        

														
														
                                                        guiID++;

                                                        if(cAttr.getStringProp("type").containsIgnoreCase("form"))
                                                                if(cAttr.getStringProp("text").length()>2)
                                                                        setPluginName(cAttr.getStringProp("text"));
                                                                else if(cAttr.getStringProp("caption").length()>2)
                                                                        setPluginName(cAttr.getStringProp("caption"));
                                                                else setPluginName("Untitled Cabbage Patch!");

                                                        //StringArray log = logGUIAttributes(cAttr, String("Non-Interactive"));
                                                        //debugMessageArray.addArray(logGUIAttributes(cAttr, String("Non-Interactive")));
                                                        sendChangeMessage();
														//if instrument uses any of the host widgets, or an xypad, turn
														//on the timer
                                                        if(tokes.getReference(0).equalsIgnoreCase(String("hostbpm"))
                                                                ||tokes.getReference(0).equalsIgnoreCase(String("hosttime"))
                                                                ||tokes.getReference(0).equalsIgnoreCase(String("hostplaying"))
                                                                ||tokes.getReference(0).equalsIgnoreCase(String("hostppqpos"))
                                                                ||tokes.getReference(0).equalsIgnoreCase(String("hostrecording")))
                                                                startTimer(20);
                                                }
                                                //populate the guiCtrls vector with interactive widgets
                                                else if(tokes.getReference(0).equalsIgnoreCase(String("hslider"))
                                                                ||tokes.getReference(0).equalsIgnoreCase(String("vslider"))
                                                                ||tokes.getReference(0).equalsIgnoreCase(String("rslider"))
                                                                ||tokes.getReference(0).equalsIgnoreCase(String("combobox"))
                                                                ||tokes.getReference(0).equalsIgnoreCase(String("checkbox"))
                                                                ||tokes.getReference(0).equalsIgnoreCase(String("xypad"))
                                                                ||tokes.getReference(0).equalsIgnoreCase(String("button"))){
                                                        CabbageGUIClass cAttr(csdLine.trimEnd(), guiID);
                                                        csdLine = "";
														//Logger::writeToLog(tokes.getReference(0));
                                                        //attach widget to plant if need be
                                                        if(cAttr.getStringProp(String("relToPlant")).equalsIgnoreCase(String(""))){
                                                                //showMessage(cAttr.getStringProp(String("relToPlant")));
                                                                cAttr.setStringProp(String("relToPlant"), plantFlag);
                                                                //showMessage(String("presetFlag:")+presetFlag);
                                                                //showMessage(cAttr.getStringProp("name"));
                                                                if(cAttr.getStringProp("preset").length()<1)
                                                                        cAttr.setStringProp(String("preset"), presetFlag.trim());
                                                                //showMessage(cAttr.getStringProp("preset"));
                                                        }
												//xypad contain two control paramters, one for x axis and another for y. As such we add two 
												//to our contorl vector so that plugin hosts display two sliders. We name one of the xypad pads
												// 'dummy' so that our editor doesn't display it. Our editor only needs to show one xypad. 
                                                        if(tokes.getReference(0).equalsIgnoreCase(String("xypad"))){
                                                                cAttr.setStringProp(String("xyChannel"), String("X"));
                                                                cAttr.setNumProp("sliderRange",  cAttr.getNumProp("xypadRangeX"));
                                                                cAttr.setNumProp("min",  cAttr.getNumProp("minX"));
                                                                cAttr.setNumProp("max",  cAttr.getNumProp("maxX"));
																cAttr.setNumProp("value", cAttr.getNumProp("valueX"));
                                                                cAttr.setStringProp(String("channel"), cAttr.getStringProp("xChannel"));
                                                                guiCtrls.add(cAttr);

                                                                cAttr.setStringProp(String("xyChannel"), String("Y"));
                                                                cAttr.setNumProp("sliderRange",  cAttr.getNumProp("xypadRangeY"));
                                                                cAttr.setNumProp("min",  cAttr.getNumProp("minY"));
                                                                cAttr.setNumProp("max",  cAttr.getNumProp("maxY"));
																cAttr.setNumProp("value", cAttr.getNumProp("valueY"));
                                                                cAttr.setStringProp(String("channel"), cAttr.getStringProp("yChannel"));
                                                                //append 'dummy' to name so the editor know not to display the 
                                                                //second xypad
                                                                cAttr.setStringProp("name", cAttr.getStringProp("name")+String("dummy"));
                                                                guiCtrls.add(cAttr);
                                                                guiID++;
																startTimer(20);
                                                        }
                                                        else{
                                                        guiCtrls.add(cAttr);
                                                        guiID++;
                                                        }

                                                        //debugMessageArray.addArray(logGUIAttributes(cAttr, String("Interactive")));
                                                        sendChangeMessage();
                                                }

                                        }
                }
                else break;
        }

		//create multitabs now that plants have been inserted to control vector..
		for(int i=0;i<csdText.size();i++)
			{
			if(csdText[i].contains("multitab ") && !csdText[i].contains(";"))
			{
				csdLine = csdText[i];                                             
				csdLine = csdLine.trimStart();
				StringArray tokes;
				tokes.addTokens(csdLine.trimEnd(), ", ", "\"");
				if(tokes.getReference(0).equalsIgnoreCase(String("multitab"))){
					CabbageGUIClass cAttr(csdLine.trimEnd(), guiID);
					//showMessage(cAttr.getStringProp("type"));
					csdLine = "";
					//set up plant flag if needed for other widgets
					if(cAttr.getStringProp(String("plant")).isNotEmpty()){
					plantFlag = cAttr.getStringProp(String("plant"));
					presetFlag = cAttr.getStringProp(String("preset"));
					}
					else if(cAttr.getStringProp(String("relToPlant")).equalsIgnoreCase(String("")))
					cAttr.setStringProp(String("relToPlant"), plantFlag);
					guiLayoutCtrls.add(cAttr);
					guiID++;
				}
			}
		}

		//init all channels with their init val
		for(int i=0;i<guiCtrls.size();i++)
		{
		csound->SetChannel( guiCtrls.getReference(i).getStringProp("channel").toUTF8(), 
												guiCtrls.getReference(i).getNumProp("value"));
		//Logger::writeToLog(guiCtrls.getReference(i).getStringProp("channel")+": "+String(guiCtrls.getReference(i).getNumProp("value")));
		}

		#ifdef Cabbage_Build_Standalone

		if(this->getActiveEditor()){
			getActiveEditor()->repaint();
			//((CabbagePluginAudioProcessorEditor*)getActiveEditor())->setEditMode(false);
				((CabbagePluginAudioProcessorEditor*)getActiveEditor())->setEditMode(false);
			((CabbagePluginAudioProcessorEditor*)getActiveEditor())->InsertGUIControls();
			((CabbagePluginAudioProcessorEditor*)getActiveEditor())->setEditMode(checkGUI);
			//((CabbagePluginAudioProcessorEditor*)getActiveEditor())->setEditMode(true);
		}

#endif
}

//===========================================================
// SHOW SOURCE EDITOR
//===========================================================
//void CabbagePluginAudioProcessor::createAndShowSourceEditor(LookAndFeel* looky)
//{
//if(!cabbageCsoundEditor){
//cabbageCsoundEditor = new CabbageEditorWindow(looky);
//cabbageCsoundEditor->setCsoundFile(csdFile);
//cabbageCsoundEditor->setCsoundOutputText(csoundOutput);
//}
//cabbageCsoundEditor->setVisible(true);
//}

//===========================================================
// CALLBACKS FOR STANDALONE
//===========================================================
//I don't think this is thread safe! 
#ifndef Cabbage_No_Csound
void CabbagePluginAudioProcessor::messageCallback(CSOUND* csound, int /*attr*/,  const char* fmt, va_list args)
{
  CabbagePluginAudioProcessor* ud = (CabbagePluginAudioProcessor *) csoundGetHostData(csound);
  char msg[MAX_BUFFER_SIZE];
  vsnprintf(msg, MAX_BUFFER_SIZE, fmt, args);
// MOD - Stefano Bonetti
  ud->debugMessage += String(msg); //We have to append the incoming msg
  ud->csoundOutput += ud->debugMessage;
  ud->debugMessageArray.add(ud->debugMessage);
  //Logger::writeToLog(String(msg).trim());
  ud->sendChangeMessage();
// MOD - End
//#endif
  //ud->clearDebugMessageArray();
  ud->debugMessage = "";
  ud = nullptr;
}
#endif


//==============================================================================
#ifdef Cabbage_Build_Standalone
CabbagePluginAudioProcessor* JUCE_CALLTYPE createCabbagePluginFilter(String inputfile, bool guiOnOff)
{
    return new CabbagePluginAudioProcessor(inputfile, false);
}
#else
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new CabbagePluginAudioProcessor();
}
#endif

//==========================================================================
//action listener. Listen to messages being sent form xypad automations
//==========================================================================
void CabbagePluginAudioProcessor::changeListenerCallback(ChangeBroadcaster *source)
{
	//is message coming from an xypad
	XYPadAutomation* xyPad = dynamic_cast< XYPadAutomation*>(source);
	if(xyPad){
	#ifdef Cabbage_Build_Standalone
		setParameterNotifyingHost(xyPad->paramIndex, xyPad->getXValue());
		setParameterNotifyingHost(xyPad->paramIndex+1, xyPad->getYValue());
	#else
		setParameterNotifyingHost(xyPad->paramIndex, xyPad->getXValue()/xyPad->getXRange());
		setParameterNotifyingHost(xyPad->paramIndex+1, xyPad->getYValue()/xyPad->getYRange());	
	#endif
	}
}

//==============================================================================
// getTable data from Csound so table editor can draw table 
//==============================================================================
const Array<float> CabbagePluginAudioProcessor::getTable(int tableNum){
		Array<float> points;
		MYFLT* temp;
		int tableSize = csound->GetTable(temp, tableNum);
		if(tableSize>0)
		points = Array<float>(temp, tableSize);
		return points;        
}


//=================================================================================
// Get and Set Parameter methods, called by our editor, and the plugin host...
//=================================================================================
float CabbagePluginAudioProcessor::getParameter (int index)
{       
//Logger::writeToLog("parameterGet:"+String(index)+String(":")+String(getGUICtrls(index).getNumProp("value")));
float range = getGUICtrls(index).getNumProp("sliderRange");
/* this gets called at any time by our host or out GUI editor */	
if(index<(int)guiCtrls.size())//make sure index isn't out of range
	#ifndef Cabbage_Build_Standalone 
	return getGUICtrls(index).getNumProp("value")/range;
	#else
	return getGUICtrls(index).getNumProp("value");
	#endif
else 
	return 0.0f;
}

void CabbagePluginAudioProcessor::setParameter (int index, float newValue)
{
#ifndef Cabbage_No_Csound
/* this will get called by the plugin GUI sliders or 
by the host, via automation. The timer thread in the plugin's editor
will constantly update with the values that have been set here.
We don't actually change any parameters here, we simply add the messages
to a queue. See next method. The updates will only happen when it's safe to do. */
float range, min, max;
//Logger::writeToLog("parameterSet:"+String(newValue));
if(index<(int)guiCtrls.size())//make sure index isn't out of range
   {
	#ifndef Cabbage_Build_Standalone        
    //scaling in here because incoming values in plugin mode range from 0-1
	range = getGUICtrls(index).getNumProp("sliderRange");
	//Logger::writeToLog("inValue:"+String(newValue));
	min = getGUICtrls(index).getNumProp("min");
	
	if(getGUICtrls(index).getStringProp("type")=="xypad")
		newValue = (newValue*range);
	else if(getGUICtrls(index).getStringProp("type")=="combobox")//combo box value need to be rounded...
		newValue = (int)(newValue*range)+min;
	else
		newValue = (newValue*range)+min;
		
	guiCtrls.getReference(index).setNumProp("value", newValue);
	messageQueue.addOutgoingChannelMessageToQueue(guiCtrls.getReference(index).getStringProp("channel").toUTF8(),  newValue, 
	guiCtrls.getReference(index).getStringProp("type"));
	//Logger::writeToLog(String("parameterSet:"+String(newValue)));
	#else 
	//no need to scale here when in standalone mode
    guiCtrls.getReference(index).setNumProp("value", newValue);
	messageQueue.addOutgoingChannelMessageToQueue(guiCtrls.getReference(index).getStringProp("channel").toUTF8(), newValue, 
																			guiCtrls.getReference(index).getStringProp("type"));
	#endif
   }
#endif
}

//==============================================================================
//this method gets called after a performKsmps() to update our GUI controls
//with messages from Csound. For instance, a user might wish to change the position
//of a GUI slider from Csound by using a chnset opcode. The speed at which this is
//updated can be teaked, so as not to hog resources. It might be worth allowing users 
//the option of setting how fast this update...  
void CabbagePluginAudioProcessor::updateCabbageControls()
{
String chanName;
MYFLT* val=0;
//update all control widgets
for(int index=0;index<getGUICtrlsSize();index++)
	{
	float value = csound->GetChannel(guiCtrls.getReference(index).getStringProp("channel").toUTF8());
    getGUICtrls(index).setNumProp("value", value);
	}
	
//update all layout control widgets
//currently this is only needed for table widgets as other layout controls
//don't use channel messages...
for(int index=0;index<getGUILayoutCtrlsSize();index++)
	{	
	if(getGUILayoutCtrls(index).getStringProp("type")=="table")
		{
		for(int y=0;y<getGUILayoutCtrls(index).getNumberOfTableChannels();y++){
			float value = csound->GetChannel(getGUILayoutCtrls(index).getTableChannel(y).toUTF8());
			getGUILayoutCtrls(index).setTableChannelValues(y, value);
			}
		}	
	}
}

//==============================================================================
//this method only gets called when it's safe to do so, i.e., between calls to performKsmps()
//this method sends any channel messages that are in the queue to Csound
#ifndef Cabbage_No_Csound

void CabbagePluginAudioProcessor::sendOutgoingMessagesToCsound()
{
for(int i=0;i<messageQueue.getNumberOfOutgoingChannelMessagesInQueue();i++)
		{
		if(messageQueue.getOutgoingChannelMessageFromQueue(i).type=="directoryList"){
		for(int y=0;y<scoreEvents.size();y++)
			csound->InputMessage(scoreEvents[y].toUTF8());
		//scoreEvents.clear();
		}
		else
		csound->SetChannel(messageQueue.getOutgoingChannelMessageFromQueue(i).channelName.toUTF8(), 
						   messageQueue.getOutgoingChannelMessageFromQueue(i).value);
		}
	messageQueue.flushOutgoingChannelMessages();
}

//==============================================================================
//set up buffered audio source for each sound file object. The method below this one
//fills Csound channels with sampler from our soundfiler controls..
void CabbagePluginAudioProcessor::addSoundfilerSource(String _filename, StringArray channels)
{

	if(File(_filename).exists())Logger::writeToLog("File exists");
	else{
		Logger::writeToLog("Sound filer can't find file");
	}
	
   audioSourcesArray.add(new CabbageAudioSource(_filename));
   audioSourcesArray[audioSourcesArray.size()-1]->channels = channels;

}


//gets sample data from any soundfiler controls and passes it to Csound
void CabbagePluginAudioProcessor::sendAudioToCsoundFromSoundFilers(int numSamples)
{
for(int i=0;i<audioSourcesArray.size();i++){
	AudioSampleBuffer output (2, numSamples);
	audioSourcesArray[i]->sourceChannelInfo.buffer = &output;
	audioSourcesArray[i]->sourceChannelInfo.startSample = 0;
	audioSourcesArray[i]->sourceChannelInfo.numSamples = output.getNumSamples();

	if(audioSourcesArray[i]->isSourcePlaying)
	audioSourcesArray[i]->audioSourceBuffer->getNextAudioBlock(audioSourcesArray[i]->sourceChannelInfo); 
	else
		output.clear();

	for(int index=0;index<audioSourcesArray[i]->channels.size();index++){
	float* samples = output.getSampleData(index);	
	
	for(int y=0;y<numSamples;y++)
		soundFilerTempVector[y] = samples[y];
		
		if(csoundGetChannelPtr(csound->GetCsound(), &soundFilerTempVector, audioSourcesArray[i]->channels[index].toUTF8(),                     
						CSOUND_INPUT_CHANNEL | CSOUND_AUDIO_CHANNEL) != 0)
						Logger::writeToLog("error sending audio to Csound");
						
		}
	}

	
}	
	
//float* audioBuffer;
//soundfilerChannelData;

//	for(int i=0;i<soundFilers.size(); i++)
//		soundFilers[i]->transportSource.getNextAudioBlock(soundfilerChannelData);
		
//		for(int channel = 0; channel < getNumInputChannels(); channel++ )
//			{
//			audioBuffer = soundfilerChannelData.buffer->getSampleData(channel);
//			Logger::writeToLog(String(*audioBuffer));
//			for(int y=0;y<soundfilerChannelData.buffer->getNumSamples();y++, soundFileIndex++)
//				soundFilerVector[soundFileIndex] = audioBuffer[y];
//				if(soundFileIndex==csdKsmps)
//					soundFileIndex=0;
//			}
//			
//	}
//
//	if(csoundGetChannelPtr(csound->GetCsound(), &soundFilerVector, "output",                     
//					CSOUND_INPUT_CHANNEL | CSOUND_AUDIO_CHANNEL) != 0)
//	       			Logger::writeToLog("error sending audio to Csound");

//========================================================================
// Standard plugin methods, getName, getNumParameters, setParamterName, get ProgramName, etc.... 
//==============================================================================
const String CabbagePluginAudioProcessor::getName() const
{
    return pluginName;
}

int CabbagePluginAudioProcessor::getNumParameters()
{
    return guiCtrls.size();
}

const String CabbagePluginAudioProcessor::getParameterName (int index)
{
    if(index<(int)guiCtrls.size())//make sure index isn't out of range
                return guiCtrls.getReference(index).getStringProp("channel");
        else return String::empty;
}

const String CabbagePluginAudioProcessor::getParameterText (int index)
{
        if(index<(int)guiCtrls.size())//make sure index isn't out of range
    return String (guiCtrls.getReference(index).getNumProp("value"), 2);
        else return String::empty;
}

const String CabbagePluginAudioProcessor::getInputChannelName (int channelIndex) const
{
        if(channelIndex<(int)guiCtrls.size())//make sure index isn't out of range
    return String (channelIndex + 1);
        else return String::empty;
}

const String CabbagePluginAudioProcessor::getOutputChannelName (int channelIndex) const
{
        if(channelIndex<(int)guiCtrls.size())//make sure index isn't out of range
    return String (channelIndex + 1);
        else return String::empty;
}

bool CabbagePluginAudioProcessor::isInputChannelStereoPair (int /*index*/) const
{
    return true;
}

bool CabbagePluginAudioProcessor::isOutputChannelStereoPair (int /*index*/) const
{
    return true;
}

bool CabbagePluginAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool CabbagePluginAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

int CabbagePluginAudioProcessor::getNumPrograms()
{
    return 0;
}

int CabbagePluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void CabbagePluginAudioProcessor::setCurrentProgram (int /*index*/)
{
}

const String CabbagePluginAudioProcessor::getProgramName (int /*index*/)
{
    return String::empty;
}

void CabbagePluginAudioProcessor::changeProgramName (int /*index*/, const String& /*newName*/)
{
}

//==============================================================================
void CabbagePluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    keyboardState.reset();
	for(int i=0;i<audioSourcesArray.size();i++)
		audioSourcesArray[i]->audioSourceBuffer->prepareToPlay(samplesPerBlock, sampleRate);
}
//==============================================================================
void CabbagePluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
        keyboardState.reset();
}
//==============================================================================
//this following callback only runs in plugin mode, and only when one of the
//host widgets are being used
void CabbagePluginAudioProcessor::timerCallback(){
#ifndef Cabbage_No_Csound

if(!isGuiEnabled()){
        //initiliase any channels send host information to Csound
        AudioPlayHead::CurrentPositionInfo hostInfo;
        for(int i=0;i<(int)getGUILayoutCtrlsSize();i++){
                if(getGUILayoutCtrls(i).getStringProp("type")==String("hostbpm")){          
                if (getPlayHead() != 0 && getPlayHead()->getCurrentPosition (hostInfo))
                        csound->SetChannel(getGUILayoutCtrls(i).getStringProp("channel").toUTF8(), hostInfo.bpm);
                }
                else if(getGUILayoutCtrls(i).getStringProp("type")==String("hosttime")){            
                if (getPlayHead() != 0 && getPlayHead()->getCurrentPosition (hostInfo))
                        csound->SetChannel(getGUILayoutCtrls(i).getStringProp("channel").toUTF8(), hostInfo.timeInSeconds);
                }
                else if(getGUILayoutCtrls(i).getStringProp("type")==String("hostplaying")){         
                if (getPlayHead() != 0 && getPlayHead()->getCurrentPosition (hostInfo))
                        csound->SetChannel(getGUILayoutCtrls(i).getStringProp("channel").toUTF8(), hostInfo.isPlaying);
                }
                else if(getGUILayoutCtrls(i).getStringProp("type")==String("hostrecording")){       
                if (getPlayHead() != 0 && getPlayHead()->getCurrentPosition (hostInfo))
                        csound->SetChannel(getGUILayoutCtrls(i).getStringProp("channel").toUTF8(), hostInfo.isRecording);
                }
                else if(getGUILayoutCtrls(i).getStringProp("type")==String("hostppqpos")){          
                if (getPlayHead() != 0 && getPlayHead()->getCurrentPosition (hostInfo))
                        csound->SetChannel(getGUILayoutCtrls(i).getStringProp("channel").toUTF8(), hostInfo.ppqPosition);
                }
        }

}// end of GUI enabled check
			for(int y=0;y<xyAutomation.size();y++){
				if(xyAutomation[y])
				xyAutomation[y]->update();
				}
#endif
}
#endif

//==============================================================================
void CabbagePluginAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
if(!isSuspended()){
	float* audioBuffer;
	#ifndef Cabbage_No_Csound

	if(csCompileResult==0){
	keyboardState.processNextMidiBuffer (midiMessages, 0, buffer.getNumSamples(), true);
	midiBuffer = midiMessages;
	ccBuffer = midiMessages;
	

	if(!midiOutputBuffer.isEmpty())
		midiMessages.swapWith(midiOutputBuffer);

	for(int i=0;i<buffer.getNumSamples();i++, csndIndex++)
	   {                                

		for(int channel = 0; channel < getNumInputChannels(); channel++ )
			{
			audioBuffer = buffer.getSampleData(channel,0);
			if(csndIndex == csound->GetKsmps())
			{
				
				getCallbackLock().enter();
				//slow down calls to these functions, no need for them to be firing at k-rate
				yieldCounter = (yieldCounter>10) ? 0 : yieldCounter+1;
				if(yieldCounter==0){
				sendOutgoingMessagesToCsound();
				updateCabbageControls();
				}
				if(audioSourcesArray.size()>0)
				sendAudioToCsoundFromSoundFilers(csound->GetKsmps());		

				CSCompResult = csound->PerformKsmps();					
				getCallbackLock().exit();
				csndIndex = 0;
			}
			if(!CSCompResult)
				{
				pos = csndIndex*getNumInputChannels();
				CSspin[channel+pos] = audioBuffer[i]*cs_scale;  
				audioBuffer[i] = (CSspout[channel+pos]/cs_scale);       
				}
			else audioBuffer[i]=0; 
			}
                        
		}
	}//if not compiled just mute output
	else{
			for(int i=0;i<buffer.getNumSamples();i++, csndIndex++)
					{
					for(int channel = 0; channel < getNumInputChannels(); channel++ )
							{
							audioBuffer = buffer.getSampleData(channel,0);
							audioBuffer[i]=0;
							}
					}
	}
		// in case we have more outputs than inputs, we'll clear any output
		// channels that didn't contain input data, (because these aren't
		// guaranteed to be empty - they may contain garbage).
		for (int i = getNumInputChannels(); i < getNumOutputChannels(); ++i)
		{
			buffer.clear (i, 0, buffer.getNumSamples());
		}
		#endif
}
	if(!midiBuffer.isEmpty())
	midiMessages.swapWith(midiOutputBuffer);
}


//==============================================================================
// MIDI functions
//==============================================================================

#ifndef Cabbage_No_Csound
int CabbagePluginAudioProcessor::OpenMidiInputDevice(CSOUND * csound, void **userData, const char* /*devName*/)
{
*userData = csoundGetHostData(csound);
if(!userData)
cout << "\n\ncan't open midi in\n\n";
return 0;
}

//==============================================================================
// Reads MIDI input data from host, gets called every time there is MIDI input to our plugin
//==============================================================================
int CabbagePluginAudioProcessor::ReadMidiData(CSOUND* /*csound*/, void *userData,
unsigned char *mbuf, int nbytes)
{
        CabbagePluginAudioProcessor *midiData = (CabbagePluginAudioProcessor *)userData;
        if(!userData){
                cout << "\n\nInvalid";
                return 0;
                }
        int cnt=0;

        if(!midiData->midiBuffer.isEmpty() && cnt <= (nbytes - 3)){
           MidiMessage message(0xf4, 0, 0, 0);
           MidiBuffer::Iterator i (midiData->midiBuffer);
           int messageFrameRelativeTothisProcess;
           while (i.getNextEvent (message, messageFrameRelativeTothisProcess))
           {
                   if(message.isNoteOn()){
                        *mbuf++ = (unsigned char)0x90 + message.getChannel();
                   *mbuf++ = (unsigned char)message.getNoteNumber();
                   *mbuf++ = (unsigned char)message.getVelocity();
                   cnt += 3;
                   }
                   else if(message.isNoteOff()){
                        *mbuf++ = (unsigned char)0x80 + message.getChannel();
                   *mbuf++ = (unsigned char)message.getNoteNumber();
                   *mbuf++ = (unsigned char)message.getVelocity();
                   cnt += 3;
                   }
                   else if(message.isAllNotesOff()){
                        *mbuf++ = (unsigned char)0x7B + message.getChannel();
                   *mbuf++ = (unsigned char)message.getNoteNumber();
                   *mbuf++ = (unsigned char)message.getVelocity();
                   cnt += 3;
                   }
				   else if(message.isController()){
						*mbuf++ = (unsigned char)0xB0 + message.getChannel();
				   *mbuf++ = (unsigned char)message.getControllerNumber();
				   *mbuf++ = (unsigned char)message.getControllerValue();
				   cnt += 3;
				   }
				 
           }
           midiData->midiBuffer.clear();
        }
 return cnt;
}

//==============================================================================
// Opens MIDI output device, adding -QN to your CsOptions will causes this method to be called
// as soon as your plugin loads
//==============================================================================
int CabbagePluginAudioProcessor::OpenMidiOutputDevice(CSOUND * csound, void **userData, const char* /*devName*/)
{
*userData = csoundGetHostData(csound); 
if(!userData)
	Logger::writeToLog("\n\ncan't open midi out\n\n");
return 0;       
} 

//==============================================================================
// Write MIDI data to plugin's MIDI output. Each time Csound outputs a midi message this
// method should be called. Note: you must have -Q set in your CsOptions
//==============================================================================
int CabbagePluginAudioProcessor::WriteMidiData(CSOUND* /*csound*/, void *_userData,
const unsigned char *mbuf, int nbytes)
{
CabbagePluginAudioProcessor *userData = (CabbagePluginAudioProcessor *)_userData;
if(!userData){
	Logger::writeToLog("\n\nInvalid");
    return 0;
  }	

MidiMessage message(mbuf, nbytes, 0);
//Logger::writeToLog(String(message.getNoteNumber()));
userData->midiOutputBuffer.addEvent(message, 0);
return nbytes;
}

#endif
//==============================================================================
bool CabbagePluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}
 
AudioProcessorEditor* CabbagePluginAudioProcessor::createEditor()
{
    return new CabbagePluginAudioProcessorEditor (this);
}

//==============================================================================
void CabbagePluginAudioProcessor::getStateInformation (MemoryBlock& /*destData*/)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void CabbagePluginAudioProcessor::setStateInformation (const void* /*data*/, int /*sizeInBytes*/)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================


