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
Image CabbageLookAndFeel::drawRotaryImage(int diameter, const Colour circleFill, float sliderPosProportional, bool useBigImage)
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

	//----- Outer grey circle and green fill.  They only get drawn if the bigger slider image is
	//to be used...
	if (useBigImage == true) {
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

	//Using a colour gradient from white to the chosen colour gives the effect of a light source.  The
	//brightness of the selected colour is also brought down a little if too great, to make it look more
	//realisitc.
	float brightness = circleFill.getBrightness();
	if (brightness > 0.8)
		brightness = 0.8;
	ColourGradient circleGrad = ColourGradient (Colours::white, diameter*-0.2, diameter*-0.2,
		circleFill.withBrightness (brightness), diameter*0.5, diameter*0.5, false);
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
																		diameter*0.01, diameter*0.05);
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

//========== Creating Image for Linear Slider Background ==========================================================
Image CabbageLookAndFeel::drawLinearBgImage (float width, float height, float sliderPosProportional, bool isVertical)
{
	//----- Creating a blank canvas.
	Image img = Image::Image(Image::ARGB, width, height, true);
	Graphics g (img);
	
	//----- For horizontal sliders ---------------------------------------------------
	if (isVertical == false) {
		//----- Drawing the small ticks
		g.setColour (Colours::whitesmoke);
		g.setOpacity (0.3);
		float markerGap = width/10; //gap between ticks
		for (int i=1; i<5; i++)
			g.drawLine ((i*markerGap), height*0.3, (i*markerGap), height*0.7, .7);
		for (int i=6; i<10; i++)
			g.drawLine ((i*markerGap), height*0.3, (i*markerGap), height*0.7, .7);
		//Drawing the centre tick, this will be longer and thicker
		g.setOpacity (0.6);
		g.drawLine ((width/2), height*0.25, (width/2), height*0.75, 1.5);

		//----- Main Rectangle. Creating the illusion of lighting by painting an almost transparent rectangle first.
		g.setColour (Colours::whitesmoke);
		g.setOpacity (0.1);
		g.fillRoundedRectangle (0, height*0.44, width, height*0.15, height*0.05); //for light effect
		g.setColour (Colours::black);
		g.fillRoundedRectangle (0, height*0.425, width*0.99, height*0.15, height*0.05); //main rectangle

		//----- Green fill.  Using sliderPosProportional to fill in the correct amount.
		ColourGradient fill = ColourGradient (Colours::transparentBlack, 0, 0, Colours::lime, width*0.1, 0, false);
		g.setGradientFill (fill);
		g.setOpacity (0.9);
		g.fillRoundedRectangle (0, height*0.47, sliderPosProportional*width, height*0.06, height*0.05);
	}
	//----- For vertical sliders ---------------------------------------------------
	if (isVertical == true) {
		//----- Drawing the small ticks
		g.setColour (Colours::whitesmoke);
		g.setOpacity (0.3);
		float markerGap = height/10; //gap between ticks
		for (int i=1; i<5; i++)
			g.drawLine (width*0.3, (i*markerGap), width*0.7, (i*markerGap), .7);
		for (int i=6; i<10; i++)
			g.drawLine (width*0.3, (i*markerGap), width*0.7, (i*markerGap), .7);
		//Drawing the centre tick, this will be longer and thicker
		g.setOpacity (0.6);
		g.drawLine (width*0.25, (height/2), width*0.75, (height/2), 1.5);

		//----- Main Rectangle. Creating the illusion of lighting by painting an almost transparent rectangle first.
		g.setColour (Colours::whitesmoke);
		g.setOpacity (0.1);
		g.fillRoundedRectangle (width*0.44, 0, width*0.15, height, width*0.05); //for light effect
		g.setColour (Colours::black);
		g.fillRoundedRectangle (width*0.425, 0, width*0.15, height*0.99, width*0.05); //main rectangle

		//----- Green fill.  Using sliderPosProportional to fill in the correct amount.
		ColourGradient fill = ColourGradient (Colours::transparentBlack, 0, height, Colours::lime, 0, height*0.9, false);
		g.setGradientFill (fill);
		g.setOpacity (0.9);
		sliderPosProportional = 1 - sliderPosProportional; //inverting y axis
		g.fillRoundedRectangle (width*0.47, sliderPosProportional*height, width*0.06, 
			height-(sliderPosProportional*height), width*0.05);
	}

	//----- Returning image
	return img;
}

