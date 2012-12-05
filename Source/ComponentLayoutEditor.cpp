/*
*  ComponentLayoutEditor.cpp
*  
*  Original written by Haydxn
*  Modified by Jordan Hochenbaum on 10/25/10.
*  http://www.rawmaterialsoftware.com/viewtopic.php?f=6&t=2635
*

and then Rory made shit of it....

*/
#include "ComponentLayoutEditor.h"
#include "./Standalone/CabbageStandaloneDialog.h"


ChildAlias::ChildAlias (Component* targetChild, String type, int ind)
:   target (targetChild), index(ind), type(type)
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
      applyToTarget ("");
   }
}

void ChildAlias::paint (Graphics& g)
{
   Colour c;
   if (interest)
	c = Colours::findColourForName("white", Colours::black);
   else
	c = findColour (ComponentLayoutEditor::aliasHoverColour,true);

   
   g.setColour (c.withMultipliedAlpha (0.2f));
   g.fillAll ();
   g.setColour (Colours::orange.withAlpha(.5f));
   g.drawRect (0,0,getWidth(),getHeight(),1);
}

CabbageMainPanel* ChildAlias::getMainPanel()
{
	return (CabbageMainPanel*)target->getParentComponent();
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

void ChildAlias::applyToTarget (String triggeredFrom)
{
   if (target != NULL)
      //!target.hasBeenDeleted ())
   {
        Component* c = (Component*) target.getComponent ();
		((CabbageMainPanel*)(getTarget()->getParentComponent()))->childBounds.clear();
		((CabbageMainPanel*)(getTarget()->getParentComponent()))->origChildBounds.clear();

		if(type.containsIgnoreCase("CabbageGroupbox")||
			type.containsIgnoreCase("CabbageImage"))
			c->toBack();
			
		else 
		c->toFront(true);

		//if just resizing is taking place we need to resize child components..
		if(startBounds.getTopLeft()==c->getBounds().getTopLeft())
		for(int i=0;i<c->getNumChildComponents();i++){
			float x = ((float)c->getWidth()/(float)startBounds.getWidth());
			float y = ((float)c->getHeight()/(float)startBounds.getHeight());
			
			c->getChildComponent(i)->setBounds(origBounds[i].getX()*x, 
												origBounds[i].getY()*y,
												origBounds[i].getWidth()*x,
												origBounds[i].getHeight()*y);
			((CabbageMainPanel*)(getTarget()->getParentComponent()))->childBounds.add(c->getChildComponent(i)->getBounds());
			((CabbageMainPanel*)(getTarget()->getParentComponent()))->origChildBounds.add(origBounds[i]);
		}
		//Logger::writeToLog("ComponentEditor:"+String(((CabbageMainPanel*)(getTarget()->getParentComponent()))->childBounds.size()));
		//Logger::writeToLog("Number of children: "+String(((CabbageMainPanel*)(getTarget()->getParentComponent()))->childBounds.size()));
	  ((CabbageMainPanel*)(getTarget()->getParentComponent()))->sendActionMessage("Message sent from CabbageMainPanel");
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
 
//if (e.mods.isLeftButtonDown()){
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

   //get the bounds of each of the child components if we are dealing with a plant
   Component* c = (Component*) target.getComponent ();
   origBounds.clear();
   for(int i=0;i<c->getNumChildComponents();i++){
   origBounds.add(c->getChildComponent(i)->getBounds());
   }


   //update dimensions
   int offX = getProperties().getWithDefault(var::identifier("plantX"), 0);
   int offY = getProperties().getWithDefault(var::identifier("plantY"), 0);

   ((CabbageMainPanel*)(getTarget()->getParentComponent()))->setIndex(index);
   ((CabbageMainPanel*)(getTarget()->getParentComponent()))->currentBounds.setBounds(getPosition().getX()-offX,
																					 getPosition().getY()-offY,
																					 getWidth(),
																					 getHeight());

   ((CabbageMainPanel*)(getTarget()->getParentComponent()))->sendActionMessage("Message sent from CabbageMainPanel");

#ifdef Cabbage_Build_Standalone
	if(e.mods.isRightButtonDown()){
		PopupMenu m;
		m.setLookAndFeel(&getParentComponent()->getLookAndFeel());
		m.addItem(1, "Add to repository");
		int choice = m.show();
		if(choice==1){
			this->getTopLevelComponent()->setAlwaysOnTop(false);
			AlertWindow alert("Add to Repository", "Enter a name and hit 'escape'", AlertWindow::NoIcon, this->getTopLevelComponent()); 
			CabbageLookAndFeel basicLookAndFeel;
			alert.setLookAndFeel(&basicLookAndFeel);
			alert.setColour(TextEditor::textColourId, Colours::white);
			//alert.addTextBlock("Enter a name and hit 'escape'(The following symbols not premitted in names:"" $ % ^ & * ( ) - + )");
			alert.addTextEditor("textEditor", "name", "");
			alert.runModalLoop();
			this->getTopLevelComponent()->setAlwaysOnTop(true);
//			if(alert.getTextEditorContents("textEditor").containsAnyOf("$%^&*()-+ "))
//				CabbageUtils::showMessage("Invalid name, please try again(names cannot contains white spaces of the following characters: $ % ^ & * ( ) - + ). Nothing has been added to your user repository", &getLookAndFeel());

						//make sure host doesn't fail if there are no Plant entries
//				ScopedPointer<XmlElement> xml;
//				xml = new XmlElement("PLANTS");
//				xml = appProperties->getUserSettings()->getXmlValue("PlantRepository");
				bool clashingNames=false;
				int result; 
				
				String plantDir = appProperties->getUserSettings()->getValue("PlantFileDir", "");				
				Array<File> tempfiles;
				StringArray plants;   
				addFileToPpopupMenu(m, tempfiles, plantDir, "*.plant");
				
				for(int i=0;i<tempfiles.size();i++){
				Logger::outputDebugString(tempfiles[i].getFullPathName());
				plants.add(tempfiles[i].getFileNameWithoutExtension());
				}
				
				for(int i=0;i<plants.size();i++)				
				if(plants[i]==alert.getTextEditorContents("textEditor"))
					clashingNames = true;

				if(clashingNames==true){
					result = CabbageUtils::showYesNoMessage("Do you wish to overwrite the existing plant?", &getLookAndFeel());	
					if(result == 0)
					((CabbageMainPanel*)(getTarget()->getParentComponent()))->sendActionMessage("Message sent from CabbageMainPanel"+alert.getTextEditorContents("textEditor"));
					else
						showMessage("Nothing written to repository", &getLookAndFeel());
				}
				else ((CabbageMainPanel*)(getTarget()->getParentComponent()))->sendActionMessage("Message sent from CabbageMainPanel"+alert.getTextEditorContents("textEditor"));

		}
		}
#endif

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
   
   int offX = getProperties().getWithDefault(var::identifier("plantX"), 0);
   int offY = getProperties().getWithDefault(var::identifier("plantY"), 0);
   //update dimensions
   ((CabbageMainPanel*)(getTarget()->getParentComponent()))->currentBounds.setBounds(getPosition().getX()-offX,
																					 getPosition().getY()-offY,
																					 getWidth(),
																					 getHeight());

   applyToTarget("");

   ((CabbageMainPanel*)(getTarget()->getParentComponent()))->sendActionMessage("Message sent from CabbageMainPanel");

   if(type.containsIgnoreCase("CabbageGroupbox")||
	   type.containsIgnoreCase("CabbageImage"))
	   toBack();
   else 
	   toFront(true);


}

void ChildAlias::mouseDrag (const MouseEvent& e)
{
if (e.mods.isLeftButtonDown()){
   if (e.eventComponent == resizer)
   {
   }
   else
   {
      if (!e.mouseWasClicked ())
      {
		 constrainer->setMinimumOnscreenAmounts(getHeight(), getWidth(), getHeight(), getWidth());
         dragger.dragComponent (this,e, constrainer);
         applyToTarget ("");
		 if(type.containsIgnoreCase("juce::GroupComponent")||
					type.containsIgnoreCase("CabbageImage"))
			toBack();
      }
   }
   if(type.containsIgnoreCase("juce::GroupComponent")||
	   type.containsIgnoreCase("CabbageImage"))
	   toBack();
   else 
	   toFront(true);
}//end of left click check
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
		 String type(typeid(*c).name());
		 if (c)
         {
                ChildAlias* alias = createAlias (c, type, compIndex++);
				//pass on relative X and Y's to alias components so they are plant aware...
				alias->getProperties().set("plantX", var(c->getProperties().getWithDefault(var::identifier("plantX"), 0)));
                alias->getProperties().set("plantY", var(c->getProperties().getWithDefault(var::identifier("plantY"), 0)));				
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

ChildAlias* ComponentLayoutEditor::createAlias (Component* child, String type, int index)
{
   return new ChildAlias (child, type, index);
}
