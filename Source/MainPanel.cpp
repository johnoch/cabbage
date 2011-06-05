#include "MainPanel.h"
#include "ComponentLayoutEditor.h"


/*
 * This is the main panel/component onto which all other components are drawn.
 * When users drop components onto this panel, ComponentLayoutEditor will 
 * create aliases of each so that they can be moved around and resized.
 * When a user disables the ComponentLayoutEditor this panel is again brought
 * to front so that components can no longer be edited. The idea is that
 * by toggling these two classes toFront() we can swap between edit and lock mode
 *
 */
  
//----------------------------------------------------------------------
MainPanel::MainPanel()
{
setName("MainPanel");
setVisible(true);
//editor = sibling;
LOCKED=true;
}

MainPanel::~MainPanel()
{
	labels.clear();
	//clipBoard.clear();
	deleteAllChildren();
}


ComponentLayoutEditor* MainPanel::getLayoutEditor()
{
return editor;
}
//----------------------------------------------------------------------
void MainPanel::mouseDown(const MouseEvent &e)
{
PopupMenu m;

if(LOCKED==true)
m.addItem(3, "Edit-mode");
else{
m.addItem(2, "Lock");
m.addItem(5, "Paste");
m.addSeparator();
m.addItem(1, "Insert H-Slider");
m.addItem(1, "Insert V-Slider");
m.addItem(1, "Insert R-Slider");
}

for(int i=0;i<selectedComps.getNumSelected();i++)
	((Label*)selectedComps.getSelectedItem(i))->setColour(Label::backgroundColourId, juce::Colour(0x1001200));
        selectedComps.deselectAll(); 

if (e.mods.isRightButtonDown())
 {
 const int result = m.show();
 if (result == 1)
     {
		Label* lab = new Label("Comp_"+String(labels.size()), "Comp_"+String(labels.size()));
		lab->getProperties().set("index", var(labels.size()));
		lab->setBounds(e.getMouseDownX(), e.getMouseDownY(), 50*(labels.size()+1), 50);
		labels.add(lab);	
		addAndMakeVisible(labels[labels.size()-1]);
		getLayoutEditor()->toFront(true);
		getLayoutEditor()->updateFrames();
		LOCKED=false;
    }
 else if (result == 2)
     {
		 getLayoutEditor()->setEnabled(false);
		 this->toFront(true);
		 LOCKED=true;
     }
 else if (result == 3)
     {
		 editor->setEnabled(true);
		 editor->toFront(true); 
		 LOCKED=false;
     }
 else if (result == 4)
     {		 
		 getLayoutEditor()->updateFrames();
     }
 else if (result == 5)
     {		 
		pasteComponents();	 
     }
}
else if (e.mods.isLeftButtonDown())
    {	
		addChildComponent(&lassoComp);
        lassoComp.beginLasso (e, this);
    }
}


//----------------------------------------------------------------------
void MainPanel::mouseDrag(const MouseEvent &e)
{
    lassoComp.toFront (false);
    lassoComp.dragLasso (e);
}

