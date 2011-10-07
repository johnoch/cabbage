/*
  Copyright (C) 2011 

  Author Damien Rennick

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

#include "../JuceLibraryCode/JuceHeader.h"
#include "../JuceLibraryCode/JucePluginCharacteristics.h"
#include "imageBinaries.h"


class CabbageLookAndFeel	:	public LookAndFeel
{
public:
	CabbageLookAndFeel();
	~CabbageLookAndFeel();
	virtual void drawRotarySlider (Graphics&, int, int, int, int, float, float, float, Slider&);
	virtual void drawLinearSlider (Graphics&, int, int, int, int, float, float, float, const Slider::SliderStyle, Slider&);
	virtual void drawButtonBackground (Graphics&, Button&, const Colour&, bool, bool);
	virtual const Font getFontForTextButton (TextButton&);
	virtual void drawComboBox (Graphics&, int, int, bool, int, int, int, int, ComboBox&);
	virtual void drawTickBox (Graphics &, Component &, float, float, float, float, bool, bool, bool, bool);	
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CabbageLookAndFeel);


private:
	int textBoxGap;	
};

