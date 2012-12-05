#include "CabbageTable.h"
#include "CabbageUtils.h"

/*
  ====================================================================================

	Cabbage Envelope Handle class

  ====================================================================================
*/
CabbageEnvelopeHandleComponent::CabbageEnvelopeHandleComponent()
{
	getProperties().set(String("shape"), var("linear"));
}

CabbageEnvelopeHandleComponent::~CabbageEnvelopeHandleComponent()
{
}

void CabbageEnvelopeHandleComponent::paint (Graphics& g)
{
	g.setColour (Colours::aqua);
	g.fillEllipse (1, 1, getWidth()-2, getHeight()-2);
}

Table* CabbageEnvelopeHandleComponent::getParentComponent()
{
	return (Table*)Component::getParentComponent();
}

void CabbageEnvelopeHandleComponent::removeThisHandle()
{
	getParentComponent()->removeHandle (this);	
}

void CabbageEnvelopeHandleComponent::mouseEnter (const MouseEvent& e)
{
	setMouseCursor (MouseCursor::DraggingHandCursor);
}

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
			getProperties().set(String("shape"), var("linear"));
		else 
			getProperties().set(String("shape"), var("expon"));
	}
}

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
																draggingHandle (0), scrubberPosition(0)
{
}

Table::~Table()
{
}

void Table::resized()
{
	//We need to make room for the h scrollbar, therefore table data
	//can't use the full height of the canvas
	tableTop = getHeight()*0.15;
	tableBottom = getHeight()*0.85;
	tableHeight = tableBottom - tableTop;
}

void Table::setOriginalWidth(int w)
{
	//Sets the original width from when the table was initialised
	origWidth = w; 
	maxZoomForOverview = tblSize / origWidth;
	cacheBackgroundImage();
}

void Table::createAmpOverviews (Array<float> csndInputData)
{
	//This method creates smaller amp overview arrays using the
	//original table data...
	tableData.amps.clear();
	tableData.y.clear();
	overview.maxAmps.clear();
	overview.minAmps.clear();

	tableData.amps = csndInputData;
	
	// Getting the min and max amplitude values....
	minAmp = maxAmp = tableData.amps.getFirst();
	for (int i=0; i<tblSize; i++) {
		if (tableData.amps[i] > maxAmp)
			maxAmp = tableData.amps[i];
		if (tableData.amps[i] < minAmp)
			minAmp = tableData.amps[i];
	}

	//if min and max amps are the same value....
	if (minAmp == maxAmp) {
		if (minAmp > 0)
			minAmp = 0;
		else if (minAmp < 0) {
			maxAmp = 0;
		}
		else { //else if both are 0
			minAmp = -1;
			maxAmp = 1;
		}
	}

	ampRange = maxAmp - minAmp; 

	// Filling overview arrays. The original table data is broken into
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
	//createEnvPath();
	//makeTableEditable();
}

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
		
			overview.maxY.add (convertAmpToPixel(maxBlockValue));
			overview.minY.add (convertAmpToPixel(minBlockValue));
			
			useOverview = true;
		}
	}
	// Else we use original table data for painting
	else {
		numPixelsPerIndex = (zoomValue*origWidth) / (float)tblSize;
		useOverview = false;
	}
	
	repaint();
}

float Table::convertAmpToPixel (float ampValue)
{
	// This method converts amps to y pixel values
	float normalisedAmp = (ampValue-minAmp) / ampRange; //first change to normalised value
	return ((1-normalisedAmp) * tableHeight) + tableTop;
}

void Table::cacheBackgroundImage()
{
	//This method creates the background image that the table data will be
	//painted on to.  It is then stored in cache.
	img = Image(Image::ARGB, origWidth, getHeight(), true);
	Graphics g (img);
		
	g.setColour (Colours::black);
	g.fillRoundedRectangle (0, 0, getWidth(), getHeight(), 5);

	// Amp horizontal markers...
	g.setColour (Colour::fromRGBA (220, 220, 240, 255));
	g.drawLine (0, getHeight()*0.5, getWidth(), getHeight()*0.5, 0.2);
	g.setColour (Colour::fromRGBA (170, 170, 190, 255));
	g.drawLine (0, (tableHeight*0.25)+tableTop, getWidth(), (tableHeight*0.25)+tableTop, 0.1);
	g.drawLine (0, (tableHeight*0.75)+tableTop, getWidth(), (tableHeight*0.75)+tableTop, 0.1);
			
	ImageCache::addImageToCache (img, 15);
}

