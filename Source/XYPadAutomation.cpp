
#include "XYPadAutomation.h"

//============================================================================

XYPadAutomation::XYPadAutomation() 
{
	xValue = getMinimumXValue();
	yValue = getMinimumYValue();
	incr = 1; //increment in pixels
	minInterval = 2; //min and max timer values for automation

	selectedToggle = 0;
	speedSliderValue = 0;
	isAutomationOn = false;
	updateCounter = 0;
	paramIndex = 0;
}

XYPadAutomation::~XYPadAutomation()
{
}

void XYPadAutomation::setBallSize (float size)
{
	ballSize = size;
}

void XYPadAutomation::setBoundsForAutomation(Rectangle<int> bounds)
{
	availableBounds.setWidth(bounds.getWidth()-ballSize);
	availableBounds.setHeight(bounds.getHeight()-ballSize);
}	

void XYPadAutomation::setBallPath (Path path)
{
	AffineTransform transform = AffineTransform::translation((ballSize/2)*-1, (ballSize/2)*-1);
	path.applyTransform(transform);
	ballPath = path;
}

bool XYPadAutomation::isAutomating()
{
	return isAutomationOn;
}

void XYPadAutomation::setMinMaxValues (float xMinimum, float xMaximum, float yMinimum, float yMaximum)
{
	xMin = xMinimum;
	yMin = yMinimum;
	xMax = xMaximum;
	yMax = yMaximum;
	xRange = xMax - xMin;
	yRange = yMax - yMin;
}

float XYPadAutomation::getMinimumXValue()
{
	return xMin;
}

float XYPadAutomation::getMinimumYValue()
{
	return yMin;
}

void XYPadAutomation::cancelAutomation()
{
	stopTimer();
	isAutomationOn = false;
	xValue_Incr = yValue_Incr = 0;
}

void XYPadAutomation::calculateTimerInterval (Path path)
{
	// Function that calculates the timer interval in milliseconds from the length
	// of the path.
	float length = path.getLength();
	// getting length of parent's diagonal using pythagoras's theorem
	int sumOfSquares = (availableBounds.getWidth()*availableBounds.getWidth()) + 
		(availableBounds.getHeight()*availableBounds.getHeight());
	int diagonal = pow(sumOfSquares, 0.5); //square root
	timerInterval = (float)minInterval / (length/diagonal);
	speedSliderValue = (float)minInterval / timerInterval;
}

void XYPadAutomation::setSpeedSliderValue(float sliderValue)
{
	speedSliderValue = sliderValue;
	//reset our counter...
	updateCounter = 0;
 //   timerInterval = minInterval/speedSliderValue;

 //   if (sliderValue==0)
 //       stopTimer();
 //   else
	//	//startTimer(timerInterval); 
}

void XYPadAutomation::beginAutomation(int selectedToggleButton)
{
	selectedToggle = selectedToggleButton;
	i = ballPath.getLength(); //ball begins traversing at the end of the path, rather than the start

	calculateTimerInterval (ballPath);
	Point<float> startDragPos = ballPath.getPointAlongPath(0);
	Point<float> secondPoint = ballPath.getPointAlongPath(1);
	Point<float> endDragPos = ballPath.getPointAlongPath(ballPath.getLength());
		
	if (selectedToggle == 0) {
		xValue_Incr = ((secondPoint.getX()-startDragPos.getX()) / (float)availableBounds.getWidth()) * xRange;
		yValue_Incr = ((secondPoint.getY()-startDragPos.getY()) / (float)availableBounds.getHeight()) * yRange;
		yValue_Incr *= -1; //inverting because y axis is also inverted
	}
	
	//initialising
	xValue = ((endDragPos.getX()/(float)availableBounds.getWidth()) * xRange) + xMin; 
	yValue = ((1 - (endDragPos.getY()/(float)availableBounds.getHeight())) * yRange) + yMin; //inverting y axis 
	//startTimer (timerInterval);
	startTimer(50);
	isAutomationOn = true;
}

