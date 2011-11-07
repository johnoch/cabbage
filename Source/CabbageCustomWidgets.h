/*
  Copyright (C) 2010 Rory Walsh, Damien Rennick

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
#include "CabbageUtils.h"

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
	
		if(caption.length()>0){
			offX=0;
			offY=20;
			offWidth=0;
			offHeight=-30;
			groupbox->setText(caption);
			groupbox->setVisible(true);
		}
	}

	else{ ///horizontal
	if(caption.length()>0){
		offX=15;
		offY=15;
		offWidth=-25;
		offHeight=-22;
		groupbox->setText(caption);
		groupbox->setVisible(true);
		}
	}


	if(colour.length()>0){
	slider->setColour(0x1001500, Colours::findColourForName(colour, Colours::whitesmoke));
	}
	this->setWantsKeyboardFocus(false);
}//--- end of constructor ----

//---------------------------------------------
~CabbageSlider(){

}

//---------------------------------------------
void resized()
{
groupbox->setBounds(0, 0, getWidth(), getHeight()); 
slider->setBounds(offX, offY, getWidth()+offWidth, getHeight()+offHeight); 
this->setWantsKeyboardFocus(false);
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
	this->setWantsKeyboardFocus(false);
}
//---------------------------------------------
~CabbageCheckbox(){

}

//---------------------------------------------
void resized()
{
groupbox->setBounds(0, 0, getWidth(), getHeight()); 
button->setBounds(offX, offY, getWidth()+offWidth, getHeight()+offHeight); 
this->setWantsKeyboardFocus(false);
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
	this->setWantsKeyboardFocus(false);

}
//---------------------------------------------
~CabbageComboBox(){

}

//---------------------------------------------
void resized()
{
groupbox->setBounds(0, 0, getWidth(), getHeight()); 
combo->setBounds(offX, offY, getWidth()+offWidth, getHeight()+offHeight); 
this->setWantsKeyboardFocus(false);
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
		this->setWantsKeyboardFocus(false);
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
		this->setWantsKeyboardFocus(false);

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
this->setWantsKeyboardFocus(false);
}

JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CabbageGroupbox);
};

//==============================================================================
// XYController, used in CabbageXYController class
//==============================================================================
class XYController	:	public Component,
						public ActionBroadcaster,
						public Timer
{
public:
	XYController(String inName, String title, int minXValue, int maxXValue, int minYValue, 
																			int maxYValue, 
																			float numDecPlaces)
		:name(inName), 
		title(title),
		outputY(0),
		outputX(0)
	{
		//----- Texteditor to display the output values for x and y
		for (int i=0; i<3; i++) {
			textEditors.add(new TextEditor());
			textEditors[i]->setReadOnly (true);
			textEditors[i]->setWantsKeyboardFocus(false);
			textEditors[i]->setColour(0x1000201, Colours::lime);	//text colour
			addAndMakeVisible(textEditors[i]);
		}

		//----- ComboBox
		autoCombo = new ComboBox (T("Auto"));
		autoCombo->addItem (T("Off"), 1);
		autoCombo->addItem (T("On"), 2);
		autoCombo->setSelectedId(1);
		autoCombo->setWantsKeyboardFocus(false);
		autoCombo->setColour (0x1000a00, Colours::whitesmoke); //text colour
		addAndMakeVisible (autoCombo);

		//----- Min and max output values
		minX = minXValue;
		maxX = maxXValue;
		minY = minYValue;
		maxY = maxYValue;

		buttonDown = 0;	
		flagX = 0;
		flagY = 0;

		//----- The title is drawn in the paint() function
		title.append (name, 100);

		/*----- Number of decimal places to be displayed. 0 seems to cause a problem, so 1
		is used instead. */
		decimalPlaces = numDecPlaces;
		if (decimalPlaces == 0) decimalPlaces = 1;
		this->setWantsKeyboardFocus(false);
	}


	//========== Destructor ===========================================================
	~XYController()
	{

	}


	//========== Resizes componenets in relation to the total width and height ========
	void resized()
	{
		//------ Total width and height of component
		totalWidth = this->getWidth();
		totalHeight = this->getHeight();	

		//------ Ball size 
		ballSize = totalWidth/10;
		if ((ballSize % 2) == 1) ballSize += 1; //to make sure that ballSize is a multiple of 2
		if (ballSize > 18) ballSize = 18;		//maximum size
		if (ballSize < 8) ballSize = 8;			//minimum size

		//------- Defining the bounds that the ball can move in....
		borderTop = 3;
		borderBottom = totalHeight - 25;
		borderLeft = 3;
		borderRight = totalWidth - 3;
		availableWidth = (borderRight - borderLeft);
		availableHeight = (borderBottom - borderTop);

		//------- Texteditor 
		textEditors[0]->setBounds (totalWidth-120, (totalHeight-18), 100, 15);
		textEditors[1]->setBounds (totalWidth-120, (totalHeight-18), 50, 15);
		textEditors[2]->setBounds (totalWidth-70, (totalHeight-18), 50, 15);

		//----- ComboBox
		autoCombo->setBounds (10, (totalHeight-18), 45, 15);

		//------ Initialising ball to be in the centre of the component 
		x = availableWidth / 2;
		y = availableHeight / 2;
		writeText (x, y);
		this->setWantsKeyboardFocus(false);
	}


	//========== Returns outputY to another component =================================
	float getBallY()
	{
		//return  1-(float(y)/availableHeight);
		return outputY;
	}


	//========== Returns outputX to another component =================================
	float getBallX()
	{
		//return float(x)/availableWidth;
		return outputX;
	}


	/*========== This function calculates the (x, y) coordinates from the slider input. 
	xIn and yIn are not the actual slider values, they are normalised decimal fractions of 
	the slider range. ballSize has to be subtracted from available width and height, as the 
	ball is drawn from its top-left corner. Border sizes also have to be added on. */
	void setBallXY(float xIn, float yIn, bool Normal) 
	{
		if(Normal){
		x = ((xIn * (availableWidth - ballSize)) + borderLeft);
		yIn = 1 - yIn;  //inverting y values
		y = ((yIn * (availableHeight - ballSize)) + borderTop);
		writeText (x, y);
		repaint();
		}
		else{
		float max = getMaxX();
		float min = getMinX();
		xIn = (xIn-getMinX()) / (getMaxX()-getMinX());
		x = ((xIn * (availableWidth - ballSize)) + borderLeft);
		max = getMaxY();
		min = getMinY();
		yIn = (yIn-getMinY()) / (getMaxY()-getMinY());
		yIn = 1 - yIn;
		y = ((yIn * (availableHeight - ballSize)) + borderTop);
		//writeText (x, y);
		//repaint();
		}
	}



	//========== Returns Y Minimum =======================================================
	float getMinY()
	{
		return minY;
	}


	//========== Returns X Minimum =========================================================
	float getMinX()
	{
		return minX;
	}

	//========== Returns Y Maximum =======================================================
	float getMaxY()
	{
		return maxY;
	}


	//========== Returns X maximum =========================================================
	float getMaxX()
	{
		return maxX;
	}


	//=========== Paint function ============================================================
	void paint (Graphics& g)
	{
		this->setWantsKeyboardFocus(false);
		//----- For drawing the border 
		g.setColour (Colours::black);
		g.setOpacity (0.4f);
		g.fillRoundedRectangle (0, 0, totalWidth, totalHeight, (totalWidth/15));

		//----- For drawing the actual area that the ball can move in
		g.setColour (Colours::black);
		g.setOpacity (0.9f);
		g.fillRoundedRectangle (3, 3, totalWidth-6, totalHeight-25, (totalWidth/20));

		//----- For drawing the title
		g.setColour (Colours::whitesmoke);
		g.setOpacity (0.8);
		g.setFont (15, 0);
		Justification just(1);
		g.drawText (title, 10, 0, totalWidth-20, 30, just, false); 

		//----- Grid 
		float thickness = 0.5; //for grid lines
		ColourGradient cg = ColourGradient (Colours::grey, x+(ballSize)/2, y+(ballSize)/2, 
			Colours::transparentBlack, (x+(ballSize)/2)-50, (y+(ballSize)/2)-50, true);
		g.setGradientFill (cg);

		for (int i=1; i<11; i++) {
			g.drawLine ((totalWidth/10)*i, 3, (totalWidth/10)*i, availableHeight, thickness);//vertical lines
			g.drawLine (3, (totalHeight/10)*i, availableWidth, (totalHeight/10)*i, thickness);//horizontal lines
		}
  
		//----- Switch statement to see if mouse button is up or down
		switch (buttonDown)
		{
		case 0: //button up
			g.drawLine (x + (ballSize/2) - 0.5, 0, x + (ballSize/2) - 0.5, availableHeight, 1); //to emphasise the centre lines
			g.drawLine (0, y + (ballSize/2) - 0.5, availableWidth, y + (ballSize/2) - 0.5, 1);
			g.setColour (Colours::lime);
			g.drawEllipse (x, y, ballSize-1, ballSize-1, 2); //width of 2
			break;
		case 1: //button down
			cg = ColourGradient (Colours::lime, totalWidth/2, totalHeight/2, Colours::transparentBlack, 
				(totalWidth/2)-50, (totalHeight/2)-50, true); //changing colour of lines
			g.setGradientFill (cg);
			g.drawLine (x + (ballSize/2) - 0.5, 0, x + (ballSize/2) - 0.5, availableHeight, 2); //to emphasise the centre lines
			g.drawLine (0, y + (ballSize/2) - 0.5, availableWidth, y + (ballSize/2) - 0.5, 1);
			g.setColour (Colours::lime);
			g.fillEllipse (x, y, ballSize, ballSize);
			break;
		}
		this->setWantsKeyboardFocus(false);
	}


	//================ Mouse Down =======================================================
	void mouseDown (const MouseEvent& e)
	{
		this->stopTimer();
	
		//----- Hiding cursor
		setMouseCursor (MouseCursor::NoCursor); 
		Desktop::getInstance().getMainMouseSource().forceMouseCursorUpdate(); 

		/*----- Getting current (x,y) position. Subtracting (ballSize/2) means the 
		cursor should be in the middle of the ball */
		x = (e.getPosition().getX()) - (ballSize/2); 
		y = (e.getPosition().getY()) - (ballSize/2);

		checkBoundary (x, y);

		buttonDown = 1;
		repaint();

		//----- Clearing vectors and writing the first values, point (x, y). Resetting count.....
		xValue.clear();
		yValue.clear();
		xValue.push_back(x);
		yValue.push_back(y);
		count = 0;
		flagX = 0;
		flagY = 0;

		//----- Writing ouput values
		writeText (x, y);
		sendActionMessage(name);
	}


	//============== Mouse Up ==============================================================
	void mouseUp (const MouseEvent& /*e*/)
	{
		//----- Cursor is made visible on mouseUp
		setMouseCursor (MouseCursor::NormalCursor);
		Desktop::getInstance().getMainMouseSource().forceMouseCursorUpdate();

		buttonDown = 0;
		repaint();

		//---- If Auto is set to "On"...........
		if (autoCombo->getSelectedId() == 2) {
		
			/*----- This block makes sure that the vectors have at least 2 elements. If 
			they don't, then they are filled up with the first element + 1...*/
			if (count < 1) {
				for (int i=0; i<1; i++) {
					xValue.push_back((xValue[count]) + 1);
					yValue.push_back((yValue[count]) + 1);
					count++;
				}
			}
	
			//----- The increments will determine the overall direction for the flight of the ball....
			incrementX = xValue[count] - xValue[count-1];
			incrementY = yValue[count] - yValue[count-1];

			/*----- The following checks if a boundary was hit during mouseDrag, as this could result in the border
			value being added more than once to the vectors. If it was hit then the increments could be 0, which 
			would result in no movement for that axis. Therefore, they have to look further back in the vectors to 
			get the most recent value that differs from the current one.*/
			if (flagX == 1) {
				while (incrementX == 0) {
					incrementX = xValue[count] - xValue[count-1];
					count--;
				}
			}

			if (flagY == 1) {
				while (incrementY == 0) {
					incrementY = yValue[count] - yValue[count-1];
					count--;
				}
			}
			//----------------------------------

			
			/*----- The following block makes sure that the higest increment will be 10 and lowest will be -10. 
			The corresponding x or y increment will still remain in proportion.  For instance, if incrementX
			is 30 and incrementY is 15, incrementX will become 10 and incrementY will become 5 etc.....*/
			if ((incrementX > 10) || (incrementY > 10)) {
				if (incrementX > incrementY) {
					incrementY = (int)((incrementY / incrementX) * 10) + 0.5;
					incrementX = 10;
				}
				if (incrementY > incrementX) {
					incrementX = (int)((incrementX / incrementY) * 10) + 0.5;
					incrementY = 10;
				}
			}

			if ((incrementX < -10) || (incrementY < -10)) {
				if (incrementX < incrementY) {
					incrementY = (int)((incrementY / incrementX) * -10) - 0.5;
					incrementX = -10;
				}
				if (incrementY < incrementX) {
					incrementX = (int)((incrementX / incrementY) * -10) - 0.5;
					incrementY = -10;
				}
			}
			//-----------------------------

	
			/*----- The following code calculates the timeValue in miliseconds in relation to the highest
			increment value*/
			int timeValue;
			int tempX = incrementX;
			int tempY = incrementY;

			if (tempX < 0) tempX *= -1;
			if (tempY < 0) tempY *= -1;
			if (tempX > tempY) timeValue = tempX;
			else timeValue = tempY;

			timeValue = (timeValue * -3) + 65;	//the biggest increment will have the fastest timeValue
			//------------------------------

			buttonDown = 1;
			this->startTimer(timeValue);
		}
		//----- end of if Auto is set to "On"
	}


	//============= Mouse Drag =============================================================
	void mouseDrag (const MouseEvent& e)
	{
		//----- Getting current (x,y) position
		x = (e.getPosition().getX()) - (ballSize/2);
		y = (e.getPosition().getY()) - (ballSize/2);

		checkBoundary (x, y);

		repaint();

		//----- Writing coordinates to vectors
		xValue.push_back(x); 
		yValue.push_back(y);
		count ++;
	
		//----- Writing output values
		writeText (x, y);
		sendActionMessage(name);
	} 


	//============= Timer function =========================================================
	void timerCallback()
	{
		//----- Incrementing (x,y) coordinate
		x += incrementX;
		y += incrementY;
	
		//----- If the ball hits a border, its increment value should be reversed
		if ((x < borderLeft) || (x > (borderRight - ballSize))) {
			incrementX *= -1;
			x += incrementX;
		}
		if ((y < borderTop) || (y > (borderBottom - ballSize))) {
			incrementY *= -1;
			y += incrementY;
		}	

		//----- Writing output values
		writeText (x, y);
		repaint();
	//	sendActionMessage(name);
		
	}
	  

	//=========== Checking to make sure point is inside boundary ============================
	void checkBoundary (float &x, float&y)
	{
		/*----- The ball is drawn from the top-left corner of the rectangle around the ellipse so 
		the diameter of the ball has to be subtracted for borders right and bottom to get the true 
		border value. If a border is hit, flags are also set to 1.*/
		if (x < borderLeft) {
			x = borderLeft;
			flagX = 1;
		}
		if (x > (borderRight - ballSize)) {
			x = (borderRight - ballSize);
			flagX = 1;
		}
		if (y < borderTop) {
			y = borderTop;
			flagY = 1;
		}
		if (y > (borderBottom - ballSize)) {
			y = (borderBottom - ballSize);
			flagY = 1;
		}
	}


	//=========== Writing values to textbox =================================================
	void writeText (float &x, float &y)
	{
		/*----- Putting together the textbox string. The values are first converted to lie between
		0 and 1. Border size and ballSize are needed to make this accurate. These values can then 
		be applied to the x and y ranges to output the correct value.*/
		outputX = (float)(x - 3) / (float)(availableWidth - ballSize);	//left border is 3
		outputY = (float)(y - 3) / (float)(availableHeight - ballSize);	//top border is 3
		outputY = 1 - outputY;  //inverting the y values

		xRange = maxX - minX;
		yRange = maxY - minY;

		outputX = (outputX * xRange) + minX;
		outputY = (outputY * yRange) + minY;

		String str1(outputX, decimalPlaces);
		String str2(outputY, decimalPlaces);

		Font font(15, 0);
		float xStrWidth = font.getStringWidthFloat(str1);
		float yStrWidth = font.getStringWidthFloat(str2);
		//------- Texteditor 
		textEditors[0]->setBounds (totalWidth-(xStrWidth+yStrWidth+50), (totalHeight-18), xStrWidth+yStrWidth+10, 15);
		textEditors[1]->setBounds (totalWidth-(xStrWidth+yStrWidth+40), (totalHeight-18), xStrWidth+15, 15);
		textEditors[2]->setBounds (totalWidth-(yStrWidth+30), (totalHeight-18), yStrWidth+15, 15);

		textEditors[1]->setText (str1, false);  
		textEditors[2]->setText (str2, false); 
	}

	juce_UseDebuggingNewOperator

