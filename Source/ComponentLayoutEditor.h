/*
*  ComponentLayoutEditor.h
*  
*  Original written by Haydxn
*  Modified by Jordan Hochenbaum on 10/25/10.
*  http://www.rawmaterialsoftware.com/viewtopic.php?f=6&t=2635
*
*/

#ifndef _COMPONENTLAYOUTEDITOR_H_
#define _COMPONENTLAYOUTEDITOR_H_


#include "../JuceLibraryCode/JuceHeader.h"


class MainPanel;
class ComponentLayoutEditor;
class SelectedComponents;

//=============================================================================
class ChildAlias   :   public Component
    {
    public:
      ChildAlias (Component* targetChild, int ID);
      ~ChildAlias ();
      
      void resized ();
      void paint (Graphics& g);
      
      const Component* getTargetChild ();
	  ComponentLayoutEditor* getParent();
	  MainPanel* getTarget();
      
      void updateFromTarget ();
      void applyToTarget ();
      
      virtual void userChangedBounds ();
      virtual void userStartedChangingBounds ();
      virtual void userStoppedChangingBounds ();
      
      bool boundsChangedSinceStart ();
      
      void mouseEnter (const MouseEvent& e);
      void mouseExit (const MouseEvent& e);
      void mouseDown (const MouseEvent& e);
      void mouseUp (const MouseEvent& e);
      void mouseDrag (const MouseEvent& e);
   
    private:
      
      CriticalSection bounds;
	  Array<Rectangle<int> > coordinates;
      ComponentBoundsConstrainer*  constrainer;
      ComponentDragger dragger;
      SafePointer<Component> target;
      bool interest;
      bool userAdjusting;
      Rectangle<int> startBounds;
      ComponentBoundsConstrainer* resizeContainer; //added resizeContainer to limit resizing sizes
      ResizableBorderComponent* resizer;
    };

//=============================================================================
class ComponentLayoutEditor   :   public Component
    {
    public:
      
      enum ColourIds
      {
         aliasIdleColour,
         aliasHoverColour
      };
      
      ComponentLayoutEditor ();
      ~ComponentLayoutEditor ();
      
      void resized ();
      void paint (Graphics& g);
      
      void setTargetComponent (Component* target);
      
      void bindWithTarget ();
      void updateFrames ();

      void enablementChanged ();
      const Component* getTarget ();
	  ChildAlias* getFrame(int index);
	  void removeFrame(int index);
	  int compID;

	  
    
	private:
      
      virtual ChildAlias* createAlias (Component* child, int compID);
      SafePointer<Component> target;
	  OwnedArray<ChildAlias> frames;
      
	  
	  

      
    };

#endif//_COMPONENTLAYOUTEDITOR_H_
