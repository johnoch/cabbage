#include "CabbageLookAndFeel.h"

//========== Constructor ================================================================================
CabbageLookAndFeel::CabbageLookAndFeel()
{
}


//========= Destructor ====================================================================================
CabbageLookAndFeel::~CabbageLookAndFeel()
{
}


//========= Creating a custom r slider image that can be usede in the drawRotary() method =================
Image CabbageLookAndFeel::drawRotaryImage(int diameter, const Colour circleFill, float sliderPosProportional)
{
	//This is where the rotary slider image gets drawn.  The diameter width is passed in to draw the exact size
	//slider where no scaling is needed.  A simpler slider is used if the diameter is 25 or less.

	//----- Creating a blank canvas with the same width as the slider.
	Image img = Image::Image(Image::ARGB, diameter, diameter, true);
	Graphics g (img);

	//----- Calculating number of radians for 300 degrees.  This is because the slider goes from -150 to 150 degrees 
	//where 12 o'clock is zero.
	float numRadians = (300*3.14) / 180; 
	AffineTransform tnsForm = AffineTransform::identity; //this means no transform, or identical

	//----- Outer grey circle and green fill.  They only get drawn if the diameter is greater than
	//25 pixels.
	if (diameter > 25) {
		g.setColour (Colours::grey);
		g.setOpacity (0.5);
		g.drawEllipse (0.5, 0.5, diameter-1, diameter-1, 1);

		Path fill; //for the green fill
		fill.addArc (diameter*0.075, diameter*0.075, diameter*0.85, diameter*0.85, -2.6167, 
												(sliderPosProportional-0.5)*numRadians, true);
		PathStrokeType type (diameter*0.05);

		ColourGradient greenFill = ColourGradient (Colours::transparentBlack, 0, diameter*0.88, 
			Colours::lime, 0, diameter*0.7, false); //setting gradient
		g.setGradientFill (greenFill);
		g.setOpacity (0.9);
		g.strokePath (fill, type, tnsForm);
	}


	//----- Polygon.  Drawing a ploygon with 12 steps.  I'm using a colour gradient that goes from white 
	//to black from the top-left to the bottom-right.  This gives the illusion that a light source is shining 
	//on the slider.
	g.setColour (Colours::black); 
	g.setOpacity (0.4);
	g.fillEllipse (diameter*0.17, diameter*0.17, diameter*0.7, diameter*0.7); //for shadow

	Path newPolygon;
	Point<float> centre (diameter/2, diameter/2);
	
	if (diameter >= 40) { //If diameter is >= 40 then polygon has 12 steps
		newPolygon.addPolygon (centre, 12, diameter*0.35, 0);
		newPolygon.applyTransform (AffineTransform::rotation ((sliderPosProportional * numRadians), 
																		diameter/2, diameter/2));
	}
	else if ((diameter < 40) && (diameter > 25)) { //Polygon has 10 steps
		newPolygon.addPolygon (centre, 10, diameter*0.35, 0);
		newPolygon.applyTransform (AffineTransform::rotation ((sliderPosProportional * numRadians), 
																		diameter/2, diameter/2));
	}
	else //Else just use a circle. This is easier to see than a polygon when very small.
		newPolygon.addEllipse (diameter*0.05, diameter*0.05, diameter*0.9, diameter*0.9);

	ColourGradient cg = ColourGradient (Colours::white, 0, 0, Colours::black, diameter*0.8, diameter*0.8, false);
	g.setGradientFill (cg);
	g.fillPath (newPolygon, tnsForm);


	//----- Inner circle
	//Using different offsets and shades of black to give the illusion of a shadow...
	g.setColour (Colours::black);
	g.setOpacity (0.6);
	g.fillEllipse (diameter*0.185, diameter*0.185, diameter*0.65, diameter*0.65); 
	for (float i=0.09; i>0.0; i-=0.01) {
		g.setOpacity (i*2);
		g.fillEllipse (diameter * (0.19+i), diameter * (0.19+i), diameter*0.62, diameter*0.62);
	}

	//Using a colour gradient from white to the chosen colour gives the effect of a light source...
	ColourGradient circleGrad = ColourGradient (Colours::white, diameter*-0.2, diameter*-0.2,
											circleFill, diameter*0.5, diameter*0.5, false);
	g.setGradientFill (circleGrad);
	//filling inner circle
	if (diameter > 25)
		g.fillEllipse (diameter*0.19, diameter*0.19, diameter*0.62, diameter*0.62); 
	else
		g.fillEllipse (diameter*0.1, diameter*0.1, diameter*0.8, diameter*0.8);


	//----- Marker.  
	Path circleMarker;
	if (diameter > 25) //If diameter is greater than 25 use a rounded rectangle
		circleMarker.addRoundedRectangle (diameter*0.47, diameter*0.19, diameter*0.06, diameter*0.22, 
																		diameter*0.05, diameter*0.05);
	else //Otherwise use a normal rectangle
		circleMarker.addRectangle (diameter*0.47, diameter*0.1, diameter*0.06, diameter*0.3);

	circleMarker.applyTransform (AffineTransform::rotation (((sliderPosProportional-0.5) * numRadians), 
																		diameter/2, diameter/2));	
	//This is either black or white, depending on the brightness of the inner circle.
	if (circleFill.getBrightness() > 0.6)
		g.setColour (Colours::black);
	else
		g.setColour (Colours::whitesmoke);	
	g.fillPath (circleMarker, tnsForm);


	//Returning image...
	return img;
}