//========= Creating image for linear slider thumb ========================================================
Image CabbageLookAndFeel::drawLinearThumbImage (float width, float height, const Colour thumbFill, bool isVertical)
{
	//----- Creating a blank canvas.
	Image img = Image::Image(Image::ARGB, width, height, true);
	Graphics g (img);

	//----- Setting colour of thumb.  A colour gradient is used to give the illusion of a light source.
	float brightness = thumbFill.getBrightness();
	if (brightness > 0.8)
		brightness = 0.8;
	ColourGradient thumb = ColourGradient (Colours::white, 0, 0, 
			thumbFill.withBrightness(brightness), width/2, height/2, false);

	//----- For horizontal sliders ------------------------------------------
	if (isVertical == false) {
		//For shadow effect
		g.setColour (Colours::black);
		g.setOpacity (0.8);
		g.fillEllipse ((width*0.2)+1, (height*0.1)+1, width*0.6, height*0.8);
		g.setOpacity (0.4);
		g.fillEllipse ((width*0.2)+3, (height*0.1)+3, width*0.6, height*0.8);
		//Colouring in the thumb
		g.setGradientFill (thumb);
		g.fillEllipse (width*0.2, height*0.1, width*0.6, height*0.8);
	}

	//----- For vertical sliders ------------------------------------------
	else if (isVertical == true) {
		//For shadow effect
		g.setColour (Colours::black);
		g.setOpacity (0.8);
		g.fillEllipse ((width*0.1)+1, (height*0.2)+1, width*0.8, height*0.6);
		g.setOpacity (0.4);
		g.fillEllipse ((width*0.1)+3, (height*0.2)+3, width*0.8, height*0.6);
		//Colouring in the thumb
		g.setGradientFill (thumb);
		g.fillEllipse (width*0.1, height*0.2, width*0.8, height*0.6);
	}

	//----- Returning image
	return img;
}

//========= Creating image for toggle Button ========================================================
Image CabbageLookAndFeel::drawToggleImage (float width, float height, bool isToggleOn)
{
	//----- Creating a blank canvas.
	Image img = Image::Image(Image::ARGB, width, height, true);
	Graphics g (img);

	Colour outline = Colour::fromRGBA (0, 0, 0, 150);
	g.setColour (outline);
	g.fillRoundedRectangle (width*0.18, height*0.18, width*0.64, height*0.64, width*0.1);

	//----- If "on"
	if (isToggleOn == true) {
		//g.setColour (Colours::lime);
		ColourGradient greenFill = ColourGradient (Colours::white, 0, 0, Colours::lime,
			width/2, height/2, false);
		g.setGradientFill (greenFill);
		g.fillRoundedRectangle (width*0.2, height*0.2, width*0.6, height*0.6, width*0.1);
	}
	//----- If "0ff"
	else {
		//----- Shadow
		for (float i=0.01; i<0.1; i+=0.01) {
			Colour shadow = Colour::fromRGBA (5, 5, 5, 255/(i*100));
			g.setColour (shadow);
			g.fillRoundedRectangle (width * (0.19+i), height * (0.19+i), 
				width*0.6, height*0.6, width*0.1);
		}
		//----- Filling in the button
		Colour bg1 = Colour::fromRGBA (45, 45, 48, 255);
		Colour bg2 = Colour::fromRGBA (15, 15, 18, 255);
		ColourGradient cg = ColourGradient (bg1, 0, 0, bg2, width*0.5, height*0.5, false);
		g.setGradientFill (cg);
		g.fillRoundedRectangle (width*0.2, height*0.2, width*0.6, height*0.6, width*0.1);

		//----- For emphasising the top and left edges to give the illusion that light is shining on them
		ColourGradient edgeHighlight = ColourGradient (Colours::lightgrey, 0, 0,
			Colours::transparentWhite, 0, height*0.25, false);
		g.setGradientFill (edgeHighlight);
		g.fillRoundedRectangle (width*0.2, height*0.2, width*0.6, height*0.6, width*0.1);

		ColourGradient edgeHighlight2 = ColourGradient (Colours::lightgrey, 0, 0,
			Colours::transparentWhite, width*0.25, 0, false);
		g.setGradientFill (edgeHighlight2);
		g.fillRoundedRectangle (width*0.2, height*0.2, width*0.6, height*0.6, width*0.1);
	}

	//----- Returning image
	return img;
}

