/*
*  ComponentLayoutEditor.cpp
*  
*  Original written by Haydxn
*  Modified by Jordan Hochenbaum on 10/25/10.
*  http://www.rawmaterialsoftware.com/viewtopic.php?f=6&t=2635
*
*  Further modifications made by Rory Walsh 19/11/10
*
*/
#include "ComponentLayoutEditor.h"
#include "MainPanel.h"
#include "CabbagePluginEditor.h"

//==========================================================================
//			ChildAlias
//==========================================================================
ChildAlias::ChildAlias (Component* targetChild, int ID)
:   target (targetChild)
{   
	setName("Alias_" + String(ID));
	getProperties().set("index", var(ID));
	resizeContainer = new ComponentBoundsConstrainer();
	resizeContainer->setMinimumSize(target.getComponent()->getWidth()/2, target.getComponent()->getHeight()/2); //set minimum size so objects cant be resized too small
	resizer = new ResizableBorderComponent (this,resizeContainer);
	addAndMakeVisible (resizer);
	resizer->addMouseListener (this,false);
	constrainer = new ComponentBoundsConstrainer();

	interest = false;
	userAdjusting = false;

	updateFromTarget ();
	setRepaintsOnMouseActivity (true);
}

ChildAlias::~ChildAlias ()
{
   delete resizer;
}
//-----------------------------------------------------------------------------
void ChildAlias::resized ()
{
   resizer->setBounds (0,0,getWidth(),getHeight());
   
   if (resizer->isMouseButtonDown ())
   {
      applyToTarget ();
   }
}
//-----------------------------------------------------------------------------
void ChildAlias::paint (Graphics& g)
{
   Colour c;
   if (interest)
      c = findColour (ComponentLayoutEditor::aliasHoverColour,true);
   else c = findColour (ComponentLayoutEditor::aliasIdleColour,true);
   
   g.setColour (c.withMultipliedAlpha (0.3f));
   g.setColour (c);
   g.fillAll ();
   

   g.drawRect(0,0,getWidth(),getHeight(),3);
}
//-----------------------------------------------------------------------------
const Component* ChildAlias::getTargetChild()
{
	return target.getComponent();
}
//-----------------------------------------------------------------------------
ComponentLayoutEditor* ChildAlias::getParent()
{
	return (ComponentLayoutEditor*)getParentComponent();
}
//-----------------------------------------------------------------------------
MainPanel* ChildAlias::getTarget()
{
	return (MainPanel*)target->getParentComponent();
}
//-----------------------------------------------------------------------------
void ChildAlias::updateFromTarget ()
{
   if (target != NULL)
      //if (!target.hasBeenDeleted ())
   {
      setBounds ( target.getComponent ()->getBounds () );
   }
}
//-----------------------------------------------------------------------------
void ChildAlias::applyToTarget ()
{
   if (target != NULL)
      //!target.hasBeenDeleted ())
   {
      Component* c = (Component*) target.getComponent ();
      c->toFront(false); //added this to bring the the component to the front
      c->setBounds (getBounds ());
      userChangedBounds ();
   }
}
//-----------------------------------------------------------------------------
void ChildAlias::userChangedBounds ()
{
   //update minimum onscreen amounts so that object can't be resized past the screen area
   resizeContainer->setMinimumOnscreenAmounts(getHeight()+target.getComponent()->getHeight(), getWidth()+target.getComponent()->getWidth(), 
                                    getHeight()+target.getComponent()->getHeight(), getWidth()+target.getComponent()->getWidth());
   
}
//-----------------------------------------------------------------------------
void ChildAlias::userStartedChangingBounds ()
{
}
//-----------------------------------------------------------------------------
void ChildAlias::userStoppedChangingBounds ()
{
}
//-----------------------------------------------------------------------------
bool ChildAlias::boundsChangedSinceStart ()
{
   return startBounds != getBounds ();
}


