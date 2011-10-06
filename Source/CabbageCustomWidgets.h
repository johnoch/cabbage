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
// custom button component with optional surrounding groupbox
//==============================================================================
class CabbageButton : public Component
{
int offX, offY, offWidth, offHeight;
String buttonType;

public:
ScopedPointer<GroupComponent> groupbox;
ScopedPointer<Button> button;
//---- constructor -----
CabbageButton(String name, String caption, String buttonText, String colour)
{
	setName(name);
	offX=offY=offWidth=offHeight=0;
	groupbox = new GroupComponent(String("groupbox_")+name);
	button = new TextButton(name);
	addAndMakeVisible(button);
	addAndMakeVisible(groupbox);
	groupbox->setVisible(false);

	button->setButtonText(buttonText);
	if(caption.length()>0){
		offX=10;
		offY=15;
		offWidth=-20;
		offHeight=-25;
		groupbox->setVisible(true);
		groupbox->setText(caption);
	}

	if(colour.length()>0)
	button->setColour(TextButton::buttonColourId,
			Colours::findColourForName(colour, Colours::grey));
}
//---------------------------------------------
~CabbageButton(){

}

//---------------------------------------------
void resized()
{
groupbox->setBounds(0, 0, getWidth(), getHeight()); 
button->setBounds(offX, offY, getWidth()+offWidth, getHeight()+offHeight); 
}

JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CabbageButton);
};

//==============================================================================
// custom slider component with optional surrounding groupbox
//==============================================================================
class CabbageSlider : public Component
{
int offX, offY, offWidth, offHeight, plantX, plantY;
String sliderType;
public:
ScopedPointer<GroupComponent> groupbox;
ScopedPointer<Slider> slider;
//---- constructor -----
CabbageSlider(String name, String caption, String kind, String colour): plantX(-99), plantY(-99)
{
	setName(name);
	offX=offY=offWidth=offHeight=0;
	groupbox = new GroupComponent(String("groupbox_")+name);
	slider = new Slider(name);
	addAndMakeVisible(slider);
	addAndMakeVisible(groupbox);
	groupbox->setVisible(false);
	sliderType = kind; 
	
	groupbox->setColour(GroupComponent::outlineColourId,
		Colours::findColourForName(colour, Colours::whitesmoke));
	groupbox->setColour(GroupComponent::textColourId,
		Colours::findColourForName(colour, Colours::whitesmoke));


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
	//	slider->setColour(Slider::rotarySliderFillColourId,
	//					  Colours::findColourForName(colour, Colours::black));
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
//	slider->setColour(Slider::thumbColourId,
//					  Colours::findColourForName(colour, Colours::coral));
	}
	}
	
//	slider->setColour(Slider::rotarySliderOutlineColourId,
//					  Colours::findColourForName(colour, Colours::black));
	slider->setColour(Slider::ColourIds::textBoxBackgroundColourId, Colours::whitesmoke);
	slider->setColour(Slider::ColourIds::textBoxTextColourId, Colours::black);
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

//==============================================================================
// custom checkbox component with optional surrounding groupbox
//==============================================================================
class CabbageCheckbox : public Component
{
int offX, offY, offWidth, offHeight;

public:
ScopedPointer<GroupComponent> groupbox;
ScopedPointer<ToggleButton> button;
//---- constructor -----
CabbageCheckbox(String name, String caption, String buttonText, String colour)
{
	setName(name);
	offX=offY=offWidth=offHeight=0;
	groupbox = new GroupComponent(String("groupbox_")+name);
	button = new ToggleButton(name);
	addAndMakeVisible(button);
	addAndMakeVisible(groupbox);
	groupbox->setVisible(false);

	groupbox->setColour(GroupComponent::outlineColourId,
		Colours::findColourForName(colour, Colours::black));
	groupbox->setColour(GroupComponent::textColourId,
		Colours::findColourForName(colour, Colours::black));

	button->setButtonText(buttonText);
	if(caption.length()>0){
		offX=10;
		offY=15;
		offWidth=-20;
		offHeight=-25;
		groupbox->setVisible(true);
		groupbox->setText(caption);
	}

	if(colour.length()>0)
	button->setColour(TextButton::buttonColourId,
			Colours::findColourForName(colour, Colours::grey));
	button->setColour(ToggleButton::textColourId,
			Colours::findColourForName(colour, Colours::grey));
	button->setButtonText(buttonText);
}
//---------------------------------------------
~CabbageCheckbox(){

}

//---------------------------------------------
void resized()
{
groupbox->setBounds(0, 0, getWidth(), getHeight()); 
button->setBounds(offX, offY, getWidth()+offWidth, getHeight()+offHeight); 
}

JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CabbageCheckbox);
};