//=========== Rotary Sliders ==============================================================================
void CabbageLookAndFeel::drawRotarySlider(Graphics& g, int /*x*/, int /*y*/, int /*width*/, int /*height*/,
																	float sliderPosProportional,
																	float /*startAngle*/,
																	float /*endAngle*/,
																	Slider& slider)
{
	//----- Variables for determining new destination width and height etc...
	float destX, destY, destHeight, destWidth, sliderBottom;
	destX = destY = 0;
	sliderBottom = slider.getHeight();

	//----- Variables for slider name
	String name;
	name << slider.getName();
	Font fontName (T("Verdana"), 13, 1);
	Justification justName (36); //centered
	float nameWidth = fontName.getStringWidth(name);
	
	//----- Textbox variables
	String str;
	str << slider.getMaximum() << slider.getInterval();
	int len = str.length() * 7;

	//----- If NO textbox!!! ------------------------------------------------------------------------------
	if (slider.getTextBoxPosition() == Slider::NoTextBox) {
		slider.setTextBoxStyle (Slider::NoTextBox, true, 0, 0);

		//Name label goes at bottom
		if (slider.getName().length() > 0) {
			g.setColour (Colours::whitesmoke);
			g.setOpacity (0.5);
			g.setFont (fontName);
			g.drawText (name, (slider.getWidth()/2) - (nameWidth/2), slider.getHeight() - 14, 
															(int)nameWidth, 14, justName, false);
			sliderBottom -= 15; //gap for name label
		}
	}
	//----- If textbox --------------------------------------------------------------------------------------------
	else {
		slider.setTextBoxStyle (Slider::TextBoxBelow, true, len, 15);
		sliderBottom -= 15; //gap for textbox
		//Name label goes at top...
		if (slider.getName().length() > 0) {
			g.setColour (Colours::whitesmoke);
			g.setOpacity (0.5);
			g.setFont (fontName);
			g.drawText (name, (slider.getWidth()/2) - (nameWidth/2), 0, (int)nameWidth, 14, justName, false);
			destY += 15; //gap for name label
		}
	}
	
	//----- Getting the final destination dimensions....
	destHeight = sliderBottom - destY;
	destWidth = destHeight;							//because rotary slider images generally have the same width and height....
	destX = ((slider.getWidth() - destWidth) / 2);	//starting x position, this ensures that the image will be centered

	//----- Getting slider image and drawing it....
	Colour circleFill = slider.findColour(0x1001200, false);
	Image newSlider = drawRotaryImage(destWidth, circleFill, sliderPosProportional);
	g.setOpacity (1);
	g.drawImage (newSlider, destX, destY, destWidth, destHeight, 0, 0, destWidth, destWidth, false);

	

	/*----- If NO textbox and mouse is hovering or dragging, then draw the value across the slider.  This has to be done
	after the images as it must go on top of them. */
	if ((slider.getTextBoxPosition() == Slider::NoTextBox) && (slider.isMouseOverOrDragging() == true)) {
		//Getting number of decimal places to display...
		String interval;
		interval << slider.getInterval();
		int numDec = interval.length()-2; //taking away decimal point and number before
		if (numDec < 0)
			numDec = 0;

		//Setting up the format of the string....
		String format;
		format << "%." << numDec << "f";
		String sliderValue = String::formatted(format, slider.getValue());

		//Font etc...
		Font fontValue (T("Helvetica"), 13, 1);
		Justification just (36); //centered
		float strWidth = fontValue.getStringWidth(sliderValue);

		//Drawing background box
		float boxWidth = strWidth + 2;
		//g.setColour (circleFill);
		g.setColour (Colours::whitesmoke);
		g.fillRoundedRectangle ((slider.getWidth()/2) - (strWidth/2) - 1.0f, (destHeight/2)-7.0f, boxWidth, 13.0f, 4);
		
		//Drawing value
		//g.setColour (markerFill);
		g.setColour (Colours::black);
		g.setFont (fontValue);
		g.drawText (sliderValue, (slider.getWidth()/2) - (strWidth/2), (destHeight/2)-7.0f, (int)strWidth, 13, just, false);


		/*----- If the value box width is bigger than the slider width.  The slider size needs to be increased to cater for displaying 
		the full value.  This should only really apply to small sliders.  The parent of the slider is also increased by the same
		amount.  This means if the user is using a caption, then the slider group box will also increase. */
		if (destWidth < boxWidth) {
			//Getting difference in width between value box and slider width
			int diff = boxWidth - destWidth;

			//if there is a label at the bottom we have to add on the height of the label too...
			if (slider.getName().length() > 0)
				boxWidth += 15;

			//Getting parent's original bounds...
			Component* parent;
			parent = slider.getParentComponent();
			int parentWidth = parent->getWidth();
			int parentHeight = parent->getHeight();
			int parentX = parent->getX();
			int parentY = parent->getY();

			//setting to front so that it is not physically underneath another slider
			parent->toFront(true); 

			//Resetting parent bounds...
			int newParentX = parentX - (diff/2);
			int newParentY = parentY - (diff/2);
			int newParentWidth = parentWidth + diff;
			int newParentHeight = parentHeight + diff;
			parent->setBounds (newParentX, newParentY, newParentWidth, newParentHeight);

			/*----- Resizing slider bounds. Getting its x and y coordinates in relation to its parent, not the actual 
			background!!  Its top-left coordinate shouldn't change as its parent is expanding too. */
			int originalSliderX = slider.getX();
			int originalSliderY = slider.getY();
			slider.setBounds (originalSliderX, originalSliderY, boxWidth, boxWidth);

			/*-----Assigning a resize flag to the slider so that it can be checked in the else statement that follows.  
			Components have no ID to begin with.  This will prevent the wrong slider being picked up by the else statement 
			when they physically overlap. */
			String resizeFlag;
			resizeFlag << 1;
			slider.setComponentID (resizeFlag);
		}
	}
	
	/*----- Else if mouse is not hovering and slider ID is not empty.  This means that the slider had been
	resized previously, and will therefore enter this else statement to get resized again back to its original
	dimensions. */
	else if ((slider.isMouseOverOrDragging() == false) && (slider.getComponentID().compare ("") != 0)) {
		//Getting original parent dimensions....
		Component* parent;
		parent = slider.getParentComponent();
		int parentWidth = slider.getProperties().getWithDefault(String("origParentWidth"), -99);
		int parentHeight = slider.getProperties().getWithDefault(String("origParentHeight"), -99);
		int parentX = slider.getProperties().getWithDefault(String("origParentX"), -99);
		int parentY = slider.getProperties().getWithDefault(String("origParentY"), -99);
		//Resizing parent back to original size
		parent->setBounds (parentX, parentY, parentWidth, parentHeight);

		//Getting slider original dimensions....
		int originalWidth = slider.getProperties().getWithDefault(String("origWidth"), -99);
		int originalHeight = slider.getProperties().getWithDefault(String("origHeight"), -99);
		int originalX = slider.getProperties().getWithDefault(String("origX"), -99);
		int originalY = slider.getProperties().getWithDefault(String("origY"), -99);
		//Resizing back to original size
		slider.setBounds (originalX, originalY, originalWidth, originalHeight);	

		//assigning an empty ID back to the slider
		String emptyStr = String::empty;
		slider.setComponentID (emptyStr); 	
	}
}

				
//=========== Linear Slider Background ===========================================================================
void CabbageLookAndFeel::drawLinearSliderBackground (Graphics &g, int /*x*/, int /*y*/, int /*width*/, int /*height*/, float sliderPos, 
																								float /*minSliderPos*/, 
																								float /*maxSliderPos*/, 
																								const Slider::SliderStyle style, 
																								Slider &slider)
{
	/*----- Initialising slider to not being enabled.  It only becomes enabled when the mouse is clicked within
	the actual slider image.  This give a better mouseDrag response.  */
	slider.setEnabled (false);

	//----- Frames
	int frameWidth, frameHeight;
	const int numFrames = 75;
	Image newBackground;

	/*----- The following calculates the frame number from the position that the slider should be in. 
	slider.getMinimum() is subtracted in case that 0 is not the minimum value set. */
	const double div = (slider.getMaximum() - slider.getMinimum()) / numFrames;
	float frameToUse = (int)(((slider.getValue() - slider.getMinimum()) / div) + 0.5);
	if (frameToUse > 0) frameToUse -= 1;

	//----- Variables for determining new destination width, height, position etc...
	float destX, destY, sliderEnd, xOffset, yOffset;
	float destHeight = slider.getHeight(); 
	float destWidth = slider.getWidth();
	
	//----- Determining if the user wants a name label displayed......
	bool showName = false;
	String name;
	float nameWidth;
	Justification just (36); //centered
	if (slider.getName().length() > 0) {
		name << slider.getName();
		Font font (T("Verdana"), 13, 1);
		g.setFont (font);
		nameWidth = font.getStringWidth(name);
		g.setColour (Colours::whitesmoke);
		g.setOpacity (0.5);
		showName = true; //setting flag to true
	}

	/*----- If Horizontal Slider --------------------------------------------------------------------------*/
	if (style == Slider::LinearHorizontal) {
		//Variables for slider value / textbox
		String str;
		str << slider.getMaximum() << slider.getInterval();
		int len = str.length() * 7; //length of text

		//----- start and end of slider image, 7 is to make room for the edge of the slider thumb when at a maximum or minimum
		destX = 7; 
		sliderEnd = slider.getWidth() - (len+7); 
		//----- starting y position
		destY = 0;//(slider.getHeight()/2) - (destHeight/2);

		/*If there is a name label it will be shown at the start while the value will be at the end when 
		the mouse hovers over the slider. */
		Justification justLeft (1); //left
		if (showName == true) { //if the name should be displayed
			g.drawText (name, 0, (slider.getHeight()/2) - 6, (int)nameWidth, 11, justLeft, false);
			destX += nameWidth+5; //adjusting destX to accomodate slider name
		}

		/*----- If textbox IS NOT to be displayed!!!!!! */
		if (slider.getTextBoxPosition() == Slider::NoTextBox) 
			slider.setTextBoxStyle (Slider::NoTextBox, true, 0, 0);

		/*----- If textbox IS to be displayed!!!!  */
		else {
			slider.setTextBoxStyle (Slider::TextBoxRight, false, len, 15);
			sliderEnd -= 15; //gap of 15 between slider and textbox
		}

		//Getting the new destination width of image
		destWidth = sliderEnd - destX;

		//----- Slider is enabled and value changed if mouse click is within the actual slider image....
		Point<int> mousePos = slider.getMouseXYRelative();
		if ((slider.isMouseButtonDown() == true) && (mousePos.getX() >= (destX-5)) && (mousePos.getX() <= (sliderEnd+5))) {
			slider.setEnabled (true);
			slider.setValue(((mousePos.getX()-destX) / destWidth) * (slider.getMaximum() - slider.getMinimum()));
		}
		slider.setEnabled (false); //setting it back to disabled.

	
		/*----- The following calculates the frame number from the position that the slider should be in. 
		slider.getMinimum() is subtracted in case that 0 is not the minimum value set. */
		const double div = (slider.getMaximum() - slider.getMinimum()) / numFrames;
		float frameToUse = (int)(((slider.getValue() - slider.getMinimum()) / div) + 0.5);
		if (frameToUse > 0) frameToUse -= 1;

		//Setting frame width, height and the offset values...
		frameWidth = 128;
		frameHeight = 32;
		xOffset = 0;
		yOffset = frameToUse * frameHeight;		

		//----- Drawing marker ticks...
		g.setColour (Colours::whitesmoke);
		g.setOpacity (0.4);
		float markerGap = destWidth/10; //gap between ticks
		//Drawing the small ticks
		for (int i=1; i<5; i++)
			g.drawLine (destX+(i*markerGap), slider.getHeight()*0.35, destX+(i*markerGap), slider.getHeight()*0.65, 1);
		for (int i=6; i<10; i++)
			g.drawLine (destX+(i*markerGap), slider.getHeight()*0.35, destX+(i*markerGap), slider.getHeight()*0.65, 1);
		//Drawing the centre tick, this will be longer and thicker
		g.drawLine ((destWidth/2) + destX, slider.getHeight()*0.25, (destWidth/2) + destX, slider.getHeight()*0.75, 2);
		

		//----- Getting image from memory...
		newBackground  = ImageCache::getFromMemory (imageBinaries::horizontalbackground_png, 
												imageBinaries::horizontalbackground_pngSize);
	}

	/*----- If Vertical Slider ---------------------------------------------------------------------------*/
	if (style == Slider::LinearVertical) {

		//----- starting x position
		destX = ((slider.getWidth() - destWidth) / 2);	
		//----- starting y position, 7 is to make room for the edge of the slider thumb when at a maximum or minimum
		destY = 7; 
		sliderEnd = slider.getHeight() - 7;

		/*----- If textbox IS NOT to be displayed!!!!!! 
		If there is a name label it will be shown at the bottom	while the value will be at the top when 
		the mouse hovers over the slider. */
		if (slider.getTextBoxPosition() == Slider::NoTextBox) {
			slider.setTextBoxStyle (Slider::NoTextBox, true, 0, 0);
			if (showName == true) { //if the name should be displayed
				g.drawText (name, (slider.getWidth()/2) - (nameWidth/2), slider.getHeight() - 11, 
															(int)nameWidth, 11,	just, false);
				sliderEnd -= 21; //name label(11) + space(10) between image and text
			}
			destY += 15; //value(15) plus gap(7)
		}

		/*----- If textbox IS to be displayed!!!!  
		This means that if the user wants a label it will automatically	go at the top. */
		else {
			String str;
			str << slider.getMaximum() << slider.getInterval();
			int len = str.length() * 7; //length of text
			slider.setTextBoxStyle (Slider::TextBoxBelow, false, len, 15);
			sliderEnd -= 30; //textbox height(15) plus gap(15)

			if (showName == true) { //if name label is to be displayed
				g.drawText (name, (slider.getWidth()/2) - (nameWidth/2), 0, (int)nameWidth, 15, 
																				just, false);
				destY += 15; //name(15)
				//sliderEnd -= 0; Having a textbox will automatically adjust slider.getHeight() so no need to 
				//subtract anything here
			}
		}

		//Getting the new destination height of image
		destHeight = sliderEnd - destY;

		//----- Slider is enabled and value changed if mouse click is within the actual slider image....
		Point<int> mousePos = slider.getMouseXYRelative();
		if ((slider.isMouseButtonDown() == true) && (mousePos.getY() >= (destY-5)) && (mousePos.getY() <= (sliderEnd+5))) {
			slider.setEnabled (true);
			float yInvert = 1 - ((mousePos.getY()-destY) / destHeight);
			slider.setValue (yInvert * (slider.getMaximum() - slider.getMinimum()));
		}
		slider.setEnabled (false); //setting it back to disabled.

		//----- Setting frame width, height and the offset values...
		frameWidth = 32;
		frameHeight = 128;
		xOffset = frameToUse * frameWidth;
		yOffset = 0;		

		//----- Drawing marker ticks...
		g.setColour (Colours::whitesmoke);
		g.setOpacity (0.4);
		float markerGap = destHeight/10; //gap between ticks
		//Drawing the small ticks
		for (int i=1; i<5; i++)
			g.drawLine (slider.getWidth()*0.35, destY + (i*markerGap), slider.getWidth()*0.65, destY + (i*markerGap), 1);
		for (int i=6; i<10; i++)
			g.drawLine (slider.getWidth()*0.35, destY + (i*markerGap), slider.getWidth()*0.65, destY + (i*markerGap), 1);
		//Drawing the centre tick, this will be longer and thicker
		g.drawLine (slider.getWidth()*0.25, (destHeight/2) + destY, slider.getWidth()*0.75, (destHeight/2) + destY, 2);

		//----- Getting image from memory...
		newBackground  = ImageCache::getFromMemory (imageBinaries::verticalbackground_png, 
												imageBinaries::verticalbackground_pngSize);
	}
	
	//----- Drawing Image. 
	g.setOpacity (1);
	g.drawImage(newBackground, destX, destY, destWidth, destHeight, xOffset, yOffset, frameWidth, frameHeight, false);
}


