/*
  Copyright (C) 2007 Rory Walsh

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

#include "CabbagePluginProcessor.h"
#include "CabbagePluginEditor.h"
#include  "../CabbageCustomWidgets.h"


#ifdef Cabbage_GUI_Editor
#include "ComponentLayoutEditor.h"
#include "CabbageMainPanel.h"
#endif


//==============================================================================
CabbagePluginAudioProcessorEditor::CabbagePluginAudioProcessorEditor (CabbagePluginAudioProcessor* ownerFilter)
: AudioProcessorEditor (ownerFilter), lineNumber(0), inValue(0)
{
//set custom skin yo use
lookAndFeel = new CabbageLookAndFeel(); 
Component::setLookAndFeel(lookAndFeel);
#ifdef Cabbage_GUI_Editor
//determine whether instrument should be opened in GUI mode or not
addChangeListener(ownerFilter);
componentPanel = new CabbageMainPanel();
componentPanel->setBounds(0, 0, getWidth(), getHeight());
layoutEditor = new ComponentLayoutEditor();
layoutEditor->setBounds(0, 0, getWidth(), getHeight());
addAndMakeVisible(layoutEditor);
addAndMakeVisible(componentPanel);
layoutEditor->setTargetComponent(componentPanel);
#else
componentPanel = new Component();
addAndMakeVisible(componentPanel);
#endif

zero_dbfs = getFilter()->getCsound()->Get0dBFS();
componentPanel->addKeyListener(this);
componentPanel->setInterceptsMouseClicks(false, true);	
setSize (400, 400);
InsertGUIControls();
startTimer(5);

#ifdef Cabbage_GUI_Editor
componentPanel->addChangeListener(this);
if(!ownerFilter->isGuiEnabled()){
layoutEditor->setEnabled(false);
layoutEditor->toFront(false); 
layoutEditor->updateFrames();
#ifdef Cabbage_Build_Standalone
        //only want to grab keyboard focus on standalone mode as DAW handle their own keystrokes
        componentPanel->setWantsKeyboardFocus(true);
        componentPanel->toFront(true);
        componentPanel->grabKeyboardFocus();
#endif
}
else{
layoutEditor->setEnabled(true);
layoutEditor->toFront(true); 
layoutEditor->updateFrames();
}
#endif

#ifdef Cabbage_Build_Standalone
        //only want to grab keyboard focus on standalone mode as DAW handle their own keystrokes
        componentPanel->setWantsKeyboardFocus(true);
        componentPanel->toFront(true);
        componentPanel->grabKeyboardFocus();
#endif
}

CabbagePluginAudioProcessorEditor::~CabbagePluginAudioProcessorEditor()
{
getFilter()->editorBeingDeleted(this);
#ifdef Cabbage_GUI_Editor
//delete componentPanel;
//delete layoutEditor;
#endif
}

//===========================================================================
//WHEN IN GUI EDITOR MODE THIS CALLBACK WILL NOTIFIY THE HOST IF A MOUSE UP
//HAS BEEN TRIGGERED BY ANY OF THE INSTRUMENTS WIDGETS, THIS IN TURN UPDATED
//WINXOUND WITH THE NEW COORDINATES AND SIZE
//===========================================================================
void CabbagePluginAudioProcessorEditor::changeListenerCallback(ChangeBroadcaster *source)
{

#ifdef Cabbage_GUI_Editor
/*
StringArray csdArray;
String temp;
//break up lines in csd file into a string array
csdArray.addLines(getFilter()->getCsoundInputFileText());

if(componentPanel->getMouseState().equalsIgnoreCase("down")){
for(int i=0; i<csdArray.size(); i++){
		CabbageGUIClass cAttr(csdArray[i], 0);
		Logger::writeToLog(csdArray[i]);
		if(cAttr.getNumProp(T("top"))==componentPanel->getCurrentTop() &&
			cAttr.getNumProp(T("left"))==componentPanel->getCurrentLeft() &&
			cAttr.getNumProp(T("width"))==componentPanel->getCurrentWidth() &&
			cAttr.getNumProp(T("height"))==componentPanel->getCurrentHeight())
		{
			temp = csdArray[i].replace(componentPanel->getCurrentBounds(), T("bounds()"), true);
			csdArray.set(i, temp);
			Logger::writeToLog(temp);
			getFilter()->updateCsoundFile(csdArray.joinIntoString("\n"));
			lineNumber = i;					
		}
		getFilter()->setCurrentLine(lineNumber);
		}
}
*/

/*											
	for(int i=0; i<csdArray.size(); i++){
	if(csdArray[i].containsIgnoreCase(componentPanel->getCurrentBounds())||
	(csdArray[i].containsIgnoreCase(componentPanel->getCurrentPos())&&
	csdArray[i].containsIgnoreCase(componentPanel->getCurrentPos())))
	{
		if(csdArray[i].containsIgnoreCase(componentPanel->getCurrentBounds()))
			temp = csdArray[i].replace(componentPanel->getCurrentBounds(), T("bounds()"), true);
		else if(csdArray[i].containsIgnoreCase(componentPanel->getCurrentSize())&&
			csdArray[i].containsIgnoreCase(componentPanel->getCurrentPos())){
			String temp1 = csdArray[i].replace(componentPanel->getCurrentSize()+T(","), T(""), true);
			temp = temp1.replace(componentPanel->getCurrentPos(), T("bounds()"), true);
		}
	csdArray.set(i, temp);
	Logger::writeToLog(temp);
	getFilter()->updateCsoundFile(csdArray.joinIntoString("\n"));
	lineNumber = i;
	getFilter()->setCurrentLine(lineNumber);
	}
}
}*/
/*
else if(componentPanel->getMouseState().equalsIgnoreCase("up")){
//ONLY SEND UPDATED INFO ON A MOUSE UP
Logger::writeToLog(componentPanel->getCurrentBounds());
temp = csdArray[lineNumber].replace(T("bounds()"), componentPanel->getCurrentBounds());
csdArray.set(lineNumber, temp);
getFilter()->updateCsoundFile(csdArray.joinIntoString("\n"));
getFilter()->setGuiEnabled(true);
getFilter()->setChangeMessageType("GUI_edit");	
getFilter()->sendChangeMessage();
}
*/
#endif
}
//==============================================================================
// this function will display a context menu on right mouse click. The menu 
// is populated by all a list of GUI abstractions stored in the CabbagePlant folder.  
// Users can create their own GUI abstraction at any time, save them to this folder, and
// insert them to their instrument whenever they like
//==============================================================================

void CabbagePluginAudioProcessorEditor::mouseDown(const MouseEvent &e)
{
#ifdef Cabbage_GUI_Editor
PopupMenu m;
PopupMenu sm;
sm.addItem(1, "5_vsliders");
sm.addItem(2, "green_rslider");
if(!getFilter()->isGuiEnabled())
m.addItem(11, "Edit-mode");
else{
m.addItem(10, "Play-mode");
m.addSeparator();
m.addSubMenu(T("Plants"), sm); 

m.addItem(1, "Insert button");
m.addItem(2, "Insert rslider");
m.addItem(3, "Insert vslider");
m.addItem(4, "Insert hslider");
m.addItem(5, "Insert combobox");
m.addItem(6, "Insert checkbox");
m.addItem(7, "Insert groupbox");
m.addItem(8, "Insert image");
m.addItem(9, "Insert keyboard");
}

if (e.mods.isRightButtonDown())
 {
 switch(m.show()){
	 /* the plan here is to simply send text to WinXound and get it
	 to update the instrument. This way Cabbage don't have to keep track of 
	 anything as all controls will automatically get added to the GUI controls vector
	 when Cabbage is updated */
 case 1:
	 insertCabbageText(T("button bounds(")+String(e.getPosition().getX())+(", ")+String(e.getPosition().getY())+T(", 60, 25), channel(\"but1\"), text(\"on\", \"off\")"));
	 break;
 case 2:
	 insertCabbageText(T("rslider bounds(")+String(e.getPosition().getX())+(", ")+String(e.getPosition().getY())+T(", 90, 90), channel(\"rslider\"), caption(\"\"), range(0, 100, 0)"));
	 break;
 case 3:
	 insertCabbageText(T("vslider bounds(")+String(e.getPosition().getX())+(", ")+String(e.getPosition().getY())+T(", 50, 125), channel(\"vslider\"), caption(\"\"), range(0, 100, 0)"));
	 break;
 case 4:
	 insertCabbageText(T("hslider bounds(")+String(e.getPosition().getX())+(", ")+String(e.getPosition().getY())+T(", 225, 30), channel(\"hslider\"), caption(\"\"), range(0, 100, 0)"));
	 break;
 case 5:
	 insertCabbageText(T("combobox bounds(")+String(e.getPosition().getX())+(", ")+String(e.getPosition().getY())+T(", 100, 30), channel(\"combobox\"), items(\"1\", \"2\", \"3\")"));
	 break;
 case 6:
	 insertCabbageText(T("checkbox bounds(")+String(e.getPosition().getX())+(", ")+String(e.getPosition().getY())+T(", 100, 40), channel(\"checkbox\"), text(\"checkbox\")"));
	 break;
 case 7:
	 insertCabbageText(T("groupbox bounds(")+String(e.getPosition().getX())+(", ")+String(e.getPosition().getY())+T(", 200, 150), text(\"checkbox\"), colour(\"black\"), caption(\"groupbBox\")"));
	 break;
 case 8:
	 insertCabbageText(T("image bounds(")+String(e.getPosition().getX())+(", ")+String(e.getPosition().getY())+T(", 200, 150), text(\"checkbox\"), colour(\"white\"), caption(\"\"), outline(\"black\"), line(3)"));
	 break;
 case 9:
	 insertCabbageText(T("keyboard bounds(")+String(e.getPosition().getX())+(", ")+String(e.getPosition().getY())+T(", 150, 60)"));
	 break;
 case 10:
	 //Play-mode	 
	 layoutEditor->setEnabled(false);
	 componentPanel->toFront(true);
	 componentPanel->setInterceptsMouseClicks(false, true);	
	 getFilter()->setGuiEnabled(false);
	 getFilter()->setChangeMessageType("GUI_lock");
	 getFilter()->sendChangeMessage();
	 break;
 case 11:
	 //Edit-mode
	 layoutEditor->setEnabled(true);
	 layoutEditor->updateFrames();
	 layoutEditor->toFront(true); 
	 getFilter()->setGuiEnabled(true);
	 getFilter()->setChangeMessageType("GUI_edit");
	 getFilter()->sendChangeMessage();
	 break;
 default:
	 break;


 }

}
#endif
}