void Table::paint (Graphics& g)
{
	float viewStart, viewWidth;
	// Getting viewport's coordinates...
	Viewport* const viewport =  this->findParentComponentOfClass<Viewport> ();

	if (viewport) {
		viewStart = viewport->getViewPositionX();
		viewWidth = viewport->getViewWidth();
	}

	// Background image cache
	Image bg = ImageCache::getFromHashCode(15);
	bg.multiplyAllAlphas(.5);
	g.drawImage (bg, viewStart, 0, origWidth, getHeight(), 
		0, 0, bg.getWidth(), bg.getHeight(), false);
	
	g.setColour (cl);
	int startIndx;
	int endIndx;

	// If using overview...
	if (useOverview == true) {
		startIndx = (viewStart/getWidth()) * overview.maxY.size();
		endIndx = startIndx + viewWidth;
		float bottomYPixelValue, topYPixelValue;
		bottomYPixelValue = topYPixelValue = 0;

		// For loop which will draw a vertical line between the min and max value 
		//for each x pixel 
		int xPixelValue = 0; 
		for (int i=startIndx; i<endIndx; i++) {
			//We need to make sure that the current min value is not greater than the 
			//next max value. Otherwise there would be a gap in the wave form...
			if (overview.minY[i] < overview.maxY[i+1])
				bottomYPixelValue = overview.maxY[i+1];
			else
				bottomYPixelValue = overview.minY[i];
			if (overview.maxY[i] > overview.minY[i+1])
				topYPixelValue = overview.minY[i+1];
			else
				topYPixelValue = overview.maxY[i];
			 
			float minGap = 0.4;
			float diff = bottomYPixelValue - topYPixelValue;
			if (diff < minGap) {
				bottomYPixelValue += (minGap-diff)/2;
				topYPixelValue -= (minGap-diff)/2;
			}
			
			if (CabbageUtils::isNumber(topYPixelValue) && CabbageUtils::isNumber(bottomYPixelValue)) {
				g.drawVerticalLine (xPixelValue+viewStart, topYPixelValue, bottomYPixelValue);
				xPixelValue += 1;
			}
		}
	}
	
	//Else if using original array values for painting...
	else if (useOverview == false) {
		startIndx = ((viewStart/getWidth()) * tableData.amps.size()) + 0.5; //0.5 for rounding
		endIndx = (startIndx + (viewWidth/numPixelsPerIndex)) + 0.5; 
		float prevX = viewStart;
		float prevY = convertAmpToPixel (tableData.amps[startIndx]);
		float currY;
		for (int i=startIndx+1; i<=endIndx; i++) {
			currY = convertAmpToPixel (tableData.amps[i]);
			g.drawLine (prevX, prevY, prevX+numPixelsPerIndex, currY);
			// For drawing index markers
			if (numPixelsPerIndex > 4){
				//g.setColour (Colours::aqua);
				//g.fillEllipse ((prevX+numPixelsPerIndex)-2, currY-2, 4, 4);
				g.drawVerticalLine (prevX+numPixelsPerIndex, currY-3, currY+3);
				//g.setColour(cl);
			}
			prevX = prevX + numPixelsPerIndex;
			prevY = currY;
		}
	}

//	g.setColour(Colours::lightblue);
//	envPath.scaleToFit (0, tableTop, getWidth(), tableHeight, false);
//	g.strokePath (envPath, PathStrokeType(2.0f));
	//----- For handles....
	
	Path path;
	for(int i = 0; i < handles.size(); i++) {
		CabbageEnvelopeHandleComponent* handle = handles.getUnchecked(i);
		if(handle->getProperties().getWithDefault(String("shape"), String("")).equals("linear"))
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
		//coordinate = String(handle->getX()) + String(", ") + String(handle->getY());
		//g.setColour(Colours::lime);
		//g.drawFittedText(coordinate, handle->getX(), handle->getY(), 50, 20, Justification::centred, 1); 
		
		g.setColour(Colours::lightblue);
		g.strokePath (path, PathStrokeType(2.0f));
	}
		
	//draw the crubber if needed
	if(scrubberPosition>0){
	g.setColour(Colours::yellow);
	scrubberPosition = scrubberPosition*getWidth();
	g.drawLine(scrubberPosition, 0, scrubberPosition, getHeight(), 2);
	}
}

