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

#define VERSION "Cabbage v0.03.0 BETA"

#define MAX_BUFFER_SIZE 1024
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
guiOnOff(guiOnOff),
currentLine(-99),
noSteps(0),
noPatterns(0),
timeCounter(0),
beat(0),
bpm(120),
patMatrixActive(0)
{
//reset patMatrix. If this has more than one we know that
//pattern matrix object is being used
patStepMatrix.clear();
patPfieldMatrix.clear();
setPlayConfigDetails(2, 2, 44100, 512); 

#ifndef Cabbage_No_Csound
//don't start of run Csound in edit mode
if(!isGuiEnabled()){
csound = new Csound();
csound->PreCompile();
csound->SetMessageCallback(CabbagePluginAudioProcessor::messageCallback);
csound->SetHostData(this);
//for host midi to get sent to Csound, don't need this for standalone
//but might use it in the future foir midi mapping to controls
csound->SetExternalMidiInOpenCallback(OpenMidiInputDevice);
csound->SetExternalMidiReadCallback(ReadMidiData); 
//csound->SetExternalMidiOutOpenCallback(OpenMidiOutputDevice);
//csound->SetExternalMidiWriteCallback(WriteMidiData);
csoundChanList = NULL;
numCsoundChannels = 0;
csndIndex = 32;

//if(!csdFile.exists())showMessage("Csound file doesn't exist? The file should be in the same directory as the plugin");

if(!inputfile.isEmpty()){
csCompileResult = csound->Compile( const_cast<char*>(inputfile.toUTF8().getAddress()));
if(csCompileResult==0){
	//simple hack to allow tables to be set up correctly. 
	csound->PerformKsmps();
	csound->SetScoreOffsetSeconds(0);
	csound->RewindScore();
	//set up PVS struct
    dataout = new PVSDATEXT;
    dataout->N = 1024;
    dataout->format = 0;
    dataout->overlap = 256;
    dataout->winsize = 1024;
    dataout->frame = (float *) calloc(sizeof(float),(dataout->N+2));

	csound->SetMessageCallback(CabbagePluginAudioProcessor::messageCallback);
	if(csound->GetSpout()==nullptr);
	CSspout = csound->GetSpout();
	CSspin  = csound->GetSpin();
	numCsoundChannels = csoundListChannels(csound->GetCsound(), &csoundChanList);
	csndIndex = csound->GetKsmps();
	cs_scale = csound->Get0dBFS();
	csoundStatus = true;
	debugMessageArray.add(VERSION);
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
}//end of guiEnabled() check
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
guiOnOff(0),
currentLine(-99),
noSteps(0),
noPatterns(0),
timeCounter(0),
beat(0),
bpm(120),
patMatrixActive(0)
{
//Cabbage plugins always try to load a csd file with the same name as the plugin library.
//Therefore we need to find the name of the library and append a '.csd' to it. 
	
#ifdef MACOSX
String osxCSD = File::getSpecialLocation(File::currentApplicationFile).getFullPathName()+String("/Contents/")+File::getSpecialLocation(File::currentApplicationFile).getFileName();
File thisFile(osxCSD); 
Logger::writeToLog("MACOSX defined OK");
#else 
File thisFile(File::getSpecialLocation(File::currentApplicationFile)); 
#endif
csdFile = thisFile.withFileExtension(String(".csd")).getFullPathName();


	
if(csdFile.exists())
Logger::writeToLog("File exists:"+String(csdFile.getFullPathName()));
else
Logger::writeToLog("File doesn't exist"+String(csdFile.getFullPathName()));

createGUI(csdFile.loadFileAsString().toUTF8());

#ifndef Cabbage_No_Csound
csound = new Csound();
csound->PreCompile();
csound->SetHostData(this);
//for host midi to get sent to Csound, don't need this for standalone
//but might use it in the future for midi mapping to controls
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
csCompileResult = csound->Compile( const_cast<char*>(csdFile.getFullPathName().toUTF8().getAddress()));
if(csCompileResult==0){
	//simple hack to allow tables to be set up correctly. 
	csound->PerformKsmps();
	csound->SetScoreOffsetSeconds(0);
	csound->RewindScore();
	//set up PVS struct
    dataout = new PVSDATEXT;
    dataout->N = 1024;
    dataout->format = 0;
    dataout->overlap = 256;
    dataout->winsize = 1024;
	csound->SetMessageCallback(CabbagePluginAudioProcessor::messageCallback);
	CSspout = csound->GetSpout();
	CSspin  = csound->GetSpin();
	cs_scale = csound->Get0dBFS();
	//numCsoundChannels = csoundListChannels(csound->GetCsound(), &csoundChanList);
	for(int i=0;i<guiCtrls.size();i++){
		csound->SetChannel( guiCtrls.getReference(i).getStringProp("channel").toUTF8(), 
							guiCtrls.getReference(i).getNumProp("value"));
		setParameter(i, guiCtrls.getReference(i).getNumProp("value")); 
	}

	csndIndex = csound->GetKsmps();
	csoundStatus = true;
	debugMessageArray.add(VERSION);
	debugMessageArray.add(String("\n"));
}
else{
	Logger::writeToLog("Csound couldn't compile your file");
	csoundStatus=false;
	//debugMessage = "Csound did not compile correctly. Check for snytax errors by compiling with WinXound";
} 
#endif

}
#endif

//===========================================================
// PLUGIN - DESTRUCTOR 
//===========================================================
CabbagePluginAudioProcessor::~CabbagePluginAudioProcessor()
{
#ifndef Cabbage_No_Csound
patStepMatrix.clear();
patternNames.clear();
patPfieldMatrix.clear();
if(!isGuiEnabled()){
	const MessageManagerLock mmLock;
	if(csound){
		csound->DeleteChannelList(csoundChanList);
		Logger::writeToLog("about to cleanup Csound");
		csound->Cleanup();
		csound->Reset();
		csound = nullptr;
		Logger::writeToLog("Csound cleaned up");
	}
}//end of gui enabled check
#endif
}

//===========================================================
// PARSE CSD FILE AND FILL GUI/GUI-LAYOUT VECTORs.
// NO JUCE WIDGETS GET CREATED IN THIS CLASS. ALL
// GUI OBJECTS ARE CREATED ON THE FLY IN THE CABBAGE PLUGIN 
// EDITOR FROM INFORMATION HELD IN THE GUICONTROLS VECTOR
//===========================================================
void CabbagePluginAudioProcessor::createGUI(String source)
{
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
				if(csdText[i].indexOfWholeWord(String("groupbox "))==-1)
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
						plantFlag = "";	//reset plantFlag when a closing bracket is found
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
								||tokes.getReference(0).equalsIgnoreCase(String("infobutton"))
								||tokes.getReference(0).equalsIgnoreCase(String("snapshot"))
								||tokes.getReference(0).equalsIgnoreCase(String("table"))
								||tokes.getReference(0).equalsIgnoreCase(String("pvsview"))
								||tokes.getReference(0).equalsIgnoreCase(String("hostrecording"))
								||tokes.getReference(0).equalsIgnoreCase(String("groupbox")))
						{
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

							if(cAttr.getStringProp("type").containsIgnoreCase("form"))
								if(cAttr.getStringProp("text").length()>2)
									setPluginName(cAttr.getStringProp("text"));
								else if(cAttr.getStringProp("caption").length()>2)
									setPluginName(cAttr.getStringProp("caption"));
								else setPluginName("Untitled Cabbage Patch!");

							//StringArray log = logGUIAttributes(cAttr, String("Non-Interactive"));
							//debugMessageArray.addArray(logGUIAttributes(cAttr, String("Non-Interactive")));
							sendChangeMessage();

							if(tokes.getReference(0).equalsIgnoreCase(String("hostbpm"))
								||tokes.getReference(0).equalsIgnoreCase(String("hosttime"))
								||tokes.getReference(0).equalsIgnoreCase(String("hostplaying"))
								||tokes.getReference(0).equalsIgnoreCase(String("hostppqpos"))
								||tokes.getReference(0).equalsIgnoreCase(String("hostrecording")))
								startTimer(50);
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
								cAttr.setStringProp(String("channel"), cAttr.getStringProp("xChannel"));
								guiCtrls.add(cAttr);

								cAttr.setStringProp(String("xyChannel"), String("Y"));
								cAttr.setNumProp("sliderRange",  cAttr.getNumProp("xypadRangeY"));
								cAttr.setNumProp("min",  cAttr.getNumProp("minY"));
								cAttr.setNumProp("max",  cAttr.getNumProp("maxY"));
								cAttr.setStringProp(String("channel"), cAttr.getStringProp("yChannel"));
								//append 'dummy' to name so the editor know not to display the 
								//second xypad
								cAttr.setStringProp("name", cAttr.getStringProp("name")+String("dummy"));
								guiCtrls.add(cAttr);
								guiID++;
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

#ifdef Cabbage_Build_Standalone
#ifndef Cabbage_No_Csound
		//init all channels with their init val
		for(int i=0;i<guiCtrls.size();i++)
		csound->SetChannel( guiCtrls.getReference(i).getStringProp("channel").toUTF8(), 
							guiCtrls.getReference(i).getNumProp("value"));
#endif
#endif
}

//===========================================================
// SHOW SOURCE EDITOR
//===========================================================
void CabbagePluginAudioProcessor::createAndShowSourceEditor(LookAndFeel* looky)
{
//if(!cabbageCsoundEditor){
//cabbageCsoundEditor = new CabbageEditorWindow(looky);
//cabbageCsoundEditor->setCsoundFile(csdFile);
//cabbageCsoundEditor->setCsoundOutputText(csoundOutput);
//}
//cabbageCsoundEditor->setVisible(true);
}

//===========================================================
// CALLBACKS FOR STANDALONE
//===========================================================
#ifndef Cabbage_No_Csound
void CabbagePluginAudioProcessor::messageCallback(CSOUND* csound, int /*attr*/,  const char* fmt, va_list args)
{
try{
  CabbagePluginAudioProcessor* ud = (CabbagePluginAudioProcessor *) csoundGetHostData(csound);
  char msg[MAX_BUFFER_SIZE];
  vsnprintf(msg, MAX_BUFFER_SIZE, fmt, args);
// MOD - Stefano Bonetti
  ud->debugMessage += String(msg); //We have to append the incoming msg
  ud->csoundOutput += ud->debugMessage;
  ud->debugMessageArray.add(ud->debugMessage);
  //if(ud->cabbageCsoundEditor)
	//  if(ud->cabbageCsoundEditor->isShowing())
		//ud->cabbageCsoundEditor->setCsoundOutputText(ud->csoundOutput);
//#ifdef Cabbage_Named_Pipe && Cabbage_Build_Standalone
  ud->sendChangeMessage();
// MOD - End
//#endif
  ud->debugMessage = "";
  ud = nullptr;
}
catch(...){
		CabbageUtils::showMessage(String("If you insist on playing the keyboard\n \
				like a nutter please run Cabbage in standalone\n \
				mode, outside of WinXound, i.e., launch it on its\n \
				own and then load the csd file you wish to use."));
}
}
#endif

void CabbagePluginAudioProcessor::changeListenerCallback(ChangeBroadcaster *source)
{
#ifdef Cabbage_GUI_Editor
changeMessageType = String("GUI");
sendChangeMessage();
#endif
}

 
//==============================================================================
#ifdef Cabbage_Build_Standalone
CabbagePluginAudioProcessor* JUCE_CALLTYPE createCabbagePluginFilter(String inputfile, bool guiOnOff)
{
    return new CabbagePluginAudioProcessor(inputfile, guiOnOff);
}
#else
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new CabbagePluginAudioProcessor();
}
#endif


//==============================================================================
const String CabbagePluginAudioProcessor::getName() const
{
    return pluginName;
}

int CabbagePluginAudioProcessor::getNumParameters()
{
    return guiCtrls.size();
}

float CabbagePluginAudioProcessor::getParameter (int index)
{	

if(index<(int)guiCtrls.size()){//make sure index isn't out of range
#ifndef Cabbage_No_Csound
        MYFLT* val=0;
		float range = getGUICtrls(index).getNumProp("sliderRange");
		//String test = guiCtrls.getReference(index).getStringProp("channel");
		//Logger::writeToLog(("channel:")+test);

		csoundGetChannelPtr(getCsoundStruct(), &val, guiCtrls.getReference(index).getStringProp("channel").toUTF8(),
                                CSOUND_CONTROL_CHANNEL | CSOUND_OUTPUT_CHANNEL);
		//Logger::writeToLog(String("getParam: *val:")+String(*val));

#ifdef Cabbage_Build_Standalone
	        return *val;
#else
		//Logger::writeToLog(String(*val/range));
		return *val/range;
#endif
#endif
}
else return 0.f; 

}

void CabbagePluginAudioProcessor::setParameter (int index, float newValue)
{
/* this will get called by the plugin GUI sliders or 
by the host via automation. If it's called by the host it will send 
message back to the GUI to notify it to update controls */
#ifndef Cabbage_No_Csound
float range, min, max;
if(index<(int)guiCtrls.size())//make sure index isn't out of range
   {
 // if(guiCtrls.getReference(index).getNumProp("value") != newValue)
 //    {
#ifndef Cabbage_Build_Standalone        
        //scaling in here because incoming values in plugin mode range from 0-1
		range = getGUICtrls(index).getNumProp("sliderRange");
		min = getGUICtrls(index).getNumProp("min");
		guiCtrls.getReference(index).setNumProp("value", (newValue*range)+min);
		//Logger::writeToLog(guiCtrls.getReference(index).getStringProp("channel")+String(":")+String((newValue*range)+min));

		csound->SetChannel(guiCtrls.getReference(index).getStringProp("channel").toUTF8(),  (newValue*range)+min);
#else 
        guiCtrls.getReference(index).setNumProp("value", newValue);
        csound->SetChannel(guiCtrls.getReference(index).getStringProp("channel").toUTF8(), newValue);		
#endif
 //    }
   }
#endif
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
void CabbagePluginAudioProcessor::prepareToPlay (double /*sampleRate*/, int /*samplesPerBlock*/)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
	keyboardState.reset();
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
//host widgets is being used
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
#endif
}
//==============================================================================
void CabbagePluginAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
float* audioBuffer;
#ifndef Cabbage_No_Csound

try{

if(csCompileResult==0){
keyboardState.processNextMidiBuffer (midiMessages, 0, buffer.getNumSamples(), true);
midiBuffer = midiMessages;
ccBuffer = midiMessages;

               for(int i=0;i<buffer.getNumSamples();i++, csndIndex++)
				{
					if(patMatrixActive==1)
					{
						if(timeCounter==(int)(this->getSampleRate()/(bpm/60.f))){
							timeCounter=0;
							for(int y=0;y<noPatterns;y++){
							if(patStepMatrix[beat+(y*noSteps)].state==1){
								String scoreEv;
								scoreEv << "i \"" << patternNames[y].trim()
								<< "\" 0 .5 " << patStepMatrix[beat+(y*noSteps)].p4
								<< " "	<< patPfieldMatrix[y].p5 
								<< " " << patPfieldMatrix[y].p6 
								<< " " << patPfieldMatrix[y].p7
								<< " " << patPfieldMatrix[y].p8
								<< " " << patPfieldMatrix[y].p9;
								csound->InputMessage(scoreEv.toUTF8());
								}
							}
						beat++;
						if(beat==noSteps)
						beat=0;
						}

					timeCounter++;   
					}
				

				 for(int channel = 0; channel < getNumInputChannels(); channel++ )
					{
					audioBuffer = buffer.getSampleData(channel,0);
                       if(csndIndex == csound->GetKsmps())
                       {
                       CSCompResult = csound->PerformKsmps();
                       csndIndex = 0;
                       }
						  
					   //=============================================

					if(!CSCompResult)
	    						{
								pos = csndIndex*getNumInputChannels();
        						CSspin[channel+pos] = audioBuffer[i]*cs_scale;	
								audioBuffer[i] = (CSspout[channel+pos]/cs_scale);	
								}
						else audioBuffer[i]=0; 
				 }
			   }
}
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
}
catch(...){
	CabbageUtils::showMessage(String("If you insist on playing the keyboard\n \
				like a nutter please run Cabbage in standalone\n \
				mode, outside of WinXound, i.e., launch it on its\n \
				own and then load the csd file you wish to use."));
}
#endif

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

int CabbagePluginAudioProcessor::ReadMidiData(CSOUND* /*csound*/, void *userData,
unsigned char *mbuf, int nbytes)
{
try{	
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
		/*   
		else if(message.isController()){
			*mbuf++ = (unsigned char)0x7B + message.getChannel();
			*mbuf++ = (unsigned char)message.getControllerNumber();
			*mbuf++ = (unsigned char)message.getControllerValue();
		   cnt += 3;	  
		  }
        */    
	   }
	   midiData->midiBuffer.clear();
	}

	  return cnt;
}
catch(...){
	CabbageUtils::showMessage(String("If you insist on playing the keyboard\n \
				like a nutter please run Cabbage in standalone\n \
				mode, outside of WinXound, i.e., launch it on its\n \
				own and then load the csd file you wish to use."));
	}
}


int CabbagePluginAudioProcessor::OpenMidiOutputDevice(CSOUND * csound, void **userData, const char* /*devName*/)
{
*userData = csoundGetHostData(csound); 
if(!userData)
cout << "\n\ncan't open midi out\n\n";
return 0;	
}

int CabbagePluginAudioProcessor::WriteMidiData(CSOUND* /*csound*/, void *userData,
const unsigned char *mbuf, int nbytes)
{
	/*
CabbagePluginAudioProcessor *midiData = (CabbagePluginAudioProcessor *)userData;
if(!userData){
	cout << "\n\nInvalid";
	return 0;
	}
midiData->midiBuffer->clear();
int cnt=0;
	*mbuf++;
	*mbuf++;
	MidiMessage message(0xf4, 0, 0, 0);
	message.setChannel(1);
	message.setNoteNumber(60);
	message.setVelocity(127);
	cnt += 3;
	
	midiData->midiBuffer->addEvent(message, 0);
	midiData->keyboardState.processNextMidiEvent(message);

  return cnt;
  */
	return 0;
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