//========== Linear Slider Thumb =========================================================================
void CabbageLookAndFeel::drawLinearSliderThumb (Graphics &g, int /*x*/, int /*y*/, int width, int height, float sliderPos, 
																								float /*minSliderPos*/, 
																								float /*maxSliderPos*/, 
																								const Slider::SliderStyle style, 
																								Slider &slider)
{
	const int frameWidth = 40;
	const int frameHeight = 40;
	float destX, destY, destWidth, destHeight, sliderStart, sliderEnd;
	Image newThumb;

	/*----- The following determines if slider value should be displayed, in the event of no textbox.  It 
	also calculates the string sliderValue and the width of the string, as these are used later when drawing 
	the value....*/
	String sliderValue;
	float strWidth;
	Font fontValue (T("Helvetica"), 13, 1);
	Justification just (36); //centered
	if (slider.getTextBoxPosition() == Slider::NoTextBox) {
		//Getting number of decimal places
		String interval;
		interval << slider.getInterval();
		int numDec = interval.length()-2; //taking away decimal point and number before
		if (numDec < 0)
			numDec = 0;

		//Setting up format string....
		String format;
		format << "%." << numDec << "f";
		sliderValue = String::formatted(format, slider.getValue());
		
		strWidth = fontValue.getStringWidth(sliderValue);
	}

	//----- If Horizontal ---------------------------------------------------------------------------------
	if (style == Slider::LinearHorizontal) {
		//Getting width of name label....
		String name;
		name << slider.getName();
		Font fontName (T("Verdana"), 13, 1);
		g.setFont (fontName);
		float nameWidth = fontName.getStringWidth(name);

		//Getting width of textbox / value
		String str;
		str << slider.getMaximum() << slider.getInterval();
		int len = str.length() * 7; //length of text

		/*----- Initialising the start and end coordinates of the slider.  Gaps of 7 are left so that the edge of the thumb
		can be seen when at a minimum or maximum value. */
		sliderStart = 7; 
		sliderEnd = slider.getWidth() - (len+7); 
		
		//----- If name label is to be shown...
		if (slider.getName().length() > 0) 
			sliderStart += nameWidth+5;

		//----- If a textbox IS to be displayed
		if (slider.getTextBoxPosition() != slider.NoTextBox) 
			sliderEnd -= 15; //for gap between slider and textbox

		//----- The following code calculates the thumb position 
		float availableWidth = sliderEnd - sliderStart;
		float div = (slider.getValue() - slider.getMinimum()) / (slider.getMaximum() - slider.getMinimum());
		sliderPos = (div*availableWidth) + sliderStart;

		//----- destWidth and destHeight are based on the height
		destWidth = height * 0.6;
		destHeight = height * 0.6;
		destY = (slider.getHeight()/2) - (destHeight/2);
		destX = sliderPos - (destWidth / 2); //subtracting (destWidth/2) centres the thumb over slider position

		//----- Drawing the slider value above the thumb if there is no textbox, and mouse is hovering or dragging
		if ((slider.getTextBoxPosition() == Slider::NoTextBox) && (slider.isMouseOverOrDragging() == true)) {
			//Drawing background box
			float boxWidth = strWidth + 2;
			g.setColour (Colours::whitesmoke);
			g.setOpacity (0.7);
			g.fillRoundedRectangle (destX+21, (slider.getHeight()/2)-7, boxWidth, 13.0f, 4);

			//Text
			g.setColour (Colours::black);
			g.setFont (fontValue);
			g.drawText (sliderValue, destX+22, (slider.getHeight()/2)-7, (int)strWidth, 13, just, false);
		}			

		//----- Getting image from memory...
		newThumb  = ImageCache::getFromMemory (imageBinaries::horizontalthumb_png, imageBinaries::horizontalthumb_pngSize); 
	}

	//----- If Vertical ------------------------------------------------------------------------------
	if (style == Slider::LinearVertical) {
		/*----- Initialising the top and bottom coordinates of the slider.  Gaps of 7 are left so that the edge of the thumb
		can be seen when at a minimum or maximum value. */
		sliderStart = 7; 
		sliderEnd = slider.getHeight() - 7;
		
		/*----- The gaps at the top and bottom of the slider image will change depending on whether
		there is a textbox being displayed or not. If there is no textbox the value will be displayed 
		above the thumb, therefore a gap will be left at the top. */
		
		//----- If No Text Box is to be displayed.....
		if (slider.getTextBoxPosition() == Slider::NoTextBox) {
			sliderStart += 15;
			if (slider.getName().length() > 0) //if name label is to be displayed it will go at bottom
				sliderEnd -= 21; //name label(11) plus gap(10)
		}

		//----- If there IS a textbox to be displayed....
		else {
			if (slider.getName().length() > 0) //if name label is to be displayed it will go at top
				sliderStart += 15;	//name label(15)
			sliderEnd -= 30; //textbox height(15) plus gap(15)
		}

		//----- The following code calculates the thumb position 
		float div = (slider.getValue() - slider.getMinimum()) / (slider.getMaximum() - slider.getMinimum());
		float availableHeight = sliderEnd - sliderStart;
		sliderPos = div * availableHeight;
		sliderPos = (availableHeight - sliderPos) + sliderStart;	//inverting the y axis

		//----- destWidth and destHeight are based on the width
		destWidth = width * 0.6;
		destHeight = width * 0.6;
		destX = (slider.getWidth()/2) - (destWidth/2);
		destY = sliderPos - (destHeight / 2);

		//----- Drawing the slider value above the thumb if there is no textbox, and mouse is hovering or dragging
		if ((slider.getTextBoxPosition() == Slider::NoTextBox) && (slider.isMouseOverOrDragging() == true)) {
			//Drawing background box
			float boxWidth = strWidth + 2;
			g.setColour (Colours::whitesmoke);
			g.fillRoundedRectangle ((slider.getWidth()/2) - (strWidth/2)-1, sliderPos-20, strWidth+2, 13.0f, 4);
		
			//Text
			g.setColour (Colours::black);
			g.setFont (fontValue);
			g.drawText (sliderValue, (slider.getWidth()/2) - (strWidth/2), sliderPos-20, (int)strWidth, 13, just, false);
		}

		//----- Getting image from memory...
		newThumb  = ImageCache::getFromMemory (imageBinaries::verticalthumb_png, imageBinaries::verticalthumb_pngSize);
	}
	
	g.drawImage (newThumb, destX, destY, destWidth, destHeight, 0, 0, frameWidth, frameHeight, false);
}


