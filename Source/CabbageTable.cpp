#include "CabbageTable.h"
#include "CabbageUtils.h"

/*
  ====================================================================================

	Cabbage Envelope Handle class

  ====================================================================================
*/
CabbageEnvelopeHandleComponent::CabbageEnvelopeHandleComponent()
{
}

CabbageEnvelopeHandleComponent::~CabbageEnvelopeHandleComponent()
{
}

//==================================================================
void CabbageEnvelopeHandleComponent::paint (Graphics& g)
{
	g.setColour (Colours::aqua);
	g.fillEllipse (1, 1, getWidth()-2, getHeight()-2);
}

//==================================================================
Table* CabbageEnvelopeHandleComponent::getParentComponent()
{
	return (Table*)Component::getParentComponent();
}
//==================================================================
void CabbageEnvelopeHandleComponent::removeThisHandle()
{
	getParentComponent()->removeHandle (this);	
}

//==================================================================
void CabbageEnvelopeHandleComponent::mouseEnter (const MouseEvent& e)
{
	setMouseCursor (MouseCursor::DraggingHandCursor);
}

//==================================================================
void CabbageEnvelopeHandleComponent::mouseDown (const MouseEvent& e)
{
	//setMouseCursor (MouseCursor::NoCursor);
	setMouseCursor (MouseCursor::DraggingHandCursor);
	dragger.startDraggingComponent (this, e);

	if ((e.mods.isShiftDown() == true) && (e.mods.isRightButtonDown() == true))
		removeThisHandle();

	PopupMenu pop;
	if(e.mods.isRightButtonDown() == true)
	{
		pop.addItem(1, "Linear");
		pop.addItem(2, "Expon");
		const int result = pop.show();
		if(result==1)
			getProperties().set(String("shape"), var(T("linear")));
		else 
			getProperties().set(String("shape"), var(T("expon")));
	}
}

//==================================================================
void CabbageEnvelopeHandleComponent::mouseDrag (const MouseEvent& e)
{
	dragger.dragComponent (this, e, nullptr);
	getParentComponent()->repaint();
}


/*
  ====================================================================================

	Table class

  ====================================================================================
*/
Table::Table (int tableSize, Colour colour) : tblSize(tableSize), zoom(1.0), cl (colour),
																draggingHandle (0)
{
}

Table::~Table()
{
}

//====================================================================
void Table::resized()
{
	//We need to make room for the h scrollbar, therefore table data
	//can't use the full height of the canvas
	tableTop = getHeight()*0.15;
	tableBottom = getHeight()*0.85;
	tableHeight = tableBottom - tableTop;
}

//====================================================================
void Table::setOriginalWidth(int w)
{
	//Sets the original width from when the table was initialised
	origWidth = w; 
	maxZoomForOverview = tblSize / origWidth;
	backgroundImage();
}

//====== Creates Overview arrays ======================================
void Table::createAmpOverviews (Array<float> csndInputData)
{
	//This function creates smaller overview arrays amp values using the
	//original table data...
	tableData.amps.clear();
	tableData.y.clear();
	overview.maxAmps.clear();
	overview.minAmps.clear();

	tableData.amps = csndInputData;
	
	//----- Getting the min and max amplitude values....
	minAmp = maxAmp = tableData.amps.getFirst();
	for (int i=0; i<tblSize; i++) {
		if (tableData.amps[i] > maxAmp)
			maxAmp = tableData.amps[i];
		if (tableData.amps[i] < minAmp)
			minAmp = tableData.amps[i];
	}

	ampRange = maxAmp - minAmp; 

	//----- Filling overview arrays. The original table data is broken into
	//separate blocks. The max and min values are then stored from each block.
	//Block size is dependent on the max zoom level we use for overviews.
	float incr = (float)tblSize / (getWidth()*maxZoomForOverview);
	float maxBlockValue, minBlockValue;
	for (float i=0; i<tblSize; i+=incr) { 
		maxBlockValue = minBlockValue = tableData.amps[(int)i+0.5];
		for (int j=0; j<(int)incr+0.5; j++) { //loop for current block
			if (tableData.amps[(int)i+j+0.5] > maxBlockValue) 
				maxBlockValue = tableData.amps[(int)i+j+0.5];
			if (tableData.amps[(int)i+j+0.5] < minBlockValue) 
				minBlockValue = tableData.amps[(int)i+j+0.5]; 
		}
		overview.maxAmps.add (maxBlockValue);
		overview.minAmps.add (minBlockValue);
	}
	setDataSource (zoom);
	createEnvPath();
}

