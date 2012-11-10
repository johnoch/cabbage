
#include "XYPad.h"

/*
  ====================================================================================

	XY Handle Component

  ====================================================================================
*/
XYHandleComponent::XYHandleComponent()
{
}

XYHandleComponent::~XYHandleComponent()
{
}

XYCanvas* XYHandleComponent::getParentComponent()
{
	return (XYCanvas*)Component::getParentComponent();
}

void XYHandleComponent::paint (Graphics& g)
{
	g.setColour (Colours::cornflowerblue);
	g.setOpacity(0.5);
	g.fillEllipse (getWidth()*0.25, getHeight()*0.25, getWidth()*0.5, getHeight()*0.5);
	g.setColour (Colours::aqua.withMultipliedSaturation(0.5));
	g.fillEllipse (getWidth()*0.4, getHeight()*0.4, getWidth()*0.2, getHeight()*0.2);
}

void XYHandleComponent::mouseEnter (const MouseEvent& e)
{
	setMouseCursor (MouseCursor::DraggingHandCursor);
}

void XYHandleComponent::mouseDown (const MouseEvent& e)
{
	setMouseCursor (MouseCursor::DraggingHandCursor);
	dragger.startDraggingComponent (this, e);
}

void XYHandleComponent::mouseDrag (const MouseEvent& e)
{
	//checking constraints so that the handle isn't dragged outside of its parent
	ScopedPointer<ComponentBoundsConstrainer> constrainer = new ComponentBoundsConstrainer();
	constrainer->setMinimumOnscreenAmounts (0xffffff, 0xffffff, 
		0xffffff, 0xffffff);

	dragger.dragComponent (this, e, constrainer);
	getParentComponent()->updatePath();
	getParentComponent()->repaint();
}

/*
  ============================================================================

   XYToggle

  ============================================================================
*/
XYToggle::XYToggle(Image inputImage, Colour col)
	: img(inputImage), colourWhenOn(col)
{
}

XYToggle::~XYToggle()
{
}

void XYToggle::paintButton (Graphics& g, bool isMouseOverButton, bool isButtonDown)
{
	g.setColour(CabbageUtils::getDarkerBackgroundSkin());
	g.fillRoundedRectangle(0, 0, getWidth(), getHeight(), getHeight()/5);

	
	if (this->getToggleState() == true)
		//g.setColour(Colours::cornflowerblue);
		g.setColour(colourWhenOn);
	else {
		if (isMouseOverButton)
			g.setColour(CabbageUtils::getComponentSkin().withMultipliedBrightness(2));
		else
			g.setColour(CabbageUtils::getComponentSkin());
	}
	g.drawImage(img, 0, 0, getWidth(), getHeight(), 0, 0, img.getWidth(), img.getHeight(), true);
	g.setOpacity(0.7);
	g.drawRoundedRectangle(0.5, 0.5, getWidth()-1, getHeight()-1, getHeight()/5, 1);
	
}

/*
  ============================================================================

   XYValueDisplay Component

  ============================================================================
*/
XYValueDisplay::XYValueDisplay(Colour col)
	: colour(col)
{
}

XYValueDisplay::~XYValueDisplay()
{
}

void XYValueDisplay::setValue (String val)
{
	value = val;
	repaint();
}

void XYValueDisplay::paint (Graphics& g)
{
	Font font = (CabbageUtils::getValueFont());
	g.setFont (font);
	g.setColour (colour);
	g.setOpacity(0.6);
	value = CabbageUtils::cabbageString(value, font, getWidth());
	g.drawText (value, 0, 0, getWidth(), getHeight(), Justification::centred, false);
}


/*
  ============================================================================

   XYCanvas

  ============================================================================
*/
XYCanvas::XYCanvas(Colour ballColour, float ballSize, float xMinimum, float xMaximum, float yMinimum, float yMaximum)	
																									: col(ballColour), 
																									ballSize(ballSize),
																									xMin(xMinimum),
																									xMax(xMaximum),
																									yMin(yMinimum),
																									yMax(yMaximum)
{
	//making the overall handle component the same size as the ball.  
	//This makes it easier for controlling bounds.
	handleSize = ballSize;
	paintStaticBall = true;

	xRange = xMax-xMin;
	yRange = yMax-yMin;
}

XYCanvas::~XYCanvas()
{
}

