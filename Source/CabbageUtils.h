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

#ifndef __CabbUtilities_h__
#define __CabbUtilities_h__

#include <string>
#include <time.h>

#include "../JuceLibraryCode/JuceHeader.h" 
#include "BinaryData.h"

using namespace std;
#pragma warning(disable: 4389)
#pragma warning(disable: 4244)
#pragma warning(disable: 4706)
#pragma warning(disable: 4100)
#pragma warning(disable: 4305)

//class for our audio source, used by soundfilers
class CabbageAudioSource
{
TimeSliceThread thread;
public:
	CabbageAudioSource(String audioFile, int channels=2)
	:thread("audio source"), isSourcePlaying(false), index(0)
	{
	setFile(audioFile, channels);
	}
	
	~CabbageAudioSource()
	{
		thread.stopThread(10);
		delete audioSourceBuffer;
		audioSource= nullptr;
	}	
	
	bool setFile(String audioFile , int channels)
	{
		isSourcePlaying = false;
		AudioFormatManager formatManager;
		formatManager.registerBasicFormats(); 
		AudioFormatReader* reader;
		audioSource = nullptr;
		audioSourceBuffer = nullptr;
		if(File(audioFile).existsAsFile()) 
		reader = formatManager.createReaderFor (File(audioFile));  
			if(reader!=0){
			audioSource = new AudioFormatReaderSource (reader, true);
			audioSourceBuffer = new BufferingAudioSource(audioSource, thread, true, 32768, channels);		
			sampleRate = reader->sampleRate;
			audioSourceBuffer->setNextReadPosition(0);
			thread.startThread();
			return true;
			}
			else return false;
	}
	
	
	BufferingAudioSource* audioSourceBuffer;
	PositionableAudioSource* audioSource;
	int sampleRate, index;	
	bool isSourcePlaying;
	AudioSourceChannelInfo sourceChannelInfo;
	StringArray channels;
	

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CabbageAudioSource);	

};
//===========================================================================================
//some utility functions used across classes...
//===========================================================================================
class CabbagePatternMatrixStepData
{
public:
	int state, p4; 

	CabbagePatternMatrixStepData():
		state(0), p4(0){
	}
};

class CabbagePatternMatrixPfieldData
{
public:
	int p5, p6, p7, p8, p9, p10; 

	CabbagePatternMatrixPfieldData():
		p5(0), p6(0), p7(0), p8(0), p9(0), p10(0){
	}
};


class Gen1ScoreEvent 
{
public:
int ftableNumber;
String filename; 
	Gen1ScoreEvent(int _ftableNumber, String _filename): ftableNumber(_ftableNumber), filename(_filename){}
	
	String getGen1ScoreEvent(){
		String temp = "f "+String(ftableNumber)+" 0 0 1 \""+filename+"\" 0 4 1"; 
		return temp;
	}
};