private:
	float x, y, count;
	vector<float> xValue, yValue;
	float incrementX, incrementY, outputX, outputY, xRange, yRange;
	int flagX, flagY;
	int totalWidth, totalHeight, availableWidth, availableHeight;
	int borderTop, borderBottom, borderLeft, borderRight;
	int ballSize;
	int buttonDown;
	float minX, maxX, minY, maxY;
	String title, name;
	int decimalPlaces;

	ScopedPointer<ComboBox> autoCombo;
	OwnedArray<TextEditor> textEditors;

JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (XYController);	
};

//==============================================================================
// custom CabbageXYController
//==============================================================================
class CabbageXYController : public Component
{
//ScopedPointer<LookAndFeel> lookFeel;
int offX, offY, offWidth, offHeight;
public:
ScopedPointer<GroupComponent> groupbox;
ScopedPointer<XYController> xypad;
//---- constructor -----
CabbageXYController(String name, String text, String caption, int minX, int maxX, int minY, int maxY)
{
	setName(name);
	offX=offY=offWidth=offHeight=0;
	
	groupbox = new GroupComponent(String("groupbox_")+name);
	groupbox->setWantsKeyboardFocus(false);
	xypad = new XYController(name, text, minX, maxX, minY, maxY, 2);
	xypad->setWantsKeyboardFocus(false);
	addAndMakeVisible(xypad);
	addAndMakeVisible(groupbox);

	groupbox->setVisible(false);

	groupbox->setColour(GroupComponent::outlineColourId,
		Colours::findColourForName("white", Colours::white));
	groupbox->setColour(GroupComponent::textColourId,
		Colours::findColourForName("white", Colours::white));
	
	if(caption.length()>0){
		offX=10;
		offY=15;
		offWidth=-20;
		offHeight=-25;
		groupbox->setVisible(true);
		groupbox->setText(caption);
	}
	this->setWantsKeyboardFocus(false);
}
//---------------------------------------------
~CabbageXYController(){

}

//---------------------------------------------
void resized()
{
groupbox->setBounds(0, 0, getWidth(), getHeight()); 
xypad->setBounds(offX, offY, getWidth()+offWidth, getHeight()+offHeight); 
this->setWantsKeyboardFocus(false);
}

JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CabbageXYController);
};