void Table::mouseDown (const MouseEvent& e)
{
	// If no shift button then it's a zoom in or out.
	if (e.mods.isShiftDown() == false) {
		Viewport* const viewport = this->findParentComponentOfClass<Viewport> ();
		float viewStart = viewport->getViewPositionX();
		float x = e.getPosition().getX();
		float diff = x-viewStart;			

		//left mouse button for zooming in
		if (e.mods.isLeftButtonDown() == true) { 
			if (numPixelsPerIndex <= 50) {
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

	// With shift button down a new envelope handle is added. No zoom.
	else if (e.mods.isShiftDown() == true)
		draggingHandle = addHandle (e.x, e.y);
}

void Table::modifyHandlePos (float j)
{
	//This function changes the x position of each envelope handle
	//after a zoom in or out. j will either be 2 or 0.5.
	for (int i=0; i<handles.size(); i++) {
		CabbageEnvelopeHandleComponent* handle = handles.getUnchecked (i);
		handle->setBounds (handle->getX() * j, handle->getY(), HANDLESIZE, HANDLESIZE);
	}
}

void Table::createEnvPath()
{
	// Creates a scalable vector from our ftable waveform
	envPath.startNewSubPath(0, convertAmpToPixel(tableData.amps[0]));
	//create a vector path based on the ftable waveform
	//the path will always be tblSize pixel wide. 
	for(int i=0;i<tblSize;i++)
		envPath.lineTo(i, convertAmpToPixel(tableData.amps[i]));

	envPath.scaleToFit(0, tableTop, getWidth(), tableHeight, false);
}

void Table::makeTableEditable()
{
	// Adding handles to make table editable...	
	handles.clear();
	double angle, prevAngle;
	prevAngle = atan2(envPath.getPointAlongPath(2).getY() - envPath.getPointAlongPath(1).getY(), 
		envPath.getPointAlongPath(2).getX() - envPath.getPointAlongPath(1).getX()) * 180 / 3.14;	
	//add first handle
	addHandle (envPath.getPointAlongPath(1).getX(), envPath.getPointAlongPath(1).getY());

	for(int i=2; i<envPath.getLength(); i++){
		angle = atan2(envPath.getPointAlongPath(i).getY() - envPath.getPointAlongPath(i-1).getY(), 
			envPath.getPointAlongPath(i).getX() - envPath.getPointAlongPath(i-1).getX()) * 180 / 3.14;	
		//String msg;
		//msg << "i:" << i << " Path(" << envPath.getPointAlongPath(i).getX() << ", " << envPath.getPointAlongPath(i).getY() << ")" 
		//<< " Direction:" << angle;
		//Logger::writeToLog(msg);

		//make sure it's not a NaN
		if(CabbageUtils::isNumber(angle))
		//this is not right yet as it adds too many handles....
		//check that the angle has changed by at least 30 degrees, again, needs work
		if(((int)prevAngle!=(int)angle)&& abs(prevAngle-angle)>30)
		addHandle (envPath.getPointAlongPath(i).getX(), envPath.getPointAlongPath(i).getY());
		prevAngle = angle;
	}
	addHandle (envPath.getPointAlongPath(envPath.getLength()).getX(), envPath.getPointAlongPath(envPath.getLength()).getY());
}

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
	this->setScrollBarsShown (true, true); //only showing the h scrollbar
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

void CabbageTableViewer::addTable (String name, int tableSize, Colour colour, float alpha)
{
	//tableManager->addTable (name, tableSize, colour);

	int i = tables.size();

	tables.add (new Table(tableSize, colour));
	tables[i]->setBounds (getX(), getY(), getWidth(), getHeight());
	tables[i]->setOriginalWidth (getWidth());
	tables[i]->setAlpha(alpha);
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

void CabbageTableViewer::setScrubberPosition(int tableIndex, float position)
{
	tables[tableIndex]->scrubberPosition = position;
	tables[tableIndex]->repaint();
	
}




	