void XYCanvas::resized()
{
	cacheBackgroundImage();
}

XYPad* XYCanvas::getParentComponent()
{
	return (XYPad*)Component::getParentComponent();
}

void XYCanvas::cacheBackgroundImage()
{
	Image img = Image::Image(Image::ARGB, getWidth(), getHeight(), true);
	Graphics g(img);

	g.setColour (CabbageUtils::getBackgroundSkin());
	g.fillRoundedRectangle (0, 0, getWidth(), getHeight(), 5);

	// Grid lines
	ColourGradient vGradient = ColourGradient(Colours::transparentBlack, 0, 0, 
		Colours::transparentBlack, 0, getHeight(), false);
	vGradient.addColour(0.5, CabbageUtils::getComponentSkin());
	g.setGradientFill(vGradient);
	g.drawLine(getWidth()/2, 0, getWidth()/2, getHeight(), 1);

	ColourGradient hGradient = ColourGradient(Colours::transparentBlack, 0, 0, 
		Colours::transparentBlack, getWidth(), 0, false);
	hGradient.addColour(0.5, CabbageUtils::getComponentSkin());
	g.setGradientFill(hGradient);
	g.drawLine(0, getHeight()/2, getWidth(), getHeight()/2, 1);

	ImageCache::addImageToCache(img, 27);
	bg = ImageCache::getFromHashCode(27);
}


void XYCanvas::paint(Graphics& g)
{	
	g.drawImage(bg, 0, 0, getWidth(), getHeight(), 0, 0, bg.getWidth(), bg.getHeight(), false);	
	
	// Ball path
	if ((path.getLength() > 0) && (pathOpacity > 0)) {
		if (toggleId == 0) {
		ColourGradient cg = ColourGradient (Colours::transparentBlack, path.getPointAlongPath(0).getX(), 
			path.getPointAlongPath(0).getY(),
			col.withMultipliedSaturation(5), path.getPointAlongPath(path.getLength()).getX(), 
			path.getPointAlongPath(path.getLength()).getY(), false);
		g.setGradientFill(cg);
		}
		else
			g.setColour(col.withMultipliedSaturation(5));
		
		g.setOpacity (pathOpacity);
		g.strokePath (path, pathThickness);
	}

	// Ball h and v lines
	if (ballLineOpacity > 0) {
		ColourGradient vLineCg = ColourGradient(Colours::transparentBlack, 0, 0, Colours::transparentBlack,
			0, getHeight(), false);
		vLineCg.addColour((ballY+(ballSize/2)) / getHeight(), col);
		g.setGradientFill(vLineCg);
		g.setOpacity(ballLineOpacity);
		g.drawLine(ballX+ballSize/2, 0, ballX+ballSize/2, getHeight(), ballLineOpacity);

		ColourGradient hLineCg = ColourGradient(Colours::transparentBlack, 0, 0, Colours::transparentBlack,
			getWidth(), 0, false);
		hLineCg.addColour((ballX+(ballSize/2)) / getWidth(), col);
		g.setGradientFill(hLineCg);
		g.setOpacity(ballLineOpacity);
		g.drawLine(0, ballY+ballSize/2, getWidth(), ballY+ballSize/2, ballLineOpacity);
	}

	// Ball shadow
	g.setColour(Colour::fromFloatRGBA(0, 0, 0, 100));
	g.fillEllipse(ballX+ballSize*0.12, ballY+ballSize*0.12, ballSize, ballSize);

	// Ball
	g.setColour (col);
	if (paintStaticBall) 
		g.fillEllipse (ballX, ballY, ballSize, ballSize);
	else {
		g.setOpacity(0.5);
		g.fillEllipse (ballX+(ballSize*0.3), ballY+(ballSize*0.3), 
			ballSize*0.4, ballSize*0.4);
		g.setColour (col.withMultipliedSaturation(5));
		g.drawEllipse (ballX+ballSize*0.05, ballY+ballSize*0.05,
			ballSize*0.9, ballSize*0.9, ballSize*0.1);
	}
}

void XYCanvas::mouseEnter (const MouseEvent& e)
{
	setMouseCursor(MouseCursor::CrosshairCursor);
}

void XYCanvas::mouseDown (const MouseEvent& e)
{
	setMouseCursor(MouseCursor::NoCursor);
	ballLineOpacity = 0.5;
}