//==============================================================================
// custom Csound message console
//==============================================================================
class CabbageMessageConsole : public Component
{
ScopedPointer<GroupComponent> groupbox;
ScopedPointer<OldSchoolLookAndFeel> lookAndFeel;
String text;
int offX, offY, offWidth, offHeight;
public:
	ScopedPointer<TextEditor> editor;
//---- constructor -----
CabbageMessageConsole(String name, String caption, String text):
		  editor(new TextEditor(String("editor_")+name)), 
		  groupbox(new GroupComponent(String("groupbox_")+name)),
		  lookAndFeel(new OldSchoolLookAndFeel()),
		  offX(0), 
		  offY(0), 
		  offWidth(0), 
		  offHeight(0),
		  text(text)
{	
	String message("sdgsdfgdf\nsdgsdfgdf\nsdgsdfgdf\nsdgsdfgdf\nsdgsdfgdf\nsdgsdfgdf\nsdgsdfgdf\nsdgsdfgdf\nsdgsdfgdf\nsdgsdfgdf\nsdgsdfgdf\nsdgsdfgdf\nsdgsdfgdf\nsdgsdfgdf\nsdgsdfgdf\nsdgsdfgdf\nsdgsdfgdf\nsdgsdfgdf\nsdgsdfgdf\nsdgsdfgdf\nsdgsdfgdf\nsdgsdfgdf\nsdgsdfgdf\nsdgsdfgdf\nsdgsdfgdf\nsdgsdfgdf\nsdgsdfgdf\n");
	editor->setLookAndFeel(lookAndFeel);
	addAndMakeVisible(editor);
	editor->setMultiLine(true);
	editor->setScrollbarsShown(true);
	editor->setReturnKeyStartsNewLine(true);
	editor->setReadOnly(true);
	
	editor->setColour(TextEditor::ColourIds::backgroundColourId, Colours::black);
	editor->setColour(TextEditor::ColourIds::textColourId, Colours::green);
	editor->setText(message);
	if(caption.length()>0){
		offX=10;
		offY=15;
		offWidth=-20;
		offHeight=-25;
		groupbox->setVisible(true);
		groupbox->setText(caption);
	}
	this->setWantsKeyboardFocus(false);
}
//---------------------------------------------
~CabbageMessageConsole(){

}

void paint(Graphics &g){
	//----- For drawing the border 
	g.setColour (Colours::black);
	g.setOpacity (0.4f);
	g.fillRoundedRectangle (0, 0, getWidth(), getHeight(), (getWidth()/25));
	g.setOpacity(1);
	g.fillRoundedRectangle (5, getHeight()-47, getWidth()-11, 40, (getWidth()/25));

	//----- For drawing the title
	g.setColour (Colours::whitesmoke);
	g.setOpacity (0.8);
	g.setFont (15, 0);
	Justification just(1);
	g.drawText (text, 20, 0, getWidth()-20, 30, just, false); 
	//g.fillRect (0, 30, getWidth(), getHeight()-40);
}

//---------------------------------------------
void resized()
{
groupbox->setBounds(0, 0, getWidth(), getHeight()); 
editor->setBounds(offX+5, offY+25, (getWidth()+offWidth)-10, (getHeight()+offHeight)-45); 
this->setWantsKeyboardFocus(false);
}

JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CabbageMessageConsole);
};