//----------------------------------------------------------------------
void MainPanel::mouseUp(const MouseEvent &)
{
for(int i=0;i<selectedComps.getNumSelected();i++)
((Label*)selectedComps.getSelectedItem(i))->setColour(Label::backgroundColourId, juce::Colour(0x1001200).withAlpha(.1f));

    lassoComp.endLasso();
    removeChildComponent (&lassoComp);
}
//----------------------------------------------------------------------
int MainPanel::getNumComponents()
{
//	int size = labels.size();
//	return labels.size();
	return 0;
}
//----------------------------------------------------------------------
void MainPanel::cutcopyclearComponents(int index, String type)
{
// I have to deselect the components before deleted..
int numComps = selectedComps.getNumSelected();
clipBoard.clear();
if(numComps>0){
	Array <int> indexArr;
		for(int i=0;i<numComps;i++)
			//take note of the selectedCOmponents indices...
			indexArr.add(selectedComps.getSelectedItem(i)->getProperties().getWithDefault(var::identifier("index"), 99));

		//deselect componets before removing them from list
		for(int i=0;i<selectedComps.getNumSelected();i++)
		((Label*)selectedComps.getSelectedItem(i))->setColour(Label::backgroundColourId, juce::Colour(0x1001200));
		selectedComps.deselectAll();
		//start with the components that have the hightest index
		for(int i=indexArr.size()-1;i>=0;i--){
			if(type.contains(T("CUT"))){
			clipBoard.add(new Label());
			clipBoard[clipBoard.size()-1]->setBounds(labels[indexArr[i]]->getBounds());
			labels.removeRange(indexArr[i], 1);			
			}
			else if(type.contains(T("COPY"))){
			clipBoard.add(new Label());
			clipBoard[clipBoard.size()-1]->setBounds(labels[indexArr[i]]->getBounds());
			}
			else if(type.contains(T("CLEAR"))){
			labels.removeRange(indexArr[i], 1);	
			}
		}
	}
else
	labels.removeRange(index, 1);

resetAndUpdate();
	
}
//----------------------------------------------------------------------
void MainPanel::pasteComponents()
{
	selectedComps.deselectAll();
for(int i = 0; i < clipBoard.size();i++){
	String test = clipBoard[i]->getName();
	Label* lab = new Label("__");
	lab->setBounds(clipBoard[i]->getBounds());
	labels.add(lab);	
	addAndMakeVisible(labels[labels.size()-1]);
	selectedComps.addToSelection(labels[labels.size()-1]);
}
//highlight slected components..
for(int i=0;i<selectedComps.getNumSelected();i++)
((Label*)selectedComps.getSelectedItem(i))->setColour(Label::backgroundColourId, juce::Colour(0x1001200).withAlpha(.1f));

resetAndUpdate();
}

//----------------------------------------------------------------------
void MainPanel::resetAndUpdate()
{
//reset all indices
for(int i =0;i<labels.size();i++){
	labels[i]->getProperties().set("index", var(i));
	labels[i]->setText("Comp_"+String(i), false);
	labels[i]->setName("Comp_"+String(i));
	}

//update editable overlays..
getLayoutEditor()->updateFrames();
}

//----------------------------------------------------------------------
Component* MainPanel::getComponent(int index)
{
	return labels[index];
}
//----------------------------------------------------------------------
void MainPanel::findLassoItemsInArea (Array <Component*>& results, const Rectangle<int>& area)
{
    const Rectangle<int> lasso (area);

	for (int i = 0; i < labels.size(); ++i)
    {
        Component* c = labels[i];

		if (c->getBounds().intersects (lasso)){
		results.addIfNotAlreadyThere(c);
		selectedComps.addToSelection(c);
		}
		else
			selectedComps.deselect(c);
    }
}

//----------------------------------------------------------------------
SelectedItemSet <Component*>& MainPanel::getLassoSelection()
{
    return selectedComps;
}
//----------------------------------------------------------------------
void MainPanel::changeListenerCallback(ChangeBroadcaster*)
{
	/*
	labels[indexOfCurrentComponent]->setBounds(propsDialog->getProps("top").getIntValue(),
												propsDialog->getProps("left").getIntValue(),
												propsDialog->getProps("width").getIntValue(),
												propsDialog->getProps("height").getIntValue());
	labels[indexOfCurrentComponent]->setColour(Label::backgroundColourId, Colours::findColourForName(propsDialog->getProps("colour"), Colours::white));
	labels[indexOfCurrentComponent]->setText(propsDialog->getProps("caption"), false);
	getLayoutEditor()->updateFrames();
	Logger::writeToLog("message recieved...");
	*/
}
//----------------------------------------------------------------------
void MainPanel::showProps(bool show, Component* )
{
	/*
if(show){
	propsDialog->setProps("top", String(labels[indexOfCurrentComponent]->getX()));
	propsDialog->setProps("left", String(labels[indexOfCurrentComponent]->getY()));
	propsDialog->setProps("width", String(labels[indexOfCurrentComponent]->getWidth()));
	propsDialog->setProps("height", String(labels[indexOfCurrentComponent]->getHeight()));
	propsDialog->setProps("caption", String(labels[indexOfCurrentComponent]->getText()));
	propsDialog->setProps("min", String(labels[indexOfCurrentComponent]->getHeight()));
	
	
	propsDialog->setVisible(true);
	propsDialog->toFront(true);
	}
else 
*/
}