//==============================================================
void Table::setDataSource (int zoomValue)
{
	//If current zoom <= max zoom for overview, then the 
	//overview arrays are used and converted to y coordinates.
	//Otherwise the initial table data is used.

	if (zoomValue <= maxZoomForOverview) { 
		int incrSize = maxZoomForOverview/zoomValue;
		float maxBlockValue, minBlockValue;
		
		overview.maxY.clear();
		overview.minY.clear();
		for (int i=0; i<overview.maxAmps.size(); i+=incrSize) {
			maxBlockValue = overview.maxAmps[i];
			minBlockValue = overview.minAmps[i];
			for (int j=0; j<incrSize; j++) { //loop for current block
				if (overview.maxAmps[i+j] > maxBlockValue) 
					maxBlockValue = overview.maxAmps[i+j];
				if (overview.minAmps[i+j]  < minBlockValue) 
					minBlockValue = overview.minAmps[i+j]; 
			}
		
			overview.maxY.add (ampToYCoordinate(maxBlockValue));
			overview.minY.add (ampToYCoordinate(minBlockValue));
			
			useOverview = true;
		}
		//makeTableEditable();
	}
	//----- Else we use original table data for painting
	else {
		pixelsPerIndx = (zoomValue*origWidth) / (float)tblSize;
		useOverview = false;
	}

	repaint();
}

//====== For converting amps to y pixel values ==================
float Table::ampToYCoordinate (float ampValue)
{
	float ampNormalise = (ampValue-minAmp) / ampRange;
	float ampInvert = 1-ampNormalise;
	float yValue  = (ampInvert * tableHeight) + tableTop;
	return yValue;
}

//======= Create background image ========================
void Table::backgroundImage()
{
	//This method creates the background image that the table data will be
	//painted on to.  It is then stored in cache.

	img = Image::Image(Image::ARGB, origWidth, getHeight(), true);
	Graphics g (img);
		
	//----- For drawing the actual canvas area
	//g.setColour (CabbageUtils::backgroundSkin());
	g.setColour (Colours::black);
	g.fillRoundedRectangle (0, 0, getWidth(), getHeight(), 5);

	//----- Amp horizontal markers...
	g.setColour (Colour::fromRGBA (220, 220, 240, 255));
	g.drawLine (0, getHeight()*0.5, getWidth(), getHeight()*0.5, 0.2);
	g.setColour (Colour::fromRGBA (170, 170, 190, 255));
	g.drawLine (0, (tableHeight*0.25)+tableTop, getWidth(), (tableHeight*0.25)+tableTop, 0.1);
	g.drawLine (0, (tableHeight*0.75)+tableTop, getWidth(), (tableHeight*0.75)+tableTop, 0.1);
			
	//----- Adding image to cache and assigning it a hash code
	ImageCache::addImageToCache (img, 15);
}

