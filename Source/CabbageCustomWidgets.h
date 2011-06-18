/*
  Copyright (C) 2010 Rory Walsh

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

#ifndef _CabbageWidgets_H__
#define _CabbageWidgets_H__

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
// all custom cabbage widgets can be decalred in this file
//==============================================================================



//==============================================================================
// custom slider component with optional surrounding groupbox
//==============================================================================
class CabbageSlider : public Component
{
public:
ScopedPointer<GroupComponent> groupbox;
ScopedPointer<Slider> slider;
int offX, offY, offWidth, offHeight;
String sliderType;
//---- constructor -----
CabbageSlider(String name, String caption, int min, int max, int value, String kind, String colour)
{
	offX=offY=offWidth=offHeight=0;
	groupbox = new GroupComponent(String("groupbox_")+name);
	slider = new Slider(name);
	addAndMakeVisible(slider);
	addAndMakeVisible(groupbox);
	groupbox->setVisible(false);
	sliderType = kind; 
	
	if(kind.contains(T("vertical"))){
	slider->setSliderStyle(Slider::LinearVertical);
	slider->setTextBoxStyle (Slider::TextBoxBelow, true, 60, 20);
	if(caption.length()>0){
		offX=10;
		offY=15;
		offWidth=-20;
		offHeight=-25;
		groupbox->setVisible(true);
		groupbox->setText(caption);
	}
	}

	else if(kind.contains(T("rotary"))){
		slider->setSliderStyle(Slider::Rotary);
		getProperties().set("type", var("rslider"));
		slider->setSliderStyle(Slider::RotaryVerticalDrag);
		slider->setRotaryParameters(float_Pi * 1.2f, float_Pi * 2.8f, false);
		slider->setTextBoxStyle (Slider::TextBoxBelow, true, 60, 20);

		if(colour.length()>0){
		slider->setColour(Slider::rotarySliderFillColourId,
						  Colours::findColourForName(colour, Colours::coral));
		}
		if(caption.length()>0){
			offX=0;
			offY=20;
			offWidth=0;
			offHeight=-30;
			groupbox->setText(caption);
			groupbox->setVisible(true);
		}
	}

	else{
	if(caption.length()>0){
		offX=10;
		offY=15;
		offWidth=-20;
		offHeight=-22;
		groupbox->setText(caption);
		groupbox->setVisible(true);
	}


	if(colour.length()>0){
	slider->setColour(Slider::thumbColourId,
					  Colours::findColourForName(colour, Colours::coral));
	}
	}


slider->setRange(min, max, 0.01);
slider->setValue(value);
}//--- end of constructor ----

//---------------------------------------------
~CabbageSlider(){

}

//---------------------------------------------
void resized()
{
groupbox->setBounds(0, 0, getWidth(), getHeight()); 
slider->setBounds(offX, offY, getWidth()+offWidth, getHeight()+offHeight); 
}

JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CabbageSlider);
};



#endif