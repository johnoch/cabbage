/*
*  ComponentLayoutEditor.cpp
*  
*  Original written by Haydxn
*  Modified by Jordan Hochenbaum on 10/25/10.
*  http://www.rawmaterialsoftware.com/viewtopic.php?f=6&t=2635
*
*/
#include "ComponentLayoutEditor.h"
#include "CabbageMainPanel.h"

ChildAlias::ChildAlias (Component* targetChild, int ind)
:   target (targetChild), index(ind)
{   
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

}

void ChildAlias::resized ()
{
   resizer->setBounds (0,0,getWidth(),getHeight());
   
   if (resizer->isMouseButtonDown ())
   {
      applyToTarget ();
   }
}

void ChildAlias::paint (Graphics& g)
{
   Colour c;
   if (interest)
      c = findColour (ComponentLayoutEditor::aliasHoverColour,true);
   else c = findColour (ComponentLayoutEditor::aliasIdleColour,true);
   
   g.setColour (c.withMultipliedAlpha (0.3f));
   g.fillAll ();
   g.setColour (c);
   g.drawRect (0,0,getWidth(),getHeight(),1);
}

const Component* ChildAlias::getTargetChild ()
{
   return target.getComponent ();
}

const Component* ChildAlias::getTarget()
{
   return target;
}

void ChildAlias::updateFromTarget ()
{
   if (target != NULL)
      //if (!target.hasBeenDeleted ())
   {
      setBounds ( target.getComponent ()->getBounds () );
   }
}

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

void ChildAlias::userChangedBounds ()
{
   //update minimum onscreen amounts so that object can't be resized past the screen area
   resizeContainer->setMinimumOnscreenAmounts(getHeight()+target.getComponent()->getHeight(), getWidth()+target.getComponent()->getWidth(), 
                                    getHeight()+target.getComponent()->getHeight(), getWidth()+target.getComponent()->getWidth());
   
}

void ChildAlias::userStartedChangingBounds ()
{
}

void ChildAlias::userStoppedChangingBounds ()
{
}

bool ChildAlias::boundsChangedSinceStart ()
{
   return startBounds != getBounds ();
}


void ChildAlias::mouseDown (const MouseEvent& e)
{
((CabbageMainPanel*)(getTarget()->getParentComponent()))->setMouseState("down");
   toFront (true);
   if (e.eventComponent == resizer)
   {
   }
   else
   {
      //added a constrainer so that components can't be dragged off-screen
      constrainer->setMinimumOnscreenAmounts(getHeight(), getWidth(), getHeight(), getWidth());
      dragger.startDraggingComponent (this,e);
   }
   userAdjusting = true;
   startBounds = getBounds ();
   userStartedChangingBounds ();
   ((CabbageMainPanel*)(getTarget()->getParentComponent()))->setIndex(index);
   updateCurrentDimensions(getPosition().getX(), getPosition().getY(), getWidth(), getHeight());
   ((CabbageMainPanel*)(getTarget()->getParentComponent()))->sendChangeMessage();
   
}

void ChildAlias::mouseUp (const MouseEvent& e)
{ 
((CabbageMainPanel*)(getTarget()->getParentComponent()))->setMouseState("up");
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
   
  // updateCurrentDimensions(getPosition().getX(), getPosition().getY(), 1000, 2000);
   ((CabbageMainPanel*)(getTarget()->getParentComponent()))->width = getWidth();
   ((CabbageMainPanel*)(getTarget()->getParentComponent()))->height = getHeight();
   ((CabbageMainPanel*)(getTarget()->getParentComponent()))->left = getPosition().getX();
   ((CabbageMainPanel*)(getTarget()->getParentComponent()))->top = getPosition().getY();      
   ((CabbageMainPanel*)(getTarget()->getParentComponent()))->sendChangeMessage();

}

void ChildAlias::mouseDrag (const MouseEvent& e)
{
   if (e.eventComponent == resizer)
   {
   }
   else
   {
      if (!e.mouseWasClicked ())
      {
		 constrainer->setMinimumOnscreenAmounts(getHeight(), getWidth(), getHeight(), getWidth());
         dragger.dragComponent (this,e, constrainer);
         applyToTarget ();
      }
   }
}

void ChildAlias::mouseEnter (const MouseEvent& e)
{
   interest = true;
   repaint ();
}

void ChildAlias::mouseExit (const MouseEvent& e)
{
   interest = false;
   repaint ();
}

void ChildAlias::updateCurrentDimensions(int x, int y, int width, int height)
{
   ((CabbageMainPanel*)(getTarget()->getParentComponent()))->width = width;
   ((CabbageMainPanel*)(getTarget()->getParentComponent()))->height = height;
   ((CabbageMainPanel*)(getTarget()->getParentComponent()))->left = x;
   ((CabbageMainPanel*)(getTarget()->getParentComponent()))->top = y;
}

//=============================================================================
ComponentLayoutEditor::ComponentLayoutEditor ()
:   target (0)
{
   setColour (ComponentLayoutEditor::aliasIdleColour,Colours::lightgrey.withAlpha(0.2f));
   setColour (ComponentLayoutEditor::aliasHoverColour,Colours::white.withAlpha(0.5f));
    setInterceptsMouseClicks (false, true);   
}

ComponentLayoutEditor::~ComponentLayoutEditor ()
{
   //if (target != getTopLevelComponent()->getChildComponent(0) ){deleteAndZero(target);} //added this to make sure we dont remove our background component
   //if (target) { deleteAndZero (target); } //original
}

void ComponentLayoutEditor::resized ()
{
   for (int i=0; i<frames.size(); i++)
   {
      frames.getUnchecked(i)->updateFromTarget ();
   }
}

void ComponentLayoutEditor::paint (Graphics& g)
{
}

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

void ComponentLayoutEditor::updateFrames ()
{
   int compIndex = 0;
   frames.clear ();
   
   if (target != NULL)
      //if (target && !target->hasBeenDeleted ())
   {
      Component* t = (Component*) target.getComponent ();
      
      int n = t->getNumChildComponents ();
      for (int i=0; i<n; i++)
      {
         Component* c = t->getChildComponent (i);
         if (c)
         {
                ChildAlias* alias = createAlias (c, compIndex++);
//				alias->addChangeListener(this);
                if (alias)
                {
               frames.add (alias);
               addAndMakeVisible (alias);
                }
         }
      }
   }
}

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


const Component* ComponentLayoutEditor::getTarget ()
{
   if (target) return target.getComponent ();
   return 0;
}

ChildAlias* ComponentLayoutEditor::createAlias (Component* child, int index)
{
   return new ChildAlias (child, index);
}