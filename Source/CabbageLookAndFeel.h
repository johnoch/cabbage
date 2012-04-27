#ifndef __CABBAGELOOKANDFEEL_H_
#define __CABBAGELOOKANDFEEL_H_

/*
  ====================================================================================

	Cabbage Look And Feel Class

  ====================================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "CabbageUtils.h"


class CabbageLookAndFeel	:	public LookAndFeel
{
public:
	CabbageLookAndFeel();
	~CabbageLookAndFeel();

	Image drawRotaryImage(int diameter, const Colour circleFill, float sliderPosProportional, float zeroPosProportional, 
																								bool useTrackerFill,
																								bool useBigImage);
	Image drawLinearBgImage (float width, float height, float sliderPosProportional, float zeroPosProportional, 
																								bool useTrackerFill,
																								bool isVertical);
	Image drawLinearThumbImage (float width, float height, const Colour thumbFill, bool isVertical);
	static Image drawToggleImage (float width, float height, bool isToggleOn, Colour colour);
	Image drawTextButtonImage (float width, float height, bool isButtonDown);
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
	virtual void drawButtonText (Graphics &g, TextButton &button, bool isMouseOverButton, bool isButtonDown);
	virtual void drawLabel (Graphics &g, Label &label);
	virtual void drawComboBox (Graphics&, int, int, bool, int, int, int, int, ComboBox&);
	virtual void drawToggleButton (Graphics &g, ToggleButton &button, bool isMouseOverButton, bool isButtonDown);
	virtual void drawTextEditorOutline (Graphics &g, int width, int height, TextEditor &textEditor);
	virtual void fillTextEditorBackground (Graphics &g, int width, int height, TextEditor &textEditor);
	virtual void drawGroupComponentOutline (Graphics &g, int w, int h, const String &text, const Justification &position, 
																									GroupComponent &group);
	virtual void drawScrollbar (Graphics &g, ScrollBar &scrollbar, int x, int y, int width, int height, 
																							bool isScrollbarVertical, 
																							int thumbStartPosition, 
																							int thumbSize, 
																							bool isMouseOver, 
																							bool isMouseDown);
	virtual void drawScrollbarButton (Graphics &g, ScrollBar &scrollbar, int width, int height, int buttonDirection, 
																								bool isScrollbarVertical, 
																								bool isMouseOverButton, 
																								bool isButtonDown);

	void setDefaultSansSerifTypefaceName (const String &newName);
	virtual void drawMenuBarBackground(Graphics &g, int width, int height, bool isMouseOverBar, MenuBarComponent &menuBar);   
    virtual void drawMenuBarItem(Graphics & g, int width, int height, int itemIndex,
                                      const String &itemText, bool isMouseOverItem,
                                      bool isMenuOpen, bool isMouseOverBar,
                                      MenuBarComponent &menuBar);

	void drawPopupMenuBackground(Graphics &g, int width, int height);
	Image drawCheckMark(); 
	virtual void drawPopupMenuItem (Graphics &g, int width, int height, bool isSeparator, bool isActive, bool isHighlighted, 
																											bool isTicked, 
																											bool hasSubMenu, 
																											const String &text, 
																											const String &shortcutKeyText, 
																											Image *image, 
																											const Colour *const textColour);
	virtual void drawDocumentWindowTitleBar (DocumentWindow &window, Graphics &g, int w, int h, int titleSpaceX, int titleSpaceW, 
																											const Image *icon, 
																											bool drawTitleTextOnLeft);
	Image drawWindowButtonNormal(int buttonType);
	Image drawWindowButtonIsOver(int buttonType);
	virtual Button* createDocumentWindowButton (int buttonType);

	juce_UseDebuggingNewOperator


private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CabbageLookAndFeel);
	
	
};


/*
  ====================================================================================

	Alternative Look And Feel Class

  ====================================================================================
*/
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


#endif //__CABBAGELOOKANDFEEL_H_