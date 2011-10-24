#include "CabbageLookAndFeel.h"

//========== Constructor ================================================================================
CabbageLookAndFeel::CabbageLookAndFeel()
{
}


//========= Destructor ====================================================================================
CabbageLookAndFeel::~CabbageLookAndFeel()
{
}


//=========== Rotary Sliders ==============================================================================
void CabbageLookAndFeel::drawRotarySlider(Graphics& g, int /*x*/, int /*y*/, int /*width*/, int /*height*/,
																	float /*sliderPosProportional*/,
																	float /*startAngle*/,
																	float /*endAngle*/,
																	Slider& slider)
{
	//----- Setting textbox style
	String str;
	str << slider.getMaximum() << slider.getInterval();
	int len = str.length() * 7;
	slider.setTextBoxStyle (Slider::TextBoxBelow, false, len, 15);

	int textBoxGap = 0;  //space between textbox and slider
	
	//----- Declaration of custom slider image
	Image newSlider  = ImageCache::getFromMemory (imageBinaries::rotary1_png, imageBinaries::rotary1_pngSize); 

	//----- Frames
	const int frameHeight = 128;	
	const int frameWidth = 128;
	const int numFrames = 75;

	//----- Variables for determining new destination width and height etc...
	int destX, destY, destHeight, destWidth, textBoxHeight;
	int yOffset = 0;
	
	const double div = (slider.getMaximum() - slider.getMinimum()) / numFrames; //to get divisor number
	double pos = (int)(((slider.getValue() - slider.getMinimum()) / div) + 0.5); //0.5 makes sure it's rounded up or down correctly		

	if (pos > 0) pos -= 1;

	yOffset = pos * frameHeight;

	textBoxHeight = slider.getTextBoxHeight();					//gets height of textbox
	destHeight = (slider.getHeight() - (textBoxHeight + textBoxGap));	//destination height of just the image part!
	destWidth = destHeight;										//because rotary slider images generally have the same width and height....

	destX = ((slider.getWidth() - destWidth) / 2);				//starting x position, this ensures that the image will be centered
	destY = 0;													//starting y position

	//----- Draw Image
	g.drawImage(newSlider, destX, destY, destWidth, destHeight, 0, yOffset, frameWidth, frameHeight, false);
	
}