//-----------------------------------------------------------------------------
void ChildAlias::mouseUp (const MouseEvent& e)
{   
	//clear and update component positions
	coordinates.clear();
	//get positions of selected components in MainPanel
	for(int i=0;i<getTarget()->selectedComps.getNumSelected();i++){
		Rectangle<int> rect(getTarget()->selectedComps.getSelectedItem(i)->getPosition().getX(),
		getTarget()->selectedComps.getSelectedItem(i)->getPosition().getY(),
		getTarget()->selectedComps.getSelectedItem(i)->getWidth(),
		getTarget()->selectedComps.getSelectedItem(i)->getHeight());
		coordinates.add(rect);
	}
   if (e.eventComponent == resizer)
   {
   }
   else
   {
      //add this to reset MainComponent to have keyboard focus so that keyboard shortcuts (eg. lock/unlock) still work / intercept the messages
      getTopLevelComponent()->getChildComponent(0)->grabKeyboardFocus(); 
   }
   if (userAdjusting) userStoppedChangingBounds ();
   userAdjusting = false;
}
//-----------------------------------------------------------------------------
void ChildAlias::mouseDown (const MouseEvent& e)
{
	getTarget()->indexOfCurrentComponent = getProperties().getWithDefault(var::identifier("index"), 99);

	toFront (true);
	//clear and update component positions
	coordinates.clear();
	//get positions and sizes of selected components in MainPanel. 
	for(int i=0;i<getTarget()->selectedComps.getNumSelected();i++){
		Rectangle<int> rect(getTarget()->selectedComps.getSelectedItem(i)->getPosition().getX(),
		getTarget()->selectedComps.getSelectedItem(i)->getPosition().getY(),
		getTarget()->selectedComps.getSelectedItem(i)->getWidth(),
		getTarget()->selectedComps.getSelectedItem(i)->getHeight());
		coordinates.add(rect);
	}

	//if there are some componets slected and a user hits an unselected one
	//deselect all other components...
	int cnt=0;
	for(int i=0;i<coordinates.size();i++) 
	{
		if(coordinates.getReference(i).contains(getTarget()->getMouseXYRelative().getX()+1,	getTarget()->getMouseXYRelative().getY()+1)){
			i=coordinates.size();
			cnt++;
			//cnt will incremet if the mouse is inside the area of one of the selected components
		 }
	}
	if(cnt<1){
		//unselect and repaint the components
		for(int y=0;y<getTarget()->selectedComps.getNumSelected();y++)
			getTarget()->selectedComps.getSelectedItem(y)->setColour(Label::backgroundColourId, juce::Colour(0x1001200));
		getTarget()->selectedComps.deselectAll();
	}
		

   if (e.eventComponent == resizer)
   {
   }
   else
   {
      //added a constrainer so that components can't be dragged off-screen
      //constrainer->setMinimumOnscreenAmounts(getHeight(), getWidth(), getHeight(), getWidth());
      dragger.startDraggingComponent (this, e);
   }
   userAdjusting = true;
   startBounds = getBounds ();
   userStartedChangingBounds ();

 //  if(e.getNumberOfClicks()==2)
	   getTarget()->showProps(true, this);

PopupMenu m;
m.addItem(1, "Delete");
m.addItem(2, "Copy");
m.addItem(3, "Cut");
if (e.mods.isRightButtonDown())
 {
 const int result = m.show();
 if(result == 1){
	getTarget()->cutcopyclearComponents(getProperties().getWithDefault(var::identifier("index"), 99), "CLEAR");
 }
 else if(result == 3){
	getTarget()->cutcopyclearComponents(getProperties().getWithDefault(var::identifier("index"), 99), "CUT");
 }
 else if(result == 2){
	getTarget()->cutcopyclearComponents(getProperties().getWithDefault(var::identifier("index"), 99), "COPY");
 }
}

}
//-----------------------------------------------------------------------------
void ChildAlias::mouseDrag (const MouseEvent& e)
{
getTarget()->showProps(true, this);
   if (e.eventComponent == resizer)
   {

   }
   else
   {
      if (!e.mouseWasClicked ())
		{
		//if there are selected components move them all in one go.
		//use the coordinates array to retrieve the XY positions of the components
		//when the mouse was pressed. Then add the distance the mouse moves to each.. 
		if(getTarget()->selectedComps.getNumSelected()>0){
			  for(int i=0;i<getTarget()->selectedComps.getNumSelected();i++)
			  {
				  int index = getTarget()->selectedComps.getSelectedItem(i)->getProperties().getWithDefault(var::identifier("index"), 99);
				  String name = getTarget()->selectedComps.getSelectedItem(i)->getName();
					//position array of selected components from MainPanel
					getTarget()->selectedComps.getSelectedItem(i)->setTopLeftPosition(
						coordinates.getReference(i).getX()+e.getDistanceFromDragStartX(),
						coordinates.getReference(i).getY()+e.getDistanceFromDragStartY());
					//position aliases, each alias is stored in the frames 
					//array see. ComponentLayoutEditor::updateFrames
					name = getParent()->getFrame(index)->getName();
					getParent()->getFrame(index)->setTopLeftPosition(
						coordinates.getReference(i).getX()+e.getDistanceFromDragStartX(),
						coordinates.getReference(i).getY()+e.getDistanceFromDragStartY());		  
			}
		}
		else{
			constrainer->setMinimumOnscreenAmounts(getHeight(), getWidth(), getHeight(), getWidth());
			dragger.dragComponent(this,e,constrainer);
			applyToTarget ();
		}
	}
   }
}
//-----------------------------------------------------------------------------
void ChildAlias::mouseEnter (const MouseEvent& e)
{
   interest = true;
   repaint ();
}
//-----------------------------------------------------------------------------
void ChildAlias::mouseExit (const MouseEvent& e)
{
   interest = false;
   repaint ();
}

