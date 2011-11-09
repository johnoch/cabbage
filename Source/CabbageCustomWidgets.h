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
						public SliderListener,
						public ButtonListener,
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
		basicLookAndFeel = new CabbageLookAndFeelBasic();

		//----- Texteditor to display the output values for x and y
		for (int i=0; i<2; i++) {
			textEditors.add(new TextEditor());
			textEditors[i]->setReadOnly (true);
			textEditors[i]->setColour(0x1000201, Colours::lime);	//text colour
			textEditors[i]->setColour(0x1000200, Colours::transparentBlack);	//background colour
			addAndMakeVisible(textEditors[i]);
		}

		//----- Slider
		xySpeed = new Slider (T("speedXY"));
		xySpeed->setSliderStyle (Slider::LinearHorizontal);
		xySpeed->setRange(0, 2, 0.01);
		xySpeed->setValue(1, true, false);
		xySpeed->setLookAndFeel (basicLookAndFeel);
		xySpeed->addListener (this);
		addAndMakeVisible (xySpeed);

		//----- ToggleButton
		autoToggle = new ToggleButton (T("Off"));
		autoToggle->setToggleState(false, true);
		autoToggle->addListener (this);
		addAndMakeVisible (autoToggle);

		//----- Min and max output values
		minX = minXValue;
		maxX = maxXValue;
		minY = minYValue;
		maxY = maxYValue;

		buttonDown = 0;	
		flagX = 0;
		flagY = 0;
		speed = 0;

		//----- The title is drawn in the paint() function
		title.append (name, 100);

		/*----- Number of decimal places to be displayed. 0 seems to cause a problem, so 1
		is used instead. */
		decimalPlaces = numDecPlaces;
		if (decimalPlaces == 0) decimalPlaces = 1;
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
		borderBottom = totalHeight - 40;
		borderLeft = 3;
		borderRight = totalWidth - 3;
		availableWidth = (borderRight - borderLeft);
		availableHeight = (borderBottom - borderTop);

		//----- Slider
		//xySpeed->setBounds (10, (totalHeight-25), 80, 30);
		xySpeed->setBounds (10, (totalHeight-38), totalWidth-20, 20);

		//----- Toggle
		autoToggle->setBounds (10, (totalHeight-18), 40, 15);

		//------ Initialising ball to be in the centre of the component 
		x = availableWidth / 2;
		y = availableHeight / 2;
		writeText (x, y);

		//----- Initialising background image
		drawBackgroundImage();

		//----- Gradient fill for green lines
		cgGreen = ColourGradient (Colours::lime, totalWidth/2, totalHeight/2, 
			Colours::transparentBlack, (totalWidth/2)-50, (totalHeight/2)-50, true); 
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
	void setBallXY(float xIn, float yIn) 
	{
		x = ((xIn * (availableWidth - ballSize)) + borderLeft);

		yIn = 1 - yIn;  //inverting y values
		y = ((yIn * (availableHeight - ballSize)) + borderTop);

		writeText (x, y);
		repaint(borderLeft, borderTop, availableWidth, availableHeight);
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

	//====== Drawing the background ========================================================
	void drawBackgroundImage()
	{
		/*----- This function draws the background onto a blank image and then loads it into cache. The 
		cached image is then reused in the paint() method. This is a more efficient way to redrawing something
		that is static. */

		// Creating a blank canvas
		img = Image::Image(Image::ARGB, totalWidth, totalHeight, true);
			
		Graphics g (img);

		//----- For drawing the border 
		g.setColour (Colours::black);
		g.setOpacity (0.4);
		g.fillRoundedRectangle (0, 0, totalWidth, totalHeight, (totalWidth/15));

		//----- For drawing the actual area that the ball can move in
		g.setColour (Colours::black);
		g.setOpacity (0.7);
		g.fillRoundedRectangle (borderLeft, borderTop, availableWidth, availableHeight, (totalWidth/20));

		//----- For drawing the title
		g.setColour (Colours::whitesmoke);
		g.setOpacity (0.3);
		g.setFont (15, 0);
		Justification just(1);
		g.drawText (title, borderLeft+3, borderTop+3, totalWidth-20, 25, just, false); 

		//----- Adding image to cache and assigning it a hash code
		ImageCache::addImageToCache (img, 13);
	}


	//=========== Paint function ============================================================
	void paint (Graphics& g)
	{
		//----- Drawing the background from the imagecache
		Image bg = ImageCache::getFromHashCode(13);
		g.drawImage (bg, 0, 0, getWidth(), getHeight(), 0, 0, bg.getWidth(), bg.getHeight(), false);
		
		//----- Grid 
		ColourGradient cgGrey = ColourGradient (Colours::grey, x+(ballSize)/2, y+(ballSize)/2, 
			Colours::transparentBlack, (x+(ballSize)/2)-50, (y+(ballSize)/2)-50, true);
		g.setGradientFill (cgGrey);

		float thickness = 0.5; //thickness of lines
		for (int i=1; i<11; i++) {
			g.drawLine ((totalWidth/10)*i, borderTop, (totalWidth/10)*i, availableHeight, thickness);//vertical lines
			g.drawLine (borderLeft, (totalHeight/10)*i, availableWidth, (totalHeight/10)*i, thickness);//horizontal lines
		}
  
		/*----- Switch statement which determines what type of ball to draw. If the mouse is up the ball is 
		hollow with grey cross lines running through it.  If the mouse is down then the ball is filled and 
		the cross lines turn green. */
		switch (buttonDown)
		{
		case 0: //button up
			g.drawLine (x + (ballSize/2), 0, x + (ballSize/2), availableHeight, 1); 
			g.drawLine (0, y + (ballSize/2), availableWidth, y + (ballSize/2), 1);
			g.setColour (Colours::lime);
			g.drawEllipse (x+1, y+1, ballSize-2, ballSize-2, 2); //width of 2
			break;
		case 1: //button down
			g.setGradientFill (cgGreen); //changing colour of lines
			g.drawLine (x + (ballSize/2) - 0.5, 0, x + (ballSize/2) - 0.5, availableHeight, 2); 
			g.drawLine (0, y + (ballSize/2) - 0.5, availableWidth, y + (ballSize/2) - 0.5, 1);
			g.setColour (Colours::lime);
			g.fillEllipse (x, y, ballSize, ballSize);
			break;
		}
	}

	//========== Slider Move ============================================================
	void sliderValueChanged(Slider* slider)
	{
		//----- If ball is already being automated...
		if (autoFlag == 1) {
			incrementX = initIncX * xySpeed->getValue();
			incrementY = initIncY * xySpeed->getValue();
			this->startTimer(speed); //restarts timer if it had been stopped
		}
			

		if (xySpeed->getValue() == 0) 
			this->stopTimer();
	}

	//========== Button Click ===========================================================
	void buttonClicked (Button* button)
	{
		if (autoToggle->getToggleState() == true) 
			autoToggle->setButtonText (T("On"));
		else if (autoToggle->getToggleState() == false) {
			autoToggle->setButtonText (T("Off"));
			autoFlag = 0;
			this->stopTimer();
		}
	}

	//================ Mouse Down =======================================================
	void mouseDown (const MouseEvent& e)
	{
		//----- Only do something if the mouse click is within the available ball area.....
		if ((e.getPosition().getX() > borderLeft) && (e.getPosition().getX() < borderRight)
			&& (e.getPosition().getY() > borderTop) && (e.getPosition().getY() < borderBottom))
		{
			this->stopTimer();
			speed = 0;		//resetting speed
			autoFlag = 0;	//setting autoFlag to "off"
	
			//----- Hiding cursor
			setMouseCursor (MouseCursor::NoCursor); 
			Desktop::getInstance().getMainMouseSource().forceMouseCursorUpdate(); 

			/*----- Getting current (x,y) position. Subtracting (ballSize/2) means the 
			cursor should be in the middle of the ball */
			x = (e.getPosition().getX()) - (ballSize/2); 
			y = (e.getPosition().getY()) - (ballSize/2);

			checkBoundary (x, y);

			buttonDown = 1;
			repaint(borderLeft, borderTop, availableWidth, availableHeight);

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

			/*----- Setting slider to middle position 1, only if automation is already turned on. */
			if (autoToggle->getToggleState() == true) 
				xySpeed->setValue(1, true, false);
		}
	}


	//============== Mouse Up ==============================================================
	void mouseUp (const MouseEvent& /*e*/)
	{
		//----- Cursor is made visible on mouseUp
		setMouseCursor (MouseCursor::NormalCursor);
		Desktop::getInstance().getMainMouseSource().forceMouseCursorUpdate();

		buttonDown = 0;
		repaint(borderLeft, borderTop, availableWidth, availableHeight);

		//----- If automation is turned "on"....
		if (autoToggle->getToggleState() == true) {
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

			/*----- The following code calculates the speed in miliseconds in relation to the highest
			increment value*/
			int tempX = incrementX;
			int tempY = incrementY;

			if (tempX < 0) tempX *= -1;
			if (tempY < 0) tempY *= -1;
			if (tempX > tempY) speed = tempX;
			else speed = tempY;

			speed = (speed * -3) + 65;	//the biggest increment will produce the fastest speed
		
			/*----- Initial values are needed for sliderValueChanged(), which will adjust the speed of
			the ball. */
			initIncX = incrementX;
			initIncY = incrementY;

			buttonDown = 1;
			this->startTimer(speed);
			autoFlag = 1;
		}
	}


	//============= Mouse Drag =============================================================
	void mouseDrag (const MouseEvent& e)
	{
		//----- Only do something if the mouse click is within the available ball area.....
		if ((e.getPosition().getY() > borderBottom) && (e.getPosition().getY() < totalHeight))
			return;
		else {
			//----- Getting current (x,y) position
			x = (e.getPosition().getX()) - (ballSize/2);
			y = (e.getPosition().getY()) - (ballSize/2);

			checkBoundary (x, y);

			repaint(borderLeft, borderTop, availableWidth, availableHeight);

			//----- Writing coordinates to vectors
			xValue.push_back(x); 
			yValue.push_back(y);
			count ++;
	
			//----- Writing output values
			writeText (x, y);
		}
	} 


	//============= Timer function =========================================================
	void timerCallback()
	{
		//----- Incrementing (x,y) coordinate
		x += incrementX;
		y += incrementY;
	
		/*----- If the ball hits a border, its increment value should be reversed. Initial values 
		also need to be reversed for sliderValueChanged(). */
		if ((x < borderLeft) || (x > (borderRight - ballSize))) {
			incrementX *= -1;
			initIncX *= -1; 
			x += incrementX;
		}
		if ((y < borderTop) || (y > (borderBottom - ballSize))) {
			incrementY *= -1;
			initIncY *= -1;
			y += incrementY;
		}

		repaint(borderLeft, borderTop, availableWidth, availableHeight);

		//----- Writing output values
		writeText (x, y);
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
            outputX = (float)(x - 3) / (float)(availableWidth - ballSize);      //left border is 3
            outputY = (float)(y - 3) / (float)(availableHeight - ballSize);      //top border is 3
            outputY = 1 - outputY;  //inverting the y values

            xRange = maxX - minX;
            yRange = maxY - minY;

            outputX = (outputX * xRange) + minX;
            outputY = (outputY * yRange) + minY;

            String str1(outputX, decimalPlaces);
            String str2(outputY, decimalPlaces);

            Font font(12, 0);
            float xStrWidth = font.getStringWidthFloat(str1);
            float yStrWidth = font.getStringWidthFloat(str2);

            //------- Texteditor
            textEditors[0]->setBounds (totalWidth - (xStrWidth+yStrWidth+43), (totalHeight-18), xStrWidth+15, 15);
            textEditors[1]->setBounds (totalWidth - (yStrWidth+25), (totalHeight-18), yStrWidth+15, 15);

            textEditors[0]->setText (str1, false);
            textEditors[1]->setText (str2, false);
            sendActionMessage(name);
       }

	juce_UseDebuggingNewOperator

private:
	float x, y, count;
	vector<float> xValue, yValue;
	float incrementX, incrementY, initIncX, initIncY, outputX, outputY, xRange, yRange;
	int flagX, flagY;
	int totalWidth, totalHeight, availableWidth, availableHeight;
	int borderTop, borderBottom, borderLeft, borderRight;
	int ballSize;
	int buttonDown;
	float minX, maxX, minY, maxY;
	String title, name;
	int decimalPlaces;
	Image img;
	ColourGradient cgGreen;
	int speed;
	int autoFlag;

	ScopedPointer<Slider> xySpeed;
	ScopedPointer<ToggleButton> autoToggle;
	OwnedArray<TextEditor> textEditors;

	ScopedPointer<CabbageLookAndFeelBasic> basicLookAndFeel;
	//void resized();	


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
		//=============================================================================
		VUMeter(int input1, bool input2)
		{
			//-----1 = Mono, 2 = Left Ch., 3 = Right Ch.
			type = input1;	

			//----- Horizontal or Vertical
			style = input2;	

			clipFlag = 0;	//0 is off, 1 is on

			/*----- Declarations for each db value. The range will go logarithmically from -infinity to +3db. 
			A level of 0.9 is being assigned to 0db. */
			plus3DB = 1;
			zeroDB = 0.9;
			minus3DB = 0.71 * zeroDB; //0.639
			minus10DB = 0.3162 * zeroDB; //0.285
			minus20DB = 0.1 * zeroDB; //0.09
			minus40DB = 0.01 * zeroDB; //0.009
		}

		//=======================================================================================
		~VUMeter(){}

		//========== Resizing method ============================================================
		void resized ()
		{
			//----- Declaring dimensional properties
			top = 5;
			clipZone = 2;									//Leaving a gap for the clip marker at the top
			bottom = getHeight() - 5;

			if (type == 1)
				levelWidth = getWidth()/5;					//width of mono level				
			else if ((type == 2) || (type == 3))
				levelWidth = (getWidth()/1.25) / 5;			//width of stereo level should be the same as if it were mono

			levelHeight = bottom - (top + clipZone);		//height available to the VU part
			xOffset = getWidth()/5;							//Mono and Left Ch. VU's will be drawn 1/5 of totalWidth from left
			yOffset = 0;
			prevLevel = bottom;
			currentLevel = bottom;

			//----- Setting the location of the db markers
			plus3Mark = bottom - levelHeight;
			zeroMark = bottom - (0.9 * levelHeight);
			minus3Mark = bottom - (0.8 * levelHeight);
			minus10Mark = bottom - (0.5 * levelHeight);
			minus20Mark = bottom - (0.25 * levelHeight);
			minus40Mark = bottom - (0.1 * levelHeight);

			//----- Setting up the gradient fill for the meter level
			cg = ColourGradient (Colours::transparentBlack, 0, bottom, 
								Colours::transparentBlack, 0, top, false);
			cg.addColour (0.07, Colours::aqua);
			cg.addColour (0.2, Colours::lime);
			cg.addColour (0.5, Colours::lime);
			cg.addColour (0.6, Colours::yellow);
			cg.addColour (0.75, Colours::yellow);
			cg.addColour (0.85, Colours::orange);
			cg.addColour (0.999, Colours::orangered);

			clClip = (Colours::red);

			/*----- Calling function to draw image background. This only applies if the VU is mono or the 
			left channel of a stereo meter. Right channel meters will not use a background as they can just
			draw over the left channel background image. */
			if ((type == 1) || (type == 2))
					verticalBackground();
		}


		//===== Vertical Background =======================================================================
		void verticalBackground ()
		{
			/*----- This function draws the background onto a blank image and then loads it into cache. The 
			cached image is then reused in the paint() method. This is a more efficient way to redrawing something
			that is static. */

			// Creating a blank canvas
			img = Image::Image(Image::ARGB, getWidth(), getHeight(), true);
			
			Graphics g (img);
			g.setColour (Colours::black);
			g.setOpacity (0.4);
			g.fillRoundedRectangle (getWidth()*0.1, 0, getWidth()*.8, getHeight(), getWidth()/5);
		
			//----- Painting the db level markers
			g.setColour (Colours::white);

			int fontSize = getHeight()*0.05;
			if (fontSize > 15)
				fontSize = 15;
			g.setFont (fontSize, 0);

			int startText;	//starting x value
			if (type == 1)
				startText = getWidth() * 0.4;
			else if (type == 2)
				startText = (getWidth() * 0.4) * 1.25; //stereo VU's are 1.25 times the size of a mono VU

			Justification just (4);			//Centered
			g.drawText (T("+3"), startText, plus3Mark-5, getWidth()*.4, 10, just, false);
			g.drawText (T("0"), startText, zeroMark-5, getWidth()*.4, 10, just, false);
			g.drawText (T("-3"), startText, minus3Mark-5, getWidth()*.4, 10, just, false);
			g.drawText (T("-10"), startText, minus10Mark-5, getWidth()*.4, 10, just, false);
			g.drawText (T("-20"), startText, minus20Mark-5, getWidth()*.4, 10, just, false);
			g.drawText (T("-40"), startText, minus40Mark-5, getWidth()*.4, 10, just, false);

			//----- Painting the level background
			g.setGradientFill (cg);
			g.setOpacity (0.1);
			g.fillRect (xOffset, clipZone, levelWidth, levelHeight);

			if (type == 2)   //if stereo we need another meter level
				g.fillRect (xOffset+levelWidth+1, clipZone, levelWidth, levelHeight);

			//---- For the clip zone
			g.setColour (clClip);
			g.setOpacity (0.2);
			g.fillRect (xOffset, top, levelWidth, clipZone);

			if (type == 2)   //if stereo we need another clip zone
				g.fillRect (xOffset+levelWidth+1, top, levelWidth, clipZone);

			//----- Loading image into cache
			if (type == 1)
				ImageCache::addImageToCache (img, 11);
			else if (type == 2)
				ImageCache::addImageToCache (img, 12);
		}

				
		//========= Paint Method ====================================================================
		void paint (Graphics &g)
		{
			Image bg;
			//----- Drawing the background from the imagecache
			if (type == 1)
				bg = ImageCache::getFromHashCode(11);
			if (type == 2)
				bg = ImageCache::getFromHashCode(12);

			g.drawImage (bg, 0, 0, getWidth(), getHeight(), 0, 0, bg.getWidth(), bg.getHeight(), false);
			

			/*----- Drawing the meter level. When paintFlag is 1 the meter level is to be increased. This
			new bit of the level is painted using the gradient fill cg. If paintFlag is 0, the level is 
			to be decreased. Because there is no colour or drawing tool used for paintFlag=0, this bit of 
			the level is cleared. */
			if (paintFlag == 1) {
				g.setGradientFill (cg);
				g.setOpacity (0.7);
				if ((type == 1) || (type == 2))
					g.fillRect (xOffset, currentLevel, levelWidth, diff);
				else
					g.fillRect ((xOffset+levelWidth+1), currentLevel, levelWidth, diff);
			}
			

			/*----- Determining if the clipmarker should be shown. It is set back to 0 immediately as it
			does not need to repainted over and over. If the clipFlag is 0, the marker will stay on if 
			the repaint() bounds are not inclusive of the clipZone. When the user clicks on the VU meter
			the clipFlag will be 0 and the repaint() bounds will include the clipZone (see mouseDown()), 
			therefore turning off the clipmarker. This happens because there is no colour or draw function 
			used for clipFlag = 0, and therefore this zone is cleared exposing the background again. */
			if (clipFlag == 1) {
				g.setColour (clClip);
				if (type == 1)
					g.fillRect (xOffset, top, levelWidth, clipZone);
				else
					g.fillRect(xOffset, top, (levelWidth*2) + 1, clipZone);

				clipFlag = 0;
			}	
		}

		
		//========= Set Level =========================================================================
		void setLevel (float value)
		{
			level = value;

			// If level is 1 or more...
			if (level >= 1) {
				clipFlag = 1;
				level = 1;
			}

			/*----- The following if statements determine the offset on the y axis. Each zone 
			itself has to be treated independently to the rest of the level range.*/
			if ((level >= 0) && (level < minus40DB)){
				currentLevel = (bottom - minus40Mark) * (level / minus40DB);
				currentLevel = bottom - currentLevel;
			}
			//----- If level is between 0.009 and 0.09
			else if ((level >= minus40DB) && (level < minus20DB)){
				currentLevel = (minus40Mark-minus20Mark) * ((level - minus40DB) / (minus20DB - minus40DB));
				currentLevel = minus40Mark - currentLevel;
			}
			//----- If level is between 0.285 and 0.09
			else if ((level >= minus20DB) && (level < minus10DB)){
				currentLevel = (minus20Mark-minus10Mark) * ((level - minus20DB) / (minus10DB - minus20DB));
				currentLevel = minus20Mark - currentLevel;
			}
			//----- If level is between 0.285 and 0.639
			else if ((level >= minus10DB) && (level < minus3DB)){
				currentLevel = (minus10Mark-minus3Mark) * ((level - minus10DB) / (minus3DB - minus10DB)); 
				currentLevel = minus10Mark - currentLevel;
			}
			//----- If level is between 0.639 and 0.9
			else if ((level >= minus3DB) && (level < zeroDB)){
				currentLevel = (minus3Mark-zeroMark) * ((level - minus3DB) / (zeroDB - minus3DB));
				currentLevel = minus3Mark - currentLevel;
			}
			//----- If level is 0.9 or over
			else if (level >= zeroDB) {
				currentLevel = (zeroMark-plus3Mark) * ((level*10)-9);
				currentLevel = zeroMark - currentLevel;
			}

			/*----- We only need to repaint the level difference between this value and the previous.
			This is much more efficient than repainting the entire component each time. */
			diff = prevLevel - currentLevel;

			if (diff > 0) {
				paintFlag = 1; // to indicate that we are adding to the current meter level
				if ((type == 1) || (type == 2))
					repaint(xOffset, currentLevel, levelWidth, diff);
				else
					repaint((xOffset+levelWidth+1), currentLevel, levelWidth, diff);
			}
			else if (diff < 0) {
				diff *= -1;
				paintFlag = 0;	//to  indicate that we are subtracting from the current meter level
				if ((type == 1) || (type == 2))
					repaint(xOffset, prevLevel, levelWidth, diff);
				else
					repaint((xOffset+levelWidth+1), prevLevel, levelWidth, diff);
			}
			//else if diff = 0 then do nothing!

			//----- To see if clip marker should be shown.....
			if (clipFlag == 1) {
				if ((type == 1) || (type == 2))
					repaint (xOffset, top, levelWidth, clipZone);
				else
					repaint((xOffset+levelWidth+1), top, levelWidth, clipZone);
			}
			
			
			// Making the current level the previous level, for the next pass through....
			prevLevel = currentLevel;
		}


		//======== If mouse is clicked ================================================================
		void mouseDown (const MouseEvent& e)
		{
			//----- Getting (x,y) of the mouse click
			int x = e.getPosition().getX();
			int y = e.getPosition().getY();

			/*----- If the mouse is clicked over the VU, then the clip marker will be
			turned off. */
			if ((x >= getWidth()/5) && (x <= getWidth()*0.8) &&
				(y >= 0) && (y <= getHeight())) { 
					clipFlag = 0;
					/* Only need to repaint the clipzone. Stereo VU's just repaint both
					clip zones as it will not make much difference in terms of CPU usage.*/
					
					if (type == 1) //mono
						repaint (xOffset, top, levelWidth, clipZone);
					else	//stereo
						repaint(xOffset, top, (levelWidth*2) + 1, clipZone);
			}
		}

	private:
		bool style;
		int type, widthMeter, heightMeter;
		float level;
		int clipFlag, clipZone;
		int top, bottom, xOffset, yOffset, levelWidth, levelHeight;
		float plus3DB, zeroDB, minus3DB, minus10DB, minus20DB, minus40DB;
		int plus3Mark, zeroMark, minus3Mark, minus10Mark, minus20Mark, minus40Mark;
		Colour clClip;
		ColourGradient cg;
		Image img;

		int paintFlag;
		int currentLevel, prevLevel;
		int diff;

		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VUMeter);

	};
	//====== End of VUMeter class ===========================================


	//===== VU Component Constructor ========================================
	VUComponent (Array<int> conArr, bool useVertical)
	{
		totalNumLevels = 0;
		numArrElements = conArr.size();

		/*----- We need to get the total number of levels so that the index of each
		meter will be correct when setting the level. */
		for (int i=0; i<numArrElements; i++) {
			config.push_back (conArr[i]);
			totalNumLevels += conArr[i];
		}

		style = useVertical;		
	}

	//===== Destructor ====================================================
	~VUComponent()
	{
	}

	//===== Resize ========================================================
	void resized()
	{
		int arrElement = 0;
		int xOffset = 0;
		int widthMonoMeter = getWidth() / totalNumLevels;
		int widthStereoMeter = widthMonoMeter * 1.25;
		
		for (int i=0; i<totalNumLevels; i++) {
			//----- Adding a mono meter
			if (config[arrElement] == 1 ) {
				meters.add (new VUMeter(1, style));
				meters[i]->setBounds (xOffset, 0, widthMonoMeter, getHeight());
				addAndMakeVisible (meters[i]);
				xOffset += widthMonoMeter;
				arrElement++;
			}
			//----- Adding a stereo meter
			else if (config[arrElement] == 2 ) {
				//Left side
				meters.add (new VUMeter(2, style));
				meters[i]->setBounds (xOffset, 0, widthStereoMeter, getHeight());
				addAndMakeVisible (meters[i]);

				//Right side
				i++; //i needs to be incremented again to make sure that the meter index numbers are correct
				meters.add (new VUMeter(3, style));
				meters[i]->setBounds (xOffset, 0, widthStereoMeter, getHeight());
				addAndMakeVisible (meters[i]);

				xOffset += widthStereoMeter;
				arrElement++;
			}
			
		}
		
	}

	//===== Set VU Level Function =========================================
	void setVULevel (int VUIndex, float value)
	{
		meters[VUIndex]->setLevel (value);
	}

private:
	OwnedArray<VUMeter> meters;
	bool style;
	int widthMeter, heightMeter;
	int totalNumLevels;
	vector<int> config;
	int numArrElements;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VUComponent);
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
	
	Array<int> test;
	test.add(2);
	test.add(2);
	test.add(1);
	vuMeter = new VUComponent(test, true);
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