//=========== Linear Slider Background ===========================================================================
void CabbageLookAndFeel::drawLinearSliderBackground (Graphics &g, int /*x*/, int /*y*/, int /*width*/, int /*height*/, float sliderPos, 
																								float /*minSliderPos*/, 
																								float /*maxSliderPos*/, 
																								const Slider::SliderStyle style, 
																								Slider &slider)
{
	//----- Setting textbox style
	String str;
	str << slider.getMaximum() << slider.getInterval();
	int len = str.length() * 7;

	int textBoxGap = 15;  //space between textbox and slider


	//----- Frames
	int frameWidth, frameHeight;
	const int numFrames = 75;
	Image newBackground;

	//----- Variables for determining new destination width, height, position etc...
	int destX, destY, destHeight, destWidth, xOffset, yOffset;
	
	/*----- The following calculates the position that the slider should be in. slider.getMinimum() is
	subtracted in case that 0 is not the minimum value set. */
	const double div = (slider.getMaximum() - slider.getMinimum()) / numFrames;
	sliderPos = (int)(((slider.getValue() - slider.getMinimum()) / div) + 0.5);

	if (sliderPos > 0) sliderPos -= 1;


	//----- If Horizontal Slider -----------------------------------
	if (style == Slider::LinearHorizontal) {
		//----- Textbox style
		slider.setTextBoxStyle (Slider::TextBoxLeft, false, len, 15);

		frameWidth = 240;
		frameHeight = 40;
		xOffset = 0;

		newBackground  = ImageCache::getFromMemory (imageBinaries::horizontalbackground_png, imageBinaries::horizontalbackground_pngSize);

		yOffset = sliderPos * frameHeight;	

		/*----- desWiddth and destHeight.  7 is taken from the end of destWidth to allow room
		for the edge of the slider thumb.....*/
		destHeight = (slider.getHeight());		
		destWidth = (slider.getWidth() - (slider.getTextBoxWidth() + textBoxGap));
		destWidth -= 7;

		//----- starting x and y positions, this ensures that the image will be centered with the textbox
		destX = (slider.getTextBoxWidth() + textBoxGap);				
		destY = (slider.getHeight() - destHeight) / 2;	//starting y position
	}

	//----- If Vertical Slider ------------------------------------
	if (style == Slider::LinearVertical) {
		//----- Textbox style
		slider.setTextBoxStyle (Slider::TextBoxBelow, false, len, 15);

		frameWidth = 40;
		frameHeight = 240;
		yOffset = 0;

		newBackground  = ImageCache::getFromMemory (imageBinaries::verticalbackground_png, imageBinaries::verticalbackground_pngSize);

		xOffset = sliderPos * frameWidth;	

		/*----- destWiddth and destHeight.  7 is taken from the end of destHeight to allow room
		for the edge of the slider thumb.....*/
		destHeight = slider.getHeight() - (slider.getTextBoxHeight() + textBoxGap);	
		destHeight -= 7;
		destWidth = slider.getWidth();

		//----- starting x position, this ensures that the image will be centered with the textbox
		destX = ((slider.getWidth() - destWidth) / 2);				
		destY = 7;		//starting y position. Making it 5 means that the full slider thumb will be visible when at maximum.
	}
	

	//----- Drawing Image. 
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
	int destX, destY, destWidth, destHeight;
	Image newThumb;
	String str;

	int textBoxGap = 15;  //space between textbox and slider

	//----- If Horizontal ------------------------------------
	if (style == Slider::LinearHorizontal) {
		newThumb  = ImageCache::getFromMemory (imageBinaries::horizontalthumb_png, imageBinaries::horizontalthumb_pngSize);

		/*----- The following code calculates the real sliderPos that takes into account textbox width
		and gap etc.  An additional gap of 7 is left at the end so that the whole of the slider thumb is 
		visible when at the maximum value. */
		float div = (slider.getValue() - slider.getMinimum()) / (slider.getMaximum() - slider.getMinimum());
		int availableWidth = slider.getWidth() - (slider.getTextBoxWidth() + textBoxGap);
		sliderPos = div * (availableWidth - 7);
		sliderPos += (slider.getTextBoxWidth() + textBoxGap);

		//----- destWidth and destHeight are based on the height
		destWidth = height * 0.6;
		destHeight = height * 0.6;
		destX = sliderPos-(destWidth/2);
		destY = height / 3;
	}

	//----- If Vertical ------------------------------------
	if (style == Slider::LinearVertical) {
		newThumb  = ImageCache::getFromMemory (imageBinaries::verticalthumb_png, imageBinaries::verticalthumb_pngSize);

		/*----- The following code calculates the sliderPos. A gap of 7 is left at the top so that
		the entire slider thumb is visible when at the maximum value.*/
		float div = (slider.getValue() - slider.getMinimum()) / (slider.getMaximum() - slider.getMinimum());
		int availableHeight = slider.getHeight() - (slider.getTextBoxHeight() + textBoxGap) - 7;
		sliderPos = div * availableHeight;
		sliderPos = availableHeight - sliderPos;	//inverting the y axis
		sliderPos += 7;								//adjusts the position to deal with the gap at the top	

		//----- destWidth and destHeight are based on the width
		destWidth = width * 0.6;
		destHeight = width * 0.6;
		destX = width/3;
		destY = sliderPos - (destHeight / 2);
	}
	
	g.drawImage (newThumb, destX, destY, destWidth, destHeight, 0, 0, frameWidth, frameHeight, false);
}


//======= Toggle Buttons ========================================================================
void CabbageLookAndFeel::drawToggleButton (Graphics &g, ToggleButton &button, bool /*isMouseOverButton*/, bool /*isButtonDown*/)
{
	int yOffset= 0;
	int frameWidth = 60;
	int frameHeight = 20; 

	Image newButton = ImageCache::getFromMemory (imageBinaries::togglebutton1_png, imageBinaries::togglebutton1_pngSize);
	
	//----- If button is switched on
	if (button.getToggleState() == true) yOffset = 20;	//for changing frame

	//----- Drawing image
	g.drawImage (newButton, 0, 0, frameWidth, frameHeight, 0, yOffset, frameWidth, frameHeight, false);

	//----- Text
	Justification just (36);
	g.setColour (Colours::whitesmoke);
	g.drawText (button.getButtonText(), 0, 0, button.getWidth(), button.getHeight(), just, false);
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


//=========== Text Button Font ======================================================================
const Font CabbageLookAndFeel::getFontForTextButton (TextButton& /*button*/)
{
	Font font;
	font.setTypefaceName (T("Verdana"));
	font.setHeight (12);
	return font;
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
	g.setColour (Colours::black);
	g.setOpacity (0.9);
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

	if (dynamic_cast<Slider*>(comp)) { //If slider
	//----- Drawing rounded rectangle
		Colour cl = label.findColour (0x1000280, true); //getting background colour specified
		g.setColour (cl);
		if (cl != Colours::white) g.setOpacity (0.7);	//if the colour is white then no opacity is used

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






