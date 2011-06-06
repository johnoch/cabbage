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

#include "CabbageMainPanel.h"
#include "ComponentLayoutEditor.h"

// This is the main panel/component onto which all other components are drawn.
// When users drop components onto this panel, ComponentLayoutEditor will 
// create aliases of each so that they can be moved around and resized.
// When a user disables the ComponentLayoutEditor this panel is again brought
// to front so that components can no longer be edited. The idea is that
// by toggling these two classes toFront() we can swap between edit and lock mode
 

CabbageMainPanel::CabbageMainPanel()
{
setName("CabbageMainPanel");
setVisible(true);
setSize(800, 700);
LOCKED=true;
}

CabbageMainPanel::~CabbageMainPanel()
{

}

void CabbageMainPanel::paint(Graphics &g)
{
//	g.fillAll(Colours::findColourForName(Colours::white, Colours::white));
}

ComponentLayoutEditor* CabbageMainPanel::getLayoutEditor()
{
return layoutEditor;
}

void CabbageMainPanel::mouseDown(const MouseEvent &e)
{

};
