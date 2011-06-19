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
#include  "CabbageCustomWidgets.h"

#ifdef Cabbage_GUI_Editor
#include "ComponentLayoutEditor.h"
#include "CabbageMainPanel.h"
#endif


//==============================================================================
CabbagePluginAudioProcessorEditor::CabbagePluginAudioProcessorEditor (CabbagePluginAudioProcessor* ownerFilter)
: AudioProcessorEditor (ownerFilter), lineNumber(0)
{
//This size will be altered if a valid file is input
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


componentPanel->addKeyListener(this);
componentPanel->setInterceptsMouseClicks(false, true);	
setSize (400, 400);
InsertGUIControls();
startTimer(10);

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

}

CabbagePluginAudioProcessorEditor::~CabbagePluginAudioProcessorEditor()
{
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
//this needs to be modified so that it works with both pos()+size() as well as bounds()
#ifdef Cabbage_GUI_Editor
StringArray csdArray;
String temp;
csdArray.addLines(getFilter()->getCsoundInputFileText());

if(componentPanel->getMouseState().equalsIgnoreCase("down")){
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
}
else if(componentPanel->getMouseState().equalsIgnoreCase("up")){
//ONLY SEND UPDATED INFO ON A MOUSE UP
Logger::writeToLog(componentPanel->getCurrentBounds());
temp = csdArray[lineNumber].replace(T("bounds()"), componentPanel->getCurrentBounds());
csdArray.set(lineNumber, temp);
Logger::writeToLog(csdArray[lineNumber]);
getFilter()->updateCsoundFile(csdArray.joinIntoString("\n"));
getFilter()->setGuiEnabled(true);
getFilter()->setChangeMessageType("GUI_edit");	
getFilter()->sendChangeMessage();
repaint();
}
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
		 Logger::writeToLog(String(getFilter()->getCurrentLine()));

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
if(getFilter()->getCsoundStatus()){
	if(formPic.length()>2)
			{
			Image img = ImageCache::getFromFile (File (formPic));
			g.drawImage(img, 0, 0, getWidth(), getHeight(), 0, 0, img.getWidth(), img.getHeight());
			}
		else
			g.fillAll (formColour);

//componentPanel->toFront(true);
#ifdef Cabbage_Build_Standalone
componentPanel->grabKeyboardFocus();
#endif

#ifdef Cabbage_GUI_Editor

#endif
}

else{
#ifndef Cabbage_No_Csound
	Image cabbageImg;
	File thisFile(File::getSpecialLocation(File::currentApplicationFile));
	cabbageImg = ImageCache::getFromFile (thisFile.withFileExtension(T(".png")).getFullPathName());
	g.drawImage(cabbageImg, 0, 0, getWidth(), getHeight(),
                 0, 0, cabbageImg.getWidth(), cabbageImg.getHeight());
	//g.drawFittedText(T("Csound did not compile correctly or you have support for Csound turned off"), 
	//	20, getHeight()-40, getWidth()-40, 30, Justification::centred, 3); 
#endif	
	}
	
}

