#ifndef __XYPAD_H_
#define __XYPAD_H_

#include "../JuceLibraryCode/JuceHeader.h"
#include "CabbageUtils.h"
#include "CabbageLookAndFeel.h"
#include "./Plugin/CabbagePluginProcessor.h"

/*
  ====================================================================================

	XY Handle Component

  ====================================================================================
*/
class XYHandleComponent	:	public Component
{
public:
	XYHandleComponent();
	~XYHandleComponent();

	class XYCanvas* getParentComponent();
	void paint (Graphics& g);
	void mouseEnter (const MouseEvent& e);
	void mouseDown (const MouseEvent& e);
	void mouseDrag (const MouseEvent& e);

private:
	ComponentDragger dragger;
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (XYHandleComponent);
};

/*
  ============================================================================

   XYImages
   
   -------------------------------------------------------------------------

   Returns custom images for toggle buttons.

  ============================================================================
*/
class XYImages
{
public:
	XYImages()
	{
	}

	~XYImages()
	{
	}

	static Image getImageForAutomation_Type1(int width, int height)
	{
		Image img = Image::Image(Image::ARGB, width, height, true);
		Graphics g(img);
		g.setColour(Colours::white);

		Path pathLine, pathBall;
		pathLine.startNewSubPath(width*0.2, height*0.8);
		pathLine.lineTo(width*0.7, height*0.2);
		g.strokePath(pathLine, width*0.04);
		pathBall.addEllipse(width*0.6, height*0.45, width*0.2, width*0.2);
		g.fillPath(pathBall);
		
		return img;
	}

	static Image getImageForAutomation_Type2(int width, int height)
	{
		Image img = Image::Image(Image::ARGB, width, height, true);
		Graphics g(img);
		g.setColour(Colours::white);

		Path pathLine, pathBall;
		pathLine.startNewSubPath(width*0.2, height*0.8);
		pathLine.lineTo(width*0.8, height*0.2);
		g.strokePath(pathLine, width*0.05);
		Point<float> pt = pathLine.getPointAlongPath(pathLine.getLength()*0.6);
		pathBall.addEllipse(pt.getX()-(width*0.1), pt.getY()-(width*0.1), width*0.2, width*0.2);
		g.fillPath(pathBall);
		
		return img;
	}

};


/*
  ============================================================================

   XYToggle
   
   -------------------------------------------------------------------------

   Custom toggle button.

  ============================================================================
*/
class XYToggle	:	public ToggleButton
{
public:
	XYToggle(Image inputImage, Colour col);
	~XYToggle();
	void paintButton (Graphics& g, bool isMouseOverButton, bool isButtonDown);

private:
	Image img;
	Colour colourWhenOn;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (XYToggle);
};


/*
  ============================================================================

   XYValueDisplay Component

   -------------------------------------------------------------------------

   This displays the outputted value, similar to a textbox.

  ============================================================================
*/
class XYValueDisplay	:	public Component
{
public:
	XYValueDisplay(Colour col);
	~XYValueDisplay();
	void setValue(String value);
	void paint(Graphics& g);

private:
	Colour colour;
	String value;
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (XYValueDisplay);
};

/*
  =========================================================================

	XYCanvas

	--------------------------------------------------------------------

	The area which the ball can move around in.

  =========================================================================
*/

class XYCanvas	: public Component,
					public MultiTimer
{
public:
	XYCanvas(Colour ballColour, float ballSize, float xMinimum, float xMaximum, 
																float yMinimum, 
																float yMaximum);
	~XYCanvas();
	class XYPad* getParentComponent();
	void resized();
	void cacheBackgroundImage();
	void paint(Graphics& g);
	void mouseEnter (const MouseEvent& e);
	void mouseDown (const MouseEvent& e);
	void mouseDrag (const MouseEvent& e);
	void mouseUp (const MouseEvent& e);
	Point<float> checkBounds (Point<float> pt);
	void setBallPosition(Point<float> pt);
	void setBallPositionFromXYValues(float xValue, float yValue);
	float getBallX();
	float getBallY();
	void updatePath();
	XYHandleComponent* makeHandle(Point<float> pt);
	void setStartHandle(Point<float> pt);
	void setEndHandle(Point<float> pt);
	void clearHandles();
	void setToggleId(int selection);
	void useStaticBall(bool useStaticBall);
	void startBallPathTimer();
	
private:
	Image bg;
	float ballX, ballY;
	float ballSize, handleSize;
	float yMax, xMax, yMin, xMin, xRange, yRange;
	Colour col;
	Path path;
	float pathOpacity, pathThickness, ballLineOpacity;
	int toggleId;
	bool paintStaticBall;
	OwnedArray<XYHandleComponent> handles;

	void timerCallback(int id)
	{
		//This reduces the opacity that is used to paint the path and ball lines. It will
		//give it a fade out effect.

		// Path 
		if (id == 1) {
			pathOpacity -= 0.1;
			pathThickness -= 0.1;
			
			repaint();

			if (toggleId == 0) {
				if (pathOpacity <= 0) 
					stopTimer(1);
			}
			else if (toggleId == 1) {
				if (pathOpacity <= 0.4)
					stopTimer(1);			
			}	
		}
		// Ball lines
		else if (id == 2) {
			ballLineOpacity -= 0.1;
			if (ballLineOpacity <= 0)
				stopTimer(2);
		}
	}

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (XYCanvas);
};


/*
  ============================================================================

   XYPad

  ----------------------------------------------------------------------------
   
   This is the main parent class!!

  ============================================================================
*/
class XYPad	:	public Component,
				public SliderListener,
				public ButtonListener,
				public ActionBroadcaster
{
public:
	XYPad(XYPadAutomation* xyPadAutomation, String title, int minXValue, int maxXValue, 
																				int minYValue, 
																				int maxYValue, 
																				int numberOfDecimalPlaces,
																				Colour ballColour,
																				Colour fontColour);
	~XYPad();

	void resized();
	void buttonClicked(Button* button);
	void sliderValueChanged(Slider* slider);
	void paint (Graphics& g);
	void setXYValues(float x, float y);
	float getXValue();
	float getYValue();
	Point<float> checkBounds(float x, float y);
	void mouseDown (const MouseEvent& e);
	void mouseDrag (const MouseEvent& e);
	void mouseUp (const MouseEvent& e);
	void mouseEnter (const MouseEvent& e);
	void displayXYValues();
	
	XYPadAutomation* xyPadAutomation;
	
private:
	ScopedPointer<XYCanvas> canvas;
	float ballSize;
	int speed;
	String title, name;
	Colour ballColour, fontColour;
	float yMax, xMax, yMin, xMin;
	float xOut, yOut, xRange, yRange;
	int decimalPlaces;	
	OwnedArray<XYValueDisplay> valueDisplays;
	OwnedArray<XYToggle> xyToggles;
	ScopedPointer<Slider> speedSlider;
	ScopedPointer<CabbageLookAndFeel> lookAndFeel;
	ScopedPointer<CabbageLookAndFeelBasic> lookAndFeelBasic;
	bool canvasHasFocus;
	int currentSelectedToggle;
	String format;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (XYPad);
};


#endif //__XYPAD_H_
