#include "CabbageLookAndFeel.h"


CabbageLookAndFeel::CabbageLookAndFeel()
{
	setColour(AlertWindow::backgroundColourId, CabbageUtils::getDarkerBackgroundSkin());
	setColour(AlertWindow::textColourId, CabbageUtils::getComponentFontColour());
	setColour(AlertWindow::outlineColourId, Colours::grey);
}

CabbageLookAndFeel::~CabbageLookAndFeel()
{
}

//========= Rotary slider image ==============================================================================
Image CabbageLookAndFeel::drawRotaryImage(int diameter, const Colour circleFill, const Colour trackerCol, float sliderPosProportional, 
																							float zeroPosProportional, 
																							bool useTrackerFill,
																							bool useBigImage)
{
	//A simpler slider is created if the diameter is 25 or less.

	Image img = Image(Image::ARGB, diameter, diameter, true);
	Graphics g (img);

	//----- Calculating number of radians for 300 degrees.  This is because the slider goes from -150 to 150 degrees 
	//where 12 o'clock is zero.
	float numRadians = (300*3.14) / 180; 
	AffineTransform tnsForm = AffineTransform::identity; //this means no transform, or identical

	//----- Outer grey circle and green fill.  Only used if using big slider image.
	if (useBigImage == true) {
		g.setColour (Colour::fromRGBA(150, 165, 170, 150));
		g.drawEllipse (0.5, 0.5, diameter-1, diameter-1, 1);

		if (useTrackerFill == true) {
			Path path; 
			path.addArc (diameter*0.075, diameter*0.075, diameter*0.85, diameter*0.85, -2.6167 + (zeroPosProportional*numRadians), 
													(sliderPosProportional-0.5)*numRadians, true);
			PathStrokeType type (diameter*0.05);
			g.setColour(trackerCol);
			g.setOpacity (0.7);
			g.strokePath (path, type, tnsForm);
		}
	}

	//----- Polygon
	g.setColour (Colour::fromRGBA(0, 0, 0, 150));
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
	g.setColour (Colour::fromRGBA(0, 0, 0, 180)); //for inner shadow
	g.fillEllipse (diameter*0.185, diameter*0.185, diameter*0.65, diameter*0.65); 
	for (float i=0.09; i>0.0; i-=0.01) {
		g.setOpacity (i*2);
		g.fillEllipse (diameter * (0.19+i), diameter * (0.19+i), diameter*0.62, diameter*0.62);
	}

	//Using a colour gradient from white to the chosen colour gives the effect of a light source.  
	float brightness = circleFill.getBrightness();
	if (brightness > 0.9)
		brightness = 0.9;
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
		g.setColour (Colour::fromRGBA (10, 10, 10, 255));
	else
		g.setColour (Colours::whitesmoke);	
	g.fillPath (circleMarker, tnsForm);

	return img;
}

//===== Linear slider bg image =========================================================================
Image CabbageLookAndFeel::drawLinearBgImage (float width, float height, float sliderPosProportional, 
																					float zeroPosProportional,
																					bool useTrackerFill,
																					bool isVertical,
																					const Colour trackerFill)
{
	Image img = Image(Image::ARGB, width, height, true);
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
		g.setColour (Colour::fromRGBA(5, 5, 5, 255));
		g.fillRoundedRectangle (0, height*0.425, width*0.99, height*0.15, height*0.05); //main rectangle




		//----- Green tracker fill. 
		if (useTrackerFill == true) {
			ColourGradient fill;
			if (zeroPosProportional != 0) {
				fill = ColourGradient (trackerFill, 0, 0, trackerFill, width, 0, false);
				fill.addColour(zeroPosProportional, Colours::transparentBlack);
				fill.addColour(zeroPosProportional+0.05, trackerFill);
				fill.addColour(zeroPosProportional-0.05, trackerFill);
			}
			else
				fill = ColourGradient (Colours::transparentBlack, 0, 0, trackerFill, width*0.1, 0, false);

			g.setGradientFill (fill);
			g.setOpacity(0.9);
			g.drawLine (zeroPosProportional*width, height*0.5, sliderPosProportional*width, height*0.5, height*0.05);
		}
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
		g.setColour (Colour::fromRGBA(5, 5, 5, 255));
		g.fillRoundedRectangle (width*0.425, 0, width*0.15, height*0.99, width*0.05); //main rectangle

		//----- Green tracker fill.  
		if (useTrackerFill == true) {
			ColourGradient fill;
			if (zeroPosProportional != 0) {
				fill = ColourGradient (trackerFill, 0, height, trackerFill, 0, 0, false);
				fill.addColour(zeroPosProportional, Colours::transparentBlack);
				fill.addColour(zeroPosProportional+0.05, trackerFill);
				fill.addColour(zeroPosProportional-0.05,trackerFill);
			}
			else
				fill = ColourGradient (Colours::transparentBlack, 0, height, trackerFill, 0, height*0.9, false);

			g.setGradientFill (fill);
			g.setOpacity(0.9);
			sliderPosProportional = 1 - sliderPosProportional; //inverting y axis
			zeroPosProportional = 1 - zeroPosProportional;
			g.drawLine (width*0.5, sliderPosProportional*height, width*0.5, zeroPosProportional*height, width*0.05);
		}
	}

	return img;
}