//==============================================================================
// this function will any mouse up events on our editor
//==============================================================================
void CabbagePluginAudioProcessorEditor::mouseUp(const MouseEvent &e)
{

}

//==============================================================================
void CabbagePluginAudioProcessorEditor::insertCabbageText(String text)
{
	 StringArray csdArray;
	 csdArray.addLines(getFilter()->getCsoundInputFileText());
	 for(int i=0;i<csdArray.size();i++)
		 if(csdArray[i].containsIgnoreCase("</Cabbage>")){
			 csdArray.insert(i, text);
			 getFilter()->setCurrentLine(i);
			 i=csdArray.size();
		 }
		// Logger::writeToLog(String(getFilter()->getCurrentLine()));

	 getFilter()->updateCsoundFile(csdArray.joinIntoString("\n"));
	 getFilter()->setChangeMessageType("GUI_edit");
	 getFilter()->sendChangeMessage();
}

//==============================================================================
void CabbagePluginAudioProcessorEditor::resized()
{
#ifdef Cabbage_GUI_Editor
if(componentPanel)componentPanel->setBounds(0, 0, this->getWidth(), this->getHeight());
if(layoutEditor)layoutEditor->setBounds(0, 0, this->getWidth(), this->getHeight());
#endif
}

//==============================================================================
void CabbagePluginAudioProcessorEditor::paint (Graphics& g)
{
	for(int i=0;i<getFilter()->getGUILayoutCtrlsSize();i++){
		if(getFilter()->getGUILayoutCtrls(i).getStringProp("type").equalsIgnoreCase("keyboard")){
#ifdef Cabbage_Build_Standalone
		if(isMouseOver(true)){
			//this lets controls keep focus even when you are palying the keyboard
			layoutComps[i]->setWantsKeyboardFocus(true);
			layoutComps[i]->grabKeyboardFocus();
			layoutComps[i]->toFront(true);
		}
#endif
		}
	}
#ifdef Cabbage_Build_Standalone
	if(getFilter()->getCsoundInputFile().loadFileAsString().isEmpty()){
		g.setColour (Colours::black);
			g.fillAll();
			Image logo = ImageCache::getFromMemory (BinaryData::logo_cabbage_Black_png, BinaryData::logo_cabbage_Black_pngSize);
			g.drawImage(logo, 10, 10, getWidth(), getHeight()-60, 0, 0, logo.getWidth(), logo.getHeight());
	}
	else {
		/*g.setColour (Colours::black);
		 g.fillAll();
		 g.setColour (Colours::grey);
		 g.setOpacity (0.1);
		 g.fillAll();
		 */
		Colour bg = CabbageUtils::backgroundSkin();
		g.setColour (bg);
		g.fillAll();
	}
//componentPanel->toFront(true);
//componentPanel->grabKeyboardFocus();
#else
	Colour bg = CabbageUtils::backgroundSkin();
	g.setColour (bg);
	g.fillAll();
#endif
}

//==============================================================================
void CabbagePluginAudioProcessorEditor::InsertGUIControls()
{
//add layout controls, non interactive..
for(int i=0;i<getFilter()->getGUILayoutCtrlsSize();i++){
if(getFilter()->getGUILayoutCtrls(i).getStringProp("type")==T("image")){
		InsertImage(getFilter()->getGUILayoutCtrls(i));   
		}	
}
for(int i=0;i<getFilter()->getGUILayoutCtrlsSize();i++){

	if(getFilter()->getGUILayoutCtrls(i).getStringProp("type")==T("form")){
		SetupWindow(getFilter()->getGUILayoutCtrls(i));   //set main application
		}
	else if(getFilter()->getGUILayoutCtrls(i).getStringProp("type")==T("groupbox")){
		InsertGroupBox(getFilter()->getGUILayoutCtrls(i));  
		}
	else if(getFilter()->getGUILayoutCtrls(i).getStringProp("type")==T("tabbox")){
//		InsertTabBox(getFilter()->getGUILayoutCtrls(i));   
		}
	else if(getFilter()->getGUILayoutCtrls(i).getStringProp("type")==T("keyboard")){
		InsertMIDIKeyboard(getFilter()->getGUILayoutCtrls(i));   
		}
	else if(getFilter()->getGUILayoutCtrls(i).getStringProp("type")==T("label")){
		InsertLabel(getFilter()->getGUILayoutCtrls(i));   
		}
	else if(getFilter()->getGUILayoutCtrls(i).getStringProp("type")==T("csoundoutput")){
		InsertCsoundOutput(getFilter()->getGUILayoutCtrls(i));   
		}
	else if(getFilter()->getGUILayoutCtrls(i).getStringProp("type")==T("vumeter")){
		InsertVUMeter(getFilter()->getGUILayoutCtrls(i));   
		}
	else if(getFilter()->getGUILayoutCtrls(i).getStringProp("type")==T("source")){
		InsertSourceButton(getFilter()->getGUILayoutCtrls(i));   
		}
}
//add interactive controls
for(int i=0;i<getFilter()->getGUICtrlsSize();i++){
	if(getFilter()->getGUICtrls(i).getStringProp("type")==T("hslider")
		||getFilter()->getGUICtrls(i).getStringProp("type")==T("vslider")
		||getFilter()->getGUICtrls(i).getStringProp("type")==T("rslider")){				
		InsertSlider(getFilter()->getGUICtrls(i));       //insert slider			
		}
	else if(getFilter()->getGUICtrls(i).getStringProp("type")==T("button")){				
		InsertButton(getFilter()->getGUICtrls(i));       //insert button	
		}
	else if(getFilter()->getGUICtrls(i).getStringProp("type")==T("checkbox")){				
		InsertCheckBox(getFilter()->getGUICtrls(i));       //insert checkbox
		}
	else if(getFilter()->getGUICtrls(i).getStringProp("type")==T("combobox")){				
		InsertComboBox(getFilter()->getGUICtrls(i));       //insert combobox	
		}
	else if(getFilter()->getGUICtrls(i).getStringProp("type")==T("xypad")){	
		InsertXYPad(getFilter()->getGUICtrls(i));       //insert xypad	
		}
	else if(getFilter()->getGUICtrls(i).getStringProp("type")==T("table")){	
		InsertTable(getFilter()->getGUICtrls(i));       //insert xypad	
		}
	}



}


//=======================================================================================
//	non-interactive components
//=======================================================================================
//+++++++++++++++++++++++++++++++++++++++++++
//					groupbox
//+++++++++++++++++++++++++++++++++++++++++++
void CabbagePluginAudioProcessorEditor::InsertGroupBox(CabbageGUIClass cAttr)
{
try{
	layoutComps.add(new CabbageGroupbox(cAttr.getStringProp("name"), 
										 cAttr.getStringProp("caption"), 
										 cAttr.getItems(0), 
										 cAttr.getStringProp("colour")
										 ));
	int idx = layoutComps.size()-1;

	float left = cAttr.getNumProp("left");
	float top = cAttr.getNumProp("top");
	float width = cAttr.getNumProp("width");
	float height = cAttr.getNumProp("height");

	if(cAttr.getStringProp("plant").length()>1){
		width = width*cAttr.getNumProp("scaleX");
		height = height*cAttr.getNumProp("scaleY");
	}

	int relY=0,relX=0;
	for(int y=0;y<layoutComps.size();y++){
	if(cAttr.getStringProp("reltoplant").length()>0)
	if(layoutComps[y]->getProperties().getWithDefault(String("plant"), -99).toString().equalsIgnoreCase(cAttr.getStringProp("reltoplant")))
	{
		relX = layoutComps[y]->getPosition().getX();
		relY = layoutComps[y]->getPosition().getY();
	}
	}

	((GroupComponent*)layoutComps[idx])->setBounds (left+relX, top+relY, width, height);
	layoutComps[idx]->toBack();
	componentPanel->addAndMakeVisible(layoutComps[idx]);
	layoutComps[idx]->getProperties().set(String("plant"), var(cAttr.getStringProp("plant")));

}
catch(...){
    Logger::writeToLog(T("Syntax error: 'groupbox..."));
    }
}

