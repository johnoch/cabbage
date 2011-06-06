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
        float width, height, top, left, min, max, tabpage, 
		noOfMenus, onoff, midiChan, midiCtrl, 
		line, anchor, linkTo, scaleX, scaleY;
        float value;
        StringArray items;
        StringArray onoffcaptions;
		StringArray key;
        String channel, name, text, type, colour, plant, reltoplant, textcolour, bounds, range,
        fontcolour, outline, fill, shape, beveltype, caption, kind, topitem,
        exit, csstdout, cssetup, file, debugMessage;

public:
	//constructor
	CabbageGUIClass(String str, int ID);
    ~CabbageGUIClass();
	int parse(String str);


	//retrieve numerical attributes
	inline float getNumProp(String prop){
		if(prop.equalsIgnoreCase(T("width")))
			return width;
		else if(prop.equalsIgnoreCase(T("height")))
			return height;
		else if(prop.equalsIgnoreCase(T("top")))
			return top;
		else if(prop.equalsIgnoreCase(T("left")))
			return left;
		else if(prop.equalsIgnoreCase(T("min")))
			return min;
		else if(prop.equalsIgnoreCase(T("midichan")))
			return midiChan;
		else if(prop.equalsIgnoreCase(T("midictrl")))
			return midiCtrl;
		else if(prop.equalsIgnoreCase(T("max")))
			return max;
		else if(prop.equalsIgnoreCase(T("tabpage")))
			return tabpage;
		else if(prop.equalsIgnoreCase(T("noOfMenus")))
			return noOfMenus;
		else if(prop.equalsIgnoreCase(T("onoff")))
			return onoff;
		else if(prop.equalsIgnoreCase(T("value")))
			return value;
		else if(prop.equalsIgnoreCase(T("line")))
			return line;
		else if(prop.equalsIgnoreCase(T("scaleX")))
			return scaleX;
		else if(prop.equalsIgnoreCase(T("scaleY")))
			return scaleY;
		else if(prop.equalsIgnoreCase(T("linkTo")))
			return linkTo;
		else return -9999;
	}

	//set numerical attributes
	inline void setNumProp(String prop, float val){
		if(prop.equalsIgnoreCase(T("width")))
			 width = val;
		else if(prop.equalsIgnoreCase(T("height")))
			 height = val;
		else if(prop.equalsIgnoreCase(T("top")))
			 top = val;
		else if(prop.equalsIgnoreCase(T("left")))
			 left = val;
		else if(prop.equalsIgnoreCase(T("min")))
			 min = val;
		else if(prop.equalsIgnoreCase(T("midichan")))
			 midiChan = val;
		else if(prop.equalsIgnoreCase(T("midictrl")))
			 midiCtrl = val;
		else if(prop.equalsIgnoreCase(T("max")))
			 max = val;
		else if(prop.equalsIgnoreCase(T("tabpage")))
			 tabpage = val;
		else if(prop.equalsIgnoreCase(T("noOfMenus")))
			 noOfMenus = val;
		else if(prop.equalsIgnoreCase(T("onoff")))
			 onoff = val;
		else if(prop.equalsIgnoreCase(T("value")))
			 value = val;
		else if(prop.equalsIgnoreCase(T("line")))
			 line = val;
		else if(prop.equalsIgnoreCase(T("scaleX")))
			 scaleX = val;
		else if(prop.equalsIgnoreCase(T("scaleY")))
			 scaleY = val;
		else if(prop.equalsIgnoreCase(T("linkTo")))
			 linkTo = val;
	}

	inline String getStringProp(String prop){
		if(prop.equalsIgnoreCase(T("channel")))
			return channel.trim();
		else if(prop.equalsIgnoreCase(T("name")))
			return name.trim();
		else if(prop.equalsIgnoreCase(T("text")))
			return text.trim();
		else if(prop.equalsIgnoreCase(T("type")))
			return type.trim();
		else if(prop.equalsIgnoreCase(T("colour")))
			return colour.trim();
		else if(prop.equalsIgnoreCase(T("fontcolour")))
			return fontcolour.trim();
		else if(prop.equalsIgnoreCase(T("outline")))
			return outline.trim();
		else if(prop.equalsIgnoreCase(T("fill")))
			return fill.trim();
		else if(prop.equalsIgnoreCase(T("shape")))
			return shape.trim();
		else if(prop.equalsIgnoreCase(T("beveltype")))
			return beveltype.trim();
		else if(prop.equalsIgnoreCase(T("caption")))
			return caption.trim();
		else if(prop.equalsIgnoreCase(T("kind")))
			return kind.trim();
		else if(prop.equalsIgnoreCase(T("topitem")))
			return topitem.trim();
		else if(prop.equalsIgnoreCase(T("file")))
			return file.trim();
		else if(prop.equalsIgnoreCase(T("cssetup")))
			return cssetup.trim();
		else if(prop.equalsIgnoreCase(T("csstdout")))
			return csstdout.trim();
		else if(prop.equalsIgnoreCase(T("exit")))
			return exit.trim();
		else if(prop.equalsIgnoreCase(T("plant")))
			return plant.trim();
		else if(prop.equalsIgnoreCase(T("reltoplant")))
			return reltoplant.trim();
		else if(prop.equalsIgnoreCase(T("textcolour")))
			return textcolour.trim();
		else if(prop.equalsIgnoreCase(T("debugmessage")))
			return debugMessage;
		else return "";
	}

	inline void setStringProp(String prop, String val){
		if(prop.equalsIgnoreCase(T("channel")))
			channel = val;
		else if(prop.equalsIgnoreCase(T("name")))
			name = val;
		else if(prop.equalsIgnoreCase(T("text")))
			 text = val;
		else if(prop.equalsIgnoreCase(T("type")))
			 type = val;
		else if(prop.equalsIgnoreCase(T("colour")))
			 colour = val;
		else if(prop.equalsIgnoreCase(T("fontcolour")))
			 fontcolour = val;
		else if(prop.equalsIgnoreCase(T("outline")))
			 outline = val;
		else if(prop.equalsIgnoreCase(T("fill")))
			 fill = val;
		else if(prop.equalsIgnoreCase(T("shape")))
			 shape = val;
		else if(prop.equalsIgnoreCase(T("beveltype")))
			 beveltype = val;
		else if(prop.equalsIgnoreCase(T("caption")))
			 caption = val;
		else if(prop.equalsIgnoreCase(T("kind")))
			 kind = val;
		else if(prop.equalsIgnoreCase(T("topitem")))
			 topitem = val;
		else if(prop.equalsIgnoreCase(T("file")))
			 file = val;
		else if(prop.equalsIgnoreCase(T("cssetup")))
			 cssetup = val;
		else if(prop.equalsIgnoreCase(T("csstdout")))
			 csstdout = val;
		else if(prop.equalsIgnoreCase(T("exit")))
			 exit = val;
		else if(prop.equalsIgnoreCase(T("plant")))
			 plant = val;
		else if(prop.equalsIgnoreCase(T("reltoplant")))
			 reltoplant = val;
		else if(prop.equalsIgnoreCase(T("textcolour")))
			textcolour = val;
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

	inline int getItemsSize(){
		return items.size();
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