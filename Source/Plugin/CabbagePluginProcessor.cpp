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

#define VERSION "Cabbage v0.02.1 BETA"

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
currentLine(-99)
{
#ifndef Cabbage_No_Csound
//don't start of run Csound in edit mode
if(!isGuiEnabled()){
csound = new Csound();
csound->PreCompile();
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
csound->SetMessageCallback(CabbagePluginAudioProcessor::messageCallback);

if(!inputfile.isEmpty()){
csCompileResult = csound->Compile( const_cast<char*>(inputfile.toUTF8().getAddress()));
if(csCompileResult==0){
	//simple hack to allow tables to be set up correctly. 
	csound->PerformKsmps();
	csound->SetScoreOffsetSeconds(0);
	csound->RewindScore();
	CSspout = csound->GetSpout();
	CSspin  = csound->GetSpin();
	numCsoundChannels = csoundListChannels(csound->GetCsound(), &csoundChanList);
	csndIndex = csound->GetKsmps();
	cs_scale = csound->Get0dBFS();
	csoundStatus = true;
	debugMessageArray.add(VERSION);
	debugMessageArray.add(T("\n"));
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
}
#else

//===========================================================
// PLUGIN - CONSTRUCTOR 
//===========================================================
CabbagePluginAudioProcessor::CabbagePluginAudioProcessor():
guiOnOff(false)
{
//Cabbage plugins always try to load a csd file with the same name as the plugin library.
//Therefore we need to find the name of the library and append a '.csd' to it. 
#ifdef MACOSX
String osxCSD = File::getSpecialLocation(File::currentApplicationFile).getFullPathName()+T("/Contents/")+File::getSpecialLocation(File::currentApplicationFile).getFileName();
File thisFile(osxCSD); 
#else 
File thisFile(File::getSpecialLocation(File::currentApplicationFile)); 
#endif
File csdFile = thisFile.withFileExtension(T(".csd")).getFullPathName();

if(csdFile.exists())
Logger::writeToLog("File exists:"+String(csdFile.getFullPathName()));
else
Logger::writeToLog("File doesn't exist");

createGUI(csdFile.loadFileAsString().toUTF8());

#ifndef Cabbage_No_Csound
csound = new Csound();
csound->PreCompile();
csound->SetHostData(this);
//for host midi to get sent to Csound, don't need this for standalone
//but might use it in the future foir midi mapping to controls
csound->SetExternalMidiInOpenCallback(OpenMidiInputDevice);
csound->SetExternalMidiReadCallback(ReadMidiData); 
csound->SetExternalMidiOutOpenCallback(OpenMidiOutputDevice);
csound->SetExternalMidiWriteCallback(WriteMidiData);


csoundChanList = NULL;
numCsoundChannels = 0;
csndIndex = 32;
csound->SetMessageCallback(CabbagePluginAudioProcessor::messageCallback);
startTimer(15);
csCompileResult = csound->Compile( const_cast<char*>(csdFile.getFullPathName().toUTF8().getAddress()));
if(csCompileResult==0){
	CSspout = csound->GetSpout();
	CSspin  = csound->GetSpin();
	cs_scale = csound->Get0dBFS();
	numCsoundChannels = csoundListChannels(csound->GetCsound(), &csoundChanList);
	for(int i=0;i<guiCtrls.size();i++)
		csound->SetChannel( guiCtrls.getReference(i).getStringProp("channel").toUTF8(), 
							guiCtrls.getReference(i).getNumProp("value"));

	csndIndex = csound->GetKsmps();
	csoundStatus = true;
	debugMessageArray.add(VERSION);
	debugMessageArray.add(T("\n"));
}
else{
	Logger::writeToLog("Csound couldn't compile your file");
	csoundStatus=false;
	//debugMessage = "Csound did not compile correctly. Check for snytax errors by compiling with WinXound";
} 
#endif

}
#endif

CabbagePluginAudioProcessor::~CabbagePluginAudioProcessor()
{
#ifndef Cabbage_No_Csound
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
	csdText.addLines(source);
	bool multiComment = false;
	//reset the size of filter's guiCtrls as the constructor can be called many times..
    for(int i=0;i<csdText.size();i++)
        {
		if(csdText[i].indexOfWholeWordIgnoreCase(T("</Cabbage>"))==-1)
            {
				if(csdText[i].indexOfWholeWord(T("groupbox "))==-1)
					if(csdText[i].trim().isNotEmpty()){
						StringArray tokes;
						tokes.addTokens(csdText[i].trimEnd(), ", ", "\"");
						
						if(tokes.getReference(0).containsIgnoreCase(T("/*"))){
						multiComment = true;
						}
						if(tokes.getReference(0).containsIgnoreCase(T("*\\"))){
						multiComment = false;
						}

						if(tokes.getReference(0).containsIgnoreCase(T(";"))){
						//allows for single line comments
						}
						else if(tokes.getReference(0).containsIgnoreCase(T("}"))){
						plantFlag = "";	//reset plantFlag when a closing bracket is found
						}

						if(!multiComment)
						//populate the guiLayoutCtrls vector with non-interactive widgets
						//the host widgets aren't GUI based but they can be added to this
						//vector too. 
						if(tokes.getReference(0).equalsIgnoreCase(T("form"))
								||tokes.getReference(0).equalsIgnoreCase(T("image"))
								||tokes.getReference(0).equalsIgnoreCase(T("keyboard"))
								||tokes.getReference(0).equalsIgnoreCase(T("csoundoutput"))
								||tokes.getReference(0).equalsIgnoreCase(T("label"))
								||tokes.getReference(0).equalsIgnoreCase(T("hostbpm"))
								||tokes.getReference(0).equalsIgnoreCase(T("hosttime"))
								||tokes.getReference(0).equalsIgnoreCase(T("hostplaying"))
								||tokes.getReference(0).equalsIgnoreCase(T("hostppqpos"))
								||tokes.getReference(0).equalsIgnoreCase(T("vumeter"))
								||tokes.getReference(0).equalsIgnoreCase(T("hostrecording"))
								||tokes.getReference(0).equalsIgnoreCase(T("groupbox"))){
							CabbageGUIClass cAttr(csdText[i].trimEnd(), guiID);
							//set up plant flag if needed for other widgets
							if(cAttr.getStringProp(T("plant")).isNotEmpty())
								plantFlag = cAttr.getStringProp(T("plant"));
							else if(cAttr.getStringProp(T("relToPlant")).equalsIgnoreCase(T("")))
								cAttr.setStringProp(T("relToPlant"), plantFlag);
							guiLayoutCtrls.add(cAttr);
							guiID++;

							if(cAttr.getStringProp("type").containsIgnoreCase("form"))
								if(cAttr.getStringProp("text").length()>2)
									setPluginName(cAttr.getStringProp("text"));
								else if(cAttr.getStringProp("caption").length()>2)
									setPluginName(cAttr.getStringProp("caption"));
								else setPluginName("Untitled Cabbage Patch!");

							//StringArray log = logGUIAttributes(cAttr, T("Non-Interactive"));
							//debugMessageArray.addArray(logGUIAttributes(cAttr, T("Non-Interactive")));
							sendChangeMessage();

							if(tokes.getReference(0).equalsIgnoreCase(T("hostbpm"))
								||tokes.getReference(0).equalsIgnoreCase(T("hosttime"))
								||tokes.getReference(0).equalsIgnoreCase(T("hostplaying"))
								||tokes.getReference(0).equalsIgnoreCase(T("hostppqpos"))
								||tokes.getReference(0).equalsIgnoreCase(T("hostrecording")))
								startTimer(50);
						}
						//populate the guiCtrls vector with interactive widgets
						else if(tokes.getReference(0).equalsIgnoreCase(T("hslider"))
								||tokes.getReference(0).equalsIgnoreCase(T("vslider"))
								||tokes.getReference(0).equalsIgnoreCase(T("rslider"))
								||tokes.getReference(0).equalsIgnoreCase(T("combobox"))
								||tokes.getReference(0).equalsIgnoreCase(T("table"))
								||tokes.getReference(0).equalsIgnoreCase(T("checkbox"))
								||tokes.getReference(0).equalsIgnoreCase(T("xypad"))
								||tokes.getReference(0).equalsIgnoreCase(T("button"))){
							CabbageGUIClass cAttr(csdText[i].trimEnd(), guiID);
							//attach widget to plant if need be
							if(cAttr.getStringProp(T("relToPlant")).equalsIgnoreCase(T("")))
								cAttr.setStringProp(T("relToPlant"), plantFlag);
	//xypad contain two control paramters, one for x axis and another for y. As such we add two 
	//to our contorl vector so that plugin hosts display two sliders. We name one of the xypad pads
	// 'dummy' so that our editor doesn't display it. Our editor only needs to show one xypad. 
							if(tokes.getReference(0).equalsIgnoreCase(T("xypad"))){
								cAttr.setStringProp(T("xyChannel"), T("X"));
								cAttr.setNumProp("sliderRange",  cAttr.getNumProp("xypadRangeX"));
								cAttr.setNumProp("min",  cAttr.getNumProp("minX"));
								cAttr.setNumProp("max",  cAttr.getNumProp("maxX"));
								cAttr.setStringProp(T("channel"), cAttr.getStringProp("xChannel"));
								guiCtrls.add(cAttr);

								cAttr.setStringProp(T("xyChannel"), T("Y"));
								cAttr.setNumProp("sliderRange",  cAttr.getNumProp("xypadRangeY"));
								cAttr.setNumProp("min",  cAttr.getNumProp("minY"));
								cAttr.setNumProp("max",  cAttr.getNumProp("maxY"));
								cAttr.setStringProp(T("channel"), cAttr.getStringProp("yChannel"));
								//append 'dummy' to name so the editor know not to display the 
								//second xypad
								cAttr.setStringProp("name", cAttr.getStringProp("name")+T("dummy"));
								guiCtrls.add(cAttr);
								guiID++;
							}
							else{
							guiCtrls.add(cAttr);
							guiID++;
							}
							//debugMessageArray.addArray(logGUIAttributes(cAttr, T("Interactive")));
							sendChangeMessage();
						}

					}
		}
		else break;
        }

#ifdef Cabbage_Build_Standalone
		//init all channels with their init val
		for(int i=0;i<guiCtrls.size();i++)
		csound->SetChannel( guiCtrls.getReference(i).getStringProp("channel").toUTF8(), 
							guiCtrls.getReference(i).getNumProp("value"));
#endif
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
#ifdef Cabbage_Named_Pipe
  ud->sendChangeMessage();
// MOD - End
#endif
  ud->debugMessage = "";
  ud = nullptr;
}
catch(...){
		CabbageUtils::showMessage(T("If you insist on playing the keyboard\n \
				like a nutter please run Cabbage in standalone\n \
				mode, outside of WinXound, i.e., launch it on its\n \
				own and then load the csd file you wish to use."));
}
}
#endif

void CabbagePluginAudioProcessor::changeListenerCallback(ChangeBroadcaster *source)
{
#ifdef Cabbage_GUI_Editor
changeMessageType = T("GUI");
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
#ifndef Cabbage_No_Csound
if(index<(int)guiCtrls.size()){//make sure index isn't out of range
        MYFLT* val=0;
		//String test = guiCtrls.getReference(index).getStringProp("channel");
		//Logger::writeToLog(("channel:")+test);
		csoundGetChannelPtr(getCsoundStruct(), &val, guiCtrls.getReference(index).getStringProp("channel").toUTF8(),
                                CSOUND_CONTROL_CHANNEL | CSOUND_OUTPUT_CHANNEL);
		//Logger::writeToLog(guiCtrls.getReference(index).getStringProp("channel"));

//#ifdef Cabbage_Build_Standalone
	        return *val;
//#else

//		return getGUICtrls(index).getNumProp("value");
//#endif
}
else return 0.f; 
#endif
}

void CabbagePluginAudioProcessor::setParameter (int index, float newValue)
{
/* this will get called by the plugin GUI sliders or 
by the host via automation. If it's called by the host it will send 
message back to the GUI to notify it to update controls */
float range, min, max;
if(index<(int)guiCtrls.size())//make sure index isn't out of range
   {
  if(guiCtrls.getReference(index).getNumProp("value") != newValue)
     {
#ifndef Cabbage_Build_Standalone        
        //scaling in here because incoming values in plugin mode range from 0-1
		range = getGUICtrls(index).getNumProp("sliderRange");
		min = getGUICtrls(index).getNumProp("min");
		guiCtrls.getReference(index).setNumProp("value", (newValue*range)+min);
		//Logger::writeToLog(T("setParamters():")+String((newValue*range)+min));
		csound->SetChannel(guiCtrls.getReference(index).getStringProp("channel").toUTF8(),  (newValue*range)+min);
#else 
        guiCtrls.getReference(index).setNumProp("value", newValue);
        csound->SetChannel(guiCtrls.getReference(index).getStringProp("channel").toUTF8(), newValue);		
#endif
     }
   }
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
		if(getGUILayoutCtrls(i).getStringProp("type")==T("hostbpm")){	    
		if (getPlayHead() != 0 && getPlayHead()->getCurrentPosition (hostInfo))
			csound->SetChannel(getGUILayoutCtrls(i).getStringProp("channel").toUTF8(), hostInfo.bpm);
		}
		else if(getGUILayoutCtrls(i).getStringProp("type")==T("hosttime")){	    
		if (getPlayHead() != 0 && getPlayHead()->getCurrentPosition (hostInfo))
			csound->SetChannel(getGUILayoutCtrls(i).getStringProp("channel").toUTF8(), hostInfo.timeInSeconds);
		}
		else if(getGUILayoutCtrls(i).getStringProp("type")==T("hostplaying")){	    
		if (getPlayHead() != 0 && getPlayHead()->getCurrentPosition (hostInfo))
			csound->SetChannel(getGUILayoutCtrls(i).getStringProp("channel").toUTF8(), hostInfo.isPlaying);
		}
		else if(getGUILayoutCtrls(i).getStringProp("type")==T("hostrecording")){	    
		if (getPlayHead() != 0 && getPlayHead()->getCurrentPosition (hostInfo))
			csound->SetChannel(getGUILayoutCtrls(i).getStringProp("channel").toUTF8(), hostInfo.isRecording);
		}
		else if(getGUILayoutCtrls(i).getStringProp("type")==T("hostppqpos")){	    
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
//ccBuffer = midiMessages;

               for(int i=0;i<buffer.getNumSamples();i++, csndIndex++)
				{
				 for(int channel = 0; channel < getNumInputChannels(); channel++ )
					{
					audioBuffer = buffer.getSampleData(channel,0);
                       if(csndIndex == csound->GetKsmps())
                       {
                       CSCompResult = csound->PerformKsmps();
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
	CabbageUtils::showMessage(T("If you insist on playing the keyboard\n \
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
		   else if(message.isController()){
	  
		  }
            
	   }
	   midiData->midiBuffer.clear();
	}

	  return cnt;
}
catch(...){
	CabbageUtils::showMessage(T("If you insist on playing the keyboard\n \
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


