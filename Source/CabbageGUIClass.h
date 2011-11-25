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

#include "../JuceLibraryCode/juce_PluginHeaders.h"
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
        float width, height, top, left, min, max, tabpage, minX, minY, maxX, maxY, comboRange,
		noOfMenus, onoff, midiChan, midiCtrl, sliderRange, xypadRangeY, xypadRangeX,
		line, anchor, linkTo, scaleX, scaleY, value, valueX, valueY, maxItems;
        StringArray items;
        StringArray onoffcaptions;
		StringArray key;
		StringArray channels;
        String channel, name, sizeText, posText, boundsText, text, type, colour, plant, reltoplant, textcolour, bounds, range,
        fontcolour, outline, fill, shape, beveltype, caption, kind, topitem, yChannel, xChannel,
        exit, csstdout, cssetup, file, debugMessage, xyChannel;
		int tableNum;

public:
	//constructor
	CabbageGUIClass(String str, int ID);
    ~CabbageGUIClass();
	int parse(String str);
	float getNumProp(String prop);
	void setNumProp(String prop, float val);
	void setStringProp(String prop, String val);
	String getStringProp(String prop);
	String getStringProp(String prop, int index);
	String getPropsString();


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

	inline int getItemsSize(){
		return items.size();
	}

	inline String getChannels(int index){
		return channels[index];
	}

	inline int getKeySize(){
		return key.size();
	}

	inline String getkey(int index){
		return key.getReference(index);
	}
	//JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CabbageGUIClass);
};

#endif