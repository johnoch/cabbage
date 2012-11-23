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
#include "../ComponentLayoutEditor.h"
#include "../CabbageMainPanel.h"
#endif


//==============================================================================
CabbagePluginAudioProcessorEditor::CabbagePluginAudioProcessorEditor (CabbagePluginAudioProcessor* ownerFilter)
: AudioProcessorEditor (ownerFilter), lineNumber(0), inValue(0), authorText(""), keyIsPressed(false), xyPadIndex(0)
{
//set custom skin yo use
lookAndFeel = new CabbageLookAndFeel(); 
Component::setLookAndFeel(lookAndFeel);
oldSchoolLook = new OldSchoolLookAndFeel();
#ifdef Cabbage_GUI_Editor
//determine whether instrument should be opened in GUI mode or not
componentPanel = new CabbageMainPanel();
componentPanel->setLookAndFeel(lookAndFeel);
componentPanel->setBounds(0, 0, getWidth(), getHeight());
layoutEditor = new ComponentLayoutEditor();
layoutEditor->setLookAndFeel(lookAndFeel);
layoutEditor->setBounds(0, 0, getWidth(), getHeight());
addAndMakeVisible(layoutEditor);
addAndMakeVisible(componentPanel);
layoutEditor->setTargetComponent(componentPanel);
#else
componentPanel = new Component();
addAndMakeVisible(componentPanel);
#endif

#ifndef Cabbage_No_Csound
zero_dbfs = getFilter()->getCsound()->Get0dBFS();
#endif
componentPanel->addKeyListener(this);
componentPanel->setInterceptsMouseClicks(false, true);  
setSize (400, 400);
InsertGUIControls();

//this will prevent editors from creating xyAutos if they have already been crated. 
getFilter()->setHaveXYAutoBeenCreated(true);

//start timer for updating GUI controls if someone is using a host to contorl parameters. 
startTimer(30);


#ifdef Cabbage_GUI_Editor
componentPanel->addActionListener(this);
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
if(presetFileText.length()>1)
{
        SnapShotFile.replaceWithText(presetFileText);
}
#ifdef Cabbage_GUI_Editor
//delete componentPanel;
//delete layoutEditor;
#endif
}

//===========================================================================
//WHEN IN GUI EDITOR MODE THIS CALLBACK WILL NOTIFIY THE HOST IF A MOUSE UP
//HAS BEEN TRIGGERED BY ANY OF THE INSTRUMENTS WIDGETS, THIS IN TURN UPDATES
//THE SOURCE WITH THE NEW COORDINATES AND SIZE
//===========================================================================
void CabbagePluginAudioProcessorEditor::changeListenerCallback(ChangeBroadcaster *source)
{


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
ScopedPointer<XmlElement> xml;
xml = new XmlElement("PLANTS");
PopupMenu m;
Array<File> plantFiles;
m.setLookAndFeel(lookAndFeel);
if(getFilter()->isGuiEnabled()){
PopupMenu subm;
subm.setLookAndFeel(&this->getLookAndFeel());
subm.addItem(2, "rslider");
subm.addItem(3, "vslider");
subm.addItem(4, "hslider");
subm.addItem(5, "combobox");
subm.addItem(6, "checkbox");
subm.addItem(7, "groupbox");
subm.addItem(8, "image");
subm.addItem(9, "keyboard");
m.addSubMenu(String("Indigenous"), subm);
subm.clear();

String plantDir = appProperties->getUserSettings()->getValue("PlantFileDir", "");	
addFileToPpopupMenu(m, plantFiles, plantDir, "*.plant"); 
}

if (e.mods.isRightButtonDown())
 {
 int choice = m.show();
         /* the plan here is to simply send text to WinXound and get it
         to update the instrument. This way Cabbage don't have to keep track of 
         anything as all controls will automatically get added to the GUI controls vector
         when Cabbage is updated */
	 if(choice==1)
			 insertCabbageText(String("button bounds(")+String(e.getPosition().getX())+(", ")+String(e.getPosition().getY())+String(", 60, 25), channel(\"but1\"), text(\"on\", \"off\")"));
	 else if(choice==2)
			 insertCabbageText(String("rslider bounds(")+String(e.getPosition().getX())+(", ")+String(e.getPosition().getY())+String(", 50, 50), channel(\"rslider\"), text(\"\"), range(0, 100, 0)"));
	 else if(choice==3)
			 insertCabbageText(String("vslider bounds(")+String(e.getPosition().getX())+(", ")+String(e.getPosition().getY())+String(", 30, 200), channel(\"vslider\"), text(\"\"), range(0, 100, 0), colour(\"white\")"));
 	 else if(choice==4)
			 insertCabbageText(String("hslider bounds(")+String(e.getPosition().getX())+(", ")+String(e.getPosition().getY())+String(", 200, 30), channel(\"hslider\"), text(\"\"), range(0, 100, 0), colour(\"white\")"));
 	 else if(choice==5)
 			 insertCabbageText(String("combobox bounds(")+String(e.getPosition().getX())+(", ")+String(e.getPosition().getY())+String(", 100, 30), channel(\"combobox\"), items(\"Item 1\", \"Item 2\", \"Item 3\")"));
	 else if(choice==6)
	 		 insertCabbageText(String("checkbox bounds(")+String(e.getPosition().getX())+(", ")+String(e.getPosition().getY())+String(", 80, 20), channel(\"checkbox\"), text(\"checkbox\")"));
	 else if(choice==7)
			 insertCabbageText(String("groupbox bounds(")+String(e.getPosition().getX())+(", ")+String(e.getPosition().getY())+String(", 200, 150), text(\"groupbox\"), colour(\"black\"), caption(\"groupbBox\")"));
	 else if(choice==8)
			 insertCabbageText(String("image bounds(")+String(e.getPosition().getX())+(", ")+String(e.getPosition().getY())+String(", 200, 150), colour(\"white\"), line(0)"));
	 else if(choice==9)
			 insertCabbageText(String("keyboard bounds(")+String(e.getPosition().getX())+(", ")+String(e.getPosition().getY())+String(", 150, 60)"));
	 else if(choice>=100){
		 //showMessage(xml->getAttributeValue(choice-100));
		 //update X and Y positions from plants
		 String customPlantControl = plantFiles[choice-100].loadFileAsString();
		 //xml->getAttributeValue(choice-100);
		 if(!customPlantControl.contains("bounds("))
			 showMessage("Invalid control(No bounds identifier found)", &getLookAndFeel());
		 else{
		    CabbageGUIClass cAttr(customPlantControl, -99);
			String origBounds = cAttr.getStringProp("initBoundsText");
			//Logger::writeToLog(origBounds);
			cAttr.setNumProp("left", e.getPosition().getX());
			cAttr.setNumProp("top", e.getPosition().getY());
			String newBounds = cAttr.getStringProp("newBoundsText");
			//Logger::writeToLog(newBounds);
			customPlantControl = customPlantControl.replace(origBounds, newBounds);
			//make sure each plant uses a unique plant name
			if(customPlantControl.contains("plant")){
				String plantText = cAttr.getStringProp("plantText");
				String origPlant = cAttr.getStringProp("plant");
				int cnt = 1;
				while(cnt>0){
					if(!getFilter()->getCsoundInputFileText().contains(origPlant+String(cnt)))
						break;
					else
					cnt++;
				}
				String newPlant = origPlant+String(cnt);
				cAttr.setStringProp("plant", newPlant);
				customPlantControl = customPlantControl.replace(plantText, cAttr.getStringProp("plantText"));
				//Logger::writeToLog(customPlantControl);
			}
			insertCabbageText(customPlantControl);
		 }
	 }

	}
#endif
}

void CabbagePluginAudioProcessorEditor::setEditMode(bool on){
#ifdef Cabbage_GUI_Editor
		if(on){
			layoutEditor->setEnabled(true);
			layoutEditor->updateFrames();
			layoutEditor->toFront(true); 
		}
		else{
			layoutEditor->setEnabled(false);
			componentPanel->toFront(true);
			componentPanel->setInterceptsMouseClicks(false, true); 
			}
#endif
}

//==============================================================================
// this function will postion component
//==============================================================================
void CabbagePluginAudioProcessorEditor::positionComponentWithinPlant(String type, int idx, float left, float top, float width, float height, Component *layout, Component *control)
{			
//if dimensions are < 1 then the user is using the decimal proportional of positioning
if(width>1 && height>1){
width = width*layout->getProperties().getWithDefault(String("scaleX"), 1).toString().getFloatValue();
height = height*layout->getProperties().getWithDefault(String("scaleY"), 1).toString().getFloatValue();
top = top*layout->getProperties().getWithDefault(String("scaleY"), 1).toString().getFloatValue();
left = left*layout->getProperties().getWithDefault(String("scaleX"), 1).toString().getFloatValue();
}
else{    
	width = (width>1 ? .5 : width*layout->getWidth());
    height = (height>1 ? .5 : height*layout->getHeight());
	top = (top*layout->getHeight());
	left = (left*layout->getWidth());
}
	if(type.equalsIgnoreCase("rslider"))
		if(width<height) height = width;
		else if(height<width) width = height;

if(layout->getName().containsIgnoreCase("groupbox")||
        layout->getName().containsIgnoreCase("image"))
        {                       
        control->setBounds(left, top, width, height);
        layout->addAndMakeVisible(controls[idx]);
        }
}


//==============================================================================
// utility function to insert cabbage text into source code
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

	//Logger::writeToLog(String(lineNumber));
	getFilter()->updateCsoundFile(csdArray.joinIntoString("\n"));
	getFilter()->setGuiEnabled(true);
	getFilter()->sendActionMessage("GUI Updated, controls added");
}