//========= Linear slider thumb image ====================================================================
Image CabbageLookAndFeel::drawLinearThumbImage (float width, float height, const Colour thumbFill, 
																					bool isVertical)
{
	Image img = Image(Image::ARGB, width, height, true);
	Graphics g (img);

	//----- Setting colour of thumb.
	float brightness = thumbFill.getBrightness();
	if (brightness > 0.9)
		brightness = 0.9;
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

	return img;
}

//========= Toggle Button image ========================================================
Image CabbageLookAndFeel::drawToggleImage (float width, float height, bool isToggleOn, Colour colour, bool isRect)
{
    Image img = Image(Image::ARGB, width, height, true);
    Graphics g (img);
	float opacity = 0;
    //base
    ColourGradient base = ColourGradient (Colours::white, width*-0.3, height*-0.3, Colours::black,
        width*0.8, height*0.8, false);
    g.setGradientFill(base);

	//------------------------------------
	if(isRect){
        g.setColour (Colour::fromRGBA (10, 10, 10, 255));
        g.fillRoundedRectangle (0, 0, width*0.95, height*0.95, height*0.1);

        //----- If "on"
        if (isToggleOn == true) {
                g.setColour (colour);
                g.fillRoundedRectangle (width*0.01, height*0.01, width*0.93, height*0.93, height*0.1);
                opacity = 0.4;
        }
        //----- If "0ff"
        else {
                //----- Shadow
                for (float i=0.01; i<0.05; i+=0.01) {
                        g.setColour (Colour::fromRGBA (0, 0, 0, 255/(i*100)));
                        g.fillRoundedRectangle (width*i, height*i, 
                                width*0.95, height*0.95, height*0.1);
                }
                //----- Filling in the button
                Colour bg1 = Colour::fromRGBA (25, 25, 28, 255);
                Colour bg2 = Colour::fromRGBA (15, 15, 18, 255);
                ColourGradient cg = ColourGradient (bg1, 0, 0, bg2, width*0.5, height*0.5, false);
                g.setGradientFill (cg);
                g.fillRoundedRectangle (width*0.01, height*0.01, width*0.93, height*0.93, height*0.1);
                opacity = 0.2;
        }

        //----- For emphasising the top and left edges to give the illusion that light is shining on them
        ColourGradient edgeHighlight = ColourGradient (Colours::whitesmoke, 0, 0,
                Colours::transparentWhite, 0, height*0.1, false);
        g.setGradientFill (edgeHighlight);
        g.setOpacity (opacity);
        g.fillRoundedRectangle (0, 0, width*0.95, height*0.95, height*0.1);

        ColourGradient edgeHighlight2 = ColourGradient (Colours::whitesmoke, 0, 0,
                Colours::transparentWhite, height*0.1, 0, false);
        g.setGradientFill (edgeHighlight2);
    g.setOpacity (opacity);
        g.fillRoundedRectangle (0, 0, width*0.95, height*0.95, height*0.1);
	}
	else{
	g.fillEllipse (0, 0, width, height);
    
    g.setColour(Colour::fromRGB(70, 70, 70));
    g.fillEllipse(width*0.04, height*0.04, width*0.92, height*0.92);

    //----- If "on"
    if (isToggleOn == true) {
        ColourGradient cg = ColourGradient(colour.withSaturation(0.2), width*0.4, height*0.4, colour,
            width*0.8, height*0.8, true);        
        g.setGradientFill (cg);
		g.fillEllipse(width*0.09, height*0.09, width*0.82, height*0.82);
    }
    //----- If "0ff"
    else {
        g.setColour(Colours::black);
        g.fillEllipse(width*0.09, height*0.09, width*0.82, height*0.82);
    
        ColourGradient cg = ColourGradient (Colours::white, width*0.4, height*0.4, colour.darker(0.9), width*0.3, height*0.3, true);
        g.setGradientFill (cg);
        g.setOpacity(0.4);
        g.fillEllipse(width*0.1, height*0.1, width*0.8, height*0.8);
    }
	}
    return img;	
}