//========= Creating image for text Button ========================================================
Image CabbageLookAndFeel::drawTextButtonImage (float width, float height, bool isButtonDown)
{
	//----- Creating a blank canvas.
	Image img = Image::Image(Image::ARGB, width, height, true);
	Graphics g (img);

	//----- Outline
	Colour outline = Colour::fromRGBA (0, 0, 0, 150);
	g.setColour (outline);
	g.fillRoundedRectangle (width*0.09, height*0.09, width*0.82, height*0.82, height*0.1);

	//----- If "off"
	if (isButtonDown == false) {
		//----- Shadow
		for (float i=0.01; i<0.05; i+=0.01) {
			Colour shadow = Colour::fromRGBA (5, 5, 5, 255/(i*100));
			g.setColour (shadow);
			g.fillRoundedRectangle (width * (0.1+i), height * (0.1+i), 
				width*0.8, height*0.8, height*0.1);
		}
	}

	//----- Filling in the button
	Colour bg1 = Colour::fromRGBA (45, 45, 48, 255);
	Colour bg2 = Colour::fromRGBA (15, 15, 18, 255);
	ColourGradient cg = ColourGradient (bg1, (width*0.2) * -1, (height*0.2) * -1, 
		bg2, width*0.5, height*0.5, false);
	g.setGradientFill (cg);
	g.fillRoundedRectangle (width*0.1, height*0.1, width*0.8, height*0.8, height*0.1);

	//----- If "off".  (This can't be put in the above if statement as it has to be drawn after the filling
	//in of the button).
	if (isButtonDown == false) {
		//----- For emphasising the top and left edges to give the illusion that light is shining on them
		ColourGradient edgeHighlight = ColourGradient (Colours::lightgrey, 0, (height*0.25) * -1,
			Colours::transparentWhite, 0, height*0.15, false);
		g.setGradientFill (edgeHighlight);
		g.fillRoundedRectangle (width*0.1, height*0.1, width*0.8, height*0.8, height*0.1);

		ColourGradient edgeHighlight2 = ColourGradient (Colours::lightgrey, (width*0.25) * -1, 0,
			Colours::transparentWhite, width*0.15, 0, false);
		g.setGradientFill (edgeHighlight2);
		g.fillRoundedRectangle (width*0.1, height*0.1, width*0.8, height*0.8, height*0.1);
	}

	//----- Returning image
	return img;
}