//======= Toggle Buttons ========================================================================
void CabbageLookAndFeel::drawToggleButton (Graphics &g, ToggleButton &button, bool /*isMouseOverButton*/, bool /*isButtonDown*/)
{
	int yOffset= 0;
	int frameWidth = 80;
	int frameHeight = 80; 
	int destWidth = button.getHeight();
	int destHeight = destWidth;
	int destX = 0;
	int destY = (button.getHeight() - destHeight) / 2;

	Image newButton = ImageCache::getFromMemory (imageBinaries::togglebutton_png, imageBinaries::togglebutton_pngSize);
	
	//----- If button is switched on
	if (button.getToggleState() == true) 
		yOffset = 80;	//for changing frame

	//----- Drawing image
	g.drawImage (newButton, destX, destY, destWidth, destHeight, 0, yOffset, frameWidth, frameHeight, false);

	//----- Text
	Justification just (1); //left
	Font font (T("Verdana"), 13, 1);
	g.setFont (font);
	g.setColour (Colours::whitesmoke);
	g.setOpacity (0.5);

	/*if (button.getHeight() < 14) 
		g.setFont(button.getHeight());
	else
		g.setFont (11); //Text height is 11 if the total height is 14 or more
		*/

	g.drawText (button.getButtonText(), destWidth+1, destY, button.getWidth(), button.getHeight(), just, false);
}