//==============================================================================
// custom VUComponent
//==============================================================================
class VUComponent	:	public Component
{
public:
	//=================================================================================
	//		 Nested class VU Meter
	//=================================================================================
	class VUMeter	:	public Component
	{
	public:
		VUMeter()
		{
			clipMarker = 0;	//0 is off, 1 is on
			this->setWantsKeyboardFocus(false);
		}

		~VUMeter(){}

		//========= Paint Function ===================================================
		void paint (Graphics &g)
		{
			//----- Painting the background of the component
			g.setColour (Colours::black);
			g.setOpacity (0.4);
			g.fillRoundedRectangle (getWidth()*0.1, 0, getWidth()*.8, getHeight(), getWidth()/5);

			//----- Declaring dimensional properties
			int top = 5;
			int clipZone = top + 3;								//Leaving a gap for the clip marker at the top
			int bottom = getHeight() - 5;
			int availableWidth = getWidth()/5;					//width of the actual vu is 1 fifth of total width
			int availableHeight = bottom - clipZone;			//height available to the VU part
			int xOffset = getWidth()/5;							// The vu be drawn in 1 fifth of totalWidth from left
			
			// 1-level inverts the y axis. It also has to be incremented by the clipZone height and top border
			int yOffset = ((1 - level) * availableHeight) + clipZone;

			/*----- Declarations for each db value. The range will go logarithmically from -infinity to +3db. Therefore 
			the input level (between 0 and 1) will have to be divided up accordingly. */
			float plus3DB = 1;
			float zeroDB = 0.9;
			float minus3DB = 0.71 * zeroDB; //0.639
			float minus10DB = 0.3162 * zeroDB; //0.285
			float minus20DB = 0.1 * zeroDB; //0.09
			float minus40DB = 0.01 * zeroDB; //0.009

			//----- Setting the location of the db markers
			int plus3Mark = bottom - availableHeight;
			int zeroMark = bottom - (0.9 * availableHeight);
			int minus3Mark = bottom - (0.8 * availableHeight);
			int minus10Mark = bottom - (0.5 * availableHeight);
			int minus20Mark = bottom - (0.25 * availableHeight);
			int minus40Mark = bottom - (0.1 * availableHeight);

			//----- Colour fills
			Colour clClip = (Colours::red);		//red for clip marker
			/*Colour clRed = (Colours::orangered);
			Colour clOrange = (Colours::orange);
			Colour clYellow = (Colours::yellow);
			Colour clGreen = (Colours::lime);*/
			ColourGradient clRed = ColourGradient (Colours::orangered, 0, clipZone, Colours::orange, 0, zeroMark, false);
			ColourGradient clOrange = ColourGradient (Colours::orange, 0, zeroMark, Colours::yellow, 0, minus3Mark, false);
			ColourGradient clYellow = ColourGradient (Colours::yellow, 0, minus3Mark, Colours::lime, 0, minus10Mark, false);
			ColourGradient clGreen = ColourGradient (Colours::lime, 0, minus10Mark, Colours::darkgreen, 0, bottom, false);

			//----- Painting the VU background before any levels are painted.
			g.setGradientFill (clGreen);
			g.setOpacity (0.1);
			g.fillRect (xOffset, minus10Mark, availableWidth, bottom-minus10Mark);
			g.setGradientFill (clYellow);
			g.setOpacity (0.1);
			g.fillRect (xOffset, minus3Mark, availableWidth, minus10Mark-minus3Mark);
			g.setGradientFill (clOrange);
			g.setOpacity (0.1);
			g.fillRect (xOffset, zeroMark, availableWidth, minus3Mark-zeroMark);
			g.setGradientFill (clRed);
			g.setOpacity (0.1);
			g.fillRect (xOffset, plus3Mark, availableWidth, zeroMark-plus3Mark);

			
			//----- Painting the db level markers
			g.setColour (Colours::white);
			g.setFont (getWidth()*0.3, 0); //Font size is calculated using the total width
			Justification just (4);			//Centered
			g.drawText (T("+3"), getWidth()*.4, plus3Mark-5, getWidth()*.4, 10, just, false);
			g.drawText (T("0"), getWidth()*.4, zeroMark-5, getWidth()*.4, 10, just, false);
			g.drawText (T("-3"), getWidth()*.4, minus3Mark-5, getWidth()*.4, 10, just, false);
			g.drawText (T("-10"), getWidth()*.4, minus10Mark-5, getWidth()*.4, 10, just, false);
			g.drawText (T("-20"), getWidth()*.4, minus20Mark-5, getWidth()*.4, 10, just, false);
			g.drawText (T("-40"), getWidth()*.4, minus40Mark-5, getWidth()*.4, 10, just, false);


			/*----- The following if statements determine which colours should be used for the fill. Each
			zone itself has to be treated independently to the rest of the level range.*/
			if ((level >= 0) && (level < minus40DB)){
				g.setGradientFill (clGreen);
				g.setOpacity (0.8);
				yOffset = (bottom - minus40Mark) * (level / minus40DB);
				g.fillRect (xOffset, bottom-yOffset, availableWidth, yOffset);
			}
			//----- If level is between 0.009 and 0.09
			else if ((level >= minus40DB) && (level < minus20DB)){
				g.setGradientFill (clGreen);
				g.setOpacity (0.8);
				yOffset = (minus40Mark-minus20Mark) * ((level - minus40DB) / (minus20DB - minus40DB));
				g.fillRect (xOffset, minus40Mark-yOffset, availableWidth, yOffset);
				g.fillRect (xOffset, minus40Mark, availableWidth, bottom - minus40Mark);
			}
			//----- If level is between 0.285 and 0.09
			else if ((level >= minus20DB) && (level < minus10DB)){
				g.setGradientFill (clGreen);
				g.setOpacity (0.8);
				yOffset = (minus20Mark-minus10Mark) * ((level - minus20DB) / (minus10DB - minus20DB));
				g.fillRect (xOffset, minus20Mark-yOffset, availableWidth, yOffset);
				g.fillRect (xOffset, minus20Mark, availableWidth, bottom - minus20Mark);
			}
			//----- If level is between 0.285 and 0.639
			else if ((level >= minus10DB) && (level < minus3DB)){
				g.setGradientFill (clYellow);
				g.setOpacity (0.8);
				yOffset = (minus10Mark-minus3Mark) * ((level - minus10DB) / (minus3DB - minus10DB)); 
				g.fillRect (xOffset, minus10Mark-yOffset, availableWidth, yOffset);
				g.setGradientFill (clGreen);
				g.setOpacity (0.8);
				g.fillRect (xOffset, minus10Mark, availableWidth, bottom - minus10Mark);
			}
			//----- If level is between 0.639 and 0.9
			else if ((level >= minus3DB) && (level < zeroDB)){
				g.setGradientFill (clOrange);
				g.setOpacity (0.8);
				yOffset = (minus3Mark-zeroMark) * ((level - minus3DB) / (zeroDB - minus3DB));
				g.fillRect (xOffset, minus3Mark-yOffset, availableWidth, yOffset);
				g.setGradientFill (clYellow);
				g.setOpacity (0.8);
				g.fillRect (xOffset, minus3Mark, availableWidth, minus10Mark - minus3Mark);
				g.setGradientFill (clGreen);
				g.setOpacity (0.8);
				g.fillRect (xOffset, minus10Mark, availableWidth, bottom - minus10Mark);
			}
			//----- If level is 0.9 or over
			else if (level >= zeroDB) {
				g.setGradientFill (clRed);
				g.setOpacity (0.8);
				yOffset = (zeroMark-plus3Mark) * ((level*10)-9);
				g.fillRect (xOffset, zeroMark-yOffset, availableWidth, yOffset);
				g.setGradientFill (clOrange);
				g.setOpacity (0.8);
				g.fillRect (xOffset, zeroMark, availableWidth, minus3Mark - zeroMark);
				g.setGradientFill (clYellow);
				g.setOpacity (0.8);
				g.fillRect (xOffset, minus3Mark, availableWidth, minus10Mark - minus3Mark);
				g.setGradientFill (clGreen);
				g.setOpacity (0.8);
				g.fillRect (xOffset, minus10Mark, availableWidth, bottom - minus10Mark);
			}

			//----- To check if clip marker should be displayed
			switch (clipMarker)
			{
			case 0: //if off
				g.setColour (clClip);
				g.setOpacity (0.2);
				g.fillRect (xOffset, top, availableWidth, 2);
				break;
			case 1: //if on
				g.setColour (clClip);
				g.setOpacity (1);
				g.fillRect (xOffset, top, availableWidth, 2);
				break;
			}

		}

		
		//========= Set Level ==========================================================
		void setLevel (float value)
		{
			level = value;
			if (level >= 1) clipMarker = 1;
			
			repaint();
		}