//==============================================================================
// custom checkbox component with optional surrounding groupbox
//==============================================================================
class CabbageComboBox : public Component
{
//ScopedPointer<LookAndFeel> lookFeel;
int offX, offY, offWidth, offHeight;
public:
ScopedPointer<GroupComponent> groupbox;
ScopedPointer<ComboBox> combo;
//---- constructor -----
CabbageComboBox(String name, String caption, String text, String colour)
{
	setName(name);
	offX=offY=offWidth=offHeight=0;
	
	groupbox = new GroupComponent(String("groupbox_")+name);
	combo = new ComboBox(name);
	addAndMakeVisible(combo);
	addAndMakeVisible(groupbox);

	groupbox->setVisible(false);

	groupbox->setColour(GroupComponent::outlineColourId,
		Colours::findColourForName(colour, Colours::black));
	groupbox->setColour(GroupComponent::textColourId,
		Colours::findColourForName(colour, Colours::black));
	
	if(caption.length()>0){
		offX=10;
		offY=15;
		offWidth=-20;
		offHeight=-25;
		groupbox->setVisible(true);
		groupbox->setText(caption);
	}

	combo->setColour(ComboBox::ColourIds::textColourId,
			Colours::findColourForName("white", Colours::grey));


	combo->setEditableText (false);
    combo->setJustificationType (Justification::centredLeft);
	combo->setTextWhenNothingSelected(text);

}
//---------------------------------------------
~CabbageComboBox(){

}

//---------------------------------------------
void resized()
{
groupbox->setBounds(0, 0, getWidth(), getHeight()); 
combo->setBounds(offX, offY, getWidth()+offWidth, getHeight()+offHeight); 
}

//JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CabbageComboBox);
};

//==============================================================================
// custom image component
//==============================================================================
class CabbageImage : public Component,
						public CabbageUtils
{
public:
	CabbageImage(String name, String file, int top, int left, int width, int height, String outline, String fill, String shape, int line):
	Component(name), picFile(file), file(file), top(top), left(left), width(width), height(height), fill(fill), outline(outline), shape(shape), line(line){
		setName(name);
		toBack();
		img = ImageCache::getFromFile (File (file));
	}
	~CabbageImage(){
	}

private:
	Image img;
	int top, left, width, height, line;
	String fill, outline, shape, file;
	File picFile;	

	void paint (Graphics& g){
		if(file.length()>5){
		g.drawImage(img, 0, 0, width, height, 0, 0, img.getWidth(), img.getHeight());
		}
		else{
			if(shape.contains("rounded")){
				g.setColour(Colours::findColourForName(outline, Colours::black));
				g.fillRoundedRectangle(0,0, width, height, width*.02);
				g.setColour(Colours::findColourForName(fill, Colours::black));
				g.fillRoundedRectangle(line,line, width-(line*2), height-(line*2), width*.02);				
			}
			else if(shape.contains("ellipse")){
				g.setColour(Colours::findColourForName(outline, Colours::black));
				g.fillEllipse(0,0, width, height);
				g.setColour(Colours::findColourForName(fill, Colours::black));
				g.fillEllipse(line,line, width-(line*2), height-(line*2));				
			}
			else if(shape.contains("sharp")){
				
				g.setColour(Colours::findColourForName(outline, Colours::black));
				g.fillRect(0,0, width, height);
				g.setColour(Colours::findColourForName(fill, Colours::black));
				g.fillRect(line,line, width-(line*2), height-(line*2));				
			}
		}

	}
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CabbageImage);
};

//==============================================================================
// custom groupbox component, this can act as a plant for other components 
//==============================================================================
class CabbageGroupbox : public GroupComponent
{
OwnedArray<Component> comps;
int offX, offY, offWidth, offHeight;
public:
//---- constructor -----
CabbageGroupbox(String name, String caption, String text, String colour):GroupComponent(name)
{
	toBack();
	offX=offY=offWidth=offHeight=0;
	if(colour.length()>0){
	setColour(GroupComponent::outlineColourId,
		Colours::findColourForName(colour, Colours::black));
	setColour(GroupComponent::textColourId,
		Colours::findColourForName(colour, Colours::black));
	}
	this->setText(text);

}
//---------------------------------------------
~CabbageGroupbox(){

}

void addComponent(Component* comp)
{
	comps.add(comp);
	int size = comps.size();
	addAndMakeVisible(comps[size-1]);
}

//---------------------------------------------
void resized()
{

}

JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CabbageGroupbox);
};

#endif