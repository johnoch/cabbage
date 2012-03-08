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
void Table::setOriginalWidth(int w)
{
	//Sets the original width when the table was initialised
	origWidth = w;
	maxOverviewZoom = tblSize / origWidth;
	drawCachedImage();
}

//====== Auto add handles ============================================
void Table::AddHandles()
{
//	for(float i=0;i<tblSize;i+=100)
//		addHandle((i/(float)tblSize)*getWidth(), ampValues.y[i]);
}

//====== Fill Tables =================================================
void Table::fillTables (Array<float> yValues)
{
	ampValues.amps.clear();
	ampValues.y.clear();
	ampValues.amps = yValues;
	overviewMax.clear();
	overviewMin.clear();

	//----- Getting the min and max amplitude values....
	minAmp = maxAmp = yValues.getFirst();
	for (int i=0; i<tblSize; i++) {
		if (yValues[i] > maxAmp)
			maxAmp = yValues[i];
		if (yValues[i] < minAmp)
			minAmp = yValues[i];
	}

	ampRange = maxAmp - minAmp; //amp range
			
	//----- Filling the overview table....
	float incr = (float)tblSize / (getWidth()*maxOverviewZoom);
	float maxBlockValue, minBlockValue;
	for (float i=0; i<tblSize; i+=incr) { 
		maxBlockValue = minBlockValue = yValues[(int)i+0.5];
		for (int j=0; j<(int)incr+0.5; j++) { //loop for current block
			if (yValues[(int)i+j+0.5] > maxBlockValue) 
				maxBlockValue = yValues[(int)i+j+0.5];
			if (yValues[(int)i+j+0.5] < minBlockValue) 
				minBlockValue = yValues[(int)i+j+0.5]; 
		}
		overviewMax.add (maxBlockValue);
		overviewMin.add (minBlockValue);
	}
	calculateBlockAmps (zoom);
}

//==========================================================================
void Table::calculateBlockAmps (int zoomValue)
{
	//----- If zoom value is less than the max overview zoom then we use the
	//overview table to get the amp values. Otherwise we have to access the 
	//initial table.

	if (zoomValue <= maxOverviewZoom) {
		int incrSize = maxOverviewZoom/zoomValue;
		float maxBlockValue, minBlockValue;
		blockMax.clear();
		blockMin.clear();
		for (int i=0; i<overviewMax.size(); i+=incrSize) {
			maxBlockValue = overviewMax[i];
			minBlockValue = overviewMin[i];
			for (int j=0; j<incrSize; j++) { //loop for current block
				if (overviewMax[i+j] > maxBlockValue) 
					maxBlockValue = overviewMax[i+j];
				if (overviewMin[i+j]  < minBlockValue) 
					minBlockValue = overviewMin[i+j]; 
			}
			float yMaxNorm = (maxBlockValue-minAmp) / ampRange; //normalising
			float yMinNorm = (minBlockValue-minAmp) / ampRange;
			//inverting y values as y axis is upside down.  The value is then added 
			//to the array
			float yInvertMax = 1 - yMaxNorm; 
			blockMax.add (yInvertMax*getHeight());
			float yInvertMin = 1 - yMinNorm;
			blockMin.add (yInvertMin*getHeight());
			ampValues.y.add(yInvertMin*getHeight());
			useOverview = true;
		}
	}
	//----- Else we won't be using the overview array for painting
	else {
		pixPerIndx = (zoomValue*origWidth) / tblSize;
		useOverview = false;
	}
	AddHandles();
	//----- Repainting
	repaint (0, 0, getWidth(), getHeight());
}

//======= Cache Image ====================================================
void Table::drawCachedImage()
{
	// Creating a blank canvas
	img = Image::Image(Image::ARGB, origWidth, getHeight(), true);
	Graphics g (img);
		
	//----- For drawing the actual canvas area
	Colour canvasClr = CabbageUtils::backgroundSkin();
	g.setColour (canvasClr);
	g.fillRoundedRectangle (0, 0, origWidth, getHeight(), 5);

	g.setColour (Colour::fromRGBA (220, 220, 240, 255));
	g.drawLine (0, getHeight()*0.5, origWidth, getHeight()*0.5, 0.2);
	g.setColour (Colour::fromRGBA (170, 170, 190, 255));
	g.drawLine (0, getHeight()*0.25, origWidth, getHeight()*0.25, 0.1);
	g.drawLine (0, getHeight()*0.75, origWidth, getHeight()*0.75, 0.1);
			
	//----- Adding image to cache and assigning it a hash code
	ImageCache::addImageToCache (img, 15);
}