//=========== Rotary Sliders ==============================================================================
void CabbageLookAndFeel::drawRotarySlider(Graphics& g, int /*x*/, int /*y*/, int /*width*/, int /*height*/,
																	float sliderPosProportional,
																	float /*startAngle*/,
																	float /*endAngle*/,
																	Slider& slider)
{
	//----- Getting the original width of slider.  This means that the same image will be used
	//if it needs to be resized.  Smaller images don't use the green fill.
	bool useBigImage = true;
	float origWidth = slider.getProperties().getWithDefault(String("origWidth"), -99);
	if (origWidth <= 40)
		useBigImage = false;

	//----- Variables for determining new destination width and height etc...
	float destX, destY, destHeight, destWidth, sliderBottom;
	destX = destY = 0;
	sliderBottom = slider.getHeight();

	//----- Variables for slider name
	String name;
	name << slider.getName();
	Font fontName = CabbageUtils::widgetText();
	Justification justName (36); //centered
	float nameWidth = fontName.getStringWidth(name);

	//if name is too big it gets truncated, with dots displayed at the end...
	name = CabbageUtils::cabbageString (name, fontName, slider.getWidth());
	
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
	Image newSlider = drawRotaryImage(destWidth, circleFill, sliderPosProportional, useBigImage);
	g.setOpacity (1);
	g.drawImage (newSlider, destX, destY, destWidth, destHeight, 0, 0, destWidth, destWidth, false);

	//----- If NO textbox and mouse is hovering or dragging, then draw the value across the slider.  This has to be done
	//after the images as it must go on top of them. 
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


		//----- If the value box width is bigger than the slider width.  The slider size needs to be increased to cater for displaying 
		//the full value.  This should only really apply to small sliders.  Only the parent needs to be increase, CabbageSlider(), as it
		//will resize the actual slider itself.  This will work whether the slider uses a group caption or not.
		if (destWidth < boxWidth) {
			//Getting difference in width between value box and slider width
			int diff = boxWidth - destWidth;

			//if there is a label at the bottom we have to add on the height of the label too...
			if (slider.getName().length() > 0)
				boxWidth += 15;

			//Getting parent's original bounds...
			Component* parent; //CabbageSlider
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

			//-----Assigning a resize flag to the slider so that it can be checked in the else statement that follows.  
			//Components have no ID to begin with.  This will prevent the wrong slider being picked up by the else statement 
			//when they physically overlap. 
			String resizeFlag;
			resizeFlag << 1;
			slider.setComponentID (resizeFlag);
		}
	}
	
	//----- Else if mouse is not hovering and slider ID is not empty.  This means that the slider's parent had been
	//resized previously, and will therefore enter this else statement to get resized again back to its original
	//dimensions. Once the parent is resized, the slider inside it will resize accordingly.
	else if ((slider.isMouseOverOrDragging() == false) && (slider.getComponentID().compare ("") != 0)) {
		//Getting original parent dimensions...		
		Component* parent;  //CabbageSlider()
		parent = slider.getParentComponent();
		int parentWidth = slider.getProperties().getWithDefault(String("origWidth"), -99);
		int parentHeight = slider.getProperties().getWithDefault(String("origHeight"), -99);
		int parentX = slider.getProperties().getWithDefault(String("origX"), -99);
		int parentY = slider.getProperties().getWithDefault(String("origY"), -99);
		parent->setBounds (parentX, parentY, parentWidth, parentHeight);

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
	//----- Getting the proportional position that the current value has in relation to the max. This is used
	//for drawing the image in the correct position.
	float sliderPosProportional = (slider.getValue()-slider.getMinimum()) / (slider.getMaximum()-slider.getMinimum());

	Image newBackground;

	//----- Variables for determining new destination width, height, position etc...
	float destX, destY, sliderEnd;
	float destHeight = slider.getHeight(); 
	float destWidth = slider.getWidth();
	
	//----- Determining if the user wants a name label displayed......
	bool showName = false;
	String name;
	Font fontName;
	float nameWidth;
	Justification just (36); //centered
	if (slider.getName().length() > 0) {
		name << slider.getName();
		fontName = CabbageUtils::widgetText();
		g.setFont (fontName);
		nameWidth = fontName.getStringWidth(name);
		g.setColour (Colours::whitesmoke);
		g.setOpacity (0.5);
		showName = true; //setting flag to true
	}

	//----- If Horizontal Slider --------------------------------------------------------------------------
	if (style == Slider::LinearHorizontal) {
		//setting the available space for the name
		nameWidth = slider.getWidth() * 0.2; 

		//Variables for slider value / textbox
		String str;
		str << slider.getMaximum() << slider.getInterval();
		int len = str.length() * 7; //length of text

		//----- start and end of slider image, height*0.25 is to make room for the edge of the slider thumb when at 
		//a maximum or minimum
		destX = slider.getHeight()*0.25; 
		sliderEnd = slider.getWidth() - destX; 
		//----- starting y position
		destY = 0;//(slider.getHeight()/2) - (destHeight/2);

		//----- If there is a name label it will be shown at the start while the value will be at the end when 
		//the mouse hovers over the slider. 
		Justification justLeft (1); //left
		if (showName == true) { //if the name should be displayed
			name = CabbageUtils::cabbageString (name, fontName, nameWidth); //shortening string if too long
			g.drawText (name, 0, (slider.getHeight()/2) - 6, (int)nameWidth, 13, justLeft, false);
			destX += nameWidth+2; //adjusting destX to accomodate slider name
		}

		//----- If textbox IS NOT to be displayed!!!!!! 
		if (slider.getTextBoxPosition() == Slider::NoTextBox) 
			slider.setTextBoxStyle (Slider::NoTextBox, true, 0, 0);

		//----- If textbox IS to be displayed!!!!  
		else {
			slider.setTextBoxStyle (Slider::TextBoxRight, false, len, 15);
			sliderEnd -= len;
		}

		//Getting the new destination width of image
		destWidth = sliderEnd - destX;

		//----- Slider is enabled and value changed if mouse click is within the actual slider image....
		if (slider.isMouseButtonDown() == true) {
			Point<int> mousePos = slider.getMouseXYRelative();
			slider.setEnabled(true);
			if ((mousePos.getX() >= (destX-5)) && (mousePos.getX() <= (sliderEnd+5)))
				slider.setValue(((mousePos.getX()-destX) / destWidth) * (slider.getMaximum() - slider.getMinimum()));
		}
		
		slider.setEnabled (false); //setting it back to disabled. Otherwise the slider would move slightly.

		//----- Getting image
		newBackground = drawLinearBgImage (destWidth, destHeight, sliderPosProportional, false);
	}

	//----- If Vertical Slider ---------------------------------------------------------------------------
	if (style == Slider::LinearVertical) {

		//----- starting x position
		destX = ((slider.getWidth() - destWidth) / 2);	
		//----- starting y position, width*0.25 is to make room for the edge of the slider thumb when at a 
		//maximum or minimum
		destY = slider.getWidth()*0.25; 
		sliderEnd = slider.getHeight() - destY;

		//----- If textbox IS NOT to be displayed!!!!!! 
		//If there is a name label it will be shown at the bottom	while the value will be at the top when 
		//the mouse hovers over the slider. 
		if (slider.getTextBoxPosition() == Slider::NoTextBox) {
			slider.setTextBoxStyle (Slider::NoTextBox, true, 0, 0);
			if (showName == true) { //if the name should be displayed
				name = CabbageUtils::cabbageString (name, fontName, slider.getWidth()); //shortening string if too long
				g.drawText (name, (slider.getWidth()/2) - (nameWidth/2), slider.getHeight() - 13, 
															(int)nameWidth, 13,	just, false);
				sliderEnd -= 21; //name label(11) + space(10) between image and text
			}
			destY += 15; //value(15)
		}

		//----- If textbox IS to be displayed!!!!  
		//This means that if the user wants a label it will automatically	go at the top. 
		else {
			String str;
			str << slider.getMaximum() << slider.getInterval();
			int len = str.length() * 7; //length of text
			slider.setTextBoxStyle (Slider::TextBoxBelow, false, len, 15);
			sliderEnd -= 30; //textbox height(15) plus gap(15)

			if (showName == true) { //if name label is to be displayed
				name = CabbageUtils::cabbageString (name, fontName, slider.getWidth()); //shortening string if too long
				g.drawText (name, (slider.getWidth()/2) - (nameWidth/2), 0, (int)nameWidth, 13, 
																				just, false);
				destY += 15; //name(15)
				//sliderEnd -= 0; Having a textbox will automatically adjust slider.getHeight() so no need to 
				//subtract anything here
			}
		}

		//Getting the new destination height of image
		destHeight = sliderEnd - destY;

		//----- Slider is enabled and value changed if mouse click is within the actual slider image....
		if (slider.isMouseButtonDown() == true) {
			Point<int> mousePos = slider.getMouseXYRelative();
			slider.setEnabled (true);
			if ((mousePos.getY() >= (destY-5)) && (mousePos.getY() <= (sliderEnd+5))) {
				float yInvert = 1 - ((mousePos.getY()-destY) / destHeight);
				slider.setValue (yInvert * (slider.getMaximum() - slider.getMinimum()));
			}
		}
		slider.setEnabled (false); //setting it back to disabled. Otherwise the slider would move slightly.

		//----- Creating background image
		newBackground = drawLinearBgImage (destWidth, destHeight, sliderPosProportional, true);
	}
	
	//----- Drawing Image. 
	g.setOpacity (1);
	g.drawImage(newBackground, destX, destY, destWidth, destHeight, 0, 0, destWidth, destHeight, false);
}


