#ifndef __MainPanel_H_AFF8CBED__
#define __MainPanel_H_AFF8CBED__

#include "../JuceLibraryCode/JuceHeader.h"

class ComponentLayoutEditor;

// Whenever we select a group of components this array is populated.
//-----------------------------------------------------------------------------
class SelectedComponents   : public SelectedItemSet<Component*>
{
public:
	void itemSelected (Component* item){
		item->repaint ();
	}
	void itemDeselected (Component* item){
		item->repaint ();
	}
};

//-----------------------------------------------------------------------------
class MainPanel   : public Component,
					public LassoSource <Component*>,
					public ChangeListener
{
public:
	int indexOfCurrentComponent;
	MainPanel();
	~MainPanel();
	void mouseDown(const MouseEvent &e);
	void mouseDrag(const MouseEvent &e);
	void mouseUp(const MouseEvent &e);
	void findLassoItemsInArea (Array <Component*>& results, const Rectangle<int>& area);
	SelectedItemSet <Component*>& getLassoSelection();
	SelectedComponents selectedComps;
	LassoComponent <Component*> lassoComp;
	ComponentLayoutEditor* getLayoutEditor();
	void removeComponents(int index);
	void cutcopyclearComponents(int index, String type);
	void pasteComponents();
	void resetAndUpdate();
	int getNumComponents();
	Component* getComponent(int index);
	void changeListenerCallback(ChangeBroadcaster* sender);
	void showProps(bool show, Component* comp);
	void setLayoutEditor(ComponentLayoutEditor* ed){
	editor = ed;
	} 

private:
	ComponentLayoutEditor* editor;
	OwnedArray<Label > labels;
	OwnedArray<Label > clipBoard;
	bool LOCKED;

};


#endif //__MainPanel_H_AFF8CBED__