void XYPadAutomation::update()
{
if(isAutomating())
	{
//	int test = int((1-speedSliderValue))+1;
//	if(updateCounter==test)
//		{
		updateCounter = 0;
		if (selectedToggle == 0) {
		xValue += xValue_Incr*(10*speedSliderValue);
		yValue += yValue_Incr*(10*speedSliderValue);

		// If a border is hit then the increment value should be reversed... 
		if (xValue <= xMin) {
			xValue = xMin;
			xValue_Incr*=-1;
		}
		else if (xValue >= xMax) {
			xValue = xMax;
			xValue_Incr*=-1;
		}
		if (yValue <= yMin) {
			yValue = yMin;
			yValue_Incr*=-1;
		}
		else if (yValue >= yMax) {
			yValue = yMax;
			yValue_Incr*=-1;
		}		
		}
		// else if not in normal mode....
		else if (selectedToggle == 1) { 
		Point<float> pt = ballPath.getPointAlongPath(i, AffineTransform::identity);
		
		xValue =(((pt.getX()/(float)availableBounds.getWidth()) * xRange) + xMin)*(10*speedSliderValue);
		yValue = ((pt.getY()/(float)availableBounds.getHeight()) * yRange);
		yValue = ((yRange-yValue) + yMin)*(10*speedSliderValue); //inverting and adding yMin

		i += incr;
		if (i > ballPath.getLength()) {
			i = ballPath.getLength();
			incr *= -1;
		}
		else if (i < 0) {
			i = 0;
			incr *= -1;
		}
		}
//		}
//	else updateCounter++;
	}
	
	//sendChangeMessage();
}

void XYPadAutomation::timerCallback()
{
	//if (selectedToggle == 0) {
	//	xValue += xValue_Incr;
	//	yValue += yValue_Incr;

	//	// If a border is hit then the increment value should be reversed... 
	//	if (xValue <= xMin) {
	//		xValue = xMin;
	//		xValue_Incr*=-1;
	//	}
	//	else if (xValue >= xMax) {
	//		xValue = xMax;
	//		xValue_Incr*=-1;
	//	}
	//	if (yValue <= yMin) {
	//		yValue = yMin;
	//		yValue_Incr*=-1;
	//	}
	//	else if (yValue >= yMax) {
	//		yValue = yMax;
	//		yValue_Incr*=-1;
	//	}		
	//}
	//// else if not in normal mode....
	//else if (selectedToggle == 1) { 
	//	Point<float> pt = ballPath.getPointAlongPath(i, AffineTransform::identity);
	//	
	//	xValue = ((pt.getX()/(float)availableBounds.getWidth()) * xRange) + xMin;
	//	yValue = ((pt.getY()/(float)availableBounds.getHeight()) * yRange);
	//	yValue = (yRange-yValue) + yMin; //inverting and adding yMin

	//	i += incr;
	//	if (i > ballPath.getLength()) {
	//		i = ballPath.getLength();
	//		incr *= -1;
	//	}
	//	else if (i < 0) {
	//		i = 0;
	//		incr *= -1;
	//	}
	//}
	sendChangeMessage();
}

Point<float> XYPadAutomation::getStartHandle()
{
	Point<float> startHandle = ballPath.getPointAlongPath(0);
	startHandle.setX(startHandle.getX() + (ballSize/2));
	startHandle.setY(startHandle.getY() + (ballSize/2));

	return startHandle;
}

Point<float> XYPadAutomation::getEndHandle()
{
	Point<float> endHandle = ballPath.getPointAlongPath(ballPath.getLength());
	endHandle.setX(endHandle.getX() + (ballSize/2));
	endHandle.setY(endHandle.getY() + (ballSize/2));

	return endHandle;
}

float XYPadAutomation::getXValue()
{
	return xValue;
}

float XYPadAutomation::getYValue()
{
	return yValue;
}

void XYPadAutomation::setXValue(float value)
{
	xValue = value;
	sendChangeMessage();
}

void XYPadAutomation::setYValue(float value)
{
	yValue = value;
	sendChangeMessage();
}

int XYPadAutomation::getSelectedToggle()
{
	return selectedToggle;
}

float XYPadAutomation::getSpeedSliderValue()
{
	return speedSliderValue;
}