//========= Text button image ========================================================
Image CabbageLookAndFeel::drawTextButtonImage (float width, float height, bool isButtonDown)
{
	Image img = Image(Image::ARGB, width, height, true);
	Graphics g (img);
	float opacity;

	//----- Outline
	g.setColour (Colour::fromRGBA (10, 10, 10, 255));
	g.fillRoundedRectangle (0, 0, width*0.95, height*0.95, height*0.1);

	//----- If "off"
	if (isButtonDown == false) {
		//----- Shadow
		for (float i=0.01; i<0.05; i+=0.01) {
			g.setColour (Colour::fromRGBA (0, 0, 0, 255/(i*100)));
			g.fillRoundedRectangle (width*i, height*i, 
				width*0.95, height*0.95, height*0.1);
			opacity = 0.3;
		}
	}
	else
		opacity = 0.1;

	//----- Filling in the button
	Colour bg1 = Colour::fromRGBA (25, 25, 28, 255);
	Colour bg2 = Colour::fromRGBA (15, 15, 18, 255);
	ColourGradient cg = ColourGradient (bg1, 0, 0, bg2, width*0.5, height*0.5, false);
	g.setGradientFill (cg);
	g.fillRoundedRectangle (width*0.01, height*0.01, width*0.93, height*0.93, height*0.1);

	//----- For emphasising the top and left edges to give the illusion that light is shining on them
	ColourGradient edgeHighlight = ColourGradient (Colours::whitesmoke, 0, 0,
	Colours::transparentWhite, 0, height*0.1, false);
	g.setGradientFill (edgeHighlight);
	g.setOpacity (opacity);
	g.fillRoundedRectangle (0, 0, width*0.95, height*0.95, height*0.1);

	ColourGradient edgeHighlight2 = ColourGradient (Colours::whitesmoke, 0, 0,
		Colours::transparentWhite, height*0.1, 0, false);
	g.setGradientFill (edgeHighlight2);
	g.setOpacity (opacity);
	g.fillRoundedRectangle (0, 0, width*0.95, height*0.95, height*0.1);
	
	return img;
}

/*
	------------------------------------------------------------------------------------------------------
	
	Start of Look and Feel methods

	------------------------------------------------------------------------------------------------------
*/


