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

#include "CabbageLookAndFeel.h"


CabbageLookAndFeel::CabbageLookAndFeel()
{
	textBoxGap = 5;		//space to leave in between the slider image and the textbox
}

CabbageLookAndFeel::~CabbageLookAndFeel()
{
}

//-------------------- function for drawing custom rotary sliders ----------------------------------------------//
void CabbageLookAndFeel::drawRotarySlider(Graphics& g, int /*x*/, int /*y*/, int /*width*/, int /*height*/,
																	float /*sliderPosProportional*/,
																	float /*startAngle*/,
																	float /*endAngle*/,
																	Slider& slider)
{
	//frames
	const int frameHeight = 70;	
	const int frameWidth = 70;
	const int numFrames = 100;

	//variables for determining new destination width and height etc...
	int destX, destY, destHeight, destWidth, textBoxHeight;
	int yOffset = 0;
	
	const double div = slider.getMaximum() / numFrames; //to get divisor number
	double pos = (int)(slider.getValue() / div);		

	if (pos > 0) pos -= 1;

	//declaration of custom slider images
	Image newSlider = ImageCache::getFromMemory (imageBinaries::rotary1_png, imageBinaries::rotary1_pngSize);  

	if (!slider.isMouseOverOrDragging()) yOffset = (int)pos * frameHeight;	//normal
	else yOffset = (int)pos * frameHeight;	//if mouse is dragging slider


	textBoxHeight = slider.getTextBoxHeight();					//gets height of textbox
	destHeight = (slider.getHeight() - (textBoxHeight + textBoxGap));	//destination height of just the image part!
	destWidth = destHeight;										//because rotary slider images generally have the same width and height....

	destX = ((slider.getWidth() - destWidth) / 2);				//starting x position, this ensures that the image will be centered
	destY = 0;													//starting y position

	g.drawImage(newSlider, destX, destY, destWidth, destHeight, 0, yOffset, frameWidth, frameHeight, false);
}


//-------------------- function for drawing custom linear sliders ----------------------------------------------//
void CabbageLookAndFeel::drawLinearSlider (Graphics& g, int /*x*/, int /*y*/, int /*width*/, int /*height*/, float sliderPos, 
																							float /*minSliderPos*/,
																							float /*maxSliderPos*/,
																							const Slider::SliderStyle style,
																							Slider& slider)
{
	//frames
	int frameWidth, frameHeight;
	const int numFrames = 100;
	Image newSlider;

	//variables for determining new destination width, height, position etc...
	int destX, destY, destHeight, destWidth, textBoxHeight, xOffset, yOffset;
	
	const double div = slider.getMaximum() / numFrames;
	
	sliderPos = (int)(slider.getValue() / div);

	if (sliderPos > 0) sliderPos -= 1;

	textBoxHeight = slider.getTextBoxHeight();					//gets height of textbox

	
	//---------- for vertical sliders ------------------------------------------
	if (style == Slider::LinearVertical) {
		frameWidth = 35;
		frameHeight = 180;
		yOffset = 0;
 
		newSlider = ImageCache::getFromMemory (imageBinaries::vertical1_png, imageBinaries::vertical1_pngSize);
    
		if (!slider.isMouseOverOrDragging()) xOffset = (int)sliderPos * frameWidth;	//normal
		else xOffset = (int)sliderPos * frameWidth;	//if mouse is dragging slider
		slider.setTextBoxStyle (Slider::TextBoxBelow, true, 35, 15);

		destHeight = (slider.getHeight() - (textBoxHeight + textBoxGap));	//destination height of just the image part!
		destWidth = slider.getWidth();

		destX = ((slider.getWidth() - destWidth) / 2);				//starting x position, this ensures that the image will be centered with the textbox
		destY = 0;													//starting y position
	}
	//---------- for horizontal sliders ------------------------------------
	if (style == Slider::LinearHorizontal) {
		frameWidth = 180;
		frameHeight = 35;
		xOffset = 0;

		newSlider = ImageCache::getFromMemory (imageBinaries::horizontal1_png, imageBinaries::horizontal1_pngSize);

		if (!slider.isMouseOverOrDragging()) yOffset = (int)sliderPos * frameHeight;	//normal
		else yOffset = (int)sliderPos * frameHeight;	//if mouse is dragging slider
		slider.setTextBoxStyle (Slider::TextBoxLeft, true, 35, 15);

		destHeight = (slider.getHeight());								//destination height 
		destWidth = (slider.getWidth() - (slider.getTextBoxWidth() + textBoxGap));

		destX = (slider.getTextBoxWidth() + textBoxGap);				//starting x position, this ensures that the image will be centered with the textbox
		destY = (slider.getHeight() - destHeight) / 2;					//starting y position
	}

	
	g.drawImage(newSlider, destX, destY, destWidth, destHeight, xOffset, yOffset, frameWidth, frameHeight, false);
}

//------------ Function to draw new tick box for toggle buttons ----------------------------------------------
void CabbageLookAndFeel::drawTickBox (Graphics &g, Component &/*component*/, float /*x*/, float /*y*/, float /*w*/, float /*h*/, bool ticked, 
																						bool /*isEnabled*/, 
																						bool /*isMouseOverButton*/, 
																						bool /*isButtonDown*/)
{
	int yOffset= 0;
	int frameWidth = 60;
	int frameHeight = 20; 

	Image newButton = ImageCache::getFromMemory (imageBinaries::togglebutton1_png, imageBinaries::togglebutton1_pngSize);
	
	//if button is switched on
	if (ticked == true)
		yOffset = 20;	//for changing frame

	g.drawImage (newButton, 0, 0, frameWidth, frameHeight, 0, yOffset, frameWidth, frameHeight, false);
}

//------------------------ Function for drawing text buttons -----------------------------------------//
void CabbageLookAndFeel::drawButtonBackground (Graphics& g, Button& button, const Colour& /*backgroundColour*/, 
																			bool /*isButtonOver*/, 
																			bool isButtonDown)
{
	int frameWidth = 70;
	int frameHeight = 30;
	int yOffset = 0;

	Image newButton = ImageCache::getFromMemory (imageBinaries::button1_png, imageBinaries::button1_pngSize);

	if (isButtonDown == true) yOffset = 30;

	g.drawImage (newButton, 0, 0, button.getWidth(), button.getHeight(), 0, yOffset, frameWidth, frameHeight, false);
}

//--------- Function to change the font of text buttons -----------------------//
const Font CabbageLookAndFeel::getFontForTextButton (TextButton& /*button*/)
{
	
	Font font;
	font.setTypefaceName (T("Verdana"));
	font.setHeight (12);
	return font;
}


//--------- Function to change the appearance of combo boxes ---------------//
void CabbageLookAndFeel::drawComboBox(Graphics& g, int /*width*/, int /*height*/, bool /*isButtonDown*/,
																	int /*buttonX*/,
																	int /*buttonY*/,
																	int /*buttonW*/,
																	int /*buttonH*/,
																	ComboBox& box)
{
	g.setColour(Colours::black);	
	g.setOpacity(0.5);		//creating transparancy
	g.fillAll();			//filling in the background	
	g.setColour(Colours::black);
	g.fillRect (box.getWidth()-20, 0, 20, 20);
	g.setColour(Colours::lime);
	const Line<float> line((float)box.getWidth() - 10, 0, (float)box.getWidth() - 10, 13); //arrow line
	g.drawArrow(line, 0, 8, 8);	//draws arrow
}