void XYCanvas::mouseDrag (const MouseEvent& e)
{
	setMouseCursor(MouseCursor::NoCursor);
}

void XYCanvas::mouseUp (const MouseEvent& e)
{
	setMouseCursor(MouseCursor::CrosshairCursor);
	startTimer(1, 150); //ball path
	startTimer(2, 250);	//ball cross hair lines
}

Point<float> XYCanvas::checkBounds(Point<float> pt)
{
	//Checks that the pt(x,y) is inside the available bounds
	if (pt.getX() < ballSize/2)
		pt.setX (ballSize/2);
	else if (pt.getX() > (getWidth()-(ballSize/2)))
		pt.setX (getWidth() - (ballSize/2));
	if (pt.getY() < ballSize/2)
		pt.setY (ballSize/2);
	else if (pt.getY() > (getHeight()-(ballSize/2)))
		pt.setY (getHeight() - (ballSize/2));
		
	return pt;
}

void XYCanvas::setBallPosition (Point<float> pt)
{
	Point<float> pos = checkBounds(pt);
	ballX = pos.getX()-(ballSize/2);
	ballY = pos.getY()-(ballSize/2);
	repaint();
}

void XYCanvas::setBallPositionFromXYValues(float xValue, float yValue)
{
	//Sets the ball position from the x and y output values
	ballX = (((xValue-xMin)/xRange)*(getWidth()-ballSize));
	ballY = (((yValue-yMin)/yRange)*(getHeight()-ballSize));
	repaint();
}

float XYCanvas::getBallX()
{
	return ballX;
}

float XYCanvas::getBallY()
{
	return ballY;
}

XYHandleComponent* XYCanvas::makeHandle(Point<float> pt)
{
	pt = checkBounds(pt);

	XYHandleComponent* handle = new XYHandleComponent();
	handle->setBounds (pt.getX()-(handleSize/2), pt.getY()-(handleSize/2), 
		handleSize, handleSize); //their centre will be where the mouse was clicked
	if (toggleId == 1)
		addAndMakeVisible (handle); //only visible if not in normal mode
	repaint();
	return handle;
}

void XYCanvas::setStartHandle (Point<float> pt)
{
	handles.set (0, makeHandle (pt)); //'set'replaces element
}

void XYCanvas::setEndHandle (Point<float> pt)
{
	handles.set (1, makeHandle (pt)); 
	pathOpacity = 1;
	pathThickness = 2;
	updatePath();
}

void XYCanvas::clearHandles()
{
	handles.clear(true);
	updatePath();
}

void XYCanvas::updatePath()
{
	path.clear();
	if (handles.size() > 1) { //should be at least 2 handles
		path.startNewSubPath (handles[0]->getX()+(handleSize/2), handles[0]->getY()+(handleSize/2));
		path.lineTo (handles[1]->getX()+(handleSize/2), handles[1]->getY()+(handleSize/2));
		getParentComponent()->xyPadAutomation->setBallPath (path);
	}
	repaint();	
}

void XYCanvas::setToggleId(int id)
{
	toggleId = id; 
}

void XYCanvas::useStaticBall(bool useStaticBall)
{
	paintStaticBall = useStaticBall;
}

void XYCanvas::startBallPathTimer()
{
	// This is called by XYPad upon re-instantiation if type 2 automation is on.
	startTimer(1, 150);
}

/*
  ============================================================================

   XYPad

  ============================================================================
*/

XYPad::XYPad(XYPadAutomation* xyPadAutomation, String title, int minXValue, int maxXValue, 
																			int minYValue, 
																			int maxYValue, 
																			int numberOfDecimalPlaces,
																			Colour ballColour, 
																			Colour fontColour)
																			:
																			title(title),
																			ballColour(ballColour),
																			fontColour(fontColour),
																			decimalPlaces(numberOfDecimalPlaces),
																			xyPadAutomation(xyPadAutomation)
																				
{
	lookAndFeel = new CabbageLookAndFeel();
	Component::setLookAndFeel(lookAndFeel);

	// Value displays
	for (int i=0; i<2; i++) {
		valueDisplays.add (new XYValueDisplay(ballColour));
		addAndMakeVisible (valueDisplays[i]);
	}

	// Min and max values
	xMin = minXValue;
	xMax = maxXValue;
	yMin = minYValue;
	yMax = maxYValue;
	xRange = xMax - xMin;
	yRange = yMax - yMin;
		
	title.append (name, 100);

	if (decimalPlaces < 0)
		decimalPlaces = 0; 
	format << "%." << decimalPlaces << "f";
}

