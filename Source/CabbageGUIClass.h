/*
  Copyright (C) 2007 Rory Walsh

  Cabbage is free software; you can redistribute it
  and/or modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  Cabbage is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Lesser General Public License for more details.

//   You should have received a copy of the GNU Lesser General Public
  License along with Csound; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
  02111-1307 USA
    
*/
#ifndef CABBPARSE_H
#define CABBPARSE_H

#include <stdio.h>
#include <string>
#include <iostream>
#include <vector>
#include <string>
#include <cctype>
#include <algorithm>
#include <fstream>
#include <sstream>

#include "CabbageUtils.h"
using namespace std;

class CabbageGUIClass : public CabbageUtils
{
        double width, height, top, left, isRect, min, max, tabpage, minX, minY, maxX, maxY, comboRange, fftSize, overlapSize, frameSize,
		noOfMenus, onoff, midiChan, midiCtrl, sliderRange, xypadRangeY, xypadRangeX, noSteps, noPatterns, pvsChannel, alpha,
		line, anchor, linkTo, scaleX, scaleY, value, valueX, valueY, maxItems, sliderIncr, sliderSkew, decimalPlaces, rCtrls, lineIsVertical;
        StringArray items, onoffcaptions, key, channels, snapshotData, colours;

        String channel, name, sizeText, posText, boundsText, text, type, plant, reltoplant, bounds, range, fileType, workingDir,
        shape, beveltype, caption, kind, topitem, yChannel, xChannel, author,
        exit, csstdout, cssetup, file, debugMessage, xyChannel, pluginID, preset;
		Colour outline, fill, fontcolour, textcolour, colour, trackerFill;
		int tableNum, textBox, numPresets, masterSnap, plantButton, xyAutoIndex, paramIndex;
		Array<int> vuConfig;
		Array<int> tableNumbers;

		//JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CabbageGUIClass);
public:
	//constructor
	CabbageGUIClass(String str, int ID);
    ~CabbageGUIClass();
	int parse(String str);
	double getNumProp(String prop);
	double getNumProp(String prop, int index);
	void setNumProp(String prop, float val);
	void setStringProp(String prop, String val);
	void setStringProp(String prop, int index, String value);
	String getStringProp(String prop);
	String getStringProp(String prop, int index);
	String getPropsString();
	String getColourProp(String prop);
	Rectangle<int> getComponentBounds();

	inline int getNumPresets(){
		return snapshotData.size();
	}

	inline void clearPresets(){
		snapshotData.clear();
	}

	inline String setOnOffcaptions(int index, String str){
		if(index<=onoffcaptions.size())
		onoffcaptions.getReference(index)=str;
	}

	inline String getOnOffcaptions(int index){
		if(index<=onoffcaptions.size())
		return onoffcaptions.getReference(index);
		else return "";
	}

	inline String setItems(int index, String str){
		if(index<=items.size())
		items.getReference(index)=str;
		else
			return "";
	}

	inline String getItems(int index){
		if(items.size())
		return items.getReference(index);
		else return "";
	}

	inline String getColours(int index){
		if(colours.size())
		return colours.getReference(index);
		else return "";
	}
	
	inline int getNumberOfColours(){
		return colours.size();

	}	

	inline StringArray getItemArray(){
		if(items.size())
		return items;
		else return StringArray();
	}

	inline int getItemsSize(){
		return items.size();
	}

	inline Array<int> getVUConfig(){
		return vuConfig;
	}

	inline String getChannel(int index){
		return channels[index];
	}

	inline int getNumberOfSoftwareChannels(){
		return channels.size();
	}

	inline int getKeySize(){
		return key.size();
	}

	inline String getkey(int index){
		return key.getReference(index);
	}

	int getTableNumbers(int index)
	{
		if(index<tableNumbers.size())
		return tableNumbers[index];		
	}

	inline Array<int> getTableNumbersArray()
	{
		return tableNumbers;		
	}
	
};

#endif