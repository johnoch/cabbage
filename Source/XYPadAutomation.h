/*
  Copyright (C) 2012 Damien Rennick

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

#ifndef __PLUGINPROCESSOR_AUTO__
#define __PLUGINPROCESSOR_AUTO__



//==============================================================================
// XYPad Automation class. Allows plugin editor to close while maintining automation
//==============================================================================
class XYPadAutomation	:	public Timer
{
public:
	XYPadAutomation();
	~XYPadAutomation();

	void beginAutomation(int selectedToggleButton);
	void setBallSize (float size);
	void setMinMaxValues (float xMinimum, float xMaximum, float yMinimum, float yMaximum);
	float getMinimumXValue();
	float getMinimumYValue();
	void calculateTimerInterval (Path path);
	void setBallPath (Path path);
	void cancelAutomation();
	bool isAutomating();
	void setSpeedSliderValue(float sliderValue);
	void setBoundsForAutomation(Rectangle<int> bounds);
	void timerCallback();
	Point<float> getStartHandle();
	Point<float> getEndHandle();
	float getXValue();
	float getYValue();
	void setXValue(float value);
	void setYValue(float value);
	int getSelectedToggle();
	float getSliderValue();
	
private:
	float xValue, yValue;
	int timerInterval, minInterval;
	float xValue_Incr, yValue_Incr;
	Rectangle<int> availableBounds;
	float ballSize;
	float xMin, yMin, xMax, yMax, xRange, yRange;
	float xOut, yOut;
	int selectedToggle;
	float speedSliderValue;
	Path ballPath;
	int i, incr;
	bool isAutomationOn;
	

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (XYPadAutomation);
};


#endif