XYPad::~XYPad()
{
}

void XYPad::resized()
{
	// Toggles
	float toggleWidth;
	if (getWidth() >= 150)
		toggleWidth = 30;
	else
		toggleWidth = getWidth()/5;
		
	xyToggles.add(new XYToggle(XYImages::getImageForAutomation_Type1(toggleWidth, 15), fontColour));
	xyToggles.add(new XYToggle(XYImages::getImageForAutomation_Type2(toggleWidth, 15), fontColour));
	for (int i=0; i<2; i++) {
		xyToggles[i]->setBounds(5+(i*(toggleWidth+5)), getHeight()-18, toggleWidth, 15);
		addAndMakeVisible(xyToggles[i]);
		xyToggles[i]->addListener(this);
	}
	currentSelectedToggle = xyPadAutomation->getSelectedToggle();
	xyToggles[currentSelectedToggle]->setToggleState(true, false);
	
	// Ball
	ballSize = (float)getWidth()/10;
	if (ballSize > 18) 
		ballSize = 18;		//maximum size
	else if (ballSize < 8) 
		ballSize = 8;		//minimum size

	xyPadAutomation->setBallSize (ballSize);

	// Canvas
	canvas = new XYCanvas(ballColour, ballSize, xMin, xMax, yMin, yMax);
	canvas->setBounds (5, 5, getWidth()-10, getHeight()-45);
	addAndMakeVisible (canvas);
	//setting ball position from x and y output values, this means the pad will open with the current x and y values
	canvas->setBallPositionFromXYValues (xyPadAutomation->getXValue(), xyPadAutomation->getYValue());
	canvas->addMouseListener(this, true); //so that "this" also gets the mouse events for canvas
	canvas->setToggleId(currentSelectedToggle);

	// Value displays
	valueDisplays[0]->setBounds(3, getHeight()-38, getWidth()*0.5-6, 12);
	valueDisplays[1]->setBounds(getWidth()*0.5+3, getHeight()-38, getWidth()*0.5-6, 12);
	displayXYValues();

	// Speed slider
	speedSlider = new Slider("Speed");
	speedSlider->setBounds(getWidth()*0.3, canvas->getBottom()-25,
		getWidth()*0.4, 15);
	speedSlider->setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
	speedSlider->setSliderStyle(Slider::LinearHorizontal);
	speedSlider->setRange(0, 1, 0.01);
	speedSlider->setValue(xyPadAutomation->getSpeedSliderValue());
	addAndMakeVisible(speedSlider);
	speedSlider->addListener(this);
	speedSlider->addMouseListener(this, false);
	speedSlider->setAlpha(0.1);

	lookAndFeelBasic = new CabbageLookAndFeelBasic();
	speedSlider->setLookAndFeel(lookAndFeelBasic); //setting look and feel for slider

	//if the plugin is already automating from a previous instance then we need to reset certain things 
	//such as handles and automation type.
	if (xyPadAutomation->isAutomating()) {
		if (xyPadAutomation->getSelectedToggle() == 1) {
			canvas->setStartHandle(xyPadAutomation->getStartHandle());
			canvas->setEndHandle(xyPadAutomation->getEndHandle());
			canvas->startBallPathTimer(); //we only want to redraw the path for type 2 automation
		}
		canvas->useStaticBall(false);
	}
	else { //else if this is the first time being initialised....
		xyPadAutomation->setBoundsForAutomation (canvas->getBounds());
		xyPadAutomation->setMinMaxValues (xMin, xMax, yMin, yMax);
	}
}

void XYPad::buttonClicked(Button* button)
{
	if (button == xyToggles[0]) {
		xyToggles[1]->setToggleState(false, false);
		currentSelectedToggle = 0;
	}
	else if (button == xyToggles[1]) {
		xyToggles[0]->setToggleState(false, false);
		currentSelectedToggle = 1;
	}
	if (!xyPadAutomation->isAutomating()) //only change toggle id when not automating
		canvas->setToggleId(currentSelectedToggle);
}

void XYPad::sliderValueChanged(Slider* slider)
{
	if (slider == speedSlider)
		xyPadAutomation->setSpeedSliderValue(slider->getValue());
}


