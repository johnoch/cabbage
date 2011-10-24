


/*
  ====================================================================================

	Custom Rotary Slider Class

  ====================================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "imageBinaries.h"


class CabbageLookAndFeel	:	public LookAndFeel
{
public:
	CabbageLookAndFeel();
	~CabbageLookAndFeel();

	virtual void drawRotarySlider (Graphics &g, int x, int y, int width, int height, float sliderPosProportional, 
						float rotaryStartAngle, float endAngle, Slider &slider);
	virtual void drawLinearSliderBackground (Graphics &g, int x, int y, int width, int height,
						float sliderPos, float minSliderPos, float maxSliderPos, const Slider::SliderStyle style, Slider &slider);
	virtual void drawLinearSliderThumb (Graphics &g, int x, int y, int width, int height, float sliderPos, 
						float minSliderPos, float maxSliderPos, const Slider::SliderStyle style, Slider &slider);
	virtual void drawButtonBackground (Graphics&, Button&, const Colour&, bool, bool);
	virtual const Font getFontForTextButton (TextButton&);
	virtual void drawLabel (Graphics &g, Label &label);
	virtual void drawComboBox (Graphics&, int, int, bool, int, int, int, int, ComboBox&);
	virtual void drawToggleButton (Graphics &g, ToggleButton &button, bool isMouseOverButton, bool isButtonDown);
	virtual void drawTextEditorOutline (Graphics &g, int width, int height, TextEditor &textEditor);
	virtual void fillTextEditorBackground (Graphics &g, int width, int height, TextEditor &textEditor);

	
	juce_UseDebuggingNewOperator


private:

	
};