//+++++++++++++++++++++++++++++++++++++++++++
//					image
//+++++++++++++++++++++++++++++++++++++++++++
void CabbagePluginAudioProcessorEditor::InsertImage(CabbageGUIClass cAttr)
{
// this has to be ifdef-ed for different OS 
try{
	String pic;

#ifdef Cabbage_Build_Standalone
	pic = getFilter()->getCsoundInputFile().getParentDirectory().getFullPathName();
#else
	File thisFile(File::getSpecialLocation(File::currentApplicationFile)); 
	pic = thisFile.getParentDirectory().getFullPathName();
#endif
	float left = cAttr.getNumProp("left");
	float top = cAttr.getNumProp("top");
	float width = cAttr.getNumProp("width");
	float height = cAttr.getNumProp("height");


	if(cAttr.getStringProp("plant").length()>1){
		width = width*cAttr.getNumProp("scaleX");
		height = height*cAttr.getNumProp("scaleY");
	}

	int relY=0,relX=0;
	for(int y=0;y<layoutComps.size();y++){
	if(cAttr.getStringProp("reltoplant").length()>0)
	if(layoutComps[y]->getProperties().getWithDefault(String("plant"), -99).toString().equalsIgnoreCase(cAttr.getStringProp("reltoplant")))
	{
		relX = layoutComps[y]->getPosition().getX();
		relY = layoutComps[y]->getPosition().getY();
		width = width*layoutComps[y]->getProperties().getWithDefault(String("scaleX"), 1).toString().getFloatValue();
		height = height*layoutComps[y]->getProperties().getWithDefault(String("scaleY"), 1).toString().getFloatValue();
	}
	}

#ifdef LINUX
	pic.append(T("/")+String(cAttr.getStringProp("file")), 1024);
#else 
	pic.append(T("\\")+String(cAttr.getStringProp("file")), 1024);	
#endif

		if(cAttr.getStringProp("file").length()<2)pic="";
		layoutComps.add(new CabbageImage(cAttr.getStringProp("name"), pic, top+relY, left+relX, width, 
		height, cAttr.getStringProp("outline"), cAttr.getStringProp("colour"), 
		cAttr.getStringProp("shape"), cAttr.getNumProp("line")));

	int idx = layoutComps.size()-1;

	layoutComps[idx]->getProperties().set(String("scaleY"), cAttr.getNumProp("scaleY"));
	layoutComps[idx]->getProperties().set(String("scaleX"), cAttr.getNumProp("scaleX"));
	layoutComps[idx]->getProperties().set(String("plant"), var(cAttr.getStringProp("plant")));
	layoutComps[idx]->toBack();
	componentPanel->addAndMakeVisible(layoutComps[idx]);
	((CabbageImage*)layoutComps[idx])->setBounds (left+relX, top+relY, width, height);
	cAttr.setStringProp("yype", "image");
}
catch(...){
    Logger::writeToLog(T("Syntax error: 'groupbox..."));
    }
}

//+++++++++++++++++++++++++++++++++++++++++++
//					label
//+++++++++++++++++++++++++++++++++++++++++++
void CabbagePluginAudioProcessorEditor::InsertLabel(CabbageGUIClass cAttr)
{
try{
	layoutComps.add(new Label(cAttr.getStringProp("name"), cAttr.getStringProp("caption")));	
	int idx = layoutComps.size()-1;

	float left = cAttr.getNumProp("left");
	float top = cAttr.getNumProp("top");
	float width = cAttr.getNumProp("width");
	float height = cAttr.getNumProp("height");
	int relY=0,relX=0;
	for(int y=0;y<layoutComps.size();y++){
	if(cAttr.getStringProp("reltoplant").length()>0){
	if(layoutComps[y]->getProperties().getWithDefault(String("plant"), -99).toString().equalsIgnoreCase(cAttr.getStringProp("reltoplant")))
	{
		width = width*layoutComps[y]->getProperties().getWithDefault(String("scaleX"), 1).toString().getFloatValue();
		height = height*layoutComps[y]->getProperties().getWithDefault(String("scaleY"), 1).toString().getFloatValue();
		top = top*layoutComps[y]->getProperties().getWithDefault(String("scaleY"), 1).toString().getFloatValue();
		left = left*layoutComps[y]->getProperties().getWithDefault(String("scaleX"), 1).toString().getFloatValue();

		if(layoutComps[y]->getName().containsIgnoreCase("groupbox")||
			layoutComps[y]->getName().containsIgnoreCase("image"))
			{			
			layoutComps[idx]->setBounds(left, top, width, height);
			//if component is a member of a plant add it directly to the plant
			layoutComps[y]->addAndMakeVisible(layoutComps[idx]);
			}
	}
	}
	else{
	layoutComps[idx]->setBounds(left+relX, top+relY, width, height);
	componentPanel->addAndMakeVisible(layoutComps[idx]);
	}
	}

	cAttr.setStringProp("type", "label");
	((Label*)layoutComps[idx])->setFont(Font(height));

if(cAttr.getStringProp("colour").length()>0){
	//text colour
	((GroupComponent*)layoutComps[idx])->setColour(0x1000281,
		Colours::findColourForName(cAttr.getStringProp("colour"), Colours::black));
	}
	layoutComps[idx]->getProperties().set(String("plant"), var(cAttr.getStringProp("plant")));
}
catch(...){
	Logger::writeToLog(T("Syntax error: 'label..."));
    }
}