//==============================================================================
void CabbagePluginAudioProcessorEditor::resized()
{
if(componentPanel)componentPanel->setBounds(0, 0, this->getWidth(), this->getHeight());
#ifdef Cabbage_GUI_Editor
if(layoutEditor)layoutEditor->setBounds(0, 0, this->getWidth(), this->getHeight());
#endif
}

//==============================================================================
void CabbagePluginAudioProcessorEditor::paint (Graphics& g)
{
        for(int i=0;i<getFilter()->getGUILayoutCtrlsSize();i++){
                if(getFilter()->getGUILayoutCtrls(i).getStringProp("type").equalsIgnoreCase("keyboard")){
#ifdef Cabbage_Build_Standalone
				if(keyIsPressed)
                if(isMouseOver(true)){
                        //this lets controls keep focus even when you are playing the ASCII keyboard
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
                //g.setColour (CabbageUtils::getBackgroundSkin());
                g.fillAll();

                Image logo = ImageCache::getFromMemory (BinaryData::logo_cabbage_Black_png, BinaryData::logo_cabbage_Black_pngSize);
				g.drawImage(logo, 10, 10, getWidth(), getHeight()-60, 0, 0, logo.getWidth(), logo.getHeight());

        }
        else {
                g.setColour(formColour);
                g.fillAll();

                g.setColour (CabbageUtils::getTitleFontColour());
                Image logo = ImageCache::getFromMemory (BinaryData::logo1_png, BinaryData::logo1_pngSize);
                //g.drawImage (logo, (getWidth()/2) - (logo.getWidth()*.35), getHeight()-25, logo.getWidth()*0.65, logo.getHeight()*0.65, 
                //      0, 0, logo.getWidth(), logo.getHeight(), true);
                g.drawImage (logo, (getWidth()) - (logo.getWidth()*.75), getHeight()-25, logo.getWidth()*0.65, logo.getHeight()*0.65, 
                        0, 0, logo.getWidth(), logo.getHeight(), true);
                g.setColour(fontColour);
                g.drawFittedText(authorText, 10, getHeight()-30, getWidth()*.65, logo.getHeight(), 1, 1);   
                //g.drawLine(10, getHeight()-27, getWidth()-10, getHeight()-27, 0.2);
        }
//componentPanel->toFront(true);
//componentPanel->grabKeyboardFocus();
#else
                g.setColour(formColour);
                g.fillAll();
                g.setColour (CabbageUtils::getTitleFontColour());
                Image logo = ImageCache::getFromMemory (BinaryData::logo1_png, BinaryData::logo1_pngSize);
                g.drawImage (logo, (getWidth()) - (logo.getWidth()*.75), getHeight()-25, logo.getWidth()*0.65, logo.getHeight()*0.65, 
                        0, 0, logo.getWidth(), logo.getHeight(), true);
                g.setColour(fontColour);
                g.drawFittedText(authorText, 10, getHeight()-30, getWidth()*.65, logo.getHeight(), 1, 1);  
#endif
}

//==============================================================================
void CabbagePluginAudioProcessorEditor::InsertGUIControls()
{
controls.clear();
layoutComps.clear();
//add layout controls, non interactive..
for(int i=0;i<getFilter()->getGUILayoutCtrlsSize();i++){

}
for(int i=0;i<getFilter()->getGUILayoutCtrlsSize();i++){
        if(getFilter()->getGUILayoutCtrls(i).getStringProp("type")==String("form")){
                SetupWindow(getFilter()->getGUILayoutCtrls(i));   //set main application
                }
        else if(getFilter()->getGUILayoutCtrls(i).getStringProp("type")==String("groupbox")){
                InsertGroupBox(getFilter()->getGUILayoutCtrls(i));  
                }
        else if(getFilter()->getGUILayoutCtrls(i).getStringProp("type")==String("image")){
                InsertImage(getFilter()->getGUILayoutCtrls(i));   
                }
        else if(getFilter()->getGUILayoutCtrls(i).getStringProp("type")==String("patmatrix")){
                InsertPatternMatrix(getFilter()->getGUILayoutCtrls(i));   
                }
        else if(getFilter()->getGUILayoutCtrls(i).getStringProp("type")==String("keyboard")){
                InsertMIDIKeyboard(getFilter()->getGUILayoutCtrls(i));   
                }
        else if(getFilter()->getGUILayoutCtrls(i).getStringProp("type")==String("label")){
                InsertLabel(getFilter()->getGUILayoutCtrls(i));   
                }
        else if(getFilter()->getGUILayoutCtrls(i).getStringProp("type")==String("csoundoutput")){
                InsertCsoundOutput(getFilter()->getGUILayoutCtrls(i));   
                }
        else if(getFilter()->getGUILayoutCtrls(i).getStringProp("type")==String("vumeter")){
                InsertVUMeter(getFilter()->getGUILayoutCtrls(i));   
                }
        else if(getFilter()->getGUILayoutCtrls(i).getStringProp("type")==String("snapshot")){
                InsertSnapshot(getFilter()->getGUILayoutCtrls(i));   
                }
        else if(getFilter()->getGUILayoutCtrls(i).getStringProp("type")==String("source")){
                InsertSourceButton(getFilter()->getGUILayoutCtrls(i));   
                }
        else if(getFilter()->getGUILayoutCtrls(i).getStringProp("type")==String("infobutton")){
                InsertInfoButton(getFilter()->getGUILayoutCtrls(i));   
                }
        else if(getFilter()->getGUILayoutCtrls(i).getStringProp("type")==String("line")){
                InsertLineSeparator(getFilter()->getGUILayoutCtrls(i));   
                }
        else if(getFilter()->getGUILayoutCtrls(i).getStringProp("type")==String("table")){      
                InsertTable(getFilter()->getGUILayoutCtrls(i));       //insert xypad    
                }
        else if(getFilter()->getGUILayoutCtrls(i).getStringProp("type")==String("pvsview")){    
                InsertPVSViewer(getFilter()->getGUILayoutCtrls(i));       //insert xypad        
                }
}
//add interactive controls
for(int i=0;i<getFilter()->getGUICtrlsSize();i++){
        if(getFilter()->getGUICtrls(i).getStringProp("type")==String("hslider")
                ||getFilter()->getGUICtrls(i).getStringProp("type")==String("vslider")
                ||getFilter()->getGUICtrls(i).getStringProp("type")==String("rslider")){                                
                InsertSlider(getFilter()->getGUICtrls(i));       //insert slider                        
                }
        else if(getFilter()->getGUICtrls(i).getStringProp("type")==String("button")){                           
                InsertButton(getFilter()->getGUICtrls(i));       //insert button        
                }
        else if(getFilter()->getGUICtrls(i).getStringProp("type")==String("checkbox")){                         
                InsertCheckBox(getFilter()->getGUICtrls(i));       //insert checkbox
                }
        else if(getFilter()->getGUICtrls(i).getStringProp("type")==String("combobox")){                         
                InsertComboBox(getFilter()->getGUICtrls(i));       //insert combobox    
                }
        else if(getFilter()->getGUICtrls(i).getStringProp("type")==String("xypad")){    
                InsertXYPad(getFilter()->getGUICtrls(i));       //insert xypad  
                }
        else if(getFilter()->getGUICtrls(i).getStringProp("type")==String("table")){    
                InsertTable(getFilter()->getGUICtrls(i));       //insert xypad  
                }
        }




}


//=======================================================================================
//      non-interactive components
//=======================================================================================
//+++++++++++++++++++++++++++++++++++++++++++
//                                      groupbox
//+++++++++++++++++++++++++++++++++++++++++++
void CabbagePluginAudioProcessorEditor::InsertGroupBox(CabbageGUIClass &cAttr)
{
        layoutComps.add(new CabbageGroupbox(cAttr.getStringProp("name"), 
                                                                                 cAttr.getStringProp("caption"), 
                                                                                 cAttr.getItems(0), 
                                                                                 cAttr.getColourProp("colour"),
                                                                                 cAttr.getColourProp("fontcolour"),
                                                                                 cAttr.getNumProp("line")));
        int idx = layoutComps.size()-1;

        float left = cAttr.getNumProp("left");
        float top = cAttr.getNumProp("top");
        float width = cAttr.getNumProp("width");
        float height = cAttr.getNumProp("height");

        //if(cAttr.getStringProp("plant").length()>1){
        //      width = width*cAttr.getNumProp("scaleX");
        //      height = height*cAttr.getNumProp("scaleY");
        //}

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

        if(cAttr.getNumProp("button")==0){
        componentPanel->addAndMakeVisible(layoutComps[idx]);
        }
        else{
                plantButton.add(new CabbageButton(cAttr.getStringProp("plant"), "", cAttr.getStringProp("plant"), CabbageUtils::getComponentSkin().toString(), ""));
                plantButton[plantButton.size()-1]->setBounds(cAttr.getNumProp("left"), cAttr.getNumProp("top"), 100, 30);
                plantButton[plantButton.size()-1]->button->addListener(this);
                componentPanel->addAndMakeVisible(plantButton[plantButton.size()-1]);
                plantButton[plantButton.size()-1]->button->getProperties().set(String("index"), plantButton.size()-1); 

                layoutComps[idx]->setLookAndFeel(lookAndFeel);
                subPatch.add(new CabbagePlantWindow(getFilter()->getGUILayoutCtrls(idx).getStringProp("plant"), Colours::black));
                subPatch[subPatch.size()-1]->setAlwaysOnTop(true);

                subPatch[subPatch.size()-1]->centreWithSize(layoutComps[idx]->getWidth(), layoutComps[idx]->getHeight()+18);
                subPatch[subPatch.size()-1]->setContentNonOwned(layoutComps[idx], true);
                subPatch[subPatch.size()-1]->setTitleBarHeight(18);
        }
        layoutComps[idx]->getProperties().set(String("plant"), var(cAttr.getStringProp("plant")));
        layoutComps[idx]->getProperties().set(String("groupLine"), cAttr.getNumProp("line"));
}

//+++++++++++++++++++++++++++++++++++++++++++
//                                      image
//+++++++++++++++++++++++++++++++++++++++++++
void CabbagePluginAudioProcessorEditor::InsertImage(CabbageGUIClass &cAttr)
{
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

        if(cAttr.getStringProp("file").length()<2)
                pic="";
        else
#ifdef LINUX
        pic.append(String("/")+String(cAttr.getStringProp("file")), 1024);
#else 
        pic.append(String("\\")+String(cAttr.getStringProp("file")), 1024);  
#endif

        layoutComps.add(new CabbageImage(cAttr.getStringProp("name"), pic, cAttr.getColourProp("outline"), cAttr.getColourProp("colour"), 
        cAttr.getStringProp("shape"), cAttr.getNumProp("line")));

        int idx = layoutComps.size()-1;

        int relY=0,relX=0;
        if(layoutComps.size()>0){
        for(int y=0;y<layoutComps.size();y++)
        if(cAttr.getStringProp("reltoplant").length()>0){
        String plant = layoutComps[y]->getProperties().getWithDefault(String("plant"), -99);
        String relTo = cAttr.getStringProp("reltoplant");

        if(layoutComps[y]->getProperties().getWithDefault(String("plant"), -99).toString().equalsIgnoreCase(cAttr.getStringProp("reltoplant")))
                {
                width = width*layoutComps[y]->getWidth();
                height = height*layoutComps[y]->getHeight();
                top = (top*layoutComps[y]->getHeight());
                left = (left*layoutComps[y]->getWidth());

                if(layoutComps[y]->getName().containsIgnoreCase("groupbox")||
                        layoutComps[y]->getName().containsIgnoreCase("image"))
                        {                       
                        layoutComps[idx]->setBounds(left, top, width, height);
                        layoutComps[y]->addAndMakeVisible(layoutComps[idx]);
                        }
                }
        }
                else{
            layoutComps[idx]->setBounds(left+relX, top+relY, width, height);
                componentPanel->addAndMakeVisible(layoutComps[idx]);            
                }
        }
        else{
            layoutComps[idx]->setBounds(left+relX, top+relY, width, height);
                componentPanel->addAndMakeVisible(layoutComps[idx]);            
        }

#ifdef LINUX
        pic.append(String("/")+String(cAttr.getStringProp("file")), 1024);
#else 
        pic.append(String("\\")+String(cAttr.getStringProp("file")), 1024);     
#endif

        layoutComps[idx]->getProperties().set(String("scaleY"), cAttr.getNumProp("scaleY"));
        layoutComps[idx]->getProperties().set(String("scaleX"), cAttr.getNumProp("scaleX"));
        layoutComps[idx]->getProperties().set(String("plant"), var(cAttr.getStringProp("plant")));
        layoutComps[idx]->toBack();
        //((CabbageImage*)layoutComps[idx])->setBounds (left+relX, top+relY, width, height);
        cAttr.setStringProp("type", "image");
}

//+++++++++++++++++++++++++++++++++++++++++++
//                                      line separator
//+++++++++++++++++++++++++++++++++++++++++++
void CabbagePluginAudioProcessorEditor::InsertLineSeparator(CabbageGUIClass &cAttr)
{
        layoutComps.add(new CabbageLine(true, cAttr.getColourProp("colour")));
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
				//if left is < 1 then the user is using the new system
				if(left>1){
                width = width*layoutComps[y]->getProperties().getWithDefault(String("scaleX"), 1).toString().getFloatValue();
                height = height*layoutComps[y]->getProperties().getWithDefault(String("scaleY"), 1).toString().getFloatValue();
                top = top*layoutComps[y]->getProperties().getWithDefault(String("scaleY"), 1).toString().getFloatValue();
                left = left*layoutComps[y]->getProperties().getWithDefault(String("scaleX"), 1).toString().getFloatValue();
				}
				else{    
					width = (width>1 ? .5 : width*layoutComps[y]->getWidth());
                    height = (height>1 ? .5 : height*layoutComps[y]->getHeight());
					top = (top*layoutComps[y]->getHeight());
					left = (left*layoutComps[y]->getWidth());
				}

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

        layoutComps[idx]->getProperties().set(String("plant"), var(cAttr.getStringProp("plant")));
}

//+++++++++++++++++++++++++++++++++++++++++++
//                                      label
//+++++++++++++++++++++++++++++++++++++++++++
void CabbagePluginAudioProcessorEditor::InsertLabel(CabbageGUIClass &cAttr)
{
        layoutComps.add(new CabbageLabel(cAttr.getStringProp("text"), cAttr.getColourProp("fontcolour")));      
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
				//if left is < 1 then the user is using the new system
				if(left>1){
                width = width*layoutComps[y]->getProperties().getWithDefault(String("scaleX"), 1).toString().getFloatValue();
                height = height*layoutComps[y]->getProperties().getWithDefault(String("scaleY"), 1).toString().getFloatValue();
                top = top*layoutComps[y]->getProperties().getWithDefault(String("scaleY"), 1).toString().getFloatValue();
                left = left*layoutComps[y]->getProperties().getWithDefault(String("scaleX"), 1).toString().getFloatValue();
				}
				else{    
					width = (width>1 ? .5 : width*layoutComps[y]->getWidth());
                    height = (height>1 ? .5 : height*layoutComps[y]->getHeight());
					top = (top*layoutComps[y]->getHeight());
					left = (left*layoutComps[y]->getWidth());
				}

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
}

//+++++++++++++++++++++++++++++++++++++++++++
//                                      window
//+++++++++++++++++++++++++++++++++++++++++++
void CabbagePluginAudioProcessorEditor::SetupWindow(CabbageGUIClass &cAttr)
{
        setName(cAttr.getStringProp("caption"));
        getFilter()->setPluginName(cAttr.getStringProp("caption"));
        int left = cAttr.getNumProp("left");
        int top = cAttr.getNumProp("top");
        int width = cAttr.getNumProp("width");
        int height = cAttr.getNumProp("height");
        setSize(width, height);
        componentPanel->setBounds(left, top, width, height);
        if(cAttr.getColourProp("colour").length()>2)
        formColour = Colour::fromString(cAttr.getColourProp("colour"));
        else
        formColour = CabbageUtils::getBackgroundSkin();

        if(cAttr.getColourProp("fontcolour").length()>2)
        fontColour = Colour::fromString(cAttr.getColourProp("fontcolour"));
        else
        fontColour = CabbageUtils::getComponentFontColour();

        authorText = cAttr.getStringProp("author");

#ifdef Cabbage_Build_Standalone
        formPic = getFilter()->getCsoundInputFile().getParentDirectory().getFullPathName();

#else
        File thisFile(File::getSpecialLocation(File::currentApplicationFile)); 
                formPic = thisFile.getParentDirectory().getFullPathName();
#endif

#ifdef LINUX
        formPic.append(String("/")+String(cAttr.getStringProp("file")), 1024);
#else 
        formPic.append(String("\\")+String(cAttr.getStringProp("file")), 1024); 
#endif

        if(cAttr.getStringProp("file").length()<2)
                formPic = "";

        this->resized();

        //add a dummy control to our layoutComps vector so that our filters layout vector 
        //is the same size as our editors one. 
        layoutComps.add(new Component());

}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//      Csound output widget. 
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CabbagePluginAudioProcessorEditor::InsertCsoundOutput(CabbageGUIClass &cAttr)
{

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

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//      Source button. 
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CabbagePluginAudioProcessorEditor::InsertSourceButton(CabbageGUIClass &cAttr)
{
        layoutComps.add(new CabbageButton(cAttr.getStringProp("name"),
                cAttr.getStringProp("caption"),
                cAttr.getItems(1-(int)cAttr.getNumProp("value")),
                cAttr.getColourProp("colour"),
                ""));   
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
				positionComponentWithinPlant("", idx, left, top, width, height, layoutComps[y], controls[idx]);
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

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//      Info button. 
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CabbagePluginAudioProcessorEditor::InsertInfoButton(CabbageGUIClass &cAttr)
{
        layoutComps.add(new CabbageButton("infoButton",
                cAttr.getStringProp("caption"),
                cAttr.getItems(1-(int)cAttr.getNumProp("value")),
                cAttr.getColourProp("colour"),
                cAttr.getColourProp("fontcolour")));    
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
				positionComponentWithinPlant("", idx, left, top, width, height, layoutComps[y], controls[idx]);
        }
        }
        else{
        ((CabbageButton*)layoutComps[idx])->setBounds(left+relX, top+relY, width, height);
        componentPanel->addAndMakeVisible(layoutComps[idx]);
        }
        }
        ((CabbageButton*)layoutComps[idx])->button->setName("infobutton");
        ((CabbageButton*)layoutComps[idx])->button->getProperties().set(String("filename"), cAttr.getStringProp("file"));
        layoutComps[idx]->getProperties().set(String("plant"), var(cAttr.getStringProp("plant")));
        ((CabbageButton*)layoutComps[idx])->button->addListener(this);
        ((CabbageButton*)layoutComps[idx])->button->setButtonText(cAttr.getItems(0));
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//      VU widget. 
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CabbagePluginAudioProcessorEditor::InsertVUMeter(CabbageGUIClass &cAttr)
{
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
				positionComponentWithinPlant("", idx, left, top, width, height, layoutComps[y], controls[idx]);
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

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//      Snapshot control for saving and recalling pre-sets
// 
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CabbagePluginAudioProcessorEditor::InsertSnapshot(CabbageGUIClass &cAttr)
{
        layoutComps.add(new CabbageSnapshot(cAttr.getStringProp("name"), cAttr.getColourProp("caption"), cAttr.getStringProp("preset"), cAttr.getNumProp("masterSnap")));       
        int idx = layoutComps.size()-1;

        String snap= getFilter()->getCsoundInputFile().withFileExtension(".snaps").getFullPathName();
        SnapShotFile = File(snap);

        float left = cAttr.getNumProp("left");
        float top = cAttr.getNumProp("top");
        float width = cAttr.getNumProp("width");
        float height = cAttr.getNumProp("height");
        int relY=0,relX=0;
        for(int y=0;y<layoutComps.size();y++){
        if(cAttr.getStringProp("reltoplant").length()>0){
        if(layoutComps[y]->getProperties().getWithDefault(String("plant"), -99).toString().equalsIgnoreCase(cAttr.getStringProp("reltoplant")))
        {
				positionComponentWithinPlant("", idx, left, top, width, height, layoutComps[y], controls[idx]);
        }
        }
        else{
        layoutComps[idx]->setBounds(left+relX, top+relY, width, height);
        componentPanel->addAndMakeVisible(layoutComps[idx]);
        }
        }

        ((CabbageSnapshot*)layoutComps[idx])->addActionListener(this);
        ((CabbageSnapshot*)layoutComps[idx])->combobox->addItem("Select preset", -1);
    for(int i=1;i<(int)cAttr.getItemsSize()+1;i++){
                String test  = cAttr.getItems(i-1);
                //showMessage(test);
                ((CabbageSnapshot*)layoutComps[idx])->combobox->addItem(cAttr.getItems(i-1), i);
                cAttr.setNumProp("maxItems", i);
        }

        ((CabbageSnapshot*)layoutComps[idx])->combobox->setSelectedItemIndex(cAttr.getNumProp("value")-1);
        
        //Load any snapshot files that already exist for this instrument
        String snapshotFile = getFilter()->getCsoundInputFile().withFileExtension(".snaps").getFullPathName();
        StringArray data;
        String presetText="";
        cAttr.clearPresets();
        String openBlock, endBlock;
        //showMessage(cAttr.getStringProp("preset"));

        //I should be using the preset name here and not the instrument name as it gets
        //      changed whenever the order of the code changes. The presets will always be the same

        openBlock << "------------------------ Instrument ID: " << cAttr.getStringProp("preset");
        endBlock << "------------------------ End of Instrument ID: " << cAttr.getStringProp("preset");
        File dataFile(snapshotFile);
        if(dataFile.exists()){
                data.addLines(dataFile.loadFileAsString());
                for(int i=0;i<data.size();i++)
                        //if we find the snapshot name load the data
                        if(data[i].contains(openBlock)){
                                //read all lines of preset data until end of instrument ID
                                for(int y=1;y<data.size()-i;y++){
                                        if(data[y+i].contains(endBlock)){
                                                y=data.size();
                                                i=y;                                            
                                                break;
                                        }

                                        presetText = presetText + data[y+i];
                                        presetText.append("\n", 10);
                                        if(data[y+i].contains("-------- End of Preset:")){
                                                cAttr.setStringProp("snapshotData", y-1, presetText);
                                                //showMessage(presetText);
                                                presetText = "";
                                                }
                                }

                        }
                }
        //showMessage(cAttr.getStringProp("snapshotData"));
        //cAttr.setStringProp("snapshotData", presetText);
        cAttr.setStringProp("type", "snapshot");
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//      MIDI keyboard, I've this listed as non-interactive
// as it only sends MIDI, it doesn't communicate over the software bus
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CabbagePluginAudioProcessorEditor::InsertMIDIKeyboard(CabbageGUIClass &cAttr)
{
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
				positionComponentWithinPlant("", idx, left, top, width, height, layoutComps[y], controls[idx]);
        }
        }
        else{
        ((MidiKeyboardComponent*)layoutComps[idx])->setBounds(left+relX, top+relY, width, height);
        componentPanel->addAndMakeVisible(layoutComps[idx]);
        }
        }

        ((MidiKeyboardComponent*)layoutComps[idx])->setLowestVisibleKey(cAttr.getNumProp("value"));

        layoutComps[idx]->getProperties().set(String("plant"), var(cAttr.getStringProp("plant")));
#ifdef Cabbage_Build_Standalone
        layoutComps[idx]->setWantsKeyboardFocus(true);
        layoutComps[idx]->setAlwaysOnTop(true);
#endif
        layoutComps[idx]->addMouseListener(this, false);
        layoutComps[idx]->setName("midiKeyboard");
}

//=======================================================================================
//      interactive components - 'insert' methods followed by event methods
//=======================================================================================
//+++++++++++++++++++++++++++++++++++++++++++
//                                      slider
//+++++++++++++++++++++++++++++++++++++++++++
void CabbagePluginAudioProcessorEditor::InsertSlider(CabbageGUIClass &cAttr)
{
        float left = cAttr.getNumProp("left");
        float top = cAttr.getNumProp("top");
        float width = cAttr.getNumProp("width");
        float height = cAttr.getNumProp("height");     
        controls.add(new CabbageSlider(cAttr.getStringProp("name"),
                                            cAttr.getStringProp("text"),
                                            cAttr.getStringProp("caption"),
                                            cAttr.getStringProp("kind"),
                                            cAttr.getColourProp("colour"),
                                            cAttr.getColourProp("fontcolour"),
                                            cAttr.getNumProp("textbox"),
                                            cAttr.getColourProp("tracker")
                                            ));   
        int idx = controls.size()-1;
 
 
        int relY=0,relX=0;
        if(layoutComps.size()>0){
        for(int y=0;y<layoutComps.size();y++){
        if(cAttr.getStringProp("reltoplant").length()>0){
        if(layoutComps[y]->getProperties().getWithDefault(String("plant"), -99).toString().equalsIgnoreCase(cAttr.getStringProp("reltoplant")))
                {
					positionComponentWithinPlant(cAttr.getStringProp("type"), idx, left, top, width, height, layoutComps[y], controls[idx]);
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
 
        //if(cAttr.getStringProp("kind").equalsIgnoreCase("vertical"))
        //((CabbageSlider*)controls[idx])->setBounds(left+relX, top+relY, width, height);
        //else if(cAttr.getStringProp("kind").equalsIgnoreCase("horizontal"))
        //((CabbageSlider*)controls[idx])->setBounds(left+relX, top+relY, width, height);
		
		//((CabbageSlider*)controls[idx])->slider->setLookAndFeel(oldSchoolLook);
		//Logger::writeToLog("Skew:"+String(cAttr.getNumProp("sliderSkew")));
        ((CabbageSlider*)controls[idx])->slider->setSkewFactor(cAttr.getNumProp("sliderSkew"));
        ((CabbageSlider*)controls[idx])->slider->setRange(cAttr.getNumProp("min"), cAttr.getNumProp("max"), (double)cAttr.getNumProp("sliderIncr"));
        ((CabbageSlider*)controls[idx])->slider->setValue(cAttr.getNumProp("value"));
        ((CabbageSlider*)controls[idx])->slider->addListener(this);

        controls[idx]->getProperties().set(String("midiChan"), cAttr.getNumProp("midiChan"));
        controls[idx]->getProperties().set(String("midiCtrl"), cAttr.getNumProp("midiCtrl")); 
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
                                                getFilter()->getGUICtrls(i).setNumProp("value", (float)sliderThatWasMoved->getValue());
                                                getFilter()->beginParameterChangeGesture(i);
                                                getFilter()->setParameterNotifyingHost(i, (float)getFilter()->getGUICtrls(i).getNumProp("value"));
                                                getFilter()->endParameterChangeGesture(i);
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
//                                      button
//+++++++++++++++++++++++++++++++++++++++++++
void CabbagePluginAudioProcessorEditor::InsertButton(CabbageGUIClass &cAttr)
{
        controls.add(new CabbageButton(cAttr.getStringProp("name"),
                cAttr.getStringProp("caption"),
                cAttr.getItems(1-(int)cAttr.getNumProp("value")),
                cAttr.getColourProp("colour"),
                cAttr.getColourProp("fontcolour")));    
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
				positionComponentWithinPlant("", idx, left, top, width, height, layoutComps[y], controls[idx]);
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
        //((CabbageButton*)controls[idx])->button->setName("button");
        if(cAttr.getItemsSize()>0)
        ((CabbageButton*)controls[idx])->button->setButtonText(cAttr.getItems(cAttr.getNumProp("value")));
#ifdef Cabbage_Build_Standalone
        ((CabbageButton*)controls[idx])->button->setWantsKeyboardFocus(true);
#endif

		//showMessage(controls[idx]->getParentComponent()->getName());
}

//+++++++++++++++++++++++++++++++++++++++++++
//                                      checkbox
//+++++++++++++++++++++++++++++++++++++++++++
void CabbagePluginAudioProcessorEditor::InsertCheckBox(CabbageGUIClass &cAttr)
{
        bool RECT = cAttr.getStringProp("shape").equalsIgnoreCase("square");
                
        controls.add(new CabbageCheckbox(cAttr.getStringProp("name"),
                cAttr.getStringProp("caption"),
                cAttr.getItems(0),
                cAttr.getColourProp("colour"),
                cAttr.getColourProp("fontcolour"),
                RECT)); 
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
				positionComponentWithinPlant("", idx, left, top, width, height, layoutComps[y], controls[idx]);
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

        //set user-defined colour if given
        if(cAttr.getColourProp("colour")!=Colours::lime.toString())
                ((CabbageCheckbox*)controls[idx])->button->getProperties().set("colour", cAttr.getColourProp("colour"));

        //set initial value if given
        if(cAttr.getNumProp("value")==1)
                ((CabbageCheckbox*)controls[idx])->button->setToggleState(true, true);
        else
                ((CabbageCheckbox*)controls[idx])->button->setToggleState(false, true);

#ifdef Cabbage_Build_Standalone
        ((CabbageCheckbox*)controls[idx])->button->setWantsKeyboardFocus(true);
#endif
        

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
                //deal with non-interactive buttons first..
                        if(button->getName()=="source"){
//                              getFilter()->createAndShowSourceEditor(lookAndFeel);
                        }
                        else if(button->getName()=="infobutton"){
                                if(!infoWindow){
                                String file = getFilter()->getCsoundInputFile().getParentDirectory().getFullPathName();
                                file.append("\\", 5);
                                file.append(button->getProperties().getWithDefault("filename", ""), 1024);
                                showMessage(file);
                                infoWindow = new InfoWindow(lookAndFeel, file);
                                infoWindow->centreWithSize(600, 400);
                                infoWindow->toFront(true);
                                infoWindow->setVisible(true);
                                }
                                else
                                        infoWindow->setVisible(true);
                        }


                for(int i=0;i<(int)getFilter()->getGUICtrlsSize();i++){//find correct control from vector
                        
                        if(getFilter()->getGUICtrls(i).getStringProp("type")==String("button"))
                        {                               
                        //+++++++++++++button+++++++++++++
                                //Logger::writeToLog(getFilter()->getGUICtrls(i).getStringProp("name"));
                                //Logger::writeToLog(button->getName());
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
                                        button->setButtonText(getFilter()->getGUICtrls(i).getItems(0));
                                else if(getFilter()->getGUICtrls(i).getItems(0)==button->getButtonText())
                                        button->setButtonText(getFilter()->getGUICtrls(i).getItems(1));

                        }
                        }
                        //show plants as popup window
                        else{
                                for(int p=0;p<getFilter()->getGUILayoutCtrlsSize();p++){
                                if(getFilter()->getGUILayoutCtrls(p).getStringProp("plant") ==button->getName()){
                                int index = button->getProperties().getWithDefault(String("index"), 0);
                                subPatch[index]->setVisible(true);
                                i=getFilter()->getGUICtrlsSize();
                                break;
                                }

                        }
                        }
                }
        }
        
        else if(dynamic_cast<ToggleButton*>(button)){
        for(int i=0;i<(int)getFilter()->getGUICtrlsSize();i++)//find correct control from vector
                        if(getFilter()->getGUICtrls(i).getStringProp("name")==button->getName()){
                        //      Logger::writeToLog(String(button->getToggleStateValue().getValue()));

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
//                                      combobox
//+++++++++++++++++++++++++++++++++++++++++++
void CabbagePluginAudioProcessorEditor::InsertComboBox(CabbageGUIClass &cAttr)
{
        controls.add(new CabbageComboBox(cAttr.getStringProp("name"),
                cAttr.getStringProp("caption"),
                cAttr.getItems(0),
                cAttr.getColourProp("colour"),
                cAttr.getColourProp("fontcolour")));

        int idx = controls.size()-1;
        
        float left = cAttr.getNumProp("left");
        float top = cAttr.getNumProp("top");
        float width = cAttr.getNumProp("width");
        float height = cAttr.getNumProp("height");

        int relY=0,relX=0;
        if(layoutComps.size()>0){
        for(int y=0;y<layoutComps.size();y++)
        if(cAttr.getStringProp("reltoplant").length()>0){
        if(layoutComps[y]->getProperties().getWithDefault(String("plant"), -99).toString().equalsIgnoreCase(cAttr.getStringProp("reltoplant")))
                {
				positionComponentWithinPlant("", idx, left, top, width, height, layoutComps[y], controls[idx]);
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

//this needs some attention. 
//At present comboxbox colours can't be changed...
        int items;
		for(int i=0;i<(int)cAttr.getItemsSize();i++){
                String test  = cAttr.getItems(i);
                ((CabbageComboBox*)controls[idx])->combo->addItem(cAttr.getItems(i), i+1);
                cAttr.setNumProp("maxItems", i);
                items=i;
        }

        cAttr.setNumProp("sliderRange", cAttr.getItemsSize());
        lookAndFeel->setColour(ComboBox::textColourId, Colour::fromString(cAttr.getColourProp("fontcolour")));
        ((CabbageComboBox*)controls[idx])->combo->setSelectedId(cAttr.getNumProp("value"));
        ((CabbageComboBox*)controls[idx])->setName(cAttr.getStringProp("name"));
        ((CabbageComboBox*)controls[idx])->combo->addListener(this);

		
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
                                                
#ifndef Cabbage_Build_Standalone
                                                getFilter()->getGUICtrls(i).setNumProp("value", (int)combo->getSelectedItemIndex());
                                                getFilter()->getCsound()->SetChannel(getFilter()->getGUICtrls(i).getStringProp("channel").toUTF8(), combo->getSelectedItemIndex());
                                                //Logger::writeToLog(String("comboEvent():")+String(getFilter()->getGUICtrls(i).getNumProp("sliderRange")));
                                                getFilter()->setParameterNotifyingHost(i, (float)(combo->getSelectedItemIndex())/(getFilter()->getGUICtrls(i).getNumProp("sliderRange")));
#else
                                                getFilter()->getGUICtrls(i).setNumProp("value", (int)combo->getSelectedItemIndex()+1);
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
//                                      xypad
//+++++++++++++++++++++++++++++++++++++++++++
void CabbagePluginAudioProcessorEditor::InsertXYPad(CabbageGUIClass &cAttr)
{
/*
Our filters control vector contains two xypads, one for the X channel and one for the Y
channel. Our editor only needs to display one so the xypad with 'dummy' appended to the name
will be created but not shown. 

We also need to check to se whether the processor editor has been 're-opened'. if so we 
don't need to recreate the automation
*/
int idx;
if(getFilter()->haveXYAutosBeenCreated()){
		controls.add(new CabbageXYController(getFilter()->getXYAutomater(xyPadIndex), 
				cAttr.getStringProp("name"),
                cAttr.getStringProp("text"),
				"",
                cAttr.getNumProp("minX"),
                cAttr.getNumProp("maxX"),
                cAttr.getNumProp("minY"),
                cAttr.getNumProp("maxY"),
				xyPadIndex,
				cAttr.getNumProp("decimalPlaces"),
				cAttr.getColourProp("colour"),
				cAttr.getColourProp("fontcolour"))); 
				xyPadIndex++;  
	idx = controls.size()-1;
}
else{
	getFilter()->addXYAutomater(new XYPadAutomation());
	getFilter()->getXYAutomater(getFilter()->getXYAutomaterSize()-1)->addChangeListener(getFilter());
	getFilter()->getXYAutomater(getFilter()->getXYAutomaterSize()-1)->xChannel = cAttr.getStringProp("xChannel");
	getFilter()->getXYAutomater(getFilter()->getXYAutomaterSize()-1)->yChannel = cAttr.getStringProp("yChannel");
	cAttr.setNumProp("xyAutoIndex", getFilter()->getXYAutomaterSize()-1);

	controls.add(new CabbageXYController(getFilter()->getXYAutomater(getFilter()->getXYAutomaterSize()-1), 
				cAttr.getStringProp("name"),
                cAttr.getStringProp("text"),
				"",
                cAttr.getNumProp("minX"),
                cAttr.getNumProp("maxX"),
                cAttr.getNumProp("minY"),
                cAttr.getNumProp("maxY"),
				getFilter()->getXYAutomaterSize()-1,
				cAttr.getNumProp("decimalPlaces"),
				cAttr.getColourProp("colour"),
				cAttr.getColourProp("fontcolour")));   
	idx = controls.size()-1;
	getFilter()->getXYAutomater(getFilter()->getXYAutomaterSize()-1)->paramIndex = idx;
}

        float left = cAttr.getNumProp("left");    
        float top = cAttr.getNumProp("top");
        float width = cAttr.getNumProp("width");
        float height = cAttr.getNumProp("height");



        //check to see if widgets is anchored
        //if it is offset it's position accordingly. 
        int relY=0,relX=0;
        if(layoutComps.size()>0){
			if(controls[idx])
        for(int y=0;y<layoutComps.size();y++)
        if(cAttr.getStringProp("reltoplant").length()>0){
        if(layoutComps[y]->getProperties().getWithDefault(String("plant"), -99).toString().equalsIgnoreCase(cAttr.getStringProp("reltoplant")))
                {
				positionComponentWithinPlant("", idx, left, top, width, height, layoutComps[y], controls[idx]);
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
        //Logger::writeToLog(String("X:")+String(valueX));
        max = cAttr.getNumProp("maxY");
        min = cAttr.getNumProp("minY");
        float valueY = cabbageABS(min-cAttr.getNumProp("valueY"))/cabbageABS(min-max);
        //Logger::writeToLog(String("Y:")+String(valueY));
        ((CabbageXYController*)controls[idx])->xypad->setXYValues(valueX, valueY);
		getFilter()->setParameter(idx, valueX);
		getFilter()->setParameter(idx+1, valueY);
#ifdef Cabbage_Build_Standalone 
        controls[idx]->setWantsKeyboardFocus(false);
#endif
//        ((CabbageXYController*)controls[idx])->xypad->addActionListener(this);
        if(!cAttr.getStringProp("name").containsIgnoreCase("dummy"))
        actionListenerCallback(cAttr.getStringProp("name"));

		//cAttr.setStringProp("type", "xycontroller");
}

//+++++++++++++++++++++++++++++++++++++++++++
//                                      pvs viewer
//+++++++++++++++++++++++++++++++++++++++++++
void CabbagePluginAudioProcessorEditor::InsertPVSViewer(CabbageGUIClass &cAttr)
{
#ifndef Cabbage_No_Csound
        //set up PVS struct
        getFilter()->getPVSDataOut()->N = cAttr.getNumProp("fftSize");
        getFilter()->getPVSDataOut()->format = 0;
        getFilter()->getPVSDataOut()->winsize = cAttr.getNumProp("fftSize");
        getFilter()->getPVSDataOut()->overlap = cAttr.getNumProp("fftSize")/4;
        getFilter()->getPVSDataOut()->frame = (float *) calloc(sizeof(float),(getFilter()->getPVSDataOut()->N+2));

        layoutComps.add(new CabbagePVSView(cAttr.getStringProp("name"),
                cAttr.getStringProp("caption"),
                cAttr.getStringProp("text"),
                cAttr.getNumProp("winSize"),
                cAttr.getNumProp("fftSize"),
                cAttr.getNumProp("overlapSize"),
                getFilter()->getPVSDataOut())); 


        int idx = layoutComps.size()-1;
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
				positionComponentWithinPlant("", idx, left, top, width, height, layoutComps[y], controls[idx]);
                }
        }
                else{
            layoutComps[idx]->setBounds(left+relX, top+relY, width, height);
                componentPanel->addAndMakeVisible(layoutComps[idx]);            
                }
        }
        }
        else{
            layoutComps[idx]->setBounds(left+relX, top+relY, width, height);
                componentPanel->addAndMakeVisible(layoutComps[idx]);            
        }

#endif
        
}

//+++++++++++++++++++++++++++++++++++++++++++
//                                      table
//+++++++++++++++++++++++++++++++++++++++++++
void CabbagePluginAudioProcessorEditor::InsertTable(CabbageGUIClass &cAttr)
{
int tableSize=0;
Array <float> tableValues;
        //fill array with points from table, is table is valid
        if(getFilter()->getCompileStatus()==0 &&
		   getFilter()->getCsound()){
#ifndef Cabbage_No_Csound
        tableSize = getFilter()->getCsound()->TableLength(cAttr.getNumProp("tableNum"));
        tableValues = getFilter()->getTable(cAttr.getNumProp("tableNum"), tableSize);
#endif
        }
        else{
                //if table is not valid fill our array with at least one dummy point
                tableValues.add(0); 
        }
        layoutComps.add(new CabbageTable(cAttr.getStringProp("name"),
                cAttr.getStringProp("caption"),
                cAttr.getItems(0),
                tableSize));    
        int idx = layoutComps.size()-1;
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
				positionComponentWithinPlant("", idx, left, top, width, height, layoutComps[y], controls[idx]);
                }
        }
                else{
            layoutComps[idx]->setBounds(left+relX, top+relY, width, height);
                componentPanel->addAndMakeVisible(layoutComps[idx]);            
                }
        }
        }
        else{
            layoutComps[idx]->setBounds(left+relX, top+relY, width, height);
                componentPanel->addAndMakeVisible(layoutComps[idx]);            
        }
        
        ((CabbageTable*)layoutComps[idx])->fillTable(0, tableValues);
//      ((CabbageTable*)controls[idx])->table->addActionListener(this);

        //Logger::writeToLog(cAttr.getPropsString());

}

                                        /*********************************************************/
                                        /*     actionlistener method (xypad/table/snapshot)      */
                                        /*********************************************************/
void CabbagePluginAudioProcessorEditor::actionListenerCallback (const String& message){
//the first part of this method receives messages from the GUI editor layout/Main panel and updates the 
//source code accordingly. The second half if use for messages being sent from GUI widgets
if(message.contains("Message sent from CabbageMainPanel")){

	#ifdef Cabbage_GUI_Editor//if GUI editor has been enabled
	StringArray csdArray;
	csdArray.clear();
	String temp;

	//break up lines in csd file into a string array
	csdArray.addLines(getFilter()->getCsoundInputFileText());

		//this removes the bounds data from the string... 
		if(componentPanel->getMouseState().equalsIgnoreCase("down")){
				for(int i=0; i<csdArray.size(); i++){
				CabbageGUIClass CAttr(csdArray[i], -99);
				if(csdArray[i].contains("</Cabbage>"))
					i=csdArray.size();
				//Logger::writeToLog(csdArray[i]);
				//Logger::writeToLog("Cattr: "+getBoundsString(CAttr.getComponentBounds()));
				//Logger::writeToLog("Current: "+getBoundsString(componentPanel->currentBounds));
				if(CAttr.getComponentBounds()==componentPanel->currentBounds){
				lineNumber = i;
				//empty bounds indentifier text..
				csdArray.set(lineNumber, replaceIdentifier(csdArray[lineNumber], "bounds", "bounds()"));
				}
				}
		}//END OF MOUSE DOWN MESSAGE EVENT

		else if(componentPanel->getMouseState().equalsIgnoreCase("up")){
		//ONLY SEND UPDATED INFO ON A MOUSE UP

				//replace the bounds() indentifier with the updated one
				csdArray.set(lineNumber, replaceIdentifier(csdArray[lineNumber], "bounds", getBoundsString(componentPanel->currentBounds)));

				if(csdArray[lineNumber].contains("plant(\"")){
					for(int y=1, off=0;y<componentPanel->childBounds.size()+1;y++){		
					//stops things from getting messed up if there are line 
					if((csdArray[lineNumber+y+off].length()<2) || csdArray[lineNumber+y+off].indexOf(";")==0){
						off++;
						y--;
					}
					temp =  replaceIdentifier(csdArray[lineNumber+y+off], "bounds", getBoundsString(componentPanel->childBounds[y-1]));
					csdArray.set(lineNumber+y+off, temp);
					 //	csdArray.set(lineNumber+y, tempArray[y-1]);//.replace("bounds()", componentPanel->getCurrentChildBounds(y-1), true));
					}
				}
				getFilter()->updateCsoundFile(csdArray.joinIntoString("\n"));
				getFilter()->setCurrentLineText(csdArray[lineNumber]);
				getFilter()->setGuiEnabled(true);
				getFilter()->sendActionMessage("GUI Update");

		}//END OF MOUSE UP MESSAGE EVENT

		//doing this here because compoentLayoutManager doesn't know the csd file text...
		if(message.length()>String("Message sent from CabbageMainPanel").length()){ //ADD TO REPOSITORY
			String repoEntryName = message.substring(String("Message sent from CabbageMainPanel").length());
			String repoEntry = csdArray[lineNumber];
			int cnt = 0;
			//CabbageUtils::showMessage(repoEntryName);
			if(csdArray[lineNumber].contains("plant(\"")){
				repoEntry = "";
				while(!csdArray[lineNumber+cnt].contains("}")){
					repoEntry = repoEntry+csdArray[lineNumber+cnt]+"\n";
					cnt++;
				}
				repoEntry = repoEntry+"}";
			
			}
			//make sure host doesn't fail if there are no Plant entries
			//ScopedPointer<XmlElement> xml;
			//xml = new XmlElement("PLANTS");
//			PropertySet pSet;
			//pSet.setValue("PlantRepository", xml);
			//appProperties->getUserSettings()->setFallbackPropertySet(&pSet);	
			//xml = appProperties->getUserSettings()->getXmlValue("PlantRepository");
			//if(xml)
			//xml->setAttribute(repoEntryName, repoEntry);
			//showMessage(repoEntryName);
			//showMessage(repoEntry);
			//appProperties->getUserSettings()->setValue("PlantRepository", xml);
			String plantDir = appProperties->getUserSettings()->getValue("PlantFileDir", "");	
			String plantFile = plantDir + "/" + repoEntryName.trim() + String(".plant");
			File plant(plantFile);
			plant.replaceWithText(repoEntry);
			
			/*
						String repoEntryName = message.substring(String("Message sent from CabbageMainPanel").length());
			String repoEntry = temp;
			int cnt = 0;
			//CabbageUtils::showMessage(repoEntryName);
			if(temp.contains("plant(\"")){
				repoEntry = "";
				while(!csdArray[lineNumber+cnt].contains("}")){
					repoEntry = repoEntry+csdArray[lineNumber+cnt]+"\n";
					cnt++;
				}
				repoEntry = repoEntry+"}";
			
			}
			//make sure host doesn't fail if there are no Plant entries
			ScopedPointer<XmlElement> xml;
			xml = new XmlElement("PLANTS");
//			PropertySet pSet;
			//pSet.setValue("PlantRepository", xml);
			//appProperties->getUserSettings()->setFallbackPropertySet(&pSet);	
			xml = appProperties->getUserSettings()->getXmlValue("PlantRepository");
			if(xml)
			xml->setAttribute(repoEntryName, repoEntry);
			appProperties->getUserSettings()->setValue("PlantRepository", xml);*/
	}
	#endif


}//END OF TEST FOR MESSAGE SENT FROM CABBAGE MAIN PANEL

else{
//this event recieves action messages from custom components. 
String name = message.substring(0, message.indexOf(String("|"))); 
String type = message.substring(message.indexOf(String("|"))+1, message.indexOf(String(":")));
String action = message.substring(message.indexOf(String(":"))+1, message.indexOf(String(";")));
String preset = message.substring(message.indexOf(String(";"))+1, message.indexOf(String("?"))); 
int masterSnap = message.substring(message.indexOf(String("?"))+1, 100).getIntValue(); 

for(int i=0;i<(int)getFilter()->getGUICtrlsSize();i++)//find correct control from vector
        //if message has come from the snapshot control 
        //============================================================================================
		if(type.equalsIgnoreCase(String("snapshot"))){
                String str, presetData = "";
                //save presets to .snaps file
                //showMessage(String("preset name:")+name);
                if(action=="save"){
                        //when we save we need to save all preset to disk, not just the selected ones.
                for(int i=0;i<getFilter()->getGUILayoutCtrlsSize();i++)
                        if(getFilter()->getGUILayoutCtrls(i).getStringProp("preset").equalsIgnoreCase(name))
                        {
                                //showMessage(getFilter()->getGUILayoutCtrls(i).getStringProp("name"));
                                //showMessage(getFilter()->getGUILayoutCtrls(i).getStringProp("snapshotData"));
                                //showMessage(getFilter()->getGUILayoutCtrls(i).getNumPresets());
                                for(int x=0;x<getFilter()->getGUILayoutCtrls(i).getItemsSize();x++){
                                                //showMessage(String(x));

                                        if(getFilter()->getGUILayoutCtrls(i).getItems(x).trim().equalsIgnoreCase(preset.trim()))
                                        {
                                                //showMessage(String(x));
                                                str << "-------- Start of Preset: " << preset.trim() << "\n";
                                                for(int n=0;n<getFilter()->getGUICtrlsSize();n++){
                                                        String comp1 = getFilter()->getGUICtrls(n).getStringProp("preset");
                                                        //checks if all widgets are being controlled by this preset, or just some
                                                        if(masterSnap)
                                                                str = str << getFilter()->getGUICtrls(n).getStringProp("channel") << ":\t\t\t" << getFilter()->getGUICtrls(n).getNumProp("value") << "\n";      

                                                        else if(comp1.trim().equalsIgnoreCase(name.trim()))
                                                        {
                                                                //showMessage(getFilter()->getGUILayoutCtrls(i).getStringProp("preset"));
                                                                
                                                                str = str << getFilter()->getGUICtrls(n).getStringProp("channel") << ":\t\t\t" << getFilter()->getGUICtrls(n).getNumProp("value") << "\n";      

                                                        }
                                                }
                                                
                                                str = str << "-------- End of Preset: " << preset.trim() << "\n";
                                                //showMessage(str);
                                                //showMessage(str);
                                                getFilter()->getGUILayoutCtrls(i).setStringProp("snapshotData", x, str);
                                                str = "";
                                        }
                                //showMessage(getFilter()->getGUILayoutCtrls(i).getStringProp("snapshotData")); 
                                }
                                
                                for(int u=0;u<getFilter()->getGUILayoutCtrlsSize();u++)
                                        if(getFilter()->getGUILayoutCtrls(u).getNumPresets()>0)
                                                presetData = presetData + getFilter()->getGUILayoutCtrls(u).getStringProp("snapshotData");

                                presetFileText = presetData;
                                //showMessage(presetData);
                                presetData = "";
                        }
                
                }
                //load presets from .snaps file
                else if(action=="load"){
                //str = SnapShotFile.loadFileAsString();
                int presetIndex = 0; 
                StringArray values;
                String snapshotData, channel, start="";
                
                //if we find our preset instrument ID
                for(int i=0;i<getFilter()->getGUILayoutCtrlsSize();i++)
                        if(getFilter()->getGUILayoutCtrls(i).getStringProp("preset").equalsIgnoreCase(name))
                                for(int x=0;x<getFilter()->getGUILayoutCtrls(i).getItemsSize();x++){
                                        //showMessage(preset);
                                        snapshotData = getFilter()->getGUILayoutCtrls(i).getStringProp("snapshotData", x);
                                        //showMessage(snapshotData);
                                        start << "-------- Start of Preset: " << preset.trim() << "\n";
                                        //showMessage(start);
                                        if(snapshotData.contains(start)){
                                                //showMessage(getFilter()->getGUILayoutCtrls(i).getStringProp("snapshotData", x));
                                                values.addLines(getFilter()->getGUILayoutCtrls(i).getStringProp("snapshotData", x));
                                                for(int z=1;z<values.size()-1;z++){
                                                        channel = values[z].substring(0, values[z].indexOf(":"));
                                                        for(int u=0;u<getFilter()->getGUICtrlsSize();u++)
                                                                if(getFilter()->getGUICtrls(u).getStringProp("channel").equalsIgnoreCase(channel)){
                                                                        double val = values[z].substring(values[z].indexOf(":")+1, 100).getDoubleValue();
                                                                        //showMessage(String(val));
                                                                        getFilter()->getGUICtrls(u).setNumProp("value", val);
                                                                        if(getFilter()->getGUICtrls(i).getStringProp("type")==String("hslider")||
                                                                        getFilter()->getGUICtrls(u).getStringProp("type")==String("rslider")||
                                                                        getFilter()->getGUICtrls(u).getStringProp("type")==String("vslider")){
                                                                        if(controls[u])
                                                                        ((CabbageSlider*)controls[u])->slider->setValue(val, dontSendNotification);
                                                                        }
                                                                        else if(getFilter()->getGUICtrls(u).getStringProp("type")==String("checkbox")){
                                                                        if(controls[u])
                                                                        ((CabbageCheckbox*)controls[u])->button->setToggleState((bool)val, true);
                                                                        }
                                                                        else if(getFilter()->getGUICtrls(u).getStringProp("type")==String("combobox")){
                                                                        //if(controls[u])
                                                                        //((CabbageComboBox*)controls[u])->combo->setSelectedItemIndex(val);
                                                                        }
                                                                        //update host when preset ares recalled
                                                                        getFilter()->setParameterNotifyingHost(u, val);
                                                                }
                                                }
                                                start = "";
                                        }
                                        else
                                                start = "";
                                        //showMessage(values.joinIntoString(","));
                                }
                                values.clear();
                }
        }       
        //============================================================================================

        //if message comes from a table
/*
        else if(type.equalsIgnoreCase(String("table"))){
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
}

//+++++++++++++++++++++++++++++++++++++++++++
//                                      pattern matrix
//+++++++++++++++++++++++++++++++++++++++++++
void CabbagePluginAudioProcessorEditor::InsertPatternMatrix(CabbageGUIClass &cAttr)
{
int tableSize=0;
//getFilter()->patStepMatrix.clear();
//getFilter()->patPfieldMatrix.clear();
        controls.add(new CabbagePatternMatrix(getFilter(), cAttr.getStringProp("name"),
                cAttr.getNumProp("width"),
                cAttr.getNumProp("height"),
                cAttr.getStringProp("caption"),
                cAttr.getItemArray(),
                cAttr.getNumProp("noSteps"),
                cAttr.getNumProp("rCtrls")));   
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
                //width = width*layoutComps[y]->getProperties().getWithDefault(String("scaleX"), 1).toString().getFloatValue();
                //height = height*layoutComps[y]->getProperties().getWithDefault(String("scaleY"), 1).toString().getFloatValue();
                //top = top*layoutComps[y]->getProperties().getWithDefault(String("scaleY"), 1).toString().getFloatValue();
                //left = left*layoutComps[y]->getProperties().getWithDefault(String("scaleX"), 1).toString().getFloatValue();

				//if left is < 1 then the user is using the new system
				if(left>1){
                width = width*layoutComps[y]->getProperties().getWithDefault(String("scaleX"), 1).toString().getFloatValue();
                height = height*layoutComps[y]->getProperties().getWithDefault(String("scaleY"), 1).toString().getFloatValue();
                top = top*layoutComps[y]->getProperties().getWithDefault(String("scaleY"), 1).toString().getFloatValue();
                left = left*layoutComps[y]->getProperties().getWithDefault(String("scaleX"), 1).toString().getFloatValue();
				}
				else{    
					width = (width>1 ? .5 : width*layoutComps[y]->getWidth());
                    height = (height>1 ? .5 : height*layoutComps[y]->getHeight());
					top = (top*layoutComps[y]->getHeight());
					left = (left*layoutComps[y]->getWidth());
				}

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

        //Logger::writeToLog(cAttr.getPropsString());

}

//=============================================================================
bool CabbagePluginAudioProcessorEditor::keyPressed(const juce::KeyPress &key ,Component *)
{
if(key.getTextDescription()=="ctrl + B")
	getFilter()->sendActionMessage("MENU COMMAND: manual update instrument");

if(key.getTextDescription()=="ctrl + O")
	getFilter()->sendActionMessage("MENU COMMAND: open instrument");

if(key.getTextDescription()=="ctrl + U")
	getFilter()->sendActionMessage("MENU COMMAND: manual update GUI");

if(key.getTextDescription()=="ctrl + M")
	getFilter()->sendActionMessage("MENU COMMAND: suspend audio");

#ifndef Cabbage_No_Csound
if(!getFilter()->isGuiEnabled()){
getFilter()->getCsound()->KeyPressed(key.getTextCharacter());
//search through controls to see which is attached to the current key being pressed. 
for(int i=0;i<(int)getFilter()->getGUICtrlsSize();i++){
        if(controls[i])
                if(getFilter()->getGUICtrls(i).getKeySize()>0)
                if(getFilter()->getGUICtrls(i).getkey(0).equalsIgnoreCase(key.getTextDescription()))
                if(key.isCurrentlyDown()){
                        if(getFilter()->getGUICtrls(i).getStringProp("type")==String("button"))
                                this->buttonClicked(((CabbageButton*)controls[i])->button);
                        else if(getFilter()->getGUICtrls(i).getStringProp("type")==String("checkbox")){
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

#ifndef Cabbage_No_Csound       
//#ifndef Cabbage_Build_Standalone
for(int i=0;i<(int)getFilter()->getGUICtrlsSize();i++){
        inValue = getFilter()->getParameter(i);
        //Logger::writeToLog(getFilter()->getGUICtrls(i).getStringProp("type"));
        if(getFilter()->getGUICtrls(i).getStringProp("type")==String("hslider")||
                        getFilter()->getGUICtrls(i).getStringProp("type")==String("rslider")||
                        getFilter()->getGUICtrls(i).getStringProp("type")==String("vslider")){
        if(controls[i]){
#ifndef Cabbage_Build_Standalone
                float val = getFilter()->getGUICtrls(i).getNumProp("sliderRange")*getFilter()->getParameter(i);
                ((CabbageSlider*)controls[i])->slider->setValue(val, false);
#else
                ((CabbageSlider*)controls[i])->slider->setValue(inValue, sendNotification);
#endif
        }
        }
        
        else if(getFilter()->getGUICtrls(i).getStringProp("type")==String("button")){
        if(controls[i])
                ((CabbageButton*)controls[i])->button->setButtonText(getFilter()->getGUICtrls(i).getItems(1-(int)inValue));
        }
  
        else if(getFilter()->getGUICtrls(i).getStringProp("type")==String("xypad") &&
                getFilter()->getGUICtrls(i).getStringProp("xyChannel").equalsIgnoreCase("X")){
        if(controls[i]){
			int index = ((CabbageXYController*)controls[i])->XYAutoIndex;
			//((CabbageXYController*)controls[i])->xypad->setXYValues(getFilter()->getXYAutomater(index)->getXValue(), 
			//														getFilter()->getXYAutomater(index)->getYValue());
			//Logger::writeToLog(String(getFilter()->getParameter(i)));
			((CabbageXYController*)controls[i])->xypad->setXYValues(getFilter()->getParameter(i), getFilter()->getParameter(i+1));
				//setXYValues(getFilter()->getParameter(i), getFilter()->getParameter(i+1));
                }
        }

        //no automation for comboboxes, still problematic! 
        else if(getFilter()->getGUICtrls(i).getStringProp("type")==String("combobox")){
        //if(controls[i])
#ifdef Cabbage_Build_Standalone
                //((CabbageComboBox*)controls[i])->combo->setSelectedId((int)getFilter()->getParameter(i), false);
#else
                Logger::writeToLog(String("timerCallback():")+String(getFilter()->getParameter(i)));
                float val = getFilter()->getGUICtrls(i).getNumProp("sliderRange")*getFilter()->getParameter(i);
                ((CabbageComboBox*)controls[i])->combo->setSelectedId(int(val), false);
#endif
        }

        else if(getFilter()->getGUICtrls(i).getStringProp("type")==String("checkbox")){
        if(controls[i]){
                        ((CabbageCheckbox*)controls[i])->button->setToggleState((bool)inValue, false);
                        getFilter()->getCsound()->SetChannel(getFilter()->getGUICtrls(i).getStringProp("channel").toUTF8(), inValue);
                        }
                }
}
#ifndef Cabbage_Build_Standalone
for(int i=0;i<(int)getFilter()->getGUILayoutCtrlsSize();i++){
        if(getFilter()->getGUILayoutCtrls(i).getStringProp("type")==String("hostbpm")){     
    if (getFilter()->getPlayHead() != 0 && getFilter()->getPlayHead()->getCurrentPosition (hostInfo))
                getFilter()->getCsound()->SetChannel(getFilter()->getGUILayoutCtrls(i).getStringProp("channel").toUTF8(), hostInfo.bpm);
        }
        else if(getFilter()->getGUILayoutCtrls(i).getStringProp("type")==String("hosttime")){       
    if (getFilter()->getPlayHead() != 0 && getFilter()->getPlayHead()->getCurrentPosition (hostInfo))
                getFilter()->getCsound()->SetChannel(getFilter()->getGUILayoutCtrls(i).getStringProp("channel").toUTF8(), hostInfo.timeInSeconds);
        }
        else if(getFilter()->getGUILayoutCtrls(i).getStringProp("type")==String("hostplaying")){            
    if (getFilter()->getPlayHead() != 0 && getFilter()->getPlayHead()->getCurrentPosition (hostInfo))
                getFilter()->getCsound()->SetChannel(getFilter()->getGUILayoutCtrls(i).getStringProp("channel").toUTF8(), hostInfo.isPlaying);
        }
        else if(getFilter()->getGUILayoutCtrls(i).getStringProp("type")==String("hostrecording")){          
    if (getFilter()->getPlayHead() != 0 && getFilter()->getPlayHead()->getCurrentPosition (hostInfo))
                getFilter()->getCsound()->SetChannel(getFilter()->getGUILayoutCtrls(i).getStringProp("channel").toUTF8(), hostInfo.isRecording);
        }
        else if(getFilter()->getGUILayoutCtrls(i).getStringProp("type")==String("hostppqpos")){     
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
        else if(getFilter()->getGUILayoutCtrls(i).getStringProp("type").containsIgnoreCase("table")){
                int tableSize = getFilter()->getCsound()->TableLength(getFilter()->getGUILayoutCtrls(i).getNumProp("tableNum"));
                //float val = getFilter()->getParameter(i);
				float val = getFilter()->getCsound()->GetChannel(getFilter()->getGUILayoutCtrls(i).getStringProp("channel").toUTF8());
				//cout << String(val) << "\n"; //getFilter()->getGUILayoutCtrls(i).getStringProp("channel");
				//Logger::writeToLog(String(val));
                if(val<0){
                Array <float> tableValues = getFilter()->getTable(1, tableSize);
               // ((CabbageTable*)layoutComps[i])->fillTable(0, tableValues);
               // cout << "val less than 0";
				getFilter()->getCsound()->SetChannel(getFilter()->getGUILayoutCtrls(i).getStringProp("channel").toUTF8(), 0.f);
                }

        }
		
//		
//        else if(getFilter()->getGUILayoutCtrls(i).getStringProp("type").containsIgnoreCase("pvsview")){
//                float val = getFilter()->getCsound()->GetChannel(getFilter()->getGUILayoutCtrls(i).getStringProp("channel").toUTF8());
//                //Logger::writeToLog(String(getFilter()->getGUILayoutCtrls(i).getNumProp("pvsChannel")));
//                if(val>0){
//                getFilter()->getCsound()->PvsoutGet(getFilter()->getPVSDataOut(), getFilter()->getGUILayoutCtrls(i).getNumProp("pvsChannel"));
//                ((CabbagePVSView*)layoutComps[i])->updatePVSStruct();
//                getFilter()->getCsound()->SetChannel(getFilter()->getGUILayoutCtrls(i).getStringProp("channel").toUTF8(), 0.f);
//                }
//        }
}

//Check to see if I can use the same means to control sliders if they are in the host application. But 
//      I can't unless I hack the proceeBlcok() method...
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

                                if(getFilter()->getGUICtrls(i).getStringProp("type")==String("hslider")||
                                                getFilter()->getGUICtrls(i).getStringProp("type")==String("rslider")||
                                                getFilter()->getGUICtrls(i).getStringProp("type")==String("vslider")){
                                if(controls[i])
                                                ((CabbageSlider*)controls[i])->slider->setValue(value, dontSendNotification);
                                }
                                else if(getFilter()->getGUICtrls(i).getStringProp("type")==String("button")){
                                if(controls[i])
                                        ((CabbageButton*)controls[i])->button->setButtonText(getFilter()->getGUICtrls(i).getItems(1-(int)value));
                                }
                                else if(getFilter()->getGUICtrls(i).getStringProp("type")==String("combobox")){
                                        if(controls[i]){
                                        //((CabbageComboBox*)controls[i])->combo->setSelectedId((int)value+1.5, false);
                                        }
                                }
                                else if(getFilter()->getGUICtrls(i).getStringProp("type")==String("checkbox")){
                                if(controls[i])
                                        if(value==0)
                                                ((CabbageCheckbox*)controls[i])->button->setToggleState(0, false);
                                        else
                                                ((Button*)controls[i])->setToggleState(1, false);
                                }
                                getFilter()->getCsound()->SetChannel(getFilter()->getGUICtrls(i).getStringProp("channel").toUTF8(), value);                             
        if(message.isController())
      if(getFilter()->getMidiDebug()){
                                        String debugInfo =  String("MIDI Channel:    ")+String(message.getChannel())+String(" \t")+
                                                                                String("MIDI Controller: ")+String(message.getControllerNumber())+String("\t")+
                                                                                String("MIDI Value:      ")+String(message.getControllerValue())+String("\n");
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