//====== Paint method ============================================================
void Table::paint (Graphics& g)
{
	float viewStart, viewWidth;
	//----- Getting viewport's coordinates...
	Viewport* const viewport = this->findParentComponentOfClass ((Viewport*) nullptr);
	if(viewport){
	viewStart = viewport->getViewPositionX();
	viewWidth = viewport->getViewWidth();
	}
	//----- Background image cache
	Image bg = ImageCache::getFromHashCode(15);
	g.drawImage (bg, viewStart, 0, viewWidth, getHeight(), 0, 0, viewWidth, bg.getHeight(), false);
	
	g.setColour (cl); 
			
	float minVal, maxVal;
	int startIndx, endIndx;

	//----- Using overview array for painting...
	if (useOverview == true) {
		startIndx = (viewStart/getWidth()) * blockMax.size();
		endIndx = startIndx + viewWidth;

		//----- For loop which will draw a vertical line between the min and max value 
		//for each x pixel 
		float x=0; //represents the pixel number along the x-axis
		for (int i=startIndx; i<endIndx; i++) {
			//We need to make sure that the current min value is not greater than the 
			//next max value. Otherwise there would be a gap in the wave form...
			if (blockMin[i] < blockMax[i+1])
				minVal = blockMax[i+1];
			else
				minVal = blockMin[i];
			if (blockMax[i] > blockMin[i+1])
				maxVal = blockMin[i+1];
			else
				maxVal = blockMax[i];
			
			if(IsNumber(maxVal)&&IsNumber(minVal)){
					g.drawLine (x+viewStart, maxVal, x+viewStart, minVal, 1);
					x+=1;
					}
		}	
	}
	//----- Using original array values for painting...
	else if (useOverview == false) {
		startIndx = (viewStart/getWidth()) * ampValues.amps.size();
		endIndx = startIndx + (viewWidth/pixPerIndx);
		float prevX = viewStart;
		float prevY = getHeight()/2;
		float yNorm, yInvert;
		for (int i=startIndx; i<endIndx; i++) {
			yNorm = (ampValues.amps[(int)i+0.5] - minAmp) / ampRange; //normalising
			yInvert = 1-yNorm; //inverting
			g.setColour (cl);
			g.drawLine (prevX, prevY, prevX+pixPerIndx, yInvert*getHeight());
			
			// For drawing balls to mark indices
			if (pixPerIndx > 4) {
				g.setColour (Colours::aqua);
				g.fillEllipse ((prevX+pixPerIndx)-2, (yInvert*getHeight())-2, 4, 4);
				//g.drawLine (prevX+pixPerIndx, yNorm*getHeight()-5, prevX+pixPerIndx, yNorm*getHeight()+5);
			}
			prevX = prevX + pixPerIndx;
			prevY = yInvert * getHeight();
		}
	}



	Path path;
	for(int i = 0; i < handles.size(); i++) {
		CabbageEnvelopeHandleComponent* handle = handles.getUnchecked(i);
		if(handle->getProperties().getWithDefault(String("shape"), T("")).equals(T("linear")))
		{	//for linear envelopes
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
		//for bezier/expon envelopes
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
}

/*===== Mouse Wheel ===================================================
void mouseWheelMove (const MouseEvent& e, float incrX, float incrY)
{
	if (incrY > 0)
		zoom *= 2;
	else
		zoom /= 2;

	if (zoom > 32)
		zoom = 32;
	if (zoom < 1)
		zoom = 1;

	this->setBounds (0, 0, origWidth*zoom, getHeight());
	calculateBlockAmps (zoom);
}
*/

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
			if (pixPerIndx <= 50) {
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
		calculateBlockAmps (zoom);		
	}

	//----- With shift button down a new envelope handle is added. No zoom.
	else if (e.mods.isShiftDown() == true)
		draggingHandle = addHandle (e.x, e.y);
}

//======= Modify position of handles after zoom =================
void Table::modifyHandlePos (float j)
{
	//----- This function changes the x position of each envelope handle
	//after a zoom in or out. j will either be 2 or 0.5.
	for (int i=0; i<handles.size(); i++) {
		CabbageEnvelopeHandleComponent* handle = handles.getUnchecked (i);
		handle->setBounds (handle->getX() * j, handle->getY(), HANDLESIZE, HANDLESIZE);
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
void CabbageTableManager::fillTable (int tableID, Array<float> yValues)
{
	tables[tableID]->fillTables (yValues);
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
	tables[tableIndex]->setAlpha (0.75);
	this->setViewedComponent (tables[tableIndex], false);

	//tableManager->tableToFront (tableOnTop);
}

void CabbageTableViewer::fillTable (int tableIndex, Array<float> yValues)
{
	tables[tableIndex]->fillTables (yValues);
	//tableManager->fillTable (tableID, yValues);
}




	