//+++++++++++++++++++++++++++++++++++++++++++
//					window
//+++++++++++++++++++++++++++++++++++++++++++
void CabbagePluginAudioProcessorEditor::SetupWindow(CabbageGUIClass cAttr)
{
try{
	setName(cAttr.getStringProp("caption"));
	getFilter()->setPluginName(cAttr.getStringProp("caption"));
	int left = cAttr.getNumProp("left");
	int top = cAttr.getNumProp("top");
	int width = cAttr.getNumProp("width");
	int height = cAttr.getNumProp("height");
	setSize(width, height);
	componentPanel->setBounds(left, top, width, height);
	formColour = Colours::findColourForName(cAttr.getStringProp("colour"), Colours::floralwhite);
#ifdef Cabbage_GUI_Editor
	componentPanel->setCompColour(cAttr.getStringProp("colour"));
#else
	formColour = Colours::findColourForName(cAttr.getStringProp("colour"), Colours::floralwhite);
#endif

	
#ifdef Cabbage_Build_Standalone
	formPic = getFilter()->getCsoundInputFile().getParentDirectory().getFullPathName();

#else
	File thisFile(File::getSpecialLocation(File::currentApplicationFile)); 
		formPic = thisFile.getParentDirectory().getFullPathName();
#endif

#ifdef LINUX
	formPic.append(T("/")+String(cAttr.getStringProp("file")), 1024);
#else 
	formPic.append(T("\\")+String(cAttr.getStringProp("file")), 1024);	
#endif

	if(cAttr.getStringProp("file").length()<2)
		formPic = "";

	this->resized();

	//add a dummy control to our layoutComps vector so that our filters layout vector 
	//is the same size as our editors one. 
	layoutComps.add(new Component());
}
catch(...){
    Logger::writeToLog(T("Syntax error: 'form..."));
    }
	



}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//	Csound output widget. 
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CabbagePluginAudioProcessorEditor::InsertCsoundOutput(CabbageGUIClass cAttr)
{
try{

	layoutComps.add(new CabbageMessageConsole(cAttr.getStringProp("name"), 
										 cAttr.getStringProp("caption"), 
										 cAttr.getStringProp("text")));	
	int idx = layoutComps.size()-1;

	float left = cAttr.getNumProp("left");
	float top = cAttr.getNumProp("top");
	float width = cAttr.getNumProp("width");
	float height = cAttr.getNumProp("height");

	//check to see if widgets is anchored
	//if it is offset it's position accordingly. 
	int relY=0,relX=0;
	for(int y=0;y<layoutComps.size();y++){
	if(cAttr.getStringProp("reltoplant").length()>0){
	if(layoutComps[y]->getProperties().getWithDefault(String("plant"), -99).toString().equalsIgnoreCase(cAttr.getStringProp("reltoplant")))
	{
		width = width*layoutComps[y]->getProperties().getWithDefault(String("scaleX"), 1).toString().getFloatValue();
		height = height*layoutComps[y]->getProperties().getWithDefault(String("scaleY"), 1).toString().getFloatValue();
		top = top*layoutComps[y]->getProperties().getWithDefault(String("scaleY"), 1).toString().getFloatValue();
		left = left*layoutComps[y]->getProperties().getWithDefault(String("scaleX"), 1).toString().getFloatValue();

		if(layoutComps[y]->getName().containsIgnoreCase("groupbox")||
			layoutComps[y]->getName().containsIgnoreCase("image"))
			{			
			layoutComps[idx]->setBounds(left, top, width, height);
			//if component is a member of a plant add it directly to the plant
			layoutComps[y]->addAndMakeVisible(layoutComps[idx]);
			}
	}
	}
	else{
	((CabbageMessageConsole*)layoutComps[idx])->setBounds(left+relX, top+relY, width, height);
	componentPanel->addAndMakeVisible(layoutComps[idx]);
	}
	}
	layoutComps[idx]->setName("csoundoutput");
	layoutComps[idx]->getProperties().set(String("plant"), var(cAttr.getStringProp("plant")));

}
catch(...){
    Logger::writeToLog(T("Syntax error: 'message console..."));
    }
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//	Sourece button. 
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CabbagePluginAudioProcessorEditor::InsertSourceButton(CabbageGUIClass cAttr)
{
try{

	layoutComps.add(new CabbageButton(cAttr.getStringProp("name"),
		cAttr.getStringProp("caption"),
		cAttr.getItems(1-(int)cAttr.getNumProp("value")),
		cAttr.getStringProp("colour")));	
	int idx = layoutComps.size()-1;

	float left = cAttr.getNumProp("left");
	float top = cAttr.getNumProp("top");
	float width = cAttr.getNumProp("width");
	float height = cAttr.getNumProp("height");

	//check to see if widgets is anchored
	//if it is offset it's position accordingly. 
	int relY=0,relX=0;
	for(int y=0;y<layoutComps.size();y++){
	if(cAttr.getStringProp("reltoplant").length()>0){
	if(layoutComps[y]->getProperties().getWithDefault(String("plant"), -99).toString().equalsIgnoreCase(cAttr.getStringProp("reltoplant")))
	{
		width = width*layoutComps[y]->getProperties().getWithDefault(String("scaleX"), 1).toString().getFloatValue();
		height = height*layoutComps[y]->getProperties().getWithDefault(String("scaleY"), 1).toString().getFloatValue();
		top = top*layoutComps[y]->getProperties().getWithDefault(String("scaleY"), 1).toString().getFloatValue();
		left = left*layoutComps[y]->getProperties().getWithDefault(String("scaleX"), 1).toString().getFloatValue();

		if(layoutComps[y]->getName().containsIgnoreCase("groupbox")||
			layoutComps[y]->getName().containsIgnoreCase("image"))
			{			
			layoutComps[idx]->setBounds(left, top, width, height);
			//if component is a member of a plant add it directly to the plant
			layoutComps[y]->addAndMakeVisible(layoutComps[idx]);
			}
	}
	}
	else{
	((CabbageButton*)layoutComps[idx])->setBounds(left+relX, top+relY, width, height);
	componentPanel->addAndMakeVisible(layoutComps[idx]);
	}
	}
	layoutComps[idx]->setName("sourceButton");
	layoutComps[idx]->getProperties().set(String("plant"), var(cAttr.getStringProp("plant")));
	((CabbageButton*)layoutComps[idx])->button->addListener(this);
	((CabbageButton*)layoutComps[idx])->button->setButtonText(cAttr.getItems(0));

}
catch(...){
    Logger::writeToLog(T("Syntax error: 'source button..."));
    }
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//	VU widget. 
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CabbagePluginAudioProcessorEditor::InsertVUMeter(CabbageGUIClass cAttr)
{
try{
	float left = cAttr.getNumProp("left");
	float top = cAttr.getNumProp("top");
	float width = cAttr.getNumProp("width");
	float height = cAttr.getNumProp("height");

	//set up meters. If config() is not used then the number of channels
	//is used to determine the layout. 1 = Mono, 2 = Stereo, and anything
	//above that will be an array of N mono channels. 

	float noOfMeters = cAttr.getNumProp("channels");
	Array<int> config;
	if(noOfMeters==1)
		config.add(1);
	else if(noOfMeters==2)
		config.add(2);
	else{
		int size = cAttr.getVUConfig().size();
		if(size>0)
			for(int i=0;i<size;i++){
			config.add(cAttr.getNumProp("configArray", i));
			}
		else
			for(int y=0;y<noOfMeters;y++)
			config.add(1);
	}

	layoutComps.add(new CabbageVUMeter(cAttr.getStringProp("name"), 
										 cAttr.getStringProp("text"),
										 cAttr.getStringProp("caption"), 
										 config));	
	int idx = layoutComps.size()-1;

	//check to see if widgets is anchored
	//if it is offset it's position accordingly. 
	int relY=0,relX=0;
	for(int y=0;y<layoutComps.size();y++){
	if(cAttr.getStringProp("reltoplant").length()>0){
	if(layoutComps[y]->getProperties().getWithDefault(String("plant"), -99).toString().equalsIgnoreCase(cAttr.getStringProp("reltoplant")))
	{
		width = width*layoutComps[y]->getProperties().getWithDefault(String("scaleX"), 1).toString().getFloatValue();
		height = height*layoutComps[y]->getProperties().getWithDefault(String("scaleY"), 1).toString().getFloatValue();
		top = top*layoutComps[y]->getProperties().getWithDefault(String("scaleY"), 1).toString().getFloatValue();
		left = left*layoutComps[y]->getProperties().getWithDefault(String("scaleX"), 1).toString().getFloatValue();

		if(layoutComps[y]->getName().containsIgnoreCase("groupbox")||
			layoutComps[y]->getName().containsIgnoreCase("image"))
			{			
			layoutComps[idx]->setBounds(left, top, width, height);
			//if component is a member of a plant add it directly to the plant
			layoutComps[y]->addAndMakeVisible(layoutComps[idx]);
			}
	}
	}
	else{
	((CabbageVUMeter*)layoutComps[idx])->setBounds(left+relX, top+relY, width, height);
	componentPanel->addAndMakeVisible(layoutComps[idx]);
	}
	}
	layoutComps[idx]->setName("vumeter");
	layoutComps[idx]->getProperties().set(String("plant"), var(cAttr.getStringProp("plant")));

}
catch(...){
    Logger::writeToLog(T("Syntax error: 'vu meter..."));
    }
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//	MIDI keyboard, I've this listed as non-interactive
// as it only sends MIDI, it doesn't communicate over the software bus
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CabbagePluginAudioProcessorEditor::InsertMIDIKeyboard(CabbageGUIClass cAttr)
{
try{

	layoutComps.add(new MidiKeyboardComponent(getFilter()->keyboardState,
                                     MidiKeyboardComponent::horizontalKeyboard));	
	int idx = layoutComps.size()-1;

	float left = cAttr.getNumProp("left");
	float top = cAttr.getNumProp("top");
	float width = cAttr.getNumProp("width");
	float height = cAttr.getNumProp("height");

	//check to see if widgets is anchored
	//if it is offset it's position accordingly. 
	int relY=0,relX=0;
	for(int y=0;y<layoutComps.size();y++){
	if(cAttr.getStringProp("reltoplant").length()>0){
	if(layoutComps[y]->getProperties().getWithDefault(String("plant"), -99).toString().equalsIgnoreCase(cAttr.getStringProp("reltoplant")))
	{
		width = width*layoutComps[y]->getProperties().getWithDefault(String("scaleX"), 1).toString().getFloatValue();
		height = height*layoutComps[y]->getProperties().getWithDefault(String("scaleY"), 1).toString().getFloatValue();
		top = top*layoutComps[y]->getProperties().getWithDefault(String("scaleY"), 1).toString().getFloatValue();
		left = left*layoutComps[y]->getProperties().getWithDefault(String("scaleX"), 1).toString().getFloatValue();

		if(layoutComps[y]->getName().containsIgnoreCase("groupbox")||
			layoutComps[y]->getName().containsIgnoreCase("image"))
			{			
			layoutComps[idx]->setBounds(left, top, width, height);
			//if component is a member of a plant add it directly to the plant
			layoutComps[y]->addAndMakeVisible(layoutComps[idx]);
			}
	}
	}
	else{
	((MidiKeyboardComponent*)layoutComps[idx])->setBounds(left+relX, top+relY, width, height);
	componentPanel->addAndMakeVisible(layoutComps[idx]);
	}
	}

	layoutComps[idx]->getProperties().set(String("plant"), var(cAttr.getStringProp("plant")));
#ifdef Cabbage_Build_Standalone
	layoutComps[idx]->setWantsKeyboardFocus(true);
	layoutComps[idx]->setAlwaysOnTop(true);
#endif
	layoutComps[idx]->addMouseListener(this, false);
	layoutComps[idx]->setName("midiKeyboard");
}
catch(...){
    Logger::writeToLog(T("Syntax error: 'keyboard..."));
    }
}

//=======================================================================================
//	interactive components - 'insert' methods followed by event methods
//=======================================================================================
//+++++++++++++++++++++++++++++++++++++++++++
//                                      slider
//+++++++++++++++++++++++++++++++++++++++++++
void CabbagePluginAudioProcessorEditor::InsertSlider(CabbageGUIClass cAttr)
{
try{
        float left = cAttr.getNumProp("left");
        float top = cAttr.getNumProp("top");
        float width = cAttr.getNumProp("width");
        float height = cAttr.getNumProp("height");     
        controls.add(new CabbageSlider(cAttr.getStringProp("name"),
                                                                                 cAttr.getStringProp("text"),
                                                                                 cAttr.getStringProp("caption"),
                                                                                 cAttr.getStringProp("kind"),
                                                                                 cAttr.getStringProp("colour"),
                                                                                 cAttr.getNumProp("textbox")
                                                                                 ));   
        int idx = controls.size()-1;
 
 
        int relY=0,relX=0;
        if(layoutComps.size()>0){
        for(int y=0;y<layoutComps.size();y++){
        if(cAttr.getStringProp("reltoplant").length()>0){
        if(layoutComps[y]->getProperties().getWithDefault(String("plant"), -99).toString().equalsIgnoreCase(cAttr.getStringProp("reltoplant")))
                {
                width = width*layoutComps[y]->getProperties().getWithDefault(String("scaleX"), 1).toString().getFloatValue();
                height = height*layoutComps[y]->getProperties().getWithDefault(String("scaleY"), 1).toString().getFloatValue();
                top = top*layoutComps[y]->getProperties().getWithDefault(String("scaleY"), 1).toString().getFloatValue();
                left = left*layoutComps[y]->getProperties().getWithDefault(String("scaleX"), 1).toString().getFloatValue();
 
                if(layoutComps[y]->getName().containsIgnoreCase("groupbox")||
                        layoutComps[y]->getName().containsIgnoreCase("image"))
                        {                      
                        controls[idx]->setBounds(left, top, width, height);
                        //if component is a member of a plant add it directly to the plant
                        layoutComps[y]->addAndMakeVisible(controls[idx]);
                        }
                }
        }
                else{
            controls[idx]->setBounds(left+relX, top+relY, width, height);
                componentPanel->addAndMakeVisible(controls[idx]);              
                }
        }
        }
        else{
            controls[idx]->setBounds(left+relX, top+relY, width, height);
                componentPanel->addAndMakeVisible(controls[idx]);              
        }
 
        if(cAttr.getStringProp("kind").equalsIgnoreCase("vertical"))
        ((CabbageSlider*)controls[idx])->setBounds(left+relX, top+relY, width, height);
        else if(cAttr.getStringProp("kind").equalsIgnoreCase("horizontal"))
        ((CabbageSlider*)controls[idx])->setBounds(left+relX, top+relY, width, height);


		((CabbageSlider*)controls[idx])->slider->setSkewFactor(cAttr.getNumProp("sliderSkew"));
        ((CabbageSlider*)controls[idx])->slider->setRange(cAttr.getNumProp("min"), cAttr.getNumProp("max"), (double)cAttr.getNumProp("sliderIncr"));
        ((CabbageSlider*)controls[idx])->slider->setValue(cAttr.getNumProp("value"));
        ((CabbageSlider*)controls[idx])->slider->addListener(this);
 
 
        controls[idx]->getProperties().set(String("midiChan"), cAttr.getNumProp("midiChan"));
        controls[idx]->getProperties().set(String("midiCtrl"), cAttr.getNumProp("midiCtrl")); 
}
catch(...){
    Logger::writeToLog(T("Syntax error: 'slider..."));
    }
}

					/******************************************/
					/*             slider event               */
					/******************************************/
void CabbagePluginAudioProcessorEditor::sliderValueChanged (Slider* sliderThatWasMoved)
{
#ifndef Cabbage_No_Csound

//check for parent name rather than slider name which is used as a place holder for a label
//Logger::writeToLog(sliderThatWasMoved->getParentComponent()->getName());

//if(sliderThatWasMoved->isEnabled()) // before sending data to on named channel
  //  {
    //if(RUNNING){make sure Csound is playing before calling SetChannel()
                for(int i=0;i<(int)getFilter()->getGUICtrlsSize();i++)//find correct control from vector
                        if(getFilter()->getGUICtrls(i).getStringProp("name")==sliderThatWasMoved->getParentComponent()->getName()){
						//getFilter()->getCsound()->SetChannel(getFilter()->getGUICtrls(i).getStringProp("channel").toUTF8(), sliderThatWasMoved->getValue());
                        //getFilter()->guiCtrls[i].value = (float)sliderThatWasMoved->getValue();
#ifndef Cabbage_Build_Standalone
						getFilter()->getGUICtrls(i).setNumProp("value", (float)sliderThatWasMoved->getValue());
                        //Logger::writeToLog(getFilter()->getGUICtrls(i).getPropsString());
						float min = getFilter()->getGUICtrls(i).getNumProp("min");
						float range = getFilter()->getGUICtrls(i).getNumProp("sliderRange");
						//getFilter()->setParameterNotifyingHost(i, (float)(sliderThatWasMoved->getValue()));
						//normalise parameters in plugin mode.
						getFilter()->beginParameterChangeGesture(i);
                        getFilter()->setParameterNotifyingHost(i, (float)((sliderThatWasMoved->getValue()-min)/range));
						getFilter()->endParameterChangeGesture(i);
						//Logger::writeToLog(String((float)((sliderThatWasMoved->getValue()-min)/range)));
#else
						//Logger::writeToLog("Current Slider Value"+String(sliderThatWasMoved->getValue()));
						getFilter()->setParameterNotifyingHost(i, (float)sliderThatWasMoved->getValue());
#endif
                        }
     else{// The next bit of code lets us change channel data even if Csound is not running
        for(int i=0;i<(int)getFilter()->getGUICtrlsSize();i++)//find correct control from vector
                if(getFilter()->getGUICtrls(i).getStringProp("name")==sliderThatWasMoved->getParentComponent()->getName()){
                                getFilter()->getGUICtrls(i).setNumProp("value", (float)sliderThatWasMoved->getValue());
                        getFilter()->getCsound()->SetChannel(getFilter()->getGUICtrls(i).getStringProp("channel").toUTF8(), sliderThatWasMoved->getValue());
                        }
                }
				
//}
#endif
}

//+++++++++++++++++++++++++++++++++++++++++++
//					button
//+++++++++++++++++++++++++++++++++++++++++++
void CabbagePluginAudioProcessorEditor::InsertButton(CabbageGUIClass cAttr)
{
try{
	controls.add(new CabbageButton(cAttr.getStringProp("name"),
		cAttr.getStringProp("caption"),
		cAttr.getItems(1-(int)cAttr.getNumProp("value")),
		cAttr.getStringProp("colour")));	
	int idx = controls.size()-1;

	float left = cAttr.getNumProp("left");
	float top = cAttr.getNumProp("top");
	float width = cAttr.getNumProp("width");
	float height = cAttr.getNumProp("height");

	//check to see if widgets is anchored
	//if it is offset it's position accordingly. 
	int relY=0,relX=0;
	if(layoutComps.size()>0){
	for(int y=0;y<layoutComps.size();y++)
	if(cAttr.getStringProp("reltoplant").length()>0){
	if(layoutComps[y]->getProperties().getWithDefault(String("plant"), -99).toString().equalsIgnoreCase(cAttr.getStringProp("reltoplant")))
		{
		width = width*layoutComps[y]->getProperties().getWithDefault(String("scaleX"), 1).toString().getFloatValue();
		height = height*layoutComps[y]->getProperties().getWithDefault(String("scaleY"), 1).toString().getFloatValue();
		top = top*layoutComps[y]->getProperties().getWithDefault(String("scaleY"), 1).toString().getFloatValue();
		left = left*layoutComps[y]->getProperties().getWithDefault(String("scaleX"), 1).toString().getFloatValue();

		if(layoutComps[y]->getName().containsIgnoreCase("groupbox")||
			layoutComps[y]->getName().containsIgnoreCase("image"))
			{			
			controls[idx]->setBounds(left, top, width, height);
			layoutComps[y]->addAndMakeVisible(controls[idx]);
			}
		}
	}
		else{
	    controls[idx]->setBounds(left+relX, top+relY, width, height);
		componentPanel->addAndMakeVisible(controls[idx]);		
		}
	}
	else{
	    controls[idx]->setBounds(left+relX, top+relY, width, height);
		componentPanel->addAndMakeVisible(controls[idx]);		
	}
	((CabbageButton*)controls[idx])->button->addListener(this);
	if(cAttr.getItemsSize()>0)
	((CabbageButton*)controls[idx])->button->setButtonText(cAttr.getItems(cAttr.getNumProp("value")));
#ifdef Cabbage_Build_Standalone
	((CabbageButton*)controls[idx])->button->setWantsKeyboardFocus(true);
#endif
}
catch(...){
    Logger::writeToLog(T("Syntax error: 'button..."));
    }
	
}

//+++++++++++++++++++++++++++++++++++++++++++
//					checkbox
//+++++++++++++++++++++++++++++++++++++++++++
void CabbagePluginAudioProcessorEditor::InsertCheckBox(CabbageGUIClass cAttr)
{
try{
	controls.add(new CabbageCheckbox(cAttr.getStringProp("name"),
		cAttr.getStringProp("caption"),
		cAttr.getItems(0),
		cAttr.getStringProp("colour")));	
	int idx = controls.size()-1;
	float left = cAttr.getNumProp("left");
	float top = cAttr.getNumProp("top");
	float width = cAttr.getNumProp("width");
	float height = cAttr.getNumProp("height");


	int relY=0,relX=0;
	if(layoutComps.size()>0){
	for(int y=0;y<layoutComps.size();y++){
	if(cAttr.getStringProp("reltoplant").length()>0){
	if(layoutComps[y]->getProperties().getWithDefault(String("plant"), -99).toString().equalsIgnoreCase(cAttr.getStringProp("reltoplant")))
		{
		width = width*layoutComps[y]->getProperties().getWithDefault(String("scaleX"), 1).toString().getFloatValue();
		height = height*layoutComps[y]->getProperties().getWithDefault(String("scaleY"), 1).toString().getFloatValue();
		top = top*layoutComps[y]->getProperties().getWithDefault(String("scaleY"), 1).toString().getFloatValue();
		left = left*layoutComps[y]->getProperties().getWithDefault(String("scaleX"), 1).toString().getFloatValue();

		if(layoutComps[y]->getName().containsIgnoreCase("groupbox")||
			layoutComps[y]->getName().containsIgnoreCase("image"))
			{			
			controls[idx]->setBounds(left, top, width, height);
			//if component is a member of a plant add it directly to the plant
			layoutComps[y]->addAndMakeVisible(controls[idx]);
			}
		}
	}
		else{
	    controls[idx]->setBounds(left+relX, top+relY, width, height);
		componentPanel->addAndMakeVisible(controls[idx]);		
		}
	}
	}
	else{
	    controls[idx]->setBounds(left+relX, top+relY, width, height);
		componentPanel->addAndMakeVisible(controls[idx]);		
	}
	
	((CabbageCheckbox*)controls[idx])->button->addListener(this);
	if(cAttr.getItemsSize()>0)
	((CabbageCheckbox*)controls[idx])->button->setButtonText(cAttr.getItems(0));

	if(cAttr.getNumProp("value")==1)
		((CabbageCheckbox*)controls[idx])->button->setToggleState(true, true);
	else
		((CabbageCheckbox*)controls[idx])->button->setToggleState(false, true);

#ifdef Cabbage_Build_Standalone
	((CabbageCheckbox*)controls[idx])->button->setWantsKeyboardFocus(true);
#endif
	Logger::writeToLog(cAttr.getPropsString());
}
catch(...){
    Logger::writeToLog(T("Syntax error: 'checkbox..."));
    }

}

						/*****************************************************/
						/*     button/filebutton/checkbox press event        */
						/*****************************************************/
void CabbagePluginAudioProcessorEditor::buttonClicked(Button* button)
{
#ifndef Cabbage_No_Csound
if(!getFilter()->isGuiEnabled()){
	if(button->isEnabled()){     // check button is ok before sending data to on named channel
	if(dynamic_cast<TextButton*>(button)){//check what type of button it is
		for(int i=0;i<(int)getFilter()->getGUICtrlsSize();i++){//find correct control from vector
			if(getFilter()->getGUICtrls(i).getStringProp("type")==T("button"))
			{
			//+++++++++++++button+++++++++++++
				if(getFilter()->getGUICtrls(i).getStringProp("name")==button->getName()){
				//toggle button values
				if(getFilter()->getGUICtrls(i).getNumProp("value")==0){
				getFilter()->setParameterNotifyingHost(i, 1.f);
				//for(int p=0;p<1024;p++)
				//getFilter()->getCsound()->TableSet(1, p, 0);
				getFilter()->getCsound()->SetChannel(getFilter()->getGUICtrls(i).getStringProp("channel").toUTF8(), 1.f);
				getFilter()->getGUICtrls(i).setNumProp("value", 1);
				}
				else{
				getFilter()->setParameterNotifyingHost(i, 0.f);
				getFilter()->getCsound()->SetChannel(getFilter()->getGUICtrls(i).getStringProp("channel").toUTF8(), 0.f);
				getFilter()->getGUICtrls(i).setNumProp("value", 0);
				}
				//toggle text values
				if(getFilter()->getGUICtrls(i).getItems(1)==button->getButtonText())
					button->setButtonText(getFilter()->getGUICtrls(i).getItems(0));
				else if(getFilter()->getGUICtrls(i).getItems(0)==button->getButtonText())
					button->setButtonText(getFilter()->getGUICtrls(i).getItems(1));

			}
			}
			//source button to show editor when in plugin mode
			else if(button->getName()=="source"){
				getFilter()->createAndShowSourceEditor(lookAndFeel);
			}
		}
	}
	else if(dynamic_cast<ToggleButton*>(button)){
     	for(int i=0;i<(int)getFilter()->getGUICtrlsSize();i++)//find correct control from vector
			if(getFilter()->getGUICtrls(i).getStringProp("name")==button->getName()){
			//	Logger::writeToLog(String(button->getToggleStateValue().getValue()));

				if(button->getToggleState()){
					button->setToggleState(true, false);
					//Logger::writeToLog("unpressed");
				}
				else{
					button->setToggleState(false, false);
					//Logger::writeToLog("pressed");
				}
				getFilter()->getCsound()->SetChannel(getFilter()->getGUICtrls(i).getStringProp("channel").toUTF8(), button->getToggleStateValue().getValue());
				getFilter()->setParameterNotifyingHost(i, button->getToggleStateValue().getValue());
     			}
 			}
	}
}//end of GUI enabled check
#endif
}




//+++++++++++++++++++++++++++++++++++++++++++
//					combobox
//+++++++++++++++++++++++++++++++++++++++++++
void CabbagePluginAudioProcessorEditor::InsertComboBox(CabbageGUIClass cAttr)
{
try{
	controls.add(new CabbageComboBox(cAttr.getStringProp("name"),
		cAttr.getStringProp("caption"),
		cAttr.getItems(0),
		cAttr.getStringProp("colour")));

	int idx = controls.size()-1;
	
	float left = cAttr.getNumProp("left");
	float top = cAttr.getNumProp("top");
	float width = cAttr.getNumProp("width");
	float height = cAttr.getNumProp("height");

	//check to see if widgets is anchored
	//if it is offset it's position accordingly. 
	int relY=0,relX=0;
	if(layoutComps.size()>0){
	for(int y=0;y<layoutComps.size();y++){
	if(cAttr.getStringProp("reltoplant").length()>0){
	if(layoutComps[y]->getProperties().getWithDefault(String("plant"), -99).toString().equalsIgnoreCase(cAttr.getStringProp("reltoplant")))
		{
		width = width*layoutComps[y]->getProperties().getWithDefault(String("scaleX"), 1).toString().getFloatValue();
		height = height*layoutComps[y]->getProperties().getWithDefault(String("scaleY"), 1).toString().getFloatValue();
		top = top*layoutComps[y]->getProperties().getWithDefault(String("scaleY"), 1).toString().getFloatValue();
		left = left*layoutComps[y]->getProperties().getWithDefault(String("scaleX"), 1).toString().getFloatValue();

		if(layoutComps[y]->getName().containsIgnoreCase("groupbox")||
			layoutComps[y]->getName().containsIgnoreCase("image"))
			{			
			controls[idx]->setBounds(left, top, width, height);
			//if component is a member of a plant add it directly to the plant
			layoutComps[y]->addAndMakeVisible(controls[idx]);
			}
		}
	}
		else{
	    controls[idx]->setBounds(left+relX, top+relY, width, height);
		componentPanel->addAndMakeVisible(controls[idx]);		
		}
	}
	}
	else{
	    controls[idx]->setBounds(left+relX, top+relY, width, height);
		componentPanel->addAndMakeVisible(controls[idx]);		
	}

//this needs some attention. 
//At present comboxbox colours can't be changed...

    for(int i=1;i<(int)cAttr.getItemsSize()+1;i++){
		String test  = cAttr.getItems(i-1);
		((CabbageComboBox*)controls[idx])->combo->addItem(cAttr.getItems(i-1), i);
		cAttr.setNumProp("maxItems", i);
	}

	((CabbageComboBox*)controls[idx])->combo->setSelectedId(cAttr.getNumProp("value"));
	componentPanel->addAndMakeVisible(((CabbageComboBox*)controls[idx]));
	((CabbageComboBox*)controls[idx])->setName(cAttr.getStringProp("name"));
	((CabbageComboBox*)controls[idx])->combo->addListener(this);
}
catch(...){
    Logger::writeToLog(T("Syntax error: 'combobox..."));
    }
}
					/******************************************/
					/*             combobBox event            */
					/******************************************/
void CabbagePluginAudioProcessorEditor::comboBoxChanged (ComboBox* combo)
{
#ifndef Cabbage_No_Csound
if(!getFilter()->isGuiEnabled()){
if(combo->isEnabled()) // before sending data to on named channel
    {
		for(int i=0;i<(int)getFilter()->getGUICtrlsSize();i++){//find correct control from vector
			String test = combo->getName();
			String test2 = getFilter()->getGUICtrls(i).getStringProp("name");
			if(getFilter()->getGUICtrls(i).getStringProp("name").equalsIgnoreCase(combo->getName())){
				for(int y=0;y<(int)getFilter()->getGUICtrls(i).getItemsSize();y++)
					if(getFilter()->getGUICtrls(i).getItems(y).equalsIgnoreCase(combo->getItemText(combo->getSelectedItemIndex()))){
						getFilter()->getCsound()->SetChannel(getFilter()->getGUICtrls(i).getStringProp("channel").toUTF8(), (float)combo->getSelectedItemIndex()+1);
 						getFilter()->getGUICtrls(i).setNumProp("value", (int)combo->getSelectedItemIndex()+1);
#ifndef Cabbage_Build_Standalone

						getFilter()->getCsound()->SetChannel(getFilter()->getGUICtrls(i).getStringProp("channel").toUTF8(), combo->getSelectedItemIndex());
						Logger::writeToLog(String("comboEvent():")+String(getFilter()->getGUICtrls(i).getNumProp("comboRange")));
						getFilter()->setParameterNotifyingHost(i, (float)(combo->getSelectedItemIndex())/(getFilter()->getGUICtrls(i).getNumProp("comboRange")-1));
#else
						getFilter()->setParameterNotifyingHost(i, (float)(combo->getSelectedItemIndex()+1));
#endif
					}
			}
		}
	}
     else{// The next bit of code lets us change channel data even if Csound is not running
     	for(int i=0;i<(int)getFilter()->getGUICtrlsSize();i++)//find correct control from vector
			if(getFilter()->getGUICtrls(i).getStringProp("name").equalsIgnoreCase(combo->getName())){
				for(int y=0;y<(int)getFilter()->getGUICtrls(i).getItemsSize();y++)
					if(getFilter()->getGUICtrls(i).getItems(y).equalsIgnoreCase(combo->getItemText(combo->getSelectedItemIndex()))){
						getFilter()->getGUICtrls(i).setNumProp("value", (float)combo->getSelectedItemIndex());
     				}
		}
	 }
}//end of GUI enabled check
#endif
}

//+++++++++++++++++++++++++++++++++++++++++++
//					xypad
//+++++++++++++++++++++++++++++++++++++++++++
void CabbagePluginAudioProcessorEditor::InsertXYPad(CabbageGUIClass cAttr)
{
/*
Our filters control vector contains two xypads, one for the X channel and one for the Y
channel. Our editor only needs to display one so the xypad with 'dummy' appended to the name
will be created but not shown. 
*/
try{
	controls.add(new CabbageXYController(cAttr.getStringProp("name"),
		cAttr.getStringProp("text"),
		cAttr.getStringProp("caption"),
		cAttr.getNumProp("minX"),
		cAttr.getNumProp("maxX"),
		cAttr.getNumProp("minY"),
		cAttr.getNumProp("maxY")));	
	int idx = controls.size()-1;

	float left = cAttr.getNumProp("left");    
	float top = cAttr.getNumProp("top");
	float width = cAttr.getNumProp("width");
	float height = cAttr.getNumProp("height");



	//check to see if widgets is anchored
	//if it is offset it's position accordingly. 
	int relY=0,relX=0;
	if(layoutComps.size()>0){
	for(int y=0;y<layoutComps.size();y++)
	if(cAttr.getStringProp("reltoplant").length()>0){
	if(layoutComps[y]->getProperties().getWithDefault(String("plant"), -99).toString().equalsIgnoreCase(cAttr.getStringProp("reltoplant")))
		{
		width = width*layoutComps[y]->getProperties().getWithDefault(String("scaleX"), 1).toString().getFloatValue();
		height = height*layoutComps[y]->getProperties().getWithDefault(String("scaleY"), 1).toString().getFloatValue();
		top = top*layoutComps[y]->getProperties().getWithDefault(String("scaleY"), 1).toString().getFloatValue();
		left = left*layoutComps[y]->getProperties().getWithDefault(String("scaleX"), 1).toString().getFloatValue();

		if(layoutComps[y]->getName().containsIgnoreCase("groupbox")||
			layoutComps[y]->getName().containsIgnoreCase("image"))
			{			
			controls[idx]->setBounds(left, top, width, height);
			if(!cAttr.getStringProp("name").containsIgnoreCase("dummy"))
			layoutComps[y]->addAndMakeVisible(controls[idx]);
			}
		}
	}
		else{
	    controls[idx]->setBounds(left+relX, top+relY, width, height);
		if(!cAttr.getStringProp("name").containsIgnoreCase("dummy"))
		componentPanel->addAndMakeVisible(controls[idx]);		
		}
	}
	else{
	    controls[idx]->setBounds(left+relX, top+relY, width, height);
		if(!cAttr.getStringProp("name").containsIgnoreCase("dummy"))
		componentPanel->addAndMakeVisible(controls[idx]);		
	}


	float max = cAttr.getNumProp("maxX");
	float min = cAttr.getNumProp("minX");
	float valueX = cabbageABS(min-cAttr.getNumProp("valueX"))/cabbageABS(min-max);
	//Logger::writeToLog(T("X:")+String(valueX));
	max = cAttr.getNumProp("maxY");
	min = cAttr.getNumProp("minY");
	float valueY = cabbageABS(min-cAttr.getNumProp("valueY"))/cabbageABS(min-max);
	//Logger::writeToLog(T("Y:")+String(valueY));
	((CabbageXYController*)controls[idx])->xypad->setBallXY(valueX, valueY, true);
#ifdef Cabbage_Build_Standalone	
	controls[idx]->setWantsKeyboardFocus(false);
#endif
	((CabbageXYController*)controls[idx])->xypad->addActionListener(this);
	if(!cAttr.getStringProp("name").containsIgnoreCase("dummy"))
	actionListenerCallback(cAttr.getStringProp("name"));

}  
catch(...){
    Logger::writeToLog(T("Syntax error: 'xy pad..."));
    }
	
}

//+++++++++++++++++++++++++++++++++++++++++++
//					table
//+++++++++++++++++++++++++++++++++++++++++++
void CabbagePluginAudioProcessorEditor::InsertTable(CabbageGUIClass cAttr)
{
int tableSize=0;
Array <float> tableValues;
try{
	//fill array with points from table, is table is valid
	if(getFilter()->getCompileStatus()==0){
	tableSize = getFilter()->getCsound()->TableLength(cAttr.getNumProp("tableNum"));
	tableValues = getFilter()->getTable(cAttr.getNumProp("tableNum"), tableSize);
	}
	else{
		//if table is not valid fill our array with at least one dummy point
		tableValues.add(0); 
	}
	controls.add(new CabbageTable(cAttr.getStringProp("name"),
		cAttr.getStringProp("caption"),
		cAttr.getItems(0),
		tableSize));	
	int idx = controls.size()-1;
	float left = cAttr.getNumProp("left");
	float top = cAttr.getNumProp("top");
	float width = cAttr.getNumProp("width");
	float height = cAttr.getNumProp("height");


	int relY=0,relX=0;
	if(layoutComps.size()>0){
	for(int y=0;y<layoutComps.size();y++){
	if(cAttr.getStringProp("reltoplant").length()>0){
	if(layoutComps[y]->getProperties().getWithDefault(String("plant"), -99).toString().equalsIgnoreCase(cAttr.getStringProp("reltoplant")))
		{
		width = width*layoutComps[y]->getProperties().getWithDefault(String("scaleX"), 1).toString().getFloatValue();
		height = height*layoutComps[y]->getProperties().getWithDefault(String("scaleY"), 1).toString().getFloatValue();
		top = top*layoutComps[y]->getProperties().getWithDefault(String("scaleY"), 1).toString().getFloatValue();
		left = left*layoutComps[y]->getProperties().getWithDefault(String("scaleX"), 1).toString().getFloatValue();

		if(layoutComps[y]->getName().containsIgnoreCase("groupbox")||
			layoutComps[y]->getName().containsIgnoreCase("image"))
			{			
			controls[idx]->setBounds(left, top, width, height);
			//if component is a member of a plant add it directly to the plant
			layoutComps[y]->addAndMakeVisible(controls[idx]);
			}
		}
	}
		else{
	    controls[idx]->setBounds(left+relX, top+relY, width, height);
		componentPanel->addAndMakeVisible(controls[idx]);		
		}
	}
	}
	else{
	    controls[idx]->setBounds(left+relX, top+relY, width, height);
		componentPanel->addAndMakeVisible(controls[idx]);		
	}
	
	((CabbageTable*)controls[idx])->fillTable(0, tableValues);
//	((CabbageTable*)controls[idx])->table->addActionListener(this);

	Logger::writeToLog(cAttr.getPropsString());
}
catch(...){
    Logger::writeToLog(T("Syntax error: 'table..."));
    }
	
}

					/******************************************/
					/*     actionlistener method (xypad/table)      */
					/******************************************/
void CabbagePluginAudioProcessorEditor::actionListenerCallback (const String& message){
//this event recieves action messages from custom components. For now it's only
//needed for the xypad but could potentially be used later for other custom controls
String type = message.substring(message.indexOf(T("|"))+1, 100);
String name = message.substring(0, message.indexOf(T("|"))); 

for(int i=0;i<(int)getFilter()->getGUICtrlsSize();i++)//find correct control from vector
	//if message came from an XY pad...
	if(type.equalsIgnoreCase(T("xycontroller"))){
		if(getFilter()->getGUICtrls(i).getStringProp("name")==name)
		{
			if(getFilter()->getGUICtrls(i).getStringProp("xyChannel").equalsIgnoreCase("X")){	
			getFilter()->getCsound()->SetChannel(getFilter()->getGUICtrls(i).getStringProp("xChannel").toUTF8(), ((CabbageXYController*)controls[i])->xypad->getBallX());
			getFilter()->getCsound()->SetChannel(getFilter()->getGUICtrls(i).getStringProp("yChannel").toUTF8(), ((CabbageXYController*)controls[i])->xypad->getBallY());
			}
#ifndef Cabbage_Build_Standalone 
            if(getFilter()->getGUICtrls(i).getStringProp("xyChannel").equalsIgnoreCase("X")){
						//normalising values to host 
						float min = getFilter()->getGUICtrls(i).getNumProp("min");
						float max = getFilter()->getGUICtrls(i).getNumProp("max");
						float ballPos  = ((CabbageXYController*)controls[i])->xypad->getBallX();
						float value = (ballPos - min)/(max-min);
						//Logger::writeToLog(T("actionListenerX:")+String(value));
						getFilter()->setParameterNotifyingHost(i, value);
						min = getFilter()->getGUICtrls(i+1).getNumProp("min");
						max = getFilter()->getGUICtrls(i+1).getNumProp("max");
						ballPos  = ((CabbageXYController*)controls[i])->xypad->getBallY();
						value = (ballPos - min)/(max-min);						
						//Logger::writeToLog(T("actionListenerY:")+String(value));
						getFilter()->setParameterNotifyingHost(i+1, value);
			}
#else                        
			if(getFilter()->getGUICtrls(i).getStringProp("xyChannel").equalsIgnoreCase("X")){
			getFilter()->setParameterNotifyingHost(i, (float)(((CabbageXYController*)controls[i])->xypad->getBallX()));
			getFilter()->setParameterNotifyingHost(i+1, (float)(((CabbageXYController*)controls[i])->xypad->getBallY()));
			}
#endif
		}
	}
	//if message comes from a table
/*
	else if(type.equalsIgnoreCase(T("table"))){
	//now we know it's a table, just check teh channel and away we go...
		int tableNum = getFilter()->getGUICtrls(i).getNumProp("tableNum");
		Array<float> table = ((CabbageTable*)controls[i])->getTable();
		if(getFilter()->getGUICtrls(i).getStringProp("name")==name)
		{	
			getFilter()->setParameterNotifyingHost(i, 0);
			for(int y=0;y<table.size();y++){
			getFilter()->getCsound()->TableSet(tableNum, y, table[y]);
			//Logger::writeToLog(String(table[y]));
			}
			getFilter()->setParameterNotifyingHost(i, 1);

		}
	}
	*/
}

//=============================================================================
bool CabbagePluginAudioProcessorEditor::keyPressed(const juce::KeyPress &key ,Component *)
{
#ifndef Cabbage_No_Csound
if(!getFilter()->isGuiEnabled()){
getFilter()->getCsound()->KeyPressed(key.getTextCharacter());
//search through controls to see which is attached to the current key being pressed. 
for(int i=0;i<(int)getFilter()->getGUICtrlsSize();i++){
	if(controls[i])
		if(getFilter()->getGUICtrls(i).getKeySize()>0)
		if(getFilter()->getGUICtrls(i).getkey(0).equalsIgnoreCase(key.getTextDescription()))
		if(key.isCurrentlyDown()){
			if(getFilter()->getGUICtrls(i).getStringProp("type")==T("button"))
				this->buttonClicked(((CabbageButton*)controls[i])->button);
			else if(getFilter()->getGUICtrls(i).getStringProp("type")==T("checkbox")){
				if(((CabbageCheckbox*)controls[i])->button->getToggleState())
					((CabbageCheckbox*)controls[i])->button->setToggleState(0, false);
				else
					((CabbageCheckbox*)controls[i])->button->setToggleState(1, false);
				this->buttonClicked(((CabbageCheckbox*)controls[i])->button);
			}
		}
	
}
}//end of GUI enabled check
#endif
return true;
}
//==============================================================================
void CabbagePluginAudioProcessorEditor::timerCallback()
{	
// update our GUI so that whenever a VST parameter is changed in the 
// host the corresponding GUI control gets updated. 
// It is possible in here to update our GUI controls with control
// signals from Csound. I've removed this for now as most host allow automation.
// It may prove useful however when running Cabbage in standalone mode...
#ifndef Cabbage_No_Csound	
#ifndef Cabbage_Build_Standalone
for(int i=0;i<(int)getFilter()->getGUICtrlsSize();i++){
	//inValue = getFilter()->getParameter(i);
	if(getFilter()->getGUICtrls(i).getStringProp("type")==T("hslider")||
			getFilter()->getGUICtrls(i).getStringProp("type")==T("rslider")||
			getFilter()->getGUICtrls(i).getStringProp("type")==T("vslider")){
	if(controls[i]){
		//if(getFilter()->getGUICtrls(i).getNumProp("value")!= getFilter()->getParameter(i))
		float val = getFilter()->getGUICtrls(i).getNumProp("sliderRange")*getFilter()->getParameter(i);
		((CabbageSlider*)controls[i])->slider->setValue(val, false);
	}
	}
	
	else if(getFilter()->getGUICtrls(i).getStringProp("type")==T("button")){
	if(controls[i])
		((CabbageButton*)controls[i])->button->setButtonText(getFilter()->getGUICtrls(i).getItems(1-(int)inValue));
	}
	
	else if(getFilter()->getGUICtrls(i).getStringProp("type")==T("xypad") &&
		getFilter()->getGUICtrls(i).getStringProp("xyChannel").equalsIgnoreCase("X")){
	if(controls[i]){
		((CabbageXYController*)controls[i])->xypad->setBallXY(getFilter()->getParameter(i), getFilter()->getParameter(i+1), false);
		}
	}

	else if(getFilter()->getGUICtrls(i).getStringProp("type")==T("combobox")){
	//if(controls[i])
#ifdef Cabbage_Build_Standalone
	//	((CabbageComboBox*)controls[i])->combo->setSelectedId((int)getFilter()->getParameter(i), false);
#else
		//Logger::writeToLog(T("timerCallback():")+String(getFilter()->getParameter(i)));
		((CabbageComboBox*)controls[i])->combo->setSelectedId(int(getFilter()->getParameter(i)+.5), false);
#endif
	}

	else if(getFilter()->getGUICtrls(i).getStringProp("type")==T("checkbox")){
	if(controls[i]){
	//		((CabbageCheckbox*)controls[i])->button->setToggleState((bool)inValue, false);
	//		getFilter()->getCsound()->SetChannel(getFilter()->getGUICtrls(i).getStringProp("channel").toUTF8(), inValue);
			}
		}
}

for(int i=0;i<(int)getFilter()->getGUILayoutCtrlsSize();i++){
	if(getFilter()->getGUILayoutCtrls(i).getStringProp("type")==T("hostbpm")){	    
    if (getFilter()->getPlayHead() != 0 && getFilter()->getPlayHead()->getCurrentPosition (hostInfo))
		getFilter()->getCsound()->SetChannel(getFilter()->getGUILayoutCtrls(i).getStringProp("channel").toUTF8(), hostInfo.bpm);
	}
	else if(getFilter()->getGUILayoutCtrls(i).getStringProp("type")==T("hosttime")){	    
    if (getFilter()->getPlayHead() != 0 && getFilter()->getPlayHead()->getCurrentPosition (hostInfo))
		getFilter()->getCsound()->SetChannel(getFilter()->getGUILayoutCtrls(i).getStringProp("channel").toUTF8(), hostInfo.timeInSeconds);
	}
	else if(getFilter()->getGUILayoutCtrls(i).getStringProp("type")==T("hostplaying")){	    
    if (getFilter()->getPlayHead() != 0 && getFilter()->getPlayHead()->getCurrentPosition (hostInfo))
		getFilter()->getCsound()->SetChannel(getFilter()->getGUILayoutCtrls(i).getStringProp("channel").toUTF8(), hostInfo.isPlaying);
	}
	else if(getFilter()->getGUILayoutCtrls(i).getStringProp("type")==T("hostrecording")){	    
    if (getFilter()->getPlayHead() != 0 && getFilter()->getPlayHead()->getCurrentPosition (hostInfo))
		getFilter()->getCsound()->SetChannel(getFilter()->getGUILayoutCtrls(i).getStringProp("channel").toUTF8(), hostInfo.isRecording);
	}
	else if(getFilter()->getGUILayoutCtrls(i).getStringProp("type")==T("hostppqpos")){	    
    if (getFilter()->getPlayHead() != 0 && getFilter()->getPlayHead()->getCurrentPosition (hostInfo))
		getFilter()->getCsound()->SetChannel(getFilter()->getGUILayoutCtrls(i).getStringProp("channel").toUTF8(), hostInfo.ppqPosition);
	debugLabel->setText(String(hostInfo.ppqPosition), false);
	}
}
#endif
  
for(int i=0;i<getFilter()->getGUILayoutCtrlsSize();i++){
	//String test = getFilter()->getGUILayoutCtrls(i).getStringProp("type");
	//Logger::writeToLog(test);
	if(getFilter()->getGUILayoutCtrls(i).getStringProp("type").containsIgnoreCase("csoundoutput")){
		((CabbageMessageConsole*)layoutComps[i])->editor->setText(getFilter()->getCsoundOutput());
		((CabbageMessageConsole*)layoutComps[i])->editor->setCaretPosition(getFilter()->getCsoundOutput().length());
	}
	else if(getFilter()->getGUILayoutCtrls(i).getStringProp("type").containsIgnoreCase("vumeter")){
		//Logger::writeToLog(layoutComps[i]->getName());
		for(int y=0;y<((CabbageVUMeter*)layoutComps[i])->getNoMeters();y++){
		//String chann = getFilter()->getGUILayoutCtrls(i).getStringProp("channel", y);
		float val = getFilter()->getCsound()->GetChannel(getFilter()->getGUILayoutCtrls(i).getStringProp("channel", y).toUTF8());
		((CabbageVUMeter*)layoutComps[i])->vuMeter->setVULevel(y, val);
		}
	}
	
}

//Check to see if I can use the same means to control sliders if they are in the host application. But 
//	I can't unless I hack the proceeBlcok() method...
#ifdef Cabbage_Build_Standalone
	//make sure that the instrument needs midi before turning this on
   MidiMessage message(0xf4, 0, 0, 0);
  
   if(!getFilter()->ccBuffer.isEmpty()){
   MidiBuffer::Iterator y(getFilter()->ccBuffer);
   int messageFrameRelativeTothisProcess;
   while (y.getNextEvent (message, messageFrameRelativeTothisProcess))
   {
   if(message.isController())
	 for(int i=0;i<(int)getFilter()->getGUICtrlsSize();i++)
	   if((message.getChannel()==getFilter()->getGUICtrls(i).getNumProp("midichan"))&&
		   (message.getControllerNumber()==getFilter()->getGUICtrls(i).getNumProp("midictrl")))
	   {
				float value = message.getControllerValue()/127.f*
					(getFilter()->getGUICtrls(i).getNumProp("max")-getFilter()->getGUICtrls(i).getNumProp("min")+
					getFilter()->getGUICtrls(i).getNumProp("min"));

				if(getFilter()->getGUICtrls(i).getStringProp("type")==T("hslider")||
						getFilter()->getGUICtrls(i).getStringProp("type")==T("rslider")||
						getFilter()->getGUICtrls(i).getStringProp("type")==T("vslider")){
				if(controls[i])
						((CabbageSlider*)controls[i])->slider->setValue(value, false);
				}
				else if(getFilter()->getGUICtrls(i).getStringProp("type")==T("button")){
				if(controls[i])
					((CabbageButton*)controls[i])->button->setButtonText(getFilter()->getGUICtrls(i).getItems(1-(int)value));
				}
				else if(getFilter()->getGUICtrls(i).getStringProp("type")==T("combobox")){
					if(controls[i]){
					((CabbageComboBox*)controls[i])->combo->setSelectedId((int)value+1.5, false);
					}
				}
				else if(getFilter()->getGUICtrls(i).getStringProp("type")==T("checkbox")){
				if(controls[i])
					if(value==0)
						((CabbageCheckbox*)controls[i])->button->setToggleState(0, false);
					else
						((Button*)controls[i])->setToggleState(1, false);
				}
				getFilter()->getCsound()->SetChannel(getFilter()->getGUICtrls(i).getStringProp("channel").toUTF8(), value);				
	if(message.isController())
      if(getFilter()->getMidiDebug()){
					String debugInfo =  T("MIDI Channel:    ")+String(message.getChannel())+T(" \t")+
										T("MIDI Controller: ")+String(message.getControllerNumber())+T("\t")+
										T("MIDI Value:      ")+String(message.getControllerValue())+T("\n");
					getFilter()->addDebugMessage(debugInfo);
					getFilter()->sendChangeMessage();
				 }
	   }
}
   }
   getFilter()->ccBuffer.clear();
   
  #endif
#endif


}
//==============================================================================