//========= Text Button Background ================================================================
void CabbageLookAndFeel::drawButtonBackground (Graphics& g, Button& button, const Colour& /*backgroundColour*/, 
																			bool /*isButtonOver*/, 
																			bool isButtonDown)
{

	int frameWidth = 104;
	int frameHeight = 40;
	int yOffset = 0;

	Image newButton = ImageCache::getFromMemory (imageBinaries::button1_png, imageBinaries::button1_pngSize);

	//----- There are only 2 frames in the image strip
	if (isButtonDown == true) yOffset = 40;

	g.drawImage (newButton, 0, 0, button.getWidth(), button.getHeight(), 0, yOffset, frameWidth, frameHeight, false);

}


//=========== ComboBoxes ============================================================================
void CabbageLookAndFeel::drawComboBox(Graphics& g, int width, int height, bool /*isButtonDown*/,
																	int /*buttonX*/,
																	int /*buttonY*/,
																	int /*buttonW*/,
																	int /*buttonH*/,
																	ComboBox& box)
{
	//----- For the main background
	g.setColour(Colours::black);	
	g.setOpacity(0.8);		
	g.fillRoundedRectangle (0, 0, width, height, height/3);
	
	//----- The box that contains the arrow
	g.setOpacity (0.7);
	g.fillRoundedRectangle (width-20, 0, 20, height, height/3);

	//----- Arrow
	g.setColour(Colours::lime);
	const Line<float> line((float)box.getWidth() - 10, 0, (float)box.getWidth() - 10, 13); 
	g.drawArrow(line, 0, 8, 8);	
}


