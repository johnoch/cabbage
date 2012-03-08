#ifndef __CABBAGETABLE_H_
#define __CABBAGETABLE_H_

#include "../JuceLibraryCode/JuceHeader.h"

#define HANDLESIZE 8

/*
  ====================================================================================

	Cabbage Envelope Handle

	Each envelope handle is a component.

  ====================================================================================
*/
class CabbageEnvelopeHandleComponent : public Component
{
public:
	CabbageEnvelopeHandleComponent();
	~CabbageEnvelopeHandleComponent();

	class Table* getParentComponent();
	void paint (Graphics& g);
	void removeThisHandle();
	void mouseEnter (const MouseEvent& e);
	void mouseDown (const MouseEvent& e);
	void mouseDrag (const MouseEvent& e);

private:
	ComponentDragger dragger;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CabbageEnvelopeHandleComponent);
};


/*
  ====================================================================================

	Table

  ====================================================================================
*/
class Table : public Component,
	public ActionBroadcaster
{
public:
	Table (int tableSize, Colour colour);
	~Table();

	void setOriginalWidth(int w);
	void fillTables (Array<float> yValues);
	void calculateBlockAmps (int zoomValue);
	void drawCachedImage();
	void paint (Graphics& g);
	void mouseDown (const MouseEvent& e);
	CabbageEnvelopeHandleComponent* addHandle(int x, int y);
	void removeHandle (CabbageEnvelopeHandleComponent* thisHandle);
	void modifyHandlePos (float j);
	void AddHandles();

private:

	class tableValues{
	public:
		Array<float> amps, x, y;
	};

	Image img;
	int gen, tblSize;
	float minAmp, maxAmp, ampRange;
	float zoom;
	Array<float> blockMax, blockMin;
	Array<float> overviewMax, overviewMin;
	tableValues ampValues;
	Colour cl;
	int origWidth;
	bool useOverview;
	float maxOverviewZoom, pixPerIndx;

	bool IsNumber(double x) 
    {
		// This looks like it should always be true, 
        // but it's false if x is a NaN.
        return (x == x); 
    }

	OwnedArray<CabbageEnvelopeHandleComponent> handles;
	CabbageEnvelopeHandleComponent* draggingHandle;


	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Table);
};

/*
  ====================================================================================

	Table Manager 

	This class superimposes tables on top of one another, turning them into one 
	component

  ====================================================================================

class CabbageTableManager : public Component
{
public:
	CabbageTableManager();
	~CabbageTableManager();

	void addTable (String name, int tableSize, Colour colour);
	void fillTable (int tableID, Array<float> yValues);
	void tableToFront (int tableOnTop);
	void mouseDown (const MouseEvent& e);

private:
	OwnedArray<Table> tables;
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CabbageTableManager);
};
*/

/*
  ====================================================================================

	Table Viewer

	This class provides a viewport which allows for scrolling. It views one 
	CabbageTableManager object.

  ====================================================================================
*/
class CabbageTableViewer : public Viewport
{
public:
	CabbageTableViewer();
	~CabbageTableViewer();
	void resized();
	void addTable (String name, int tableSize, Colour colour);
	void fillTable (int tableID, Array<float> yValues);
	void tableToFront (int tableOnTop);

private:
	//ScopedPointer<CabbageTableManager> tableManager;
	OwnedArray<Table> tables;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CabbageTableViewer);
};



/*
//==============================================================================
// custom CabbageTable, uses the Table class
//==============================================================================
class CabbageTable : public Component
{
	//ScopedPointer<LookAndFeel> lookFeel;
	int offX, offY, offWidth, offHeight;
public:
	ScopedPointer<GroupComponent> groupbox;
	ScopedPointer<CabbageTableViewer> table;
	//---- constructor -----
	CabbageTable (String name, String text, String caption, int tableSize, Colour colour)
	{
		setName(name);
		offX=offY=offWidth=offHeight=0;
	
		groupbox = new GroupComponent(String("groupbox_")+name);
		groupbox->setWantsKeyboardFocus(false);
		table = new CabbageTableViewer(name, tableSize, colour);
		addAndMakeVisible(table);
		addAndMakeVisible(groupbox);

		groupbox->setVisible(false);
		//outline colour ID
		groupbox->setColour(0x1005400,
			Colours::findColourForName("white", Colours::white));
		//text colour ID
		groupbox->setColour(0x1005410,
			Colours::findColourForName("white", Colours::white));
	
		if(caption.length()>0){
			offX=10;
			offY=15;
			offWidth=-20;
			offHeight=-25;
			groupbox->setVisible(true);
			groupbox->setText(caption);
		}
		this->setWantsKeyboardFocus(false);
	}
	//---------------------------------------------
	~CabbageTable()
	{
	}

	//---------------------------------------------
	void resized()
	{
		groupbox->setBounds(0, 0, getWidth(), getHeight()); 
		table->setBounds(offX, offY, getWidth()+offWidth, getHeight()+offHeight); 
		this->setWantsKeyboardFocus(false);
	}

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CabbageTable);
};
*/



#endif //__CABBAGETABLE_H_