//====== Paint method =======================================================
void Table::paint (Graphics& g)
{

	float viewStart, viewWidth;
	//----- Getting viewport's coordinates...
	Viewport* const viewport = this->findParentComponentOfClass ((Viewport*) nullptr);

	if (viewport) {
		viewStart = viewport->getViewPositionX();
		viewWidth = viewport->getViewWidth();
	}

	//----- Background image cache
	Image bg = ImageCache::getFromHashCode(15);
	g.drawImage (bg, viewStart, 0, origWidth, getHeight(), 
		0, 0, bg.getWidth(), bg.getHeight(), false);
	
	g.setColour (cl);
	int startIndx;
	int endIndx;

	//---- If using overview...
	if (useOverview == true) {
		startIndx = (viewStart/getWidth()) * overview.maxY.size();
		endIndx = startIndx + viewWidth;
		float minVal, maxVal;
		minVal = maxVal = 0;

		//----- For loop which will draw a vertical line between the min and max value 
		//for each x pixel 
		int x=0; //represents the pixel number along the x-axis
		for (int i=startIndx; i<endIndx; i++) {
			//We need to make sure that the current min value is not greater than the 
			//next max value. Otherwise there would be a gap in the wave form...
			if (overview.minY[i] < overview.maxY[i+1])
				minVal = overview.maxY[i+1];
			else
				minVal = overview.minY[i];
			if (overview.maxY[i] > overview.minY[i+1])
				maxVal = overview.minY[i+1];
			else
				maxVal = overview.maxY[i];
			
			if (IsNumber(maxVal) && IsNumber(minVal)) {
				//g.drawLine (x+viewStart, maxVal, x+viewStart, minVal, 1);
				g.drawVerticalLine (x+viewStart, maxVal, minVal);
				x+=1;//.0f;
			}
		}
	}
	
	//----- Using original array values for painting...
	else if (useOverview == false) {
		startIndx = ((viewStart/getWidth()) * tableData.amps.size()) + 0.5; //0.5 for rounding
		endIndx = (startIndx + (viewWidth/pixelsPerIndx)) + 0.5; 
		float prevX = viewStart;
		float prevY = ampToYCoordinate (tableData.amps[startIndx]);
		float currY;
		for (int i=startIndx+1; i<=endIndx; i++) {
			currY = ampToYCoordinate (tableData.amps[i]);
			g.drawLine (prevX, prevY, prevX+pixelsPerIndx, currY);
			// For drawing index markers
			if (pixelsPerIndx > 4) {
				//g.setColour (Colours::aqua);
				//g.fillEllipse ((prevX+pixelsPerIndx)-2, currY-2, 4, 4);
				g.drawVerticalLine (prevX+pixelsPerIndx, currY-3, currY+3);
				//g.setColour(cl);
			}
			prevX = prevX + pixelsPerIndx;
			prevY = currY;
		}
	}

	g.setColour(Colours::lightblue);
	envPath.scaleToFit(0, 0, getWidth(), getHeight(), false);
	g.strokePath (envPath, PathStrokeType(2.0f));
	//----- For handles....
	/*
	Path path;
	for(int i = 0; i < handles.size(); i++) {
		CabbageEnvelopeHandleComponent* handle = handles.getUnchecked(i);
		if(handle->getProperties().getWithDefault(String("shape"), T("")).equals(T("linear")))
		{	
		//for linear envelopes
		if(i==0) {
			path.startNewSubPath((handle->getX() + handle->getRight()) / 2,
									 (handle->getY() + handle->getBottom()) / 2);
			} else {
				path.lineTo((handle->getX() + handle->getRight()) / 2,
							(handle->getY() + handle->getBottom()) / 2);
			}
		}
		else
		{
		//for bezier envelopes
		if(i==0){
			path.startNewSubPath((handle->getX() + handle->getRight()) / 2,
								 (handle->getY() + handle->getBottom()) / 2);
		} else {
			int curX = handle->getX();
			int curY = handle->getY();
			int prevX = path.getCurrentPosition().getX();
			int prevY = path.getCurrentPosition().getY();
			if(curY>prevY)
			path.quadraticTo(prevX, curY, 
							(curX + handle->getRight()) / 2,
							(curY + handle->getBottom()) / 2);
			else if(curY<prevY){
			path.quadraticTo(curX, prevY,
							(curX + handle->getRight()) / 2,
							(curY + handle->getBottom()) / 2);
			}
		}
		}
		//String coordinate;
		//coordinate = String(handle->getX()) + T(", ") + String(handle->getY());
		//g.setColour(Colours::lime);
		//g.drawFittedText(coordinate, handle->getX(), handle->getY(), 50, 20, Justification::centred, 1); 
		
		g.setColour(Colours::lightblue);
		g.strokePath (path, PathStrokeType(2.0f));
	}
	*/
}

//====== Mouse Down ==============================================
void Table::mouseDown (const MouseEvent& e)
{
	//----- If no shift button then it's a zoom in or out.
	if (e.mods.isShiftDown() == false) {
		Viewport* const viewport = this->findParentComponentOfClass ((Viewport*) nullptr);
		float viewStart = viewport->getViewPositionX();
		float x = e.getPosition().getX();
		float diff = x-viewStart;			

		//left mouse button for zooming in
		if (e.mods.isLeftButtonDown() == true) { 
			if (pixelsPerIndx <= 50) {
				zoom *= 2;
				x *= 2;
				if (handles.size() > 0) //if envelope handles
					modifyHandlePos (2);
			}
		}
		//right mouse button for zooming out
		else if (e.mods.isRightButtonDown() == true) {
			if (zoom != 1) {
				zoom /= 2;
				x /= 2;
				if (handles.size() > 0) //if envelope handles
					modifyHandlePos (0.5);
			}
		}
			
		this->setBounds (0, 0, origWidth*zoom, getHeight());
		viewport->setViewPosition (x-(diff), 0);
		setDataSource (zoom);		
	}

	//----- With shift button down a new envelope handle is added. No zoom.
	else if (e.mods.isShiftDown() == true)
		draggingHandle = addHandle (e.x, e.y);
}