//============= TextEditor Outline ======================================================================
void CabbageLookAndFeel::drawTextEditorOutline (Graphics &g, int width, int height, TextEditor &/*textEditor*/)
{
	g.setColour (Colours::transparentBlack);
	//g.setOpacity (0.9);
	g.drawRoundedRectangle (0, 0, width, height, height/3, 1);
}



//============= TextEditor Background ===================================================================
void CabbageLookAndFeel::fillTextEditorBackground (Graphics &g, int width, int height, TextEditor &textEditor)
{
	textEditor.setColour (0x1000200, Colours::transparentBlack); //this gets rid of the square rectangle background
	g.setColour (Colours::black);
	g.setOpacity (0.9);
	g.fillRoundedRectangle (0, 0, width, height, height/3);
}


//=========== Labels, slider textboxes are also labels ==================================================
void CabbageLookAndFeel::drawLabel (Graphics &g, Label &label)
{	
	/*----- Getting parent component of label, if it is a slider then the background will
	be a rectangle with rounded corners.  The background colour is retrieved from the initialisation
	of the slider / label.  Otherwise it will be normal */
	Component* comp = label.getParentComponent();

	//Setting the font
	Font font (T("Helvetica"), 11, 1);
	g.setFont (font);

	if (dynamic_cast<Slider*>(comp)) { //If slider
	//----- Drawing rounded rectangle
		Colour cl = label.findColour (0x1000280, true); //getting background colour specified
		g.setColour (cl);
		//if the colour is white then no opacity is used
		if (cl != Colours::white) 
			g.setOpacity (0.7);	

		g.fillRoundedRectangle (0, 0, label.getWidth(), label.getHeight(), label.getHeight()/3);

		/*----- For the text. If the background is brighter than 0.6 then the text is black, otherwise the 
		text is whitesmoke.*/
		Justification just(36);	//justification of text
		if (cl.getBrightness() > 0.6) g.setColour (Colours::black);
		else g.setColour (Colours::whitesmoke);

		g.drawText (label.getText(), 0, 0, label.getWidth(), label.getHeight(), just, false);
	}
	else { //If not a slider
		g.setColour (Colours::transparentBlack);
		g.fillRoundedRectangle (0, 0, label.getWidth(), label.getHeight(), label.getHeight()/3);

		//----- For the text
		Justification just(36);
		g.setColour (Colours::whitesmoke);
		g.drawText (label.getText(), 0, 0, label.getWidth(), label.getHeight(), just, false);
	}
}


//======== Group Component Outline ======================================================================
void CabbageLookAndFeel::drawGroupComponentOutline (Graphics &g, int w, int h, const String &text, 
																		const Justification &position, 
																		GroupComponent &group)
{
	//----- Background
	/*int groupX = group.getX();
	int groupY = group.getY();

	Component* parent = group.getParentComponent();
	int parentWidth = parent->getWidth();
	int parentHeight = parent->getHeight();
	ColourGradient cg = ColourGradient (Colours::black, groupX*-1, groupY*-1, 
						Colours::lightgrey, parentWidth-groupX, parentHeight-groupY, false);
	g.setGradientFill (cg);
	g.setOpacity (0.4);
	g.fillRoundedRectangle (0, 0, w, h, 5);
	*/
	g.setColour(Colours::black);
	g.fillRoundedRectangle (0, 0, w, h, 5);
	g.setColour (Colours::grey);
	g.setOpacity (0.2);
	g.fillRoundedRectangle (0, 0, w, h, 5);

	//----- Outline
	g.setColour (Colours::whitesmoke);
	g.setOpacity (0.1);
	g.drawRoundedRectangle (0.5, 0.5, w-1, h-1, 5, 1);
	
	//----- Text
	String name = group.getText();
	Font font (T("Impact"), 14, 0);
	font.setFallbackFontName (T("Verdana")); //in case the user doesn't have the first font installed
	g.setFont (font);
	Justification just (36);
	g.setColour (Colours::whitesmoke);
	g.setOpacity (0.6);

	g.drawText (name, 0, 5, w, 13, just, false);
	g.drawLine (10, 20, w-10, 20, 0.5);

	//----- Corner holes
	g.setColour (Colours::black);
	g.setOpacity (0.6);
	g.fillEllipse (3, 3, 6, 6);
	g.fillEllipse (3, h-9, 6, 6);
	g.fillEllipse (w-9, 3, 6, 6);
	g.fillEllipse (w-9, h-9, 6, 6);
}

/*
void CabbageLookAndFeel::setDefaultSansSerifTypefaceName (const String &newName)
{
	Font font (T("Comic Sans"), 11, 1);
}
*/




//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++




//======= Basic Look And Feel Methods ===================================================================


//========== Constructor ================================================================================
CabbageLookAndFeelBasic::CabbageLookAndFeelBasic()
{
}


//========= Destructor ====================================================================================
CabbageLookAndFeelBasic::~CabbageLookAndFeelBasic()
{
}

//=========== Linear Slider Background ====================================================================
void CabbageLookAndFeelBasic::drawLinearSliderBackground (Graphics &g, int x, int y, int width, int height, 
																					float sliderPos, 
																					float minSliderPos, 
																					float maxSliderPos, 
																					const Slider::SliderStyle style, 
																					Slider &slider)
{
	//----- Horizontal Sliders....
	int backgroundHeight = height * 0.3;
	int fillHeight = height * 0.05;
	int destY = (height-backgroundHeight) / 2;
	int destFillY = ((backgroundHeight-fillHeight) / 2) + destY;

	slider.setTextBoxStyle (Slider::NoTextBox, true, 0, 0); //No text box
	g.setColour(Colours::black);
	g.setOpacity (0.7);
	g.fillRoundedRectangle (0, destY, slider.getWidth(), backgroundHeight, backgroundHeight/3);

	//----- For the fill
	ColourGradient cg = ColourGradient (Colours::transparentBlack, 0, 0, 
		Colours::grey, slider.getWidth()*0.5, 0, false);
	g.setGradientFill (cg);
	g.setOpacity (0.9);
	g.fillRoundedRectangle (0, destFillY, sliderPos, fillHeight, backgroundHeight/3);
}

//=========== Linear Thumb =================================================================================
void CabbageLookAndFeelBasic::drawLinearSliderThumb (Graphics &g, int x, int y, int width, int height, 
																						float sliderPos, 
																						float minSliderPos, 
																						float maxSliderPos, 
																						const Slider::SliderStyle style, 
																						Slider &slider)
{
	//----- Horizontal Sliders.....
	int rectWidth = (height*0.4);
	int rectHeight = (height*0.75);
	int destY = ((height-rectHeight)/2);
	maxSliderPos = width-(rectWidth/2);
	int availableWidth = slider.getWidth() - rectWidth;

	float div = (slider.getValue()-slider.getMinimum()) / (slider.getMaximum()-slider.getMinimum());
	sliderPos = (div*availableWidth);
	
	//g.setColour (Colours::lightgrey);
	ColourGradient cg = ColourGradient (Colours::whitesmoke, sliderPos, destY, Colours::grey, sliderPos+rectWidth, rectHeight, false);
	g.setGradientFill (cg);
	g.fillRoundedRectangle (sliderPos, destY, rectWidth, rectHeight, rectWidth/3);

	g.setColour (Colours::black);
	g.drawRoundedRectangle (sliderPos, destY, rectWidth, rectHeight, rectWidth/3, 1);
}