//=============================================================================
//		ComponentLayoutEditor
//=============================================================================
ComponentLayoutEditor::ComponentLayoutEditor ()
:   target (0), compID(0)
{
	setColour (ComponentLayoutEditor::aliasIdleColour,Colours::lightgrey.withAlpha(0.2f));
	setColour (ComponentLayoutEditor::aliasHoverColour,Colours::white.withAlpha(0.5f));
    setInterceptsMouseClicks (false, true);   
}

ComponentLayoutEditor::~ComponentLayoutEditor ()
{
   if (target != getTopLevelComponent()->getChildComponent(0) ){deleteAndZero(target);} //added this to make sure we dont remove our background component
}
//-----------------------------------------------------------------------------
void ComponentLayoutEditor::resized ()
{
   for (int i=0; i<frames.size(); i++)
   {
      frames.getUnchecked(i)->updateFromTarget ();
   }
}
//-----------------------------------------------------------------------------
void ComponentLayoutEditor::paint (Graphics& g)
{
}
//-----------------------------------------------------------------------------
void ComponentLayoutEditor::setTargetComponent (Component* targetComp)
{
   jassert (targetComp);
   jassert (targetComp->getParentComponent() == getParentComponent());
   
   if (target)
   {
      if (target.getComponent() == targetComp) return;
      deleteAndZero (target);
   }
   
   target = targetComp;
   bindWithTarget ();
}
//-----------------------------------------------------------------------------
void ComponentLayoutEditor::bindWithTarget ()
{
   if (target != NULL)
      //if (target && !target->hasBeenDeleted ())
   {
      Component* t = (Component*) target.getComponent ();
      Component* p = t->getParentComponent ();      
      p->addAndMakeVisible (this);
      setBounds (t->getBounds ());
      
      updateFrames ();
   }
}
//-----------------------------------------------------------------------------
void ComponentLayoutEditor::updateFrames ()
{
    frames.clear ();
    compID=0;

    if (target != NULL)
        //if (target && !target->hasBeenDeleted ())
    {
        Component* t = (Component*) target.getComponent ();

        int n = t->getNumChildComponents ();
        //Logger::writeToLog("UpdateFrames: " + String(n));
        for (int i=0; i<n; i++)
        {
            Component* c = t->getChildComponent (i);
            if (c)
            {
                compID = c->getProperties().getWithDefault(var::identifier("index"), 99);
                ChildAlias* alias = createAlias (c, compID);
                if (alias)
                {
                    frames.add (alias);
                    addAndMakeVisible (alias);
                    //compID++;
                }
            }
        }
    }
}
//-----------------------------------------------------------------------------
void ComponentLayoutEditor::enablementChanged ()
{
   if (isEnabled ())
   {
      setVisible (true);
   }
   else
   {
      setVisible (false);
   }
}
//-----------------------------------------------------------------------------
const Component* ComponentLayoutEditor::getTarget ()
{
   if (target) return target.getComponent ();
   return 0;
}
//-----------------------------------------------------------------------------
ChildAlias* ComponentLayoutEditor::createAlias (Component* child, int compID)
{
	return new ChildAlias (child, compID);
	
}
//-----------------------------------------------------------------------------
ChildAlias* ComponentLayoutEditor::getFrame(int index)
{
    //return the alias
    //return frames[index];

    for (int i=0; i < frames.size(); i++)
    {
        compID = frames[i]->getProperties().getWithDefault(var::identifier("index"), 99);
        if(compID == index) return frames[i];
    }
    return 0;

}

//-----------------------------------------------------------------------------
void ComponentLayoutEditor::removeFrame(int index)
{
    //return the alias
    //return frames[index];

    for (int i=0; i < frames.size(); i++)
    {
        compID = frames[i]->getProperties().getWithDefault(var::identifier("index"), 99);
		if(compID == index){
			//frames[i]->removeFromDesktop();
			frames.remove(i);
		}
    }
}