//======= Modify position of handles after zoom ======================
void Table::modifyHandlePos (float j)
{
	//This function changes the x position of each envelope handle
	//after a zoom in or out. j will either be 2 or 0.5.
	for (int i=0; i<handles.size(); i++) {
		CabbageEnvelopeHandleComponent* handle = handles.getUnchecked (i);
		handle->setBounds (handle->getX() * j, handle->getY(), HANDLESIZE, HANDLESIZE);
	}
}

//====== creates a scalable vector from our ftable waveform =============
void Table::createEnvPath()
{
envPath.startNewSubPath(0, ampToYCoordinate(tableData.amps[0]));
//create a vector path based on the ftable waveform
//the path will always be tblSize pixel wide. 
for(int i=0;i<tblSize;i++)
		envPath.lineTo(i, ampToYCoordinate(tableData.amps[i]));
}

//====== Add handles for editing table ==================================
void Table::makeTableEditable()
{
	handles.clear();
	float avg;
	int arrSize = overview.maxY.size();
	for (int i=0; i<arrSize; i+=arrSize/5) {
		avg = (overview.minY[i]-overview.maxY[i]) / 2;
		addHandle (i, (int)(overview.maxY[i]+avg+0.5));
	}
}

//========= Add Handle ===========================================
CabbageEnvelopeHandleComponent* Table::addHandle(int x, int y)
{
	int i;
	for (i=0; i<handles.size(); i++) {
		CabbageEnvelopeHandleComponent* handle = handles.getUnchecked (i);
		if (handle->getX() > x)
			break;
	}

	CabbageEnvelopeHandleComponent* handle = new CabbageEnvelopeHandleComponent();
	handle->setBounds (x, y, HANDLESIZE, HANDLESIZE);
	addAndMakeVisible (handle);
	handles.insert (i, handle);
	repaint();
	return handle;
}

//========= Remove Handle ===========================================
void Table::removeHandle (CabbageEnvelopeHandleComponent* thisHandle)
{
	if (handles.size() > 0) {
		//handles.removeObject (thisHandle, true);
		handles.removeObject(thisHandle, true);
		repaint();
	}
	delete thisHandle;
}


/*
  ====================================================================================

	Table Manager class

  ====================================================================================

CabbageTableManager::CabbageTableManager()
{
}

CabbageTableManager::~CabbageTableManager()
{
}

//======= Add table =========================================================
void CabbageTableManager::addTable (String name, int tableSize, Colour colour)
{
	int i = tables.size();
	tables.add (new Table(tableSize, colour));

	tables[i]->setBounds (getX(), getY(), getWidth(), getHeight());
	tables[i]->setOriginalWidth (getWidth());
	addAndMakeVisible (tables[i]);
	tableToFront(i); //automatically puts this table to the front
}

//====== Sets desired table to the front ======================================
void CabbageTableManager::tableToFront (int tableOnTop)
{
	tables[tableOnTop]->toFront(true);
	tables[tableOnTop]->setAlpha (0.7);

	
}

//======== Fill table =========================================================
void CabbageTableManager::fillTable (int tableID, Array<float> csndInputData)
{
	tables[tableID]->createAmpOverviews (csndInputData);
}

void CabbageTableManager::mouseDown (const MouseEvent& e)
{
}
*/

/*
  ====================================================================================

	Table Viewer class

  ====================================================================================
*/
CabbageTableViewer::CabbageTableViewer()
{
	this->setScrollBarsShown (false, true); //only showing the h scrollbar
	

	//tableManager = new CabbageTableManager();
	//view->setInterceptsMouseClicks (true, false);
	
}

CabbageTableViewer::~CabbageTableViewer()
{
}

void CabbageTableViewer::resized()
{
	//tableManager->setBounds (getX(), getY(), getWidth(), getHeight());
	//this->setViewedComponent (tableManager, false);
}

void CabbageTableViewer::addTable (String name, int tableSize, Colour colour)
{
	//tableManager->addTable (name, tableSize, colour);

	int i = tables.size();

	tables.add (new Table(tableSize, colour));
	tables[i]->setBounds (getX(), getY(), getWidth(), getHeight());
	tables[i]->setOriginalWidth (getWidth());
	tableToFront (i);	
}

void CabbageTableViewer::tableToFront (int tableIndex)
{
	tables[tableIndex]->toFront (true);
	//tables[tableIndex]->setAlpha (0.75);
	this->setViewedComponent (tables[tableIndex], false);

	//tableManager->tableToFront (tableOnTop);
}

void CabbageTableViewer::fillTable (int tableIndex, Array<float> csndInputData)
{
	tables[tableIndex]->createAmpOverviews (csndInputData);
	//tableManager->fillTable (tableID, csndInputData);
}




	