//==============================================================================
void CabbagePluginAudioProcessorEditor::InsertGUIControls()
{
//add layout controls, non interactive..
for(int i=0;i<getFilter()->getGUILayoutCtrlsSize();i++){
	Logger::writeToLog(getFilter()->getGUILayoutCtrls(i).getStringProp("type"));
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
	else if(getFilter()->getGUILayoutCtrls(i).getStringProp("type")==T("image")){
		InsertImage(getFilter()->getGUILayoutCtrls(i));   
		}
	else if(getFilter()->getGUILayoutCtrls(i).getStringProp("type")==T("label")){
		InsertLabel(getFilter()->getGUILayoutCtrls(i));   
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
	layoutComps.add(new GroupComponent(cAttr.getStringProp("name"), 
											cAttr.getStringProp("caption")));	
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
	cAttr.setStringProp("yype", "groupbbox");

	if(cAttr.getStringProp("colour").length()>0){
	((GroupComponent*)layoutComps[idx])->setColour(GroupComponent::outlineColourId,
		Colours::findColourForName(cAttr.getStringProp("colour"), Colours::black));
	((GroupComponent*)layoutComps[idx])->setColour(GroupComponent::textColourId,
		Colours::findColourForName(cAttr.getStringProp("colour"), Colours::black));
	}
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
	layoutComps.add(new CabbageImage("name", pic, top+relY, left+relX, width, 
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
	if(cAttr.getStringProp("reltoplant").length()>0)
	if(layoutComps[y]->getProperties().getWithDefault(String("plant"), -99).toString().equalsIgnoreCase(cAttr.getStringProp("reltoplant")))
	{
		relX = layoutComps[y]->getPosition().getX();
		relY = layoutComps[y]->getPosition().getY();
	}
	}

	((Label*)layoutComps[idx])->setBounds(left+relX, top+relY, width, height);
	componentPanel->addAndMakeVisible(layoutComps[idx]);
	cAttr.setStringProp("type", "label");
	((Label*)layoutComps[idx])->setFont(Font(height));

if(cAttr.getStringProp("colour").length()>0){
	((GroupComponent*)layoutComps[idx])->setColour(Label::textColourId,
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
	this->setName(cAttr.getStringProp("caption"));
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
}
catch(...){
    Logger::writeToLog(T("Syntax error: 'form..."));
    }
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//	MIDI keyboard, I've this listed as non-interactive
// as it only sends MIDI, it doesn't communicate over the software bus
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CabbagePluginAudioProcessorEditor::InsertMIDIKeyboard(CabbageGUIClass cAttr)
{
try{
	float left = cAttr.getNumProp("left");
	float top = cAttr.getNumProp("top");
	float width = cAttr.getNumProp("width");
	float height = cAttr.getNumProp("height");

	//check to see if widgets is anchored
	//if it is offset it's position accordingly. 
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

	layoutComps.add(new MidiKeyboardComponent(getFilter()->keyboardState,
                                     MidiKeyboardComponent::horizontalKeyboard));	
	int idx = layoutComps.size()-1;

	((MidiKeyboardComponent*)layoutComps[idx])->setBounds(left+relX, top+relY, width, height);
    layoutComps[idx]->getProperties().set(String("plant"), var(cAttr.getStringProp("plant")));
	componentPanel->addAndMakeVisible(layoutComps[idx]);
}
catch(...){
    Logger::writeToLog(T("Syntax error: 'keyboard..."));
    }
}
//=======================================================================================
//	interactive components - 'insert' methods followed by event methods
//=======================================================================================
//+++++++++++++++++++++++++++++++++++++++++++
//					slider
//+++++++++++++++++++++++++++++++++++++++++++
void CabbagePluginAudioProcessorEditor::InsertSlider(CabbageGUIClass cAttr)
{
try{
	float left = cAttr.getNumProp("left");
	float top = cAttr.getNumProp("top");
	float width = cAttr.getNumProp("width");
	float height = cAttr.getNumProp("height");	
	controls.add(new CabbageSlider(cAttr.getStringProp("name"), 
										 cAttr.getStringProp("caption"), 
										 cAttr.getStringProp("kind"), 
										 cAttr.getStringProp("colour")
										 ));	
	int idx = controls.size()-1;

	//check to see if widgets is anchored
	//if it is offset it's position accordingly. 
	int relY=0,relX=0;
	for(int y=0;y<layoutComps.size();y++){
	if(cAttr.getStringProp("reltoplant").length()>0)
	if(layoutComps[y]->getProperties().getWithDefault(String("plant"), -99).toString().equalsIgnoreCase(cAttr.getStringProp("reltoplant")))
	{
		relX = layoutComps[y]->getPosition().getX();
		relY = layoutComps[y]->getPosition().getY();
		((CabbageSlider*)controls[idx])->getProperties().set("plantX", var(relX));
		((CabbageSlider*)controls[idx])->getProperties().set("plantY", var(relY));
		width = width*layoutComps[y]->getProperties().getWithDefault(String("scaleX"), 1).toString().getFloatValue();
		height = height*layoutComps[y]->getProperties().getWithDefault(String("scaleY"), 1).toString().getFloatValue();
		top = top*layoutComps[y]->getProperties().getWithDefault(String("scaleY"), 1).toString().getFloatValue();
		left = left*layoutComps[y]->getProperties().getWithDefault(String("scaleX"), 1).toString().getFloatValue();		
	}
	}

	if(cAttr.getStringProp("kind").equalsIgnoreCase("vertical"))
	((CabbageSlider*)controls[idx])->setBounds(left+relX, top+relY, width, height);
	else if(cAttr.getStringProp("kind").equalsIgnoreCase("horizontal"))
	((CabbageSlider*)controls[idx])->setBounds(left+relX, top+relY, width, height);
	else
	((CabbageSlider*)controls[idx])->setBounds(left+relX, top+relY, width, height);
	//controls[idx]->setBounds(cAttr.left, cAttr.top, cAttr.width, cAttr.height);

	componentPanel->addAndMakeVisible(controls[idx]);
	((CabbageSlider*)controls[idx])->slider->setRange(cAttr.getNumProp("min"), cAttr.getNumProp("max"), 0.01);
	((CabbageSlider*)controls[idx])->slider->setValue(cAttr.getNumProp("value"));
	((CabbageSlider*)controls[idx])->slider->repaint();
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
if(!getFilter()->isGuiEnabled()){
if(sliderThatWasMoved->isEnabled()) // before sending data to on named channel
    {
    //if(RUNNING){make sure Csound is playing before calling SetChannel()
		for(int i=0;i<(int)getFilter()->getGUICtrlsSize();i++)//find correct control from vector
			if(getFilter()->getGUICtrls(i).getStringProp("name")==sliderThatWasMoved->getName()){
				getFilter()->getCsound()->SetChannel(getFilter()->getGUICtrls(i).getStringProp("channel").toUTF8(), sliderThatWasMoved->getValue());
 			    //getFilter()->guiCtrls[i].value = (float)sliderThatWasMoved->getValue();
#ifndef Cabbage_Build_Standalone
				int range = getFilter()->getGUICtrls(i).getNumProp("max")-getFilter()->getGUICtrls(i).getNumProp("min");
				getFilter()->setParameterNotifyingHost(i, (float)(sliderThatWasMoved->getValue()-(getFilter()->getGUICtrls(i).getNumProp("min")))/range);
#else
				getFilter()->setParameterNotifyingHost(i, (float)(sliderThatWasMoved->getValue()));
#endif
				//showMessage(sliderThatWasMoved->getName());
     			}

     else{// The next bit of code lets us change channel data even if Csound is not running
        for(int i=0;i<(int)getFilter()->getGUICtrlsSize();i++)//find correct control from vector
     		if(getFilter()->getGUICtrls(i).getStringProp("name")==sliderThatWasMoved->getName()){
				getFilter()->getGUICtrls(i).setNumProp("value", (float)sliderThatWasMoved->getValue());
			getFilter()->getCsound()->SetChannel(getFilter()->getGUICtrls(i).getStringProp("channel").toUTF8(), sliderThatWasMoved->getValue());
 			}
		}
}
}//end of GUI enabled check
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
	for(int y=0;y<layoutComps.size();y++)
	if(cAttr.getStringProp("reltoplant").length()>0)
	if(layoutComps[y]->getProperties().getWithDefault(String("plant"), -99).toString().equalsIgnoreCase(cAttr.getStringProp("reltoplant")))
	{
		relX = layoutComps[y]->getPosition().getX();
		relY = layoutComps[y]->getPosition().getY();
		//retain relative component position
		((CabbageButton*)controls[idx])->getProperties().set("plantX", var(relX));
		((CabbageButton*)controls[idx])->getProperties().set("plantY", var(relY));
		width = width*layoutComps[y]->getProperties().getWithDefault(String("scaleX"), 1).toString().getFloatValue();
		height = height*layoutComps[y]->getProperties().getWithDefault(String("scaleY"), 1).toString().getFloatValue();
		top = top*layoutComps[y]->getProperties().getWithDefault(String("scaleY"), 1).toString().getFloatValue();
		left = left*layoutComps[y]->getProperties().getWithDefault(String("scaleX"), 1).toString().getFloatValue();
	}

	((CabbageButton*)controls[idx])->setBounds(left+relX, top+relY, width, height);
	((CabbageButton*)controls[idx])->button->setButtonText(cAttr.getItems(0));
	((CabbageButton*)controls[idx])->button->addListener(this);
	componentPanel->addAndMakeVisible(controls[idx]);
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

	//check to see if widgets is anchored
	//if it is offset it's position accordingly. 
	int relY=0,relX=0;
	for(int y=0;y<layoutComps.size();y++)
	if(cAttr.getStringProp("reltoplant").length()>0)
	if(layoutComps[y]->getProperties().getWithDefault(String("plant"), -99).toString().equalsIgnoreCase(cAttr.getStringProp("reltoplant")))
	{
		relX = layoutComps[y]->getPosition().getX();
		relY = layoutComps[y]->getPosition().getY();
		//retain relative component position
		((CabbageCheckbox*)controls[idx])->getProperties().set("plantX", var(relX));
		((CabbageCheckbox*)controls[idx])->getProperties().set("plantY", var(relY));
		width = width*layoutComps[y]->getProperties().getWithDefault(String("scaleX"), 1).toString().getFloatValue();
		height = height*layoutComps[y]->getProperties().getWithDefault(String("scaleY"), 1).toString().getFloatValue();
		top = top*layoutComps[y]->getProperties().getWithDefault(String("scaleY"), 1).toString().getFloatValue();
		left = left*layoutComps[y]->getProperties().getWithDefault(String("scaleX"), 1).toString().getFloatValue();
	}
	
	((CabbageCheckbox*)controls[idx])->setBounds(left+relX, top+relY, width, height);
	componentPanel->addAndMakeVisible(controls[idx]);
	((CabbageCheckbox*)controls[idx])->button->addListener(this);
	((CabbageButton*)controls[idx])->button->setButtonText(cAttr.getItems(0));
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
					button->setButtonText(getFilter()->getGUICtrls(i).getItems(1));
				else if(getFilter()->getGUICtrls(i).getItems(1)==button->getButtonText())
					button->setButtonText(getFilter()->getGUICtrls(i).getItems(1));

			}
			}
		}
	}
	else if(dynamic_cast<ToggleButton*>(button)){
     	for(int i=0;i<(int)getFilter()->getGUICtrlsSize();i++)//find correct control from vector
			if(getFilter()->getGUICtrls(i).getStringProp("name")==button->getName()){
				getFilter()->getCsound()->SetChannel(getFilter()->getGUICtrls(i).getStringProp("channel").toUTF8(), button->getToggleState());
 			    //getFilter()->guiCtrls[i].value = button->getToggleState();
				getFilter()->setParameterNotifyingHost(i, button->getToggleState());
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
	for(int y=0;y<layoutComps.size();y++)
	if(cAttr.getStringProp("reltoplant").length()>0)
	if(layoutComps[y]->getProperties().getWithDefault(String("plant"), -99).toString().equalsIgnoreCase(cAttr.getStringProp("reltoplant")))
	{
		relX = layoutComps[y]->getPosition().getX();
		relY = layoutComps[y]->getPosition().getY();
		//retain relative component position
		((CabbageComboBox*)controls[idx])->getProperties().set("plantX", var(relX));
		((CabbageComboBox*)controls[idx])->getProperties().set("plantY", var(relY));
		width = width*layoutComps[y]->getProperties().getWithDefault(String("scaleX"), 1).toString().getFloatValue();
		height = height*layoutComps[y]->getProperties().getWithDefault(String("scaleY"), 1).toString().getFloatValue();
		top = top*layoutComps[y]->getProperties().getWithDefault(String("scaleY"), 1).toString().getFloatValue();
		left = left*layoutComps[y]->getProperties().getWithDefault(String("scaleX"), 1).toString().getFloatValue();
	}
    
	controls[idx]->setBounds(left+relX, top+relY, width, height);

//this needs some attention. 
//At present comboxbox colours can't be changed...

    for(int i=1;i<(int)cAttr.getItemsSize()+1;i++){
		String test  = cAttr.getItems(i-1);
		((CabbageComboBox*)controls[idx])->combo->addItem(cAttr.getItems(i-1), i);
		cAttr.setNumProp("max", i);
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
						getFilter()->setParameterNotifyingHost(i, (float)(combo->getSelectedItemIndex())/((float)getFilter()->getGUICtrls(i).getNumProp("max")-1));
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


//=============================================================================
bool CabbagePluginAudioProcessorEditor::keyPressed(const juce::KeyPress &key ,juce::Component *)
{
#ifndef Cabbage_No_Csound
if(!getFilter()->isGuiEnabled()){
getFilter()->getCsound()->KeyPressed(key.getTextCharacter());
//search through controls to see which is attached to the current key being pressed. 
for(int i=0;i<(int)getFilter()->getGUICtrlsSize();i++){
if(getFilter()->getGUICtrls(i).getStringProp("type")==T("button")){
	if(controls[i])
		if(getFilter()->getGUICtrls(i).getKeySize()>0)
		if(getFilter()->getGUICtrls(i).getkey(0).equalsIgnoreCase(key.getTextDescription()))
		if(key.isCurrentlyDown()){
			this->buttonClicked((Button*)controls[i]);
			//Logger::writeToLog("key pressed");
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
	float value = getFilter()->getParameter(i);

	if(getFilter()->getGUICtrls(i).getStringProp("type")==T("hslider")||
			getFilter()->getGUICtrls(i).getStringProp("type")==T("rslider")||
			getFilter()->getGUICtrls(i).getStringProp("type")==T("vslider")){
	if(controls[i])
			((Slider*)controls[i])->setValue(value, false);
	}
	
	else if(getFilter()->getGUICtrls(i).getStringProp("type")==T("button")){
	if(controls[i])
		((Button*)controls[i])->setButtonText(getFilter()->getGUICtrls(i).getItems(1-(int)value));
	}
	
	else if(getFilter()->getGUICtrls(i).getStringProp("type")==T("combobox")){
	if(controls[i])
		((ComboBox*)controls[i])->setSelectedId((int)value, false);
	}

	else if(getFilter()->getGUICtrls(i).getStringProp("type")==T("checkbox")){
	if(controls[i])
	((Button*)controls[i])->setToggleState((bool)value, false);
	getFilter()->getCsound()->SetChannel(getFilter()->getGUICtrls(i).getStringProp("channel").toUTF8(), value);
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
#else
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
						((Slider*)controls[i])->setValue(value, false);
				}
				else if(getFilter()->getGUICtrls(i).getStringProp("type")==T("button")){
				if(controls[i])
					((Button*)controls[i])->setButtonText(getFilter()->getGUICtrls(i).getItems(1-(int)value));
				}
				else if(getFilter()->getGUICtrls(i).getStringProp("type")==T("combobox")){
					if(controls[i]){
					((ComboBox*)controls[i])->setSelectedId((int)value+1.5, false);
					}
				}
				else if(getFilter()->getGUICtrls(i).getStringProp("type")==T("checkbox")){
				if(controls[i])
					if(value==0)
						((Button*)controls[i])->setToggleState(0, false);
					else
						((Button*)controls[i])->setToggleState(1, false);
				}
				getFilter()->getCsound()->SetChannel(getFilter()->getGUICtrls(i).getStringProp("channel").toUTF8(), value);				
	   }
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
   getFilter()->ccBuffer.clear();
#endif
#endif

}
//==============================================================================