void XYPad::paint (Graphics& g)
{
	g.setColour (CabbageUtils::getComponentSkin());
	g.fillRoundedRectangle (0, 0, getWidth(), getHeight(), 5);

	// Border outline
	float borderWidth = CabbageUtils::getBorderWidth();
	g.setColour (CabbageUtils::getBorderColour());
	g.drawRoundedRectangle (borderWidth/2, borderWidth/2, getWidth()-borderWidth, getHeight()-borderWidth, 
		5, borderWidth);	

	// For drawing the title
	//g.setColour (CabbageUtils::getComponentFontColour());
	g.setColour(fontColour);
	Font font = CabbageUtils::getComponentFont();
	g.setFont (font);
	title = CabbageUtils::cabbageString (title, font, canvas->getRight()-(getWidth()/2));
	float strWidth = font.getStringWidthFloat (title);
	g.drawText (title, canvas->getRight()-strWidth, getHeight()-(font.getHeight()+3), 
		strWidth, font.getHeight(), 1, false); 
}

void XYPad::mouseDown (const MouseEvent& e)
{	
	if (canvas->isMouseOver()) {
		if (xyPadAutomation->isAutomating()) {
			xyPadAutomation->cancelAutomation();
			canvas->setToggleId(currentSelectedToggle);
			canvas->clearHandles();
		}

		Point<float> click;
		click.setXY (e.x, e.y);
		canvas->setBallPosition (click);
		if (e.mods.isRightButtonDown()) {
			canvas->setStartHandle(click);	
			canvas->useStaticBall(false);
		}
		else if (e.mods.isLeftButtonDown()) {
			canvas->useStaticBall(true);
			speedSlider->setValue(0); //resetting slider
		}

		displayXYValues();
		canvasHasFocus = true;
	}
}

void XYPad::mouseDrag (const MouseEvent& e)
{	
	if ((canvas->isMouseButtonDownAnywhere()) && (canvasHasFocus)) {
		Point<float> click;
		click.setXY (e.x, e.y);
		canvas->setBallPosition (click);
		xyPadAutomation->setXValue(getXValue());
		xyPadAutomation->setYValue(getYValue());
		displayXYValues();
		if (e.mods.isRightButtonDown()) 
			canvas->setEndHandle(click);
	}
}

void XYPad::mouseUp (const MouseEvent& e)
{	
	canvasHasFocus = false;
	if (e.mods.isRightButtonDown()) {
		xyPadAutomation->beginAutomation(currentSelectedToggle);
		speedSlider->setValue(xyPadAutomation->getSpeedSliderValue(), false, false); //not sending update message so that it won't call sliderValueChange()
	}
}

void XYPad::mouseEnter(const MouseEvent& e)
{
	if ((speedSlider->isMouseOver()) && (xyPadAutomation->isAutomating())) {
		speedSlider->setEnabled(true);
		speedSlider->setAlpha(1.0);
	}
	else {
		speedSlider->setEnabled(false);
		speedSlider->setAlpha(0.1);
	}
}

void XYPad::displayXYValues ()
{
	valueDisplays[0]->setValue(String::formatted(format, getXValue()));
	valueDisplays[1]->setValue(String::formatted(format, getYValue()));
	//xyPadAutomation->setXValue(getXValue());
	//xyPadAutomation->setYValue(getYValue());
}

float XYPad::getXValue()
{
	//Returns the x output value, not the ball's x pixel position!!
	return ((canvas->getBallX()/(canvas->getWidth()-ballSize)) * xRange) + xMin;
}

float XYPad::getYValue()
{
	//Returns the y output value, not the ball's y pixel position!!
	return ((1-(canvas->getBallY()/(canvas->getHeight()-ballSize))) * yRange) + yMin;
}

//========= This method is used by the Plugin Editor ============================================
void XYPad::setXYValues (float x, float y)
{
	//This method gets the x and y output values from the plugin processor and converts them back 
	//into the centre coordinates of the ball.
	Point<float> newPos;
	newPos.setXY ((((x-xMin)/xRange)*(canvas->getWidth()-ballSize))+(ballSize/2), 
		((1-((y-yMin)/yRange))*(canvas->getHeight()-ballSize))+(ballSize/2));

	canvas->setBallPosition (newPos);
	//canvas->setBallPositionFromXYValues(x, y);
	displayXYValues();
}


