#ifndef __CABBAGETABLE_H_
#define __CABBAGETABLE_H_

#include "../JuceLibraryCode/JuceHeader.h"
#include "CabbageUtils.h"

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

	Data Structures to hold table data

  ====================================================================================
*/
class TableData
{
public:
	Array<float> amps, x, y;
};

class OverviewData
{
public:
	Array<float> minAmps, maxAmps, minY, maxY;
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

	void resized();
	void setOriginalWidth(int w);
	void createAmpOverviews (Array<float> csndInputData);
	void setDataSource (int zoomValue);
	float convertAmpToPixel (float ampValue);
	void cacheBackgroundImage();
	void paint (Graphics& g);
	void mouseDown (const MouseEvent& e);
	CabbageEnvelopeHandleComponent* addHandle(int x, int y);
	void removeHandle (CabbageEnvelopeHandleComponent* thisHandle);
	void modifyHandlePos (float j);
	void makeTableEditable();
	void createEnvPath();

private:
	Image img;
	int gen, tblSize;
	float tableTop, tableBottom, tableLeft, tableHeight;
	float minAmp, maxAmp, ampRange;
	float zoom;
	TableData tableData;
	OverviewData overview;
	Colour cl;
	int origWidth;
	bool useOverview;
	float maxZoomForOverview, numPixelsPerIndex;

	OwnedArray<CabbageEnvelopeHandleComponent> handles;
	CabbageEnvelopeHandleComponent* draggingHandle;
	Path envPath;


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
	void fillTable (int tableID, Array<float> csndInputData);
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
	void fillTable (int tableID, Array<float> csndInputData);
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