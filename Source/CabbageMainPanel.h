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

   You should have received a copy of the GNU Lesser General Public
  License along with Csound; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
  02111-1307 USA

*/
#ifndef CABBMAIN_H
#define CABBMAIN_H

#include "../JuceLibraryCode/JuceHeader.h"
#include "CabbageUtils.h"

class ComponentLayoutEditor;

//for grouping objects when a user drags over them
//-----------------------------------------------------------------------------
class SelectedComponents   : public SelectedItemSet<Component*>
{
public:
	void itemSelected (Component* item){
		item->repaint ();
	}
	void itemDeselected (Component* item){
		item->repaint ();
	}
};

//main panel onto which all GUI objects are placed in standalone host
//-----------------------------------------------------------------------------

class CabbageMainPanel   : public Component,
						   public CabbageUtils,
						   public ChangeBroadcaster
{
private:	
	
	ScopedPointer<ComponentLayoutEditor> layoutEditor;
	OwnedArray<Component > comps;
	OwnedArray<Component > clipBoard;
	int currentIndex;
	bool LOCKED;
	String mouseState;
	String colour;


public:
	int width, height, top, left;
	inline void setCompColour(String col){
		colour = col;
	}

	inline String getColour(){
		return colour;
	}

	inline String getMouseState(){
		return mouseState;
	}

	inline void setMouseState(String state){
		mouseState = state;
	}

	String getCurrentBounds(){
	return T("bounds(") + String(left) + T(", ") + String(top) + T(", ") + String(width) + T(", ")
			+ String(height) + T(")");
	}

	String getCurrentPos(){
	return T("pos(") + String(left) + T(", ") + String(top) + T(")");
	}

	String getCurrentSize(){
	return T("size(") + String(width) + T(", ") + String(height) + T(")");
	}

	CabbageMainPanel();
	~CabbageMainPanel();
	void paint(Graphics &g);
	ComponentLayoutEditor* getLayoutEditor();
	void setLayoutEditor(ComponentLayoutEditor* ed);

	inline int getIndex(){
	return currentIndex;
	}

	int getCurrentWidth(){
		return width;
	}

	int getCurrentHeight(){
		return height;
	}

	int getCurrentTop(){
		return top;
	}

	int getCurrentLeft(){
		return left;
	}

	inline void setIndex(int index){
	currentIndex = index;
	}

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CabbageMainPanel);

};

#endif