//===========================================================================================
//some utility functions used across classes...
//===========================================================================================
class CabbageUtils
{
public:
	CabbageUtils(){};
	~CabbageUtils(){};


//===========================================================================================
string juce2Str(juce::String inStr)
{
	string str(inStr.toUTF8());
	return str;
}

//===========================================================================================
String str2Juce(std::string inStr)
{
	String str(inStr.c_str());
	return str;
}

float cabbageABS(float in){
	if(in<0)
		return in*-1;
	else return in;
}
//
//===========================================================================================
static void showMessage(String message){
	AlertWindow alert("Cabbage Message" , message, AlertWindow::WarningIcon);
	alert.showMessageBox(AlertWindow::WarningIcon, "Cabbage Message" , message, "Ok");
	Logger::writeToLog("mess1");
}

//===========================================================================================
static void showMessageWithLocation(String message){
File thisFile(File::getSpecialLocation(File::currentApplicationFile));
	AlertWindow alert(thisFile.getFullPathName() , message, AlertWindow::WarningIcon);
	alert.showMessageBox(AlertWindow::WarningIcon, thisFile.getFullPathName() , message, "Ok");
	Logger::writeToLog("mess2");
}
//===========================================================================================
static void showMessage(double num){
	String str(num);
	File thisFile(File::getSpecialLocation(File::currentApplicationFile));
	AlertWindow alert(thisFile.getFullPathName(), str, AlertWindow::WarningIcon);
	alert.showMessageBox(AlertWindow::WarningIcon, thisFile.getFullPathName(), str, "Ok");
	Logger::writeToLog("mess3");
}
//===========================================================================================
static void showMessage(String title, String message, LookAndFeel* feel, Component* mainWindow)
{
	if(title.length()<1)title="Cabbage Message";
	mainWindow->setAlwaysOnTop(false);
	mainWindow->toBack();
	AlertWindow alert("Cabbage Message" , message, AlertWindow::WarningIcon);
    alert.setLookAndFeel(feel);
	//alert.showMessageBox(AlertWindow::WarningIcon, "Cabbage Message" , message, "Ok");
	Logger::writeToLog("mess4");
	alert.setAlwaysOnTop(true);
    alert.addButton("Ok", 1);
    alert.runModalLoop();
	mainWindow->setAlwaysOnTop(true);
}

//===========================================================================================
static void showMessage(String message, LookAndFeel* feel)
{
	AlertWindow alert("Cabbage Message" , message, AlertWindow::WarningIcon);
    alert.setLookAndFeel(feel);
	//alert.showMessageBox(AlertWindow::WarningIcon, "Cabbage Message" , message, "Ok");
	Logger::writeToLog("mess4");
    alert.addButton("Ok", 1);
    alert.runModalLoop();
}

static void showMessage(String title, String message, LookAndFeel* feel)
{
	AlertWindow alert(title, message, AlertWindow::WarningIcon);
    alert.setLookAndFeel(feel);
	//alert.showMessageBox(AlertWindow::WarningIcon, "Cabbage Message" , message, "Ok");
	Logger::writeToLog("mess4");
    alert.addButton("Ok", 1);
    alert.runModalLoop();
}

//===========================================================================================
static int showYesNoMessage(String message, LookAndFeel* feel)
{
    AlertWindow alert("Cabbage Message", message, AlertWindow::NoIcon, 0);
    alert.setLookAndFeel(feel);
	alert.addButton("Yes", 0);
	alert.addButton("No", 1);
	int result = alert.runModalLoop();
	//int result = alert.showYesNoCancelBox(AlertWindow::QuestionIcon, "Warning", message, "Yes", "No", "Cancel");
	return result;
}
//===========================================================================================
StringArray CreateStringArray(std::string str)
{
	StringArray strArray;
	//std::string str = sourceEditorComponent->textEditor->getText().toUTF8();
	int cnt=0;
	for(int i=0;i<(int)str.length();i++)
	{
		int pos = (int)str.find("\n", 0);
		if(pos!=std::string::npos){
			strArray.add(str2Juce(str.substr(0, pos+1)));
			str.erase(0, pos+1);
			cnt++;
		}
	}
	//add the very last line which won't have a "\n"...
	strArray.add(str2Juce(str.substr(0, 100)));
	return strArray;
}

//==========================================================================================
static String cabbageString (String input, Font font, float availableWidth)
{
	//This method returns a truncated string if it is too big to fit into its available
	//space.  Dots are added at the end to signify that it has been truncated.  

	String newStr;
	float stringWidth = font.getStringWidthFloat (input);
	int numChars = input.length();
	float charWidth = stringWidth / numChars;

	if (stringWidth > availableWidth) {
		int numCharsToInclude = ((availableWidth / charWidth)+0.5) - 2;
		newStr = input.substring (0, numCharsToInclude);
		newStr << "..";
		return newStr;
	}
	else
		return input;
}

//========= Normal font for components ===============================================
static int getNumberOfDecimalPlaces(StringArray array)
{
int longest=0;
int index = 0;
	for(int i=0;i<array.size();i++){
		if(array[i].length()>longest){
		longest = array[i].length();
		index = i;
		}		
	}
	
if(array[index].indexOf(".")>0){
	String subTemp = array[index].substring(array[index].indexOf("."), 10);
	return subTemp.length()-1;
	}	
	else return 0;
	
	
}

//========= Normal font for components ===============================================
static Font getComponentFont()
{
	Font font = Font ("Verdana", 11.5, 1);
	return font;
}

//======= For slider values etc ======================================================
static Font getValueFont()
{
	Font font = Font ("Helvetica", 11.5, 1);
	return font;
}

//======= For spectrograms and tables etc ============================================
static Font getSmallerValueFont()
{
	Font font = Font ("Helvetica", 10, 0);
	return font;
}

//======= For titles etc ============================================================
static Font getTitleFont()
{
	Font font = Font ("Helvetica", 12.5, 1);
	return font;
}

//======= Title font colour =========================================================
static Colour getTitleFontColour()
{
	Colour cl = Colour::fromRGBA (160, 160, 160, 255);
	return cl;
}

//====== Font Colour ================================================================
static Colour getComponentFontColour()
{
	Colour cl = Colour::fromRGBA (160, 160, 160, 255);
	return cl;
}

//====== for group components etc =================================================
static Colour getComponentSkin()
{
	//Colour skin = Colour::fromRGBA (45, 55, 60, 255);
	Colour skin = Colour::fromRGBA (75, 85, 90, 100); //some transparency
	return skin;
}

//====== border colour ============================================================
static const Colour getBorderColour()
{
	return getComponentFontColour().withMultipliedAlpha(0.2);
}

//====== border width ============================================================
static const float getBorderWidth()
{
	return 1.0f;
}

//======== for the main background =========================================================
static Colour getBackgroundSkin()
{
	Colour skin = Colour::fromRGBA (5, 15, 20, 255);
	return skin;
}

//======= method for retrieve the string values of rectangles..
static	String getBoundsString(Rectangle<int> currentBounds)
{
		return "bounds(" + String(currentBounds.getX()) + String(", ") + String(currentBounds.getY()) + String(", ") + String(currentBounds.getWidth()) + String(", ")
			+ String(currentBounds.getHeight()) + String(")");
}

//======= method for replacing the contents of an identifier with new values..
static String replaceIdentifier(String line, String identifier, String updatedIdentifier)
{
	if(identifier.length()<2)
		return line;
	if(updatedIdentifier.length()<2)
		return line;

	int startPos = line.indexOf(identifier);
	String firstSection = line.substring(0, line.indexOf(identifier));
	line = line.substring(line.indexOf(identifier));
	String secondSection = line.substring(line.indexOf(")")+1);
	
	return firstSection+updatedIdentifier+secondSection;
}

//======= for darker backgrounds ==========================================================
static Colour getDarkerBackgroundSkin()
{
	Colour skin = getBackgroundSkin().darker(0.4);
	return skin;
}

//==========================================================================================
void cabbageSleep(unsigned int mseconds)
{
    clock_t goal = mseconds + clock();
    while (goal > clock());
}

//======== Check if NaN ====================================================================
static bool isNumber(double x) 
{
	// This looks like it should always be true, 
    // but it's false if x is a NaN.
    return (x == x); 
}

//==========================================================================================
long cabbageFindPluginID(unsigned char *buf, size_t len, const char *s)
{
	long i, j;
	int slen = strlen(s);
	long imax = len - slen - 1;
	long ret = -1;
	int match;

	for(i=0; i<imax; i++) 
		{
		match = 1;
		for (j=0; j<slen; j++) 
			if (buf[i+j] != s[j]){
				match = 0;
				break;
			}

	if (match){
		ret = i;
		break;
	}
	}
	//return position of plugin ID 
	return ret;
}

static void addFileToPpopupMenu(PopupMenu &m, Array<File> &filesArray, String dir, String ext) 
{
File pluginDir(dir);
pluginDir.findChildFiles(filesArray, 2, true, ext);

for (int i = 0; i < filesArray.size(); i++)
	m.addItem (i + 100, filesArray[i].getFileNameWithoutExtension());

}

//======================================================================================
static int getPreference(ApplicationProperties* appPrefs, String pref){
	return appPrefs->getUserSettings()->getValue(pref, var(0)).getFloatValue();
}

static String getPreference(ApplicationProperties* appPrefs, String pref, String value){
	return appPrefs->getUserSettings()->getValue(pref, "");
}

static void setPreference(ApplicationProperties* appPrefs, String pref, int value){	
	appPrefs->getUserSettings()->setValue(pref, var(value));
}

static void setPreference(ApplicationProperties* appPrefs, String pref, String value){	
	appPrefs->getUserSettings()->setValue(pref, var(value));
}

};

//===========================================================================================


#endif