//=========== Rotary Sliders ==============================================================================
void CabbageLookAndFeel::drawRotarySlider(Graphics& g, int /*x*/, int /*y*/, int /*width*/, int /*height*/,
																	float sliderPosProportional,
																	float /*startAngle*/,
																	float /*endAngle*/,
																	Slider& slider)
{
	String fontColour = slider.getProperties().getWithDefault("fontcolour", "").toString();
	Colour fontcolour;
	if(fontColour.length()>1)
		fontcolour = Colour::fromString(fontColour);
	else
		fontcolour = CabbageUtils::getComponentFontColour();

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
	Font fontName = CabbageUtils::getComponentFont();
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
			g.setColour (fontcolour);
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
			g.setColour (fontcolour);
			g.setFont (fontName);
			g.drawText (name, (slider.getWidth()/2) - (nameWidth/2), 0, (int)nameWidth, 14, justName, false);
			destY += 15; //gap for name label
		}
	}
	
	//----- Getting the final destination dimensions....
	destHeight = sliderBottom - destY;
	destWidth = destHeight;							//because rotary slider images have the same width and height....
	destX = ((slider.getWidth() - destWidth) / 2);	//starting x position

	Colour circleFill = slider.findColour(0x1001200, false);

	//---- Getting the zero position in proportion to the range.  The tracker fill starts at 0, not the minimum...
	float zeroPosProportional = 0;
	if (slider.getMinimum() < 0)
		zeroPosProportional = (slider.getMinimum()*-1) / (slider.getMaximum() - slider.getMinimum());

	//----- If using a tracker fill
	bool useTracker = true;
	String trackColour = slider.getProperties().getWithDefault(String("tracker"), "");
	if(trackColour.length()<2){
		trackColour = Colours::lime.toString();
		useTracker = false;
	}


	//----- Creating slider image
	Image newSlider = drawRotaryImage(destWidth, circleFill, Colour::fromString(trackColour), sliderPosProportional, zeroPosProportional, useTracker, useBigImage);
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
		Font fontValue (CabbageUtils::getValueFont());
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
void CabbageLookAndFeel::drawLinearSliderBackground (Graphics &g, int /*x*/, int /*y*/, int /*width*/, int /*height*/, float /*sliderPos*/, 
                                                                                                float /*minSliderPos*/, 
                                                                                                float /*maxSliderPos*/, 
                                                                                                const Slider::SliderStyle style, 
                                                                                                Slider &slider)
{
    String fontColour = slider.getProperties().getWithDefault("fontcolour", "").toString();
	Colour fontcolour;
	if(fontColour.length()>1)
		fontcolour = Colour::fromString(fontColour);
	else
		fontcolour = CabbageUtils::getComponentFontColour();
	
	float range = slider.getMaximum() - slider.getMinimum();

    //----- Getting the proportional position that the current value has in relation to the range
    float sliderPosProportional = (slider.getValue()-slider.getMinimum()) / range;
    
    //---- Getting the zero position in proportion to the range.  The tracker fill starts at 0, not the minimum...
    float zeroPosProportional = 0; 
    if (slider.getMinimum() < 0)
        zeroPosProportional = (abs(slider.getMinimum())) / range;

    Image newBackground;

    float destX, destY, sliderEnd;
    float destHeight = slider.getHeight(); 
    float destWidth = slider.getWidth();
    
    //----- Determining if the user wants a name label displayed......
    bool showName = false;
    String name;
    float nameWidth;
    if (slider.getName().length() > 0) {
        name << slider.getName();
        g.setFont (CabbageUtils::getComponentFont());
        g.setColour (fontcolour);
        showName = true; //setting flag to true
    }

	//----- If using a tracker fill
	bool useTracker = true;
	String trackColour = slider.getProperties().getWithDefault(String("tracker"), "");
	if(trackColour.length()<2){
		trackColour = Colours::lime.toString();
		useTracker = false;
	}

    //----- If Horizontal Slider --------------------------------------------------------------------------
    if (style == Slider::LinearHorizontal) {
		//for name alignment
		String alignment = slider.getProperties().getWithDefault("align", "").toString();
		Justification just(1);
		if (alignment.length()>1) {
			if (alignment.compare("left") == 0)
				just.left;
			else if (alignment.compare("right") == 0)
				just.right;
			else if (alignment.compare("centre") == 0)
				just.centred;
		}

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
    
        destY = 0;

        //----- If there is a name label it will be shown at the start while the value will be at the end when 
        //the mouse hovers over the slider. 
        if (showName == true) { //if the name should be displayed
            name = CabbageUtils::cabbageString (name, CabbageUtils::getComponentFont(), nameWidth); //shortening string if too long
			g.drawText (name, 0, (slider.getHeight()/2) - (CabbageUtils::getComponentFont().getHeight()/2), 
				(int)nameWidth, CabbageUtils::getComponentFont().getHeight(), just, false);
            destX += nameWidth; //adjusting destX to accomodate slider name
        }

        //----- If textbox IS NOT to be displayed!!!!!! 
        if (slider.getTextBoxPosition() == Slider::NoTextBox) 
            slider.setTextBoxStyle (Slider::NoTextBox, true, 0, 0);

        //----- If textbox IS to be displayed!!!!  
        else {
            slider.setTextBoxStyle (Slider::TextBoxRight, false, len, 15);
            sliderEnd -= len;
        }

        destWidth = sliderEnd - destX; //destination width of image

        //----- Slider is enabled and value changed if mouse click is within the actual slider image....
        if (slider.isMouseButtonDown() == true) {
            Point<int> mousePos = slider.getMouseXYRelative();
            slider.setEnabled(true);
            if ((mousePos.getX() >= (destX-5)) && (mousePos.getX() <= (sliderEnd+5)))
                slider.setValue((((mousePos.getX()-destX)/destWidth) * range) + slider.getMinimum());
        }
        
        slider.setEnabled (false); //setting it back to disabled. Otherwise the slider would move slightly.
        
        //----- Getting image
		newBackground = drawLinearBgImage (destWidth, destHeight, sliderPosProportional, zeroPosProportional, useTracker, false, Colour::fromString(trackColour));
    }

    //----- If Vertical Slider ---------------------------------------------------------------------------
    if (style == Slider::LinearVertical) {
		nameWidth = slider.getWidth();
        destX = ((slider.getWidth() - destWidth) / 2);    
        //----- starting y position, width*0.25 is to make room for the edge of the thumb when at a 
        //maximum or minimum
        destY = slider.getWidth()*0.25; 
        sliderEnd = slider.getHeight() - destY;

        //----- If textbox IS NOT to be displayed!!!!!! 
        //If there is a name label it will be shown at the bottom while the value will be at the top when 
        //the mouse hovers over the slider. 
        if (slider.getTextBoxPosition() == Slider::NoTextBox) {
            slider.setTextBoxStyle (Slider::NoTextBox, true, 0, 0);
            if (showName == true) { //if the name should be displayed
                name = CabbageUtils::cabbageString (name, CabbageUtils::getComponentFont(), slider.getWidth()); 
                g.drawText (name, (slider.getWidth()/2) - (nameWidth/2), slider.getHeight() - 13, 
                                                            (int)nameWidth, 13, 36, false);
                sliderEnd -= 21; //name label(11) + space(10) between image and text
            }
            destY += 15; //value(15)
        }

        //----- If textbox IS to be displayed!!!!  
        //This means that if the user wants a label it will automatically go at the top. 
        else {
            String str;
            str << slider.getMaximum() << slider.getInterval();
            int len = str.length() * 7; //length of text
            slider.setTextBoxStyle (Slider::TextBoxBelow, false, len, 15);
            sliderEnd -= 30; //textbox height(15) plus gap(15)

            if (showName == true) { //if name label is to be displayed
                name = CabbageUtils::cabbageString (name, CabbageUtils::getComponentFont(), slider.getWidth());
                g.drawText (name, (slider.getWidth()/2) - (nameWidth/2), 0, (int)nameWidth, 13, 36, false);
                destY += 15; //name(15)
                //sliderEnd -= 0; Having a textbox will automatically adjust slider.getHeight() so no need to 
                //subtract anything here
            }
        }

        destHeight = sliderEnd - destY; //destination height of image

        //----- Slider is enabled and value changed if mouse click is within the actual slider image....
        if (slider.isMouseButtonDown() == true) {
            Point<int> mousePos = slider.getMouseXYRelative();
            slider.setEnabled (true);
            if ((mousePos.getY() >= (destY-5)) && (mousePos.getY() <= (sliderEnd+5))) {
                float yInvert = 1 - ((mousePos.getY()-destY) / destHeight);
                slider.setValue ((yInvert*range)+slider.getMinimum());
            }
        }
        slider.setEnabled (false); //setting it back to disabled. Otherwise the slider would move slightly.

        //----- Getting image
		newBackground = drawLinearBgImage (destWidth, destHeight, sliderPosProportional, zeroPosProportional, useTracker, true, Colour::fromString(trackColour));
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

	Colour thumbFill = slider.findColour(Slider::thumbColourId, false);
	Image newThumb;


	//----- The following determines if slider value should be displayed, in the event of no textbox.  It 
	//also calculates the string sliderValue and the width of the string, as these are used later when drawing 
	//the value...
	String sliderValue;
	float strWidth;
	Font fontValue (CabbageUtils::getValueFont());
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
		Font fontName = CabbageUtils::getComponentFont();
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
	destHeight = button.getHeight();
	if (button.getButtonText().length() > 0)
		destWidth = destHeight;
	else
		destWidth = button.getWidth();

	int destX = 0;
	int destY = 0;
	bool isToggleOn;

	if (button.getToggleState() == true)
		isToggleOn = true;
	else
		isToggleOn = false;

	//----- Getting the colours...
	Colour col, fcol;
	if (button.getProperties().getWithDefault("colour", "").toString().length()<1){
        for (float i=0.01; i<0.03; i+=0.01)
			col = Colour::fromRGBA (0, 0, 0, 255/(i*100));
    }
    else
		col = Colour::fromString(button.getProperties().getWithDefault("colour", "lime"));
	
	if (button.getProperties().getWithDefault("fontcolour", "").toString().length()<2){
		fcol = CabbageUtils::getComponentFontColour();
    }
    else
		fcol = Colour::fromString(button.getProperties().getWithDefault("fontcolour", "lime"));



	bool isRECT = button.getProperties().getWithDefault("isRect", 0);

	//----- Creating the image
	Image newButton = drawToggleImage (destWidth, destHeight, isToggleOn, col, isRECT);

	//----- Drawing image
	g.drawImage (newButton, destX, destY, destWidth, destHeight, 0, 0, destWidth, destHeight, false);

	//----- Text
	if (button.getButtonText().length() > 0) {
		Justification just (1); //left
		g.setFont (CabbageUtils::getComponentFont());
		g.setColour (fcol);

		String name;
		name << button.getButtonText();
		name = CabbageUtils::cabbageString (name, CabbageUtils::getComponentFont(), (button.getWidth()-(destWidth+5))); //shortening string if too long

		g.drawText (name, destWidth+5, destY, button.getWidth(), button.getHeight(), just, false);
	}
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
	float destWidth = width*0.8;
	Font font = CabbageUtils::getComponentFont();
	String text;
	text << button.getButtonText();
	text = CabbageUtils::cabbageString (text, font, destWidth);
	Justification just (36); //centered
	float destY = (height*0.5) - (font.getHeight()*0.5);
	
	Colour textColour;
	String col = button.getProperties().getWithDefault("fontcolour", "");
	if(col.length()>2)
		textColour = Colour::fromString(col);
	else
		textColour = CabbageUtils::getComponentFontColour();

	//----- The text colour and start position change if the button is down or up....
	if (isButtonDown == true) {
		g.setColour (textColour.brighter(0.3));
		destX = width*0.095;
	}
	else {
		g.setColour (textColour);
		destX = width*0.1;
	}

	g.setFont (font);
	g.drawText (text, destX, destY, destWidth, font.getHeight(),
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
	//----- If using a tracker fill
	String bgColour = box.getProperties().getWithDefault(String("colour"), "");
	String fontColour = box.getProperties().getWithDefault(String("fontcolour"), "");
	Colour bgCol, fCol;
	if(bgColour.length()>2){
		bgCol = Colour::fromString(bgColour);
	}
	else
		bgCol = CabbageUtils::getBackgroundSkin();

	if(fontColour.length()<2){
		fCol = Colour::fromString(fontColour);
	}

	g.setColour(bgCol);
	g.fillRoundedRectangle (0, 0, width, height, height*0.1);
	
	//----- The box that contains the arrow
	g.setColour(Colour::fromRGBA (10, 10, 10, 255));
	float arrowBoxWidth;
	if (width >= 40)
		arrowBoxWidth = 20;
	else
		arrowBoxWidth = width/2;
	g.fillRoundedRectangle (width-arrowBoxWidth, 0, arrowBoxWidth, height, height*0.1);

	//----- Border outline
	g.drawRoundedRectangle (0.5, 0.5, width-1, height-1, height*0.1, 1);

	//----- Arrow
	box.setColour (0x1000a00, CabbageUtils::getComponentFontColour());
	g.setColour(CabbageUtils::getComponentFontColour());
	const Line<float> line(width-(arrowBoxWidth/2), height*0.25, width-(arrowBoxWidth/2), height*0.75); 
	g.drawArrow(line, 0, arrowBoxWidth/2, height*0.5);	
}


//============= TextEditor Outline ======================================================================
void CabbageLookAndFeel::drawTextEditorOutline (Graphics &g, int width, int height, TextEditor &/*textEditor*/)
{
	g.setColour (Colours::transparentBlack);
	g.drawRoundedRectangle (0, 0, width, height, height/3, 1);
}



//============= TextEditor Background ==============================================================
void CabbageLookAndFeel::fillTextEditorBackground (Graphics &g, int width, int height, TextEditor &textEditor)
{
	textEditor.setColour (0x1000200, Colours::transparentBlack); //this gets rid of the square rectangle background
	//g.setColour (CabbageUtils::getBackgroundSkin());
	//g.fillRoundedRectangle (0, 0, width, height, height/3);
}


//=========== Labels, slider textboxes are also labels =============================================
void CabbageLookAndFeel::drawLabel (Graphics &g, Label &label)
{	
	//----- Getting parent component of label, if it is a slider then the background will
	//be a rectangle with rounded corners.  The background colour is retrieved from the initialisation
	//of the slider / label.  Otherwise it will be normal.
	Component* comp = label.getParentComponent();

	g.setFont (CabbageUtils::getComponentFont());

	if (dynamic_cast<Slider*>(comp)) { //If slider
	//----- Drawing rounded rectangle
		Colour cl = label.findColour (0x1000280, true); //getting background colour specified
		g.setColour (cl);
		//if the colour is white then no opacity is used
		if (cl != Colours::white) 
			g.setOpacity (0.7);	

		g.fillRoundedRectangle (0, 0, label.getWidth(), label.getHeight(), label.getHeight()/3);

		//----- For the text. If the background is brighter than 0.6 then the text is black.
		Justification just(36);	//justification of text
		if (cl.getBrightness() > 0.6) 
			g.setColour (Colours::black);
		else 
			g.setColour (CabbageUtils::getComponentFontColour());

		g.drawText (label.getText(), 0, 0, label.getWidth(), label.getHeight(), just, false);
	}
	else { //If not a slider
		g.setColour (Colours::transparentBlack);
		g.fillRoundedRectangle (0, 0, label.getWidth(), label.getHeight(), label.getHeight()/3);

		//----- For the text
		Justification just(36);
		String col = label.getProperties().getWithDefault("textColour", "");
		if(col.length()>0){
			if(col=="black")
			g.setColour (Colours::black);
			else
			g.setColour(Colour::fromString(col));
		}

		else
		g.setColour (CabbageUtils::getComponentFontColour());

		g.drawText (label.getText(), 0, 0, label.getWidth(), label.getHeight(), just, false);
	}
}


//======== Group Components ======================================================================
void CabbageLookAndFeel::drawGroupComponentOutline (Graphics &g, int w, int h, const String &text, 
																		const Justification &position, 
																		GroupComponent &group)
{
	//----- Background
	Colour col;
	String bgColour = group.getProperties().getWithDefault("colour", "").toString();
	String fontColour = group.getProperties().getWithDefault("fontcolour", "").toString();
	//Logger::writeToLog(bgColour);
	//Logger::writeToLog(fontColour);

	if (bgColour.length()>1){
		col = Colour::fromString(bgColour);
		g.setColour (col);
	}
	else
		g.setColour (CabbageUtils::getComponentSkin());

	g.fillRoundedRectangle (0, 0, w, h, 5);

	//----- Outline
	g.setColour (CabbageUtils::getComponentFontColour());
	g.setOpacity (0.1);
	g.drawRoundedRectangle (0.5, 0.5, w-1, h-1, 5, 1);
	
	//----- Text
	String name = group.getText();
	Font font = CabbageUtils::getTitleFont();
	font.setFallbackFontName ("Verdana"); //in case the user doesn't have the first font installed
	g.setFont (font);

	if (fontColour.length()>1){
		col = Colour::fromString(fontColour);
		g.setColour (col);
	}
	else 
		g.setColour (CabbageUtils::getComponentFontColour());

	name = CabbageUtils::cabbageString (name, font, group.getWidth());
	g.drawText (name, 0, 5, w, font.getHeight(), 36, false);
	if(!group.getProperties().getWithDefault("groupLine", 0).equals(var(0))){
	g.drawLine (10, 20, w-10, 20, 0.2);
	}
	//----- Corner holes
	g.setColour (CabbageUtils::getBackgroundSkin());
	g.setOpacity(0.7);
	g.fillEllipse (2, 2, 4, 4); //top-left
	g.fillEllipse (2, h-6, 4, 4); //bottom-left
	g.fillEllipse (w-6, 2, 4, 4); //top-right
	g.fillEllipse (w-6, h-6, 4, 4); //bottom-right
	
}

//======== Scrollbars ==============================================================================
void CabbageLookAndFeel::drawScrollbar (Graphics &g, ScrollBar &scrollbar, int x, int y, int width, 
																						int height, 
																						bool isScrollbarVertical, 
																						int thumbStartPosition, 
																						int thumbSize, 
																						bool isMouseOver, 
																						bool isMouseDown)
{
	g.setColour (Colours::transparentBlack);
	g.fillAll();

	g.setColour (CabbageUtils::getComponentSkin());
	g.drawRoundedRectangle (x, y, width, height, 5, 1);

	if (isScrollbarVertical == false) //horizontal
		g.fillRoundedRectangle (thumbStartPosition+3, 3, thumbSize-6, height-6, 5);
	else //vertical
		g.fillRoundedRectangle (3, thumbStartPosition+3, width-6, thumbSize-6, 5);

}

//======= Scrollbar buttons =======================================================================
void CabbageLookAndFeel::drawScrollbarButton (Graphics &g, ScrollBar &scrollbar, int width, int height, 
																					int buttonDirection, 
																					bool isScrollbarVertical, 
																					bool isMouseOverButton, 
																					bool isButtonDown)
{
	g.setColour (Colours::transparentBlack);
	g.fillAll();

	if (isButtonDown == true)
		g.setColour (CabbageUtils::getComponentSkin().brighter(0.5f));
	else
		g.setColour (CabbageUtils::getComponentSkin());

	if (buttonDirection == 3) { //left arrow
		Line<float> left (width*0.8, height/2, width*0.2, height/2);
		g.drawArrow (left, 0, 10, 10); 
	}
	else if (buttonDirection == 1) { //right arrow
		Line<float> right (width*0.2, height/2, width*0.8, height/2);
		g.drawArrow (right, 0, 10, 10); 
	}
	else if (buttonDirection == 0) { //up arrow
		Line<float> up (width/2, height*0.8, width/2, height*0.2);
		g.drawArrow (up, 0, 10, 10); 
	}
	else if (buttonDirection == 2) { //down arrow
		Line<float> down (width/2, height*0.2, width/2, height*0.8);
		g.drawArrow (down, 0, 10, 10); 
	}
	

}

//======== Popup Menu background ======================================================================
void CabbageLookAndFeel::drawPopupMenuBackground(Graphics &g, int width, int height)
{
	g.setColour (CabbageUtils::getDarkerBackgroundSkin());
    g.fillAll();

	g.setColour (CabbageUtils::getComponentSkin());
	g.drawRect (0, -5, width, height+5, 1); //dont want to see top line
}

//====== Returns image of a check mark ==============================================
Image CabbageLookAndFeel::drawCheckMark()
{
	Image img = Image(Image::ARGB, 10, 10, true);
	Graphics g(img);

	Path path;
	path.startNewSubPath(3, 7);
	path.lineTo(5, 10);
	path.lineTo(10, 0);
	g.setColour (Colours::cornflowerblue);
	g.strokePath(path, 2.0f);

	return img;
}

//======== Popup Menu Items ===========================================================================
void CabbageLookAndFeel::drawPopupMenuItem (Graphics &g, int width, int height, bool isSeparator, bool /*isActive*/, 
																								bool isHighlighted, 
																								bool isTicked, 
																								bool hasSubMenu, 
																								const String &text, 
																								const String &/*shortcutKeyText*/, 
																								Image */*image*/, 
																								const Colour */*const textColour*/)
{
	if ((isHighlighted == true) && (isSeparator == false)) {
		g.setColour (CabbageUtils::getComponentSkin());
		g.fillAll();
		g.setColour (Colours::cornflowerblue);
	}
	else
		g.setColour (CabbageUtils::getComponentFontColour());

	g.setFont (CabbageUtils::getComponentFont());
	g.drawText (CabbageUtils::cabbageString(text, CabbageUtils::getComponentFont(), width*0.8), 20, 0, width*0.8, height, 1, false);

	if (isSeparator == true) {
        g.setColour(CabbageUtils::getComponentSkin());
        g.drawLine(0, height/2, width, 3);
    }

	if (isTicked) {
		Image checkMark = drawCheckMark();
		g.setColour(Colours::cornflowerblue);
		g.drawImage(checkMark, 5, (height/2)-5, 10, 10, 0, 0, 10, 10, false);	
	}
	if (hasSubMenu) {
		g.setColour(Colours::cornflowerblue);
		const Line<float> line(width-15, height*.5, width-5, height*.5);
		g.drawArrow(line, 0, height*.3, height*.3);
	}
}

//======== Menubar background ======================================================================
void CabbageLookAndFeel::drawMenuBarBackground(Graphics &g, int width, int height, bool isMouseOverBar, MenuBarComponent &menuBar)
{
	g.setColour (CabbageUtils::getBackgroundSkin());
	g.fillAll();
}

//======== Menubar item background ======================================================================
void CabbageLookAndFeel::drawMenuBarItem(Graphics & g, int width, int height, int itemIndex,
                                                                              const String &itemText, 
																			  bool isMouseOverItem,
                                                                              bool isMenuOpen, 
																			  bool isMouseOverBar,
                                                                              MenuBarComponent &menuBar)
{
	if ((isMouseOverItem == true) || (isMenuOpen == true)) {
		g.setColour (CabbageUtils::getBackgroundSkin().darker (0.2));
		g.fillRect (0, 0, width, height);
	}

	g.setColour (CabbageUtils::getComponentFontColour());
	g.setFont (CabbageUtils::getComponentFont());
    g.drawFittedText(CabbageUtils::cabbageString(itemText, CabbageUtils::getComponentFont(), width*0.9), 5, 0, width, height, 36, 1);
}

//======== Document Window title bar ===================================================================
void CabbageLookAndFeel::drawDocumentWindowTitleBar (DocumentWindow &window, Graphics &g, int w, int h, 
																						int /*titleSpaceX*/, 
																						int titleSpaceW, 
																						const Image */*icon*/, 
																						bool /*drawTitleTextOnLeft*/)
{
	window.setUsingNativeTitleBar(false);

	g.setColour (CabbageUtils::getDarkerBackgroundSkin());
	g.fillAll();

	g.setColour (CabbageUtils::getComponentFontColour());
	Font font = CabbageUtils::getTitleFont();
	font.setFallbackFontName("Verdana"); 
	font.setHeight(16);
	g.setFont (font);
	g.drawText (CabbageUtils::cabbageString(window.getName(), font, titleSpaceW), (w/2)-(titleSpaceW/2), 
									(h/2)-(font.getHeight()/2), titleSpaceW, font.getHeight(), 36, false);
}

//====== Draw Window Button Normal Image =================================================================
Image CabbageLookAndFeel::drawWindowButtonNormal(int buttonType)
{
	int width, height;
	width = height = 20;
	Image img = Image(Image::ARGB, width, height, true);
	Graphics g (img);

	String str;

	if (buttonType == 1) 
		str << "_";
	else if (buttonType == 2)
		str << "+";
	else if (buttonType == 4) 
		str << "x";

	//----- Background
	g.setColour (Colours::transparentBlack);
	g.fillRoundedRectangle(1, 1, width-2, height-2, 2);
	
	//----- Text symbol
	Font font = CabbageUtils::getComponentFont();
	g.setFont (font);
	g.setColour (CabbageUtils::getComponentFontColour());
	g.drawText(str, (width/2) - (font.getHeight()/2), (width/2) - (font.getHeight()/2), 
		font.getHeight(), font.getHeight(), 36, false);

	return img;
}

//====== Draw Window Button Image Is Over ==============================================================
Image CabbageLookAndFeel::drawWindowButtonIsOver(int buttonType)
{
	int width, height;
	width = height = 20;
	Image img = Image(Image::ARGB, width, height, true);
	Graphics g (img);

	String str;

	if (buttonType == 1) 
		str << "_";
	else if (buttonType == 2)
		str << "+";
	else if (buttonType == 4) 
		str << "x";

	//---- Background
	g.setColour (Colours::transparentBlack);
	g.fillRoundedRectangle(1, 1, width-2, height-2, 2);

	//---- Text symbol
	Font font = CabbageUtils::getComponentFont();
	g.setFont (font);
	g.setColour (Colours::whitesmoke);
	g.drawText(str, (width/2) - (font.getHeight()/2), (width/2) - (font.getHeight()/2), 
		font.getHeight(), font.getHeight(), 36, false);

	//---- V lines
	ColourGradient cg = ColourGradient(Colours::transparentBlack, 0, 0, Colours::transparentBlack,
		0, height, false);
	cg.addColour (0.5, CabbageUtils::getComponentFontColour());
	g.setGradientFill (cg);
	g.drawLine (0, 0, 0, height, 1);
	g.drawLine (width, 0, width, height, 1);

	return img;
}

//========= Document Window Buttons =======================================================================
Button* CabbageLookAndFeel::createDocumentWindowButton (int buttonType)
{
	ImageButton* button;
	button = new ImageButton("Button");
	Image normalImage, isOverImage;

	normalImage = drawWindowButtonNormal(buttonType);
	isOverImage = drawWindowButtonIsOver(buttonType);

	button->setImages(true, false, true, normalImage, 1, Colours::transparentBlack, isOverImage,
		1, Colours::transparentBlack, isOverImage, 1, Colours::transparentBlack, 0);

	
	return button;
}

/*
  =========================================================================================================
	
	Alternative Look and Feel Class

  ---------------------------------------------------------------------------------------------------------
	
  =========================================================================================================
*/
CabbageLookAndFeelBasic::CabbageLookAndFeelBasic()
{
}

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