		//======== If mouse if clicked =================================================
		void mouseDown (const MouseEvent& e)
		{
			int x = e.getPosition().getX();
			int y = e.getPosition().getY();

			/*----- Checking to see if the mouse click is within the correct bounds. 5 is 
			added to clipZone to offer the user a bigger area */
			if ((x >= getWidth()/5) && (x <= getWidth()*0.8) &&
				(y >= 3) && (y <= 11)) { //clipZone lies within this area
					clipMarker = 0;
					repaint();
			}

		}

	private:
		int widthMeter, heightMeter;
		float level;
		int clipMarker;

//		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VUMeter);

	};
	//------------------------------------------------------------


	//===== VU Component Constructor ====================================
	VUComponent (int numberOfMeters, int widthOfMeter, int heightOfMeter)
	{
		numMeters = numberOfMeters;
		widthMeter = widthOfMeter;
		heightMeter = heightOfMeter;

		for (int i=0; i<numMeters; i++) {
			meters.add (new VUMeter());
			meters[i]->setBounds (i*widthMeter, 0, widthMeter, heightMeter);
			meters[i]->setWantsKeyboardFocus(false);
			addAndMakeVisible (meters[i]);
		}
	}

	//===== Destructor ====================================================
	~VUComponent()
	{
	}

	//===== Set VU Level Function =========================================
	void setVULevel (int VUIndex, float value)
	{
		meters[VUIndex]->setLevel (value);
	}

private:
	OwnedArray<VUMeter> meters;
	int numMeters;
	int widthMeter, heightMeter;

//	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VUComponent);
};


