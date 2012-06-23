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
#include "CabbageLookAndFeel.h"
#include "CabbageUtils.h"
#include "CabbageTable.h"

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
CabbageButton(String name, String caption, String buttonText, String colour, String fontcolour)
{
	setName(name);
	offX=offY=offWidth=offHeight=0;
	groupbox = new GroupComponent(String("groupbox_")+name);
	button = new TextButton(name);
	button->setName(buttonText);
	addAndMakeVisible(groupbox);
	addAndMakeVisible(button);
	groupbox->setVisible(false);
	groupbox->getProperties().set("groupLine", var(1));
	button->setButtonText(buttonText);
	if(caption.length()>0){
		offX=10;
		offY=35;
		offWidth=-20;
		offHeight=-45;
		groupbox->setVisible(true);
		groupbox->setText(caption);
	}

	if(fontcolour.length()>0)
	button->getProperties().set("fontcolour", fontcolour);
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
String sliderType, compName, cl;
int resizeCount;
String tracker;

public:
ScopedPointer<GroupComponent> groupbox;
ScopedPointer<Slider> slider;
//---- constructor -----
CabbageSlider(String name, String text, String caption, String kind, String colour, String fontColour, int textBox, String trackerFill)
	: plantX(-99), plantY(-99), sliderType(kind), compName(caption), cl(colour), tracker(trackerFill)
{
	setName(name);
	offX=offY=offWidth=offHeight=0;
	groupbox = new GroupComponent(String("groupbox_")+name);
	slider = new Slider(text);
	slider->toFront(true);
	addAndMakeVisible(slider);
	addAndMakeVisible(groupbox);
	groupbox->setVisible(false);
	groupbox->getProperties().set("groupLine", var(1));
	if(tracker.length()>2)
	slider->getProperties().set("tracker", tracker);

	slider->getProperties().set("fontcolour", fontColour);
	
	if(textBox<1) 
		slider->setTextBoxStyle (Slider::NoTextBox, true, 0, 0);

	this->setWantsKeyboardFocus(false);

	resizeCount = 0;
}//--- end of constructor ----

//---------------------------------------------
~CabbageSlider()
{
}

//---------------------------------------------
void resized()
{
	//if rotary
	if (sliderType.contains("rotary")) {
		if(cl.length() > 0)
			slider->setColour(0x1001200, Colour::fromString(cl));
		slider->setSliderStyle(Slider::Rotary);
		getProperties().set("type", var("rslider"));
		slider->setSliderStyle(Slider::RotaryVerticalDrag);
		slider->setRotaryParameters(float_Pi * 1.2f, float_Pi * 2.8f, false);
		//if using group caption
		if (compName.length() > 0) {  
			groupbox->setBounds(0, 0, getWidth(), getHeight());
			int textHeight = 20;
			int availableHeight = getHeight()-textHeight;
			offY = textHeight + 5;
			int sliderHeight = availableHeight - 10;
			int sliderWidth = sliderHeight;
			offX = (getWidth()/2) - (sliderWidth/2);
			slider->setBounds(offX, offY, sliderWidth, sliderHeight); 
			groupbox->setText (compName);
			groupbox->setVisible(true);
			slider->toFront(true);
		}
		//else if no group caption then the slider takes the whole area available
		else 
			slider->setBounds(0, 0, getWidth(), getHeight());
	}
	//else if vertical
	else if (sliderType.contains("vertical")) {
		slider->setSliderStyle(Slider::LinearVertical);
		if(cl.length() > 0)
			slider->setColour(Slider::thumbColourId, Colour::fromString(cl));
		if (compName.length() > 0) {
			groupbox->setBounds(0, 0, getWidth(), getHeight());
			int textHeight = 20;
			int availableHeight = getHeight()-textHeight;
			offY = textHeight + 5;
			int sliderHeight = availableHeight - 10;
			int sliderWidth = getWidth() - 20;
			offX = (getWidth()/2) - (sliderWidth/2);
			slider->setBounds(offX, offY, sliderWidth, sliderHeight); 
			groupbox->setVisible(true);
			groupbox->setText(compName);
			slider->toFront(true);
		}
		else 
			slider->setBounds(0, 0, getWidth(), getHeight());
	}
	//else if horizontal
	else {
		slider->setSliderStyle(Slider::LinearHorizontal);
		if(cl.length() > 0)
			slider->setColour(Slider::thumbColourId, Colour::fromString(cl));
		if (compName.length() > 0) {
			groupbox->setBounds(0, 0, getWidth(), getHeight());
			int textHeight = 20;
			int availableHeight = getHeight()-textHeight;
			offY = textHeight + 5;
			int sliderHeight = availableHeight - 10;
			int sliderWidth = getWidth() - 20;
			offX = (getWidth()/2) - (sliderWidth/2);
			slider->setBounds(offX, offY, sliderWidth, sliderHeight); 
			groupbox->setText(compName);
			groupbox->setVisible(true);
			slider->toFront(true);
		}
		else 
			slider->setBounds(0, 0, getWidth(), getHeight());
	}

	//We only store the original dimensions the first time resized() is called.
	//Otherwise we would be passing incorrect values to the slider l+f methods...
	if (resizeCount == 0) {
		slider->getProperties().set(String("origHeight"), getHeight());
		slider->getProperties().set(String("origWidth"), getWidth());
		slider->getProperties().set(String("origX"), getX());
		slider->getProperties().set(String("origY"), getY());
	}
	resizeCount++;
	
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
CabbageCheckbox(String name, String caption, String buttonText, String colour, String fontcolour, bool isRect)
{
	setName(name);
	offX=offY=offWidth=offHeight=0;
	groupbox = new GroupComponent(String("groupbox_")+name);
	button = new ToggleButton(name);

	addAndMakeVisible(groupbox);
	addAndMakeVisible(button);
	groupbox->setVisible(false);
	groupbox->getProperties().set("groupLine", var(1));
	
	//outline colour ID
	groupbox->setColour(0x1005400,
		Colour::fromString(colour));
	//text colour ID
	groupbox->setColour(0x1005410,
		Colour::fromString(colour));

	button->setButtonText(buttonText);
	if(caption.length()>0){
		offX=10;
		offY=20;
		offWidth=-20;
		offHeight=-30;
		groupbox->setVisible(true);
		groupbox->setText(caption);
	}

	button->getProperties().set("isRect", isRect);
	button->getProperties().set("colour", colour);
	button->getProperties().set("fontcolour", fontcolour);
	//text colour id
	button->setColour(0x1006501,
			Colour::fromString(colour));
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
CabbageComboBox(String name, String caption, String text, String colour, String fontColour)
{
	setName(name);
	offX=offY=offWidth=offHeight=0;
	
	groupbox = new GroupComponent(String("groupbox_")+name);
	combo = new ComboBox(name);
	
	addAndMakeVisible(groupbox);
	addAndMakeVisible(combo);
	groupbox->setVisible(false);
	groupbox->getProperties().set("groupLine", var(1));
	combo->getProperties().set("colour", colour);
	combo->getProperties().set("fontcolour", fontColour);
	//combo->setColour(ComboBox::ColourIds::textColourId, Colour::fromString(fontColour));
	//outline colour IDE
	groupbox->setColour(0x1005400,
		Colour::fromString(colour));
	//text colour ID
	groupbox->setColour(0x1005410,
		Colour::fromString(colour));
	
	if(caption.length()>0){
		offX=10;
		offY=35;
		offWidth=-20;
		offHeight=-45;
		groupbox->setVisible(true);
		groupbox->setText(caption);
	}
	//text colour ID
	combo->setColour(0x1000a00,
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
class CabbageImage : public Component
{
public:
	CabbageImage(String name, String file, String outline, String fill, String shape, int line):
	Component(name), picFile(file), file(file), fill(fill), outline(outline), shape(shape), line(line){
		setName(name);
		//toBack();
		img = ImageCache::getFromFile (File (file));
		this->setWantsKeyboardFocus(false);
		//this->setInterceptsMouseClicks(false, true);
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
				g.setColour(Colour::fromString(outline));
				g.drawRoundedRectangle(0,0, width, height, width*.02, line);
				g.setColour(Colour::fromString(fill));
				g.fillRoundedRectangle(line,line, width-(line*2), height-(line*2), width*.02);				
			}
			else if(shape.contains("ellipse")){
				g.setColour(Colour::fromString(outline));
				g.drawEllipse(0,0, width, height, line);
				g.setColour(Colour::fromString(fill));
				g.fillEllipse(line,line, width-(line*2), height-(line*2));				
			}
			else if(shape.contains("sharp")){
				g.setColour(Colour::fromString(outline));
				g.drawRect(0,0, width, height, line);
				g.setColour(Colour::fromString(fill));
				g.fillRect(line,line, width-(line*2), height-(line*2));				
			}
		}

	}

	//void mouseDown(const MouseEvent &e){
	//	this->setInterceptsMouseClicks(false, true);
	//DialogWindow::showDialog("test", //title
	//								this, //comp
	//								this,
	//								Colours::aliceblue,
	//								true);
	//}



	void resized(){
	top = getY();
	left = getX();
	width = getWidth();
	height = getHeight();
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
CabbageGroupbox(String name, String caption, String text, String colour, String fontColour, int line):GroupComponent(name)
{
		toBack();
        offX=offY=offWidth=offHeight=0;
        if(colour.length()>0){
		//outline colour iD
        setColour(0x1005400,
                Colour::fromString(colour));
		//text colour iD
        setColour(0x1005410,
                Colour::fromString(colour));
        }
		this->getProperties().set("colour", colour);
		Logger::writeToLog("FonrColour:"+(fontColour));
		this->getProperties().set("fontcolour", fontColour);

        this->setText(text);
		this->setWantsKeyboardFocus(false);
		if(line==0)
		this->getProperties().set("groupLine", var(0));
		else
		this->getProperties().set("groupLine", var(1));
		this->repaint();

		setName(name);

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
		name << "|xycontroller:";
		//----- Texteditor to display the output values for x and y
		for (int i=0; i<2; i++) {
			textEditors.add(new TextEditor());
			textEditors[i]->setReadOnly (true);
			textEditors[i]->setColour(0x1000201, Colours::lime);	//text colour
			textEditors[i]->setColour(0x1000200, Colours::transparentBlack);	//background colour
			addAndMakeVisible(textEditors[i]);
		}

		//----- Slider
		xySpeed = new Slider ("speedXY");
		xySpeed->setSliderStyle (Slider::LinearHorizontal);
		xySpeed->setRange(0, 2, 0.01);
		xySpeed->setValue(1, true, false);
		xySpeed->setLookAndFeel (basicLookAndFeel);
		xySpeed->addListener (this);
		addAndMakeVisible (xySpeed);

		//----- ToggleButton
		autoToggle = new ToggleButton ("Off");
		autoToggle->setToggleState(false, true);
		autoToggle->getProperties().set("isRect", 1);
		autoToggle->getProperties().set("colour", Colours::lime.toString());
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
	 void setBallXY(float xIn, float yIn, bool Normal) 
        {
            if(Normal){ //if input is a normalised value
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

	//====== Drawing the background ========================================================
	void drawBackgroundImage()
	{
		//----- This function draws the background onto a blank image and then loads it into cache. The 
		//cached image is then reused in the paint() method. This is a more efficient way to redrawing something
		//that is static. 

		// Creating a blank canvas
		img = Image::Image(Image::ARGB, totalWidth, totalHeight, true);
			
		Graphics g (img);

		//----- For drawing the background
		Colour bg = CabbageUtils::getComponentSkin();
		g.setColour (bg);
		g.fillRoundedRectangle (0, 0, totalWidth, totalHeight, 5);

		//----- For drawing the actual area that the ball can move in
		Colour cl = CabbageUtils::getBackgroundSkin();
		Colour cl2 = cl.withBrightness (0.15);
		ColourGradient cg = ColourGradient (cl2, totalWidth*0.5, (availableHeight*0.5) + borderTop, 
											cl, 0, 0, true);
		g.setGradientFill (cg);
		g.fillRoundedRectangle (borderLeft, borderTop, availableWidth, availableHeight, 5);

		//----- Drawing the centre grid lines
		Colour cl3 = CabbageUtils::getComponentSkin();
		ColourGradient cg2 = ColourGradient (cl3, totalWidth*0.5, (availableHeight*0.5) + borderTop, 
											cl, 0, 0, true);
		g.setGradientFill (cg2);
		g.drawLine (borderLeft, (availableHeight*0.5) + borderTop, borderRight, (availableHeight*0.5) + borderTop, 1);
		g.drawLine ((availableWidth*0.5) + borderLeft, borderTop, (availableWidth*0.5) + borderLeft, borderBottom, 1);

		//----- For drawing the title
		Colour txt = CabbageUtils::getComponentSkin();
		g.setColour (txt);
		Font font = CabbageUtils::getComponentFont();
		g.setFont (font);
		Justification just(1);
		title = CabbageUtils::cabbageString (title, font, availableWidth-10);
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
		g.setOpacity (0.4);

		float thickness = 0.4; //thickness of lines
		int numSlots = 4;
		for (int i=1; i<numSlots; i++) {
			g.drawLine (((availableWidth/numSlots)*i) + borderLeft, borderTop, 
				((availableWidth/numSlots)*i) + borderLeft, borderBottom, thickness);//vertical lines
			g.drawLine (borderLeft, ((availableHeight/numSlots)*i) + borderTop, borderRight, 
				((availableHeight/numSlots)*i) + borderTop, thickness);//horizontal lines
		}
  
		//----- Switch statement which determines what type of ball to draw. If the mouse is up the ball is 
		//hollow with grey cross lines running through it.  If the mouse is down then the ball is filled and 
		//the cross lines turn green. 
		switch (buttonDown)
		{
		case 0: //button up
		//	g.setOpacity (0.9);
		//	g.drawLine (x + (ballSize/2), borderTop, x + (ballSize/2), borderBottom, 1); 
		//	g.drawLine (borderLeft, y + (ballSize/2), borderRight, y + (ballSize/2), 1);
			g.setColour (Colours::lime);
			g.drawEllipse (x+1, y+1, ballSize-2, ballSize-2, 2); //width of 2
			break;
		case 1: //button down
			g.setGradientFill (cgGreen); //changing colour of lines
			g.drawLine (x + (ballSize/2) - 0.5, borderTop, x + (ballSize/2) - 0.5, borderBottom, 2); 
			g.drawLine (borderLeft, y + (ballSize/2) - 0.5, borderRight, y + (ballSize/2) - 0.5, 1);
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
			autoToggle->setButtonText ("On");
		else if (autoToggle->getToggleState() == false) {
			autoToggle->setButtonText ("Off");
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
	//outline colour ID
	groupbox->setColour(0x1005400,
		Colours::findColourForName("white", Colours::white));
	//outline text ID
	groupbox->setColour(0x1005410,
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
	//background colour ID
	editor->setColour(0x1000200, Colours::black);
	//text colour ID
	editor->setColour(0x1000201, Colours::green);
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

			//----- Declarations for each db value. The range will go logarithmically from -infinity to +3db. 
			//A level of 0.9 is being assigned to 0db. 
			plus3DB = 1;
			zeroDB = 0.9;
			minus3DB = 0.71 * zeroDB; //0.639
			minus10DB = 0.3162 * zeroDB; //0.285
			minus20DB = 0.1 * zeroDB; //0.09
			minus40DB = 0.01 * zeroDB; //0.009
		}

		//====================================================================================
		~VUMeter(){}

		//========== Resizing method =========================================================
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

			//----- Calling function to draw image background. This only applies if the VU is mono or the 
			//left channel of a stereo meter. Right channel meters will not use a background as they can just
			//draw over the left channel background image. 
			if ((type == 1) || (type == 2))
					verticalBackground();
		}


		//===== Vertical Background =======================================================================
		void verticalBackground ()
		{
			//----- This function draws the background onto a blank image and then loads it into cache. The 
			//cached image is then reused in the paint() method. This is a more efficient way to redrawing something
			//that is static. 

			// Creating a blank canvas
			img = Image::Image(Image::ARGB, getWidth(), getHeight(), true);
			
			Graphics g (img);
			Colour bg = Colour::fromRGBA (10, 10, 15, 255);
			g.fillRoundedRectangle (0, 0, getWidth(), getHeight(), 2);
		
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
			g.drawText ("+3", startText, plus3Mark-5, getWidth()*.4, 10, just, false);
			g.drawText ("0", startText, zeroMark-5, getWidth()*.4, 10, just, false);
			g.drawText ("-3", startText, minus3Mark-5, getWidth()*.4, 10, just, false);
			g.drawText ("-10", startText, minus10Mark-5, getWidth()*.4, 10, just, false);
			g.drawText ("-20", startText, minus20Mark-5, getWidth()*.4, 10, just, false);
			g.drawText ("-40", startText, minus40Mark-5, getWidth()*.4, 10, just, false);

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
			

			//----- Drawing the meter level. When paintFlag is 1 the meter level is to be increased. This
			//new bit of the level is painted using the gradient fill cg. If paintFlag is 0, the level is 
			//to be decreased. Because there is no colour or drawing tool used for paintFlag=0, this bit of 
			//the level is cleared. 
			if (paintFlag == 1) {
				g.setGradientFill (cg);
				g.setOpacity (0.7);
				if ((type == 1) || (type == 2))
					g.fillRect (xOffset, currentLevel, levelWidth, diff);
				else
					g.fillRect ((xOffset+levelWidth+1), currentLevel, levelWidth, diff);
			}
			

			//----- Determining if the clipmarker should be shown. It is set back to 0 immediately as it
			//does not need to repainted over and over. If the clipFlag is 0, the marker will stay on if 
			//the repaint() bounds are not inclusive of the clipZone. When the user clicks on the VU meter
			//the clipFlag will be 0 and the repaint() bounds will include the clipZone (see mouseDown()), 
			//therefore turning off the clipmarker. This happens because there is no colour or draw function 
			//used for clipFlag = 0, and therefore this zone is cleared exposing the background again. 
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

			//----- The following if statements determine the offset on the y axis. Each zone 
			//itself has to be treated independently to the rest of the level range.
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

			//----- We only need to repaint the level difference between this value and the previous.
			//This is much more efficient than repainting the entire component each time. 
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

			//----- If the mouse is clicked over the VU, then the clip marker will be
			//turned off. 
			if ((x >= getWidth()/5) && (x <= getWidth()*0.8) &&
				(y >= 0) && (y <= getHeight())) { 
					clipFlag = 0;
					// Only need to repaint the clipzone. Stereo VU's just repaint both
					//clip zones as it will not make much difference in terms of CPU usage.
					
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
		style = useVertical;
		numArrElements = 0;
		totalNumLevels = 0;
		numMeters = 0;

		for (int i=0; i<conArr.size(); i++) {
			//Ignoring anything that is not either 1 or 2...
			if ((conArr[i] == 1) || (conArr[i] == 2)) {
				config.add (conArr[i]);
				numArrElements++;
				totalNumLevels += conArr[i]; //needed for indexing
				//numMeters allows us to get the correct width of each meter, as stereo
				//meters are 1.25 times as wide as mono ones...
				if (conArr[i] == 1)
					numMeters += 1;
				else
					numMeters += 1.25;
			}
		}	
	}

	//===== Destructor ====================================================
	~VUComponent()
	{
	}

	//===== Resize ========================================================
	void resized()
	{
		float gap = getWidth() * 0.07; //gap between meters
		float totalGap = gap * (numArrElements-1);	//all gaps added up
		float availableWidth = getWidth() - totalGap;
		float widthMonoMeter = availableWidth / numMeters;
		float widthStereoMeter;
		
		//If there is only 1 element in the array it should take up the whole width
		if ((numArrElements == 1) && (totalNumLevels ==2))
			widthStereoMeter = availableWidth;
		else
			widthStereoMeter = widthMonoMeter * 1.25;

		//----- The following for loop adds each level with a unique index. xOffset is the 
		//x coordinate at which they are drawn. 
		int arrElement = 0;
		float xOffset = 0;
		for (int i=0; i<totalNumLevels; i++) {
			//----- Adding a mono meter
			if (config[arrElement] == 1 ) {
				meters.add (new VUMeter(1, style));
				meters[i]->setBounds (xOffset, 0, widthMonoMeter, getHeight());
				addAndMakeVisible (meters[i]);
				xOffset += widthMonoMeter;
				if (arrElement != numArrElements) //if not the last element then add a gap
					xOffset += gap;
				arrElement++;
			}
			//----- Adding a stereo meter
			else if (config[arrElement] == 2 ) {
				//Left side
				meters.add (new VUMeter(2, style));
				meters[i]->setBounds (xOffset, 0, widthStereoMeter, getHeight());
				addAndMakeVisible (meters[i]);

				//Right side
				i++; //i needs to be incremented again to make sure that the index numbers are correct
				meters.add (new VUMeter(3, style));
				meters[i]->setBounds (xOffset, 0, widthStereoMeter, getHeight());
				addAndMakeVisible (meters[i]);

				xOffset += widthStereoMeter;
				if (arrElement != numArrElements) //if not the last element then add a gap
					xOffset += gap;
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
	float numMeters;
	Array<int> config;
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
CabbageVUMeter(String name, String text, String caption, Array<int> config):
numMeters(0)
{
	setName(name);
	offX=offY=offWidth=offHeight=0;
	
	groupbox = new GroupComponent(String("groupbox_")+name);
	numMeters = 0;
	for(int i =0;i<config.size();i++)
		numMeters = numMeters+config[i];

	vuMeter = new VUComponent(config, true);
	addAndMakeVisible(vuMeter);
	addAndMakeVisible(groupbox);

	groupbox->setVisible(false);
	//outline colour ID
	groupbox->setColour(0x1005400,
		Colours::findColourForName("white", Colours::white));
	//text colour ID
	groupbox->setColour(0x1005410,
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

//==============================================================================
// custom CabbageTable, uses the Table class
//==============================================================================

class CabbageTable : public Component
{
//ScopedPointer<LookAndFeel> lookFeel;
int offX, offY, offWidth, offHeight, tableSize;
public:
ScopedPointer<GroupComponent> groupbox;
ScopedPointer<CabbageTableViewer> table;
//---- constructor -----
CabbageTable(String name, String text, String caption, int tblSize): tableSize(tblSize)
{
	setName(name);
	offX=offY=offWidth=offHeight=0;
	
	groupbox = new GroupComponent(String("groupbox_")+name);
	groupbox->setWantsKeyboardFocus(false);
	table = new CabbageTableViewer();
	//table->setBounds(0, 0, 300, 200);
	
	addAndMakeVisible(table);
	addAndMakeVisible(groupbox);

	groupbox->setVisible(false);
	//outline colour ID
	groupbox->setColour(0x1005400,
		Colours::findColourForName("white", Colours::white));
	//text colour ID
	groupbox->setColour(0x1005410,
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
~CabbageTable(){

}


//---------------------------------------------
void fillTable(int ID, Array<float> yValues){
	table->fillTable(ID, yValues);
}


//---------------------------------------------
void resized()
{
groupbox->setBounds(0, 0, getWidth(), getHeight()); 
table->setBounds(offX, offY, getWidth()+offWidth, getHeight()+offHeight); 
table->addTable("table0", tableSize, Colours::lime);
this->setWantsKeyboardFocus(false);
}

JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CabbageTable);
};

//==============================================================================
// custom SnapshotControl, used for saving and recalling presets
//==============================================================================
class CabbageSnapshot : public Component,
						public Button::Listener,
						public ActionBroadcaster,
						public ComboBox::Listener
{

ScopedPointer<TextButton> button;
ScopedPointer<GroupComponent> groupbox;
String name, preset, presetFileText;
int offX, offY, offWidth, offHeight;
bool masterSnapshot;

public:
ScopedPointer<ComboBox> combobox;
		
	CabbageSnapshot(String compName, String caption, String presetName, bool master):masterSnapshot(master), name(compName), preset(presetName){
	name << "|snapshot:";
	preset << "|snapshot:";
	setName(name);
	offX=offY=offWidth=offHeight=0;
	
	groupbox = new GroupComponent(String("groupbox_")+name);
	combobox = new ComboBox(name);
	combobox->addListener(this);
	button = new TextButton("Snapshot");
	button->addButtonListener(this);
	


	addAndMakeVisible(combobox);
	addAndMakeVisible(groupbox);
	addAndMakeVisible(button);

	groupbox->setVisible(false);
	//outline colour ID
	groupbox->setColour(0x1005400,
		Colours::findColourForName("white", Colours::white));
	//text colour ID
	groupbox->setColour(0x1005410,
		Colours::findColourForName("white", Colours::white));
	
	if(caption.length()>0){
		offX=10;
		offY=15;
		offWidth=-20;
		offHeight=-25;
		groupbox->setVisible(true);
		groupbox->setText(caption);
	}
	}
	~CabbageSnapshot(){}

	void buttonClicked(Button* button){
	//CabbageUtils::showMessage(name+"save;"+String(combobox->getText().trim()));
		sendActionMessage(preset+"save;"+String(combobox->getText().trim())+String("?")+String(masterSnapshot));
	}

	void comboBoxChanged(ComboBox* combo){
		sendActionMessage(preset+"load;"+String(combobox->getText().trim())+String("?")+String(masterSnapshot));
	}
	//---------------------------------------------
	void resized()
	{
	groupbox->setBounds(0, 0, getWidth(), getHeight()); 
	combobox->setBounds(offX, offY+2, (getWidth()+offWidth)*.60, (getHeight()+offHeight)*.8); 
	button->setBounds(offX+(getWidth()+offWidth)*.60+5, offY, (getWidth()+offWidth)*.35, getHeight()+offHeight);
	}

};
//==============================================================================
// custom NumToggleButton
//==============================================================================
class CabbageNumToggle : public Component,
						 public ActionBroadcaster
{
String name;
Colour colour;
ScopedPointer<Label> label;
int width, height, value, lastValue;
bool buttonState, mouseButtonState;
public:
	  CabbageNumToggle(String name, int width, int height): 
					width(width),
					height(height),
					value(0),
					lastValue(0),
					buttonState(false),
					mouseButtonState(false),
					colour(Colours::lime)
	  {
		setBounds(0, 0, width, height);
		label = new Label();
		label->setText("", false);
		label->getProperties().set("textColour", "black");
		label->setBounds(0, 0, width, height);
		label->setEditable(false, true);
		label->setInterceptsMouseClicks(false, true);
		addAndMakeVisible(label);
		label->toFront(true);  
	  }

	  void mouseDrag (const MouseEvent &e){
		  if(e.mods.isRightButtonDown()&&buttonState)
		  if(e.getOffsetFromDragStart().getY()<0){
			  label->setText(String(value+(e.getDistanceFromDragStart()/2)), false);
			  lastValue = e.getDistanceFromDragStart()/2;
			  sendActionMessage(String("button:")+getName()+String("|state:")+String(buttonState)+String("|value:")+label->getText());
		  }
		  else{
			  label->setText(String(value-(e.getDistanceFromDragStart()/2)), false);
			  lastValue = value-(e.getDistanceFromDragStart()/2);
			  //send a message with the button number and its value
			  sendActionMessage(String("button:")+getName()+String("|state:")+String(buttonState)+String("|value:")+label->getText());
		  }

	  }

	  void mouseExit (const MouseEvent &e){
		if(e.mods.isRightButtonDown())	  
			value = lastValue;
	  }

	void mouseDown(const MouseEvent &e){
		if(e.mods.isLeftButtonDown())
		if(getToggleState()){
			buttonState=false;
			repaint();
			//send a message with the button number and its state
			  sendActionMessage(String("button:")+getName()+String("|state:")+String(buttonState)+String("|value:")+label->getText());
		}
		else{
			buttonState=true;
			repaint();
			  sendActionMessage(String("button:")+getName()+String("|state:")+String(buttonState)+String("|value:")+label->getText());
		}
	}

	bool getToggleState(){
		return buttonState;
	}

	void setToggleState(bool val){
		buttonState = val;
	}

	int getCurrentValue(){
		  return label->getText().getIntValue();
	  }

	void setActiveColour(String inColour){
		  colour = Colours::findColourForName(inColour, Colours::lime);
	  }

	void paint(Graphics &g){
		Image newButton = CabbageLookAndFeel::drawToggleImage (width, height, buttonState, colour, true);
		g.drawImage(newButton, 0, 0, width, height, 0, 0, width, height);
	}

};

//==============================================================================
// custom PatternMatrix
//==============================================================================
class PatternMatrix : public Component, 
					  public Timer,
					  public ButtonListener,
					  public SliderListener,
					  public ActionListener
{
ScopedPointer<CabbageCheckbox> onoffButton;
ScopedPointer<Slider> bpm;
OwnedArray<Slider> pSliders;
float buttonIndex, updateVar, offX, offY, offWidth, offHeight, 
noPatterns, noSteps, beat, currentStepButton, rCtrls;
bool timerActive;

public:
OwnedArray<CabbageNumToggle> stepButton;
CabbagePluginAudioProcessor* myFilter;
//---- constructor -----
	PatternMatrix(String caption, int width, int height, StringArray patterns, int steps, int rctrls, CabbagePluginAudioProcessor* filter)
							: timerActive(false), noSteps(steps), 
								noPatterns(patterns.size()), beat(0), myFilter(filter),
								currentStepButton(0),
								buttonIndex(0),
								updateVar(0),
								rCtrls(rctrls)
	{
		onoffButton = new CabbageCheckbox("Active", "", "", Colours::red.toString(), CabbageUtils::getComponentFontColour().toString(), true);

		onoffButton->button->setButtonText("");
		onoffButton->button->addListener(this);
		//onoffButton->getProperties().set(Colours, "red");
		onoffButton->setBounds(60, 25, 20, 20);

		if(myFilter->patMatrixActive)
			onoffButton->button->setToggleState(true, true);

		addAndMakeVisible(onoffButton);

		bpm = new Slider("bpm");
		bpm->setSliderStyle(Slider::LinearBar);
		bpm->setBounds(185, 27, 180, 15);
		bpm->setRange(0, 1000, 1);
		bpm->setValue(120);
		bpm->addListener(this);
		addAndMakeVisible(bpm);

		myFilter->noSteps = noSteps;
		myFilter->noPatterns = noPatterns;
		myFilter->patternNames = patterns;
		Rectangle<int> pattRect;
		//set bounds for pattern rectangle
		pattRect.setBounds(60, 50, width-(rCtrls*40)-60, (height-50)*.95);	
		//set bounds for slider rectangle
		Rectangle<int> slidersRect;
		slidersRect.setBounds(pattRect.getWidth()+60, 50, width-pattRect.getWidth()+60, (height-50)*.95); 

		//populate matrix with step buttons
		if(myFilter->patStepMatrix.size()==0)
		{
		for(int pats=0;pats<patterns.size();pats++)
			for(int beats=0;beats<steps;beats++){
				stepButton.add(new CabbageNumToggle("", (pattRect.getWidth()/noSteps), (pattRect.getHeight()/patterns.size())));
				stepButton[stepButton.size()-1]->addActionListener(this);
				stepButton[stepButton.size()-1]->setName(String(stepButton.size()-1));
//				stepButton[stepButton.size()-1]->setToggleState(myFilter->patStepMatrix[stepButton.size()-1].state);			
				CabbagePatternMatrixStepData patMat;
				myFilter->patStepMatrix.add(patMat);
				stepButton[stepButton.size()-1]->setTopLeftPosition(pattRect.getX()+(beats*(pattRect.getWidth()/noSteps)), pats*(pattRect.getHeight())/patterns.size()+pattRect.getY());
				addAndMakeVisible(stepButton[stepButton.size()-1]);
			}
		}
		else{
		for(int pats=0;pats<patterns.size();pats++)
			for(int beats=0;beats<steps;beats++){
				stepButton.add(new CabbageNumToggle("", (pattRect.getWidth()/noSteps), (pattRect.getHeight()/patterns.size())));
				stepButton[stepButton.size()-1]->addActionListener(this);
				stepButton[stepButton.size()-1]->setName(String(stepButton.size()-1));
				stepButton[stepButton.size()-1]->setToggleState(myFilter->patStepMatrix[stepButton.size()-1].state);			
				//CabbagePatternMatrixStepData patMat;
				//myFilter->patStepMatrix.add(patMat);
				stepButton[stepButton.size()-1]->setTopLeftPosition(pattRect.getX()+(beats*(pattRect.getWidth()/noSteps)), pats*(pattRect.getHeight())/patterns.size()+pattRect.getY());
				addAndMakeVisible(stepButton[stepButton.size()-1]);
			}
		}
		//add p-field controls if needed
	   for(int pats=0;pats<patterns.size();pats++){
			for(int i=0;i<rCtrls;i++){
				pSliders.add(new Slider());
				pSliders[pSliders.size()-1]->setSliderStyle(Slider::RotaryVerticalDrag);	
				pSliders[pSliders.size()-1]->addListener(this);
				pSliders[pSliders.size()-1]->setRange(0, 127, 1);
				pSliders[pSliders.size()-1]->setTextBoxIsEditable(true);
				pSliders[pSliders.size()-1]->showTextBox();
				String sliderName("row:"+String(pats)+"|ctrl:"+String(i));
				CabbagePatternMatrixPfieldData patMat;
				myFilter->patPfieldMatrix.add(patMat);
				pSliders[pSliders.size()-1]->getProperties().set("data", var(sliderName));
				pSliders[pSliders.size()-1]->setBounds(slidersRect.getX()+(i*40), (((pats*(slidersRect.getHeight())/patterns.size())))+slidersRect.getY(), 40, 40);
				addAndMakeVisible(pSliders[pSliders.size()-1]);
				}
			}
			
		}

	~PatternMatrix(){
		}


	void actionListenerCallback(const juce::String &value)
		{
			//button:1|state:1|value:127
			//sort out buttons first
			if(value.contains("button")){
			String buttonNum(value.substring(value.indexOf("button:")+7, value.indexOf("|st")));
			String buttonState(value.substring(value.indexOf("|state:")+7, value.indexOf("|val")));
			String buttonValue(value.substring(value.indexOf("|value:")+7, value.length()));
			if(buttonState.getIntValue()==1){
				Logger::writeToLog(buttonNum);
				myFilter->patStepMatrix.getReference(buttonNum.getIntValue()).state = buttonState.getIntValue();
				myFilter->patStepMatrix.getReference(buttonNum.getIntValue()).p4 = buttonValue.getIntValue();
			}
			else
				myFilter->patStepMatrix.getReference(buttonNum.getIntValue()).state = buttonState.getIntValue();
			}
			//and now sliders
		}
		
	void sliderValueChanged(juce::Slider *slider){
		String data = slider->getProperties().getWithDefault("data", String("").contains("row"));
		if(data.contains("row")){
		String sliderRow(data.substring(data.indexOf("row:")+4, data.indexOf("|ctr")));
		String sliderCtrl(data.substring(data.indexOf("|ctrl:")+6, data.length()));		
		if(sliderCtrl=="0")
		myFilter->patPfieldMatrix.getReference(sliderRow.getIntValue()).p5 = slider->getValue();
		else if(sliderCtrl=="1")
		myFilter->patPfieldMatrix.getReference(sliderRow.getIntValue()).p6 = slider->getValue();
		else if(sliderCtrl=="2")
		myFilter->patPfieldMatrix.getReference(sliderRow.getIntValue()).p7 = slider->getValue();
		else if(sliderCtrl=="3")
		myFilter->patPfieldMatrix.getReference(sliderRow.getIntValue()).p8 = slider->getValue();
		else if(sliderCtrl=="4")
		myFilter->patPfieldMatrix.getReference(sliderRow.getIntValue()).p9 = slider->getValue();
		}

	}

	void sliderDragEnded(Slider* slider)
		{
			if(slider->getName()=="bpm"){
			myFilter->bpm = slider->getValue();
			myFilter->timeCounter = 0;
			this->startTimer(15);
			}
		}

	void buttonClicked(Button *button)
		{
			if(button->getName()=="Active"){
				if(button->getToggleState()){
				this->startTimer(15);
				myFilter->patMatrixActive=1;
				}
				else{
				this->stopTimer();
				timerActive = false;
				myFilter->patMatrixActive=0;
				}
			}
		}

	void timerCallback(){
			for(int u=0;u<stepButton.size();u++){
			if(stepButton[u]->getToggleState()==1){
			stepButton[u]->setActiveColour("lime");
			stepButton[u]->repaint();
			}
			}
	
			for(int y=0;y<noPatterns;y++){
			Logger::writeToLog(String(myFilter->beat+(y*noSteps)));
			if(stepButton[myFilter->beat+(y*noSteps)]->getToggleState()==1){			
			stepButton[myFilter->beat+(y*noSteps)]->setActiveColour("yellow");
			stepButton[myFilter->beat+(y*noSteps)]->repaint();
			}
			}

	}
JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PatternMatrix);
};

//==============================================================================
// CabbagePatternMatrix
//==============================================================================
class CabbagePatternMatrix : public Component
{
int offX, offY, offWidth, offHeight, tableSize, width, height, rCtrls;
String caption;
StringArray patterns;
public:
ScopedPointer<PatternMatrix> patternMatrix;
CabbagePluginAudioProcessor* myFilter;
//---- constructor -----
CabbagePatternMatrix(CabbagePluginAudioProcessor* filter, String name, int width, int height, String caption, StringArray patterns, int steps, int rctrls)
	: myFilter(filter), height(height), width(width), caption(caption), patterns(patterns), rCtrls(rctrls)
{
	setName(name);
	offX=offY=offWidth=offHeight=0;

	
	patternMatrix = new PatternMatrix(caption, width, height, patterns, steps, rCtrls, myFilter);

	addAndMakeVisible(patternMatrix);
	this->setWantsKeyboardFocus(false);
}
//---------------------------------------------
~CabbagePatternMatrix(){

}

void paint(Graphics& g){
	//----- Background
	Colour bg = Colours::white;
	g.setColour (bg);
	g.fillRoundedRectangle (0, 0, width, height, 5);

	//----- Outline
	g.setColour (CabbageUtils::getComponentFontColour());
	g.setOpacity (0.1);
	g.drawRoundedRectangle (0.5, 0.5, width-1, height-1, 5, 1);

	Font font (String("Impact"), 14, 0);
	font.setFallbackFontName (String("Verdana")); //in case the user doesn't have the first font installed
	g.setFont (font);
	Justification just (4);
	g.setColour (Colours::black);
	String name = CabbageUtils::cabbageString (caption, font, width);
	g.drawText (name, 0, 5, width, 13, just, false);
	g.drawLine (10, 20, width-10, 20, 0.3);

	for(int i=0;i<patterns.size();i++){
	Justification just (4);
	g.setColour (Colours::black);
	String name = CabbageUtils::cabbageString (patterns[i], font, 60);
	
	g.drawText(name, 0, (i*((height-50)/patterns.size()*.90))+50, 60, (height/patterns.size())*.90, just, false);
	}

	g.drawText("Active", 70, 25, 60, 20, just, false);
	g.drawText("BPM", 140, 25, 60, 20, just, false);


}

//---------------------------------------------
void resized()
{
patternMatrix->setBounds(offX, offY, getWidth()+offWidth, getHeight()+offHeight); 
this->setWantsKeyboardFocus(false);
}

JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CabbagePatternMatrix);
};

//==============================================================================
// custom CabbageLabel
//==============================================================================
class CabbageLabel	:	public Component
{

public:
	CabbageLabel (String text, String colour)
		: text(text), colour(colour)
	{
	}

	~CabbageLabel()
	{
	}

	void resized()
	{

	}

	void paint(Graphics& g)
	{
		g.setColour(Colour::fromString(colour));
		g.setFont(CabbageUtils::getComponentFont());
		g.setFont(getHeight(), 1);
		g.drawFittedText(text, 0, 0, getWidth(), getHeight(), Justification::left, 1, 1);
	}


private:
	String text, colour;
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CabbageLabel);
};


//==============================================================================
// custom CabbageLine
//==============================================================================
class CabbageLine	:	public Component
{
Colour col;
public:
	CabbageLine (bool isHorizontal, String colour)
		: isHorizontal(isHorizontal)
	{
		col = Colour::fromString(colour);
	}

	~CabbageLine()
	{
	}

	void resized()
	{
		if (isHorizontal == true)
			this->setBounds(getX(), getY(), getWidth(), getHeight());
		else
			this->setBounds(getX(), getY(), getWidth(), getHeight());	

		this->setAlpha(0.7);
	}

	void paint(Graphics& g)
	{
		g.setColour (col);
		g.fillRoundedRectangle(0, 0, getWidth(), getHeight(), 1);

		g.setColour (CabbageUtils::getBackgroundSkin());
		g.fillRoundedRectangle (0, 0, getWidth()-1, getHeight()-1, 1);
	}


private:
	bool isHorizontal;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CabbageLine);
};

#endif