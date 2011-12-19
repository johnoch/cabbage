
/*
  ====================================================================================

	Cabbage Look And Feel Class

  ====================================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "imageBinaries.h"

class CabbageLookAndFeel	:	public LookAndFeel
{
public:
	CabbageLookAndFeel();
	~CabbageLookAndFeel();

	Image drawRotaryImage(int diameter, const Colour circleFill, float sliderPosProportional);

	virtual void drawRotarySlider (Graphics &g, int x, int y, int width, int height, float sliderPosProportional, 
																					float rotaryStartAngle, 
																					float rotaryEndAngle, 
																					Slider &slider);
	virtual void drawLinearSliderBackground (Graphics &g, int x, int y, int width, int height, 
																					float sliderPos, 
																					float minSliderPos, 
																					float maxSliderPos, 
																					const Slider::SliderStyle style, 
																					Slider &slider);
	virtual void drawLinearSliderThumb (Graphics &g, int x, int y, int width, int height, float sliderPos, 
																						float minSliderPos, 
																						float maxSliderPos, 
																						const Slider::SliderStyle style, 
																						Slider &slider);
	virtual void drawButtonBackground (Graphics&, Button&, const Colour&, bool, bool);
	//virtual const Font getFontForTextButton (TextButton&);
	virtual void drawLabel (Graphics &g, Label &label);
	virtual void drawComboBox (Graphics&, int, int, bool, int, int, int, int, ComboBox&);
	virtual void drawToggleButton (Graphics &g, ToggleButton &button, bool isMouseOverButton, bool isButtonDown);
	virtual void drawTextEditorOutline (Graphics &g, int width, int height, TextEditor &textEditor);
	virtual void fillTextEditorBackground (Graphics &g, int width, int height, TextEditor &textEditor);
	virtual void drawGroupComponentOutline (Graphics &g, int w, int h, const String &text, const Justification &position, 
																									GroupComponent &group);
	void setDefaultSansSerifTypefaceName (const String &newName);

	
	juce_UseDebuggingNewOperator


private:

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CabbageLookAndFeel);
	
};


/*=============================================================================================================
	
	Basic Look And Feel Class

=============================================================================================================*/
class CabbageLookAndFeelBasic	:	public LookAndFeel
{
public:
	CabbageLookAndFeelBasic();
	~CabbageLookAndFeelBasic();

	virtual void drawLinearSliderBackground (Graphics &g, int x, int y, int width, int height, 
																					float sliderPos, 
																					float minSliderPos, 
																					float maxSliderPos, 
																					const Slider::SliderStyle style, 
																					Slider &slider);
	virtual void drawLinearSliderThumb (Graphics &g, int x, int y, int width, int height, float sliderPos, 
																						float minSliderPos, 
																						float maxSliderPos, 
																						const Slider::SliderStyle style, 
																						Slider &slider);
	
	juce_UseDebuggingNewOperator


private:

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CabbageLookAndFeelBasic);
	
};
