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

using namespace std;
#pragma warning(disable: 4389)
#pragma warning(disable: 4244)
#pragma warning(disable: 4706)
#pragma warning(disable: 4100)
#pragma warning(disable: 4305)

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

//===========================================================================================
static void showMessage(String message){
	AlertWindow alert("Cabbage Message" , message, AlertWindow::NoIcon);
	alert.showMessageBox(AlertWindow::NoIcon, "Cabbage Message" , message, "Ok");
}

//===========================================================================================
static void showMessageWithLocation(String message){
File thisFile(File::getSpecialLocation(File::currentApplicationFile));
	AlertWindow alert(thisFile.getFullPathName() , message, AlertWindow::NoIcon);
	alert.showMessageBox(AlertWindow::NoIcon, thisFile.getFullPathName() , message, "Ok");
}
//===========================================================================================
static void showMessage(float num){
	String str(num);
	File thisFile(File::getSpecialLocation(File::currentApplicationFile));

	AlertWindow alert(thisFile.getFullPathName(), str, AlertWindow::NoIcon);
	alert.showMessageBox(AlertWindow::NoIcon, thisFile.getFullPathName(), str, "Ok");
}

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
		float includeChars = (availableWidth / charWidth) - 3;
		newStr = input.substring (0, includeChars);
		newStr << "...";
		return newStr;
	}
	else
		return input;
}

//==========================================================================================
static Font widgetText()
{
	Font font = Font (T("Verdana"), 12, 1);
	return font;
}

//==========================================================================================
static Colour componentSkin()
{
	Colour skin = Colour::fromRGBA (45, 45, 50, 255);
	return skin;
}

//==========================================================================================

void cabbageSleep(unsigned int mseconds)
{
    clock_t goal = mseconds + clock();
    while (goal > clock());
}

};

//===========================================================================================


#endif