//==============================================================================
// custom CabbageVUMeter
//==============================================================================
class CabbageVUMeter : public Component
{
//ScopedPointer<LookAndFeel> lookFeel;
int offX, offY, offWidth, offHeight, numMeters, width, height;
String name;
public:
ScopedPointer<GroupComponent> groupbox;
ScopedPointer<VUComponent> vuMeter;
//---- constructor -----
CabbageVUMeter(String name, String text, String caption, float meters, float width, int height):
numMeters(meters)
{
	setName(name);
	offX=offY=offWidth=offHeight=0;
	
	groupbox = new GroupComponent(String("groupbox_")+name);
	

	vuMeter = new VUComponent(meters, width/(meters*1.30), height*.92);
	addAndMakeVisible(vuMeter);
	addAndMakeVisible(groupbox);

	groupbox->setVisible(false);

	groupbox->setColour(GroupComponent::outlineColourId,
		Colours::findColourForName("white", Colours::white));
	groupbox->setColour(GroupComponent::textColourId,
		Colours::findColourForName("white", Colours::white));
	
	if(caption.length()>0){
		offX=10;
		offY=15;
		offWidth=-20;
		offHeight=-25;
		groupbox->setVisible(true);
		groupbox->setText(caption);
	}
	this->setWantsKeyboardFocus(false);
}
//---------------------------------------------
~CabbageVUMeter(){

}

int getNoMeters(){
	return numMeters;
}

//---------------------------------------------
void resized()
{
groupbox->setBounds(0, 0, getWidth(), getHeight()); 
vuMeter->setBounds(offX, offY, getWidth()+offWidth, getHeight()+offHeight); 
this->setWantsKeyboardFocus(false);
}

JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CabbageVUMeter);
};

#endif