//========== Linear Slider Thumb =========================================================================
void CabbageLookAndFeel::drawLinearSliderThumb (Graphics &g, int /*x*/, int /*y*/, int width, int height, float sliderPos, 
																								float /*minSliderPos*/, 
																								float /*maxSliderPos*/, 
																								const Slider::SliderStyle style, 
																								Slider &slider)
{
	float destX, destY, destWidth, destHeight, sliderStart, sliderEnd;
	Colour thumbFill = slider.findColour(0x1001300, false);
	Image newThumb;

	//----- The following determines if slider value should be displayed, in the event of no textbox.  It 
	//also calculates the string sliderValue and the width of the string, as these are used later when drawing 
	//the value...
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
		Font fontName = CabbageUtils::widgetText();
		g.setFont (fontName);
		float nameWidth = slider.getWidth() * 0.2; //fontName.getStringWidth(name);

		//Getting width of textbox / value
		String str;
		str << slider.getMaximum() << slider.getInterval();
		int len = str.length() * 7; //length of text

		//----- destWidth and destHeight are based on the height
		destWidth = destHeight = height * 0.5;

		//----- Initialising the start and end coordinates of the slider.  A gap of destWidth/2 is left so 
		//that the edge of the thumb can be seen when at a minimum or maximum value. 
		sliderStart = destWidth/2; 
		sliderEnd = slider.getWidth() - (destWidth/2); 
		
		//----- If name label is to be shown...
		if (slider.getName().length() > 0) 
			sliderStart += nameWidth+5;

		//----- If a textbox IS to be displayed
		if (slider.getTextBoxPosition() != slider.NoTextBox) 
			sliderEnd -= len; 

		//----- The following code calculates the thumb position 
		float availableWidth = sliderEnd - sliderStart;
		float div = (slider.getValue() - slider.getMinimum()) / (slider.getMaximum() - slider.getMinimum());
		sliderPos = (div*availableWidth) + sliderStart;
		
		destY = (slider.getHeight()/2) - (destHeight/2);
		destX = sliderPos - (destWidth / 2); //subtracting (destWidth/2) centres the thumb over slider position

		//----- Drawing the slider value above the thumb if there is no textbox, and mouse is hovering or dragging
		if ((slider.getTextBoxPosition() == Slider::NoTextBox) && (slider.isMouseOverOrDragging() == true)) {
			//Drawing background box
			float boxWidth = strWidth + 2;
			float valuePos = (div * (availableWidth-boxWidth+(destWidth))) + (sliderStart-(destWidth/2));
			g.setColour (Colours::whitesmoke);
			//g.fillRoundedRectangle (destX+21, (slider.getHeight()/2)-7, boxWidth, 13.0f, 4);
			g.fillRoundedRectangle (valuePos, 0, boxWidth, 13.0f, 4);

			//Text
			g.setColour (Colours::black);
			g.setFont (fontValue);
			//g.drawText (sliderValue, destX+22, (slider.getHeight()/2)-7, (int)strWidth, 13, just, false);
			g.drawText (sliderValue, valuePos+1, 0, (int)strWidth, 13, just, false);
		}			

		//----- Creating image
		newThumb = drawLinearThumbImage (destWidth, destHeight, thumbFill, false);
	}

	//----- If Vertical ------------------------------------------------------------------------------
	if (style == Slider::LinearVertical) {
		//----- destWidth and destHeight are based on the width
		destWidth = destHeight = width * 0.5;

		//----- Initialising the top and bottom coordinates of the slider.  A gap of destHeight/2 is left so 
		//that the edge of the thumb can be seen when at a minimum or maximum value. 
		sliderStart = destHeight/2; 
		sliderEnd = slider.getHeight() - (destHeight/2);
		
		//----- The gaps at the top and bottom of the slider image will change depending on whether
		//there is a textbox being displayed or not. If there is no textbox the value will be displayed 
		//above the thumb, therefore a gap will be left at the top. 
		
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

		
		destX = (slider.getWidth()/2) - (destWidth/2);
		destY = sliderPos - (destHeight / 2);

		//----- Drawing the slider value above the thumb if there is no textbox, and mouse is hovering or dragging
		if ((slider.getTextBoxPosition() == Slider::NoTextBox) && (slider.isMouseOverOrDragging() == true)) {
			//Box
			float boxWidth = strWidth+2;
			g.setColour (Colours::whitesmoke);
			g.fillRoundedRectangle ((slider.getWidth()/2) - (boxWidth/2), sliderPos-22, boxWidth, 13.0f, 4);
			//Text
			g.setColour (Colours::black);
			g.setFont (fontValue);
			g.drawText (sliderValue, (slider.getWidth()/2) - (strWidth/2), sliderPos-22, (int)strWidth, 13, just, false);

			//----- If the value box is too wide, then the slider must be resized to accomodate it...
			if (boxWidth > slider.getWidth()) {
				//Getting difference in width between value box and slider width
				int diff = boxWidth - slider.getWidth();

				//We only need to worry about resizing the parent, CabbageSlider(), as it will automatically resize
				//the actual slider image accordingly.
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

				//-----Assigning a resize flag to the slider so that it can be checked in the else statement that follows.  
				//Components have no ID to begin with.  This will prevent the wrong slider being picked up by the else statement 
				//when they physically overlap. 
				String resizeFlag;
				resizeFlag << 1;
				slider.setComponentID (resizeFlag);
			}

		}
		//----- Else if mouse is not hovering and slider ID is not empty.  This means that the slider had been
		//resized previously, and will therefore enter this else statement to get resized again back to its original
		//dimensions. We only need to worry about resizing the parent, CabbageSlider(), as it will resize the actual
		//slider image accordingly.
		else if ((slider.isMouseOverOrDragging() == false) && (slider.getComponentID().compare ("") != 0)) {
			//Getting original parent dimensions....
			Component* parent;
			parent = slider.getParentComponent();
			int parentWidth = slider.getProperties().getWithDefault(String("origWidth"), -99);
			int parentHeight = slider.getProperties().getWithDefault(String("origHeight"), -99);
			int parentX = slider.getProperties().getWithDefault(String("origX"), -99);
			int parentY = slider.getProperties().getWithDefault(String("origY"), -99);
			//Resizing parent back to original size
			parent->setBounds (parentX, parentY, parentWidth, parentHeight);

			//assigning an empty ID back to the slider
			String emptyStr = String::empty;
			slider.setComponentID (emptyStr); 	
		}

		//----- Creating image
		newThumb = drawLinearThumbImage (destWidth, destHeight, thumbFill, true);
	}
	
	g.drawImage (newThumb, destX, destY, destWidth, destHeight, 0, 0, destWidth, destHeight, false);
}

//======= Toggle Buttons ========================================================================
void CabbageLookAndFeel::drawToggleButton (Graphics &g, ToggleButton &button, bool /*isMouseOverButton*/, bool /*isButtonDown*/)
{
	int destWidth, destHeight;
	destWidth = destHeight = button.getHeight();
	int destX = 0;
	int destY = (button.getHeight() - destHeight) / 2;
	bool isToggleOn;

	if (button.getToggleState() == true)
		isToggleOn = true;
	else
		isToggleOn = false;

	Image newButton = drawToggleImage (button.getHeight(), button.getHeight(), isToggleOn);

	//----- Drawing image
	g.drawImage (newButton, destX, destY, destWidth, destHeight, 0, 0, destWidth, destHeight, false);

	//----- Text
	Justification just (1); //left
	Font font = CabbageUtils::widgetText();
	g.setFont (font);
	g.setColour (Colours::whitesmoke);
	g.setOpacity (0.5);

	String name;
	name << button.getButtonText();
	name = CabbageUtils::cabbageString (name, font, (button.getWidth()-destWidth)); //shortening string if too long

	g.drawText (name, destWidth+1, destY, button.getWidth(), button.getHeight(), just, false);
}

//========= Text Button Background ======================================================================
void CabbageLookAndFeel::drawButtonBackground (Graphics& g, Button& button, const Colour& /*backgroundColour*/, 
																			bool /*isButtonOver*/, 
																			bool isButtonDown)
{
	float width = button.getWidth();
	float height = button.getHeight();

	Image newButton = drawTextButtonImage (width, height, isButtonDown);
	g.drawImage (newButton, 0, 0, width, height, 0, 0, width, height, false);
}

//======= Text Button text =========================================================================
void CabbageLookAndFeel::drawButtonText (Graphics &g, TextButton &button, bool isMouseOverButton, 
																				bool isButtonDown)
{
	float width = button.getWidth();
	float height = button.getHeight();
	float destX;
	float destWidth = width*0.76;
	Font font = CabbageUtils::widgetText();
	String text;
	text << button.getButtonText();
	text = CabbageUtils::cabbageString (text, font, destWidth);
	Justification just (36); //centered

	//----- The text colour and start position change if the button is down or up....
	if (isButtonDown == true) {
		g.setColour (Colours::lime);
		destX = width*0.1;
	}
	else {
		g.setColour (Colours::grey);
		destX = width*0.12;
	}

	g.setFont (font);
	g.drawText (text, destX, (height/2) - (font.getHeight()/2), destWidth, font.getHeight(),
		just, false);
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
	Colour bg = CabbageUtils::componentSkin();
	g.setColour (bg);
	g.fillRoundedRectangle (0, 0, w, h, 5);

	//----- Outline
	g.setColour (Colours::black);
	g.setOpacity (0.2);
	g.drawRoundedRectangle (0.5, 0.5, w-1, h-1, 5, 1);
	
	//----- Text
	String name = group.getText();
	Font font (T("Impact"), 14, 0);
	font.setFallbackFontName (T("Verdana")); //in case the user doesn't have the first font installed
	g.setFont (font);
	Justification just (36);
	g.setColour (Colours::whitesmoke);
	g.setOpacity (0.8);
	name = CabbageUtils::cabbageString (name, font, group.getWidth());
	g.drawText (name, 0, 5, w, 13, just, false);
	g.drawLine (10, 20, w-10, 20, 1);

	//----- Corner holes
	Colour cl = CabbageUtils::backgroundSkin();
	g.setColour (cl);
	g.fillEllipse (3, 3, 6, 6);
	g.fillEllipse (3, h-9, 6, 6);
	g.fillEllipse (w-9, 3, 6, 6);
	g.fillEllipse (w-9, h-9, 6, 6);
}



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




