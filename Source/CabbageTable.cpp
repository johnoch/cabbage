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
Table::Table (int tableSize, Colour colour) : tblSize(tableSize), zoom(1.0), activeColour(colour),
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
}

void Table::setGlobalAmpRange(float globalMaxAmp, float globalMinAmp, float globalAmpRange)
{
	minAmp = globalMinAmp;
	maxAmp = globalMaxAmp;
	ampRange = globalAmpRange;
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
	
	zeroAmpPosition = convertAmpToPixel(0);

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
			
	ImageCache::addImageToCache (img, 15);
}

void Table::paint (Graphics& g)
{
	//Image bg = ImageCache::getFromHashCode(15);
	//g.drawImage (bg, viewX, 0, origWidth, getHeight(), 
	//	0, 0, bg.getWidth(), bg.getHeight(), false);
	
	if (isCurrentlyOnTop)
		currColour = activeColour.withMultipliedSaturation(3);
	else
		currColour = activeColour;
	int startIndx, endIndx;

	// If using overview...
	if (useOverview == true) {
		startIndx = (viewX/getWidth()) * overview.maxY.size();
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
				g.setColour(currColour);
				topYPixelValue -= minWaveHeight/2;
				bottomYPixelValue += minWaveHeight/2;
				g.drawVerticalLine (xPixelValue+viewX, topYPixelValue, bottomYPixelValue);
				xPixelValue += 1;

				// Fill
				if (tblSize <= 4096) {
					g.setColour(currColour.withAlpha(0.1f));
					if (bottomYPixelValue < zeroAmpPosition)
						g.drawVerticalLine (xPixelValue+viewX, bottomYPixelValue, zeroAmpPosition);
					else if (bottomYPixelValue > zeroAmpPosition)
						g.drawVerticalLine (xPixelValue+viewX, zeroAmpPosition, bottomYPixelValue);

					if (isCurrentlyOnTop)
						currColour = activeColour.withMultipliedSaturation(3);
					else
						currColour = activeColour;
				}
			}
		}
	}
	
	//Else if using original array values for painting...
	else if (useOverview == false) {
		g.setColour(currColour);
		startIndx = ((viewX/getWidth()) * tableData.amps.size()) + 0.5; //0.5 for rounding
		endIndx = (startIndx + (viewWidth/numPixelsPerIndex)) + 0.5; 
		float prevX = viewX;
		float prevY = convertAmpToPixel (tableData.amps[startIndx]);
		float currY;
		for (int i=startIndx+1; i<=endIndx; i++) {
			currY = convertAmpToPixel (tableData.amps[i]);
			g.drawLine (prevX, prevY, prevX+numPixelsPerIndex, currY, minWaveHeight);
			// For drawing index markers
			if (numPixelsPerIndex > 4)
				g.drawVerticalLine (prevX+numPixelsPerIndex, currY-3, currY+3);
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
	g.setColour(currColour);
	scrubberPosition = scrubberPosition*getWidth();
	g.drawLine(scrubberPosition, 0, scrubberPosition, getHeight(), 2);
	}
}

void Table::mouseDown (const MouseEvent& e)
{
	/* With shift button down a new envelope handle is added. No zoom.
	else if (e.mods.isShiftDown() == true)
		draggingHandle = addHandle (e.x, e.y);
		*/
}

void Table::applyZoom (int zoomInput)
{
	zoom = zoomInput;
	setDataSource(zoom);
}

void Table::setToEnabled(bool isEnabled)
{
	isCurrentlyOnTop = isEnabled;

	if (isCurrentlyOnTop) //if on top
		minWaveHeight = 3;
	else 
		minWaveHeight = 1.5;
}

void Table::setViewStart(float x)
{
	viewX = x;
}

void Table::setViewWidth(float width)
{
	viewWidth = width;
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
		handles.removeObject(thisHandle, true);
		repaint();
	}
	delete thisHandle;
}


/*
  ====================================================================================

	Table Manager class

  ====================================================================================
*/
CabbageTableManager::CabbageTableManager(Viewport* view)
	: viewport(view), alpha(1.0f), zoom(1), maxZoom(1), maxNumPixelsPerIndex(1), prevActiveIndex(0),
	globalMaxAmp(0), globalMinAmp(0)
{
}

CabbageTableManager::~CabbageTableManager()
{
}

void CabbageTableManager::resized()
{
	//We need to make room for the h scrollbar, therefore table data
	//can't use the full height of the canvas
	tableTop = getHeight()*0.15;
	tableBottom = getHeight()*0.85;
	tableHeight = tableBottom - tableTop;
}

void CabbageTableManager::setOriginalWidth(float width)
{
	originalWidth = width;
}

Table* CabbageTableManager::getTable(int index)
{
	if(index > 0 && index < tables.size());
	return tables[index];
}


void CabbageTableManager::paint(Graphics& g)
{
	g.setColour(CabbageUtils::getDarkerBackgroundSkin());
	g.fillAll();

	// Amp horizontal markers...
	g.setColour (Colour::fromRGBA (220, 220, 240, 255));
//	g.drawLine (0, convertAmpToPixel(globalMaxAmp), getWidth(), convertAmpToPixel(globalMaxAmp), 0.1);
//	g.drawLine (0, convertAmpToPixel(globalMinAmp), getWidth(), convertAmpToPixel(globalMinAmp), 0.1);
//	g.drawLine (0, convertAmpToPixel(globalMaxAmp*0.5), getWidth(), convertAmpToPixel(globalMaxAmp*0.5), 0.1);
//	g.drawLine (0, convertAmpToPixel(globalMinAmp*0.5), getWidth(), convertAmpToPixel(globalMinAmp*0.5), 0.1);
	if (globalMinAmp < 0) 
		g.drawLine (0, convertAmpToPixel(0), getWidth(), convertAmpToPixel(0), 0.3);

	// update tables
	for (int i=0; i<tables.size(); ++i) 
		tables[i]->setViewStart(viewport->getViewPositionX());
}

float CabbageTableManager::convertAmpToPixel (float ampValue)
{
	// This method converts amps to y pixel values
	float normalisedAmp = (ampValue-globalMinAmp) / globalAmpRange; //first change to normalised value
	return ((1-normalisedAmp) * tableHeight) + tableTop;
}

void CabbageTableManager::addTable (String name, int tableSize, Colour colour)
{
	int i = tables.size();
	tables.add (new Table(tableSize, colour));
	tables[i]->setBounds (0, 0, getWidth(), getHeight());
	tables[i]->setOriginalWidth (getWidth());
	tables[i]->setViewWidth(viewport->getViewWidth());
	addAndMakeVisible (tables[i]);
	tables[i]->addMouseListener(this, true); //"this" will now also get mouse events for tables[i]
	tableToTop(i); //setting this table to the top

	//the max possible zoom is based on the table with the most indices. It will have a max zoom
	//of 50 pixels for every index.
	if ((tableSize*50) > maxNumPixelsPerIndex) {
		maxNumPixelsPerIndex = (tableSize*50);
		maxZoom = sqrt(maxNumPixelsPerIndex);
	}
}

void CabbageTableManager::tableToTop (int tableIndex)
{
	if (tables.size() > 1) {
		tables.swap(tableIndex, tables.size()-1); //swapping currently active table to end of array
		tables[prevActiveIndex]->setToEnabled(false); //disabling the previously active table
		tables[prevActiveIndex]->setAlpha(0.7);
	}

	tables.getLast()->setToEnabled(true);
	tables.getLast()->setAlpha(1.0);

	prevActiveIndex = tableIndex;
	//alpha /= 2;
}

void CabbageTableManager::fillTable (int tableIndex, Array<float> csndInputData)
{
	float currTableMaxAmp, currTableMinAmp;

	// Getting the min and max amplitude values....
	currTableMinAmp = currTableMaxAmp = csndInputData.getFirst();
	for (int i=0; i<csndInputData.size(); ++i) {
		if (csndInputData[i] > currTableMaxAmp)
			currTableMaxAmp = csndInputData[i];
		if (csndInputData[i] < currTableMinAmp)
			currTableMinAmp = csndInputData[i];
	}

	//if min and max amps are the same value....
	if (currTableMinAmp == currTableMaxAmp) {
		if (currTableMinAmp > 0)
			currTableMinAmp = 0;
		else if (currTableMinAmp < 0) {
			currTableMaxAmp = 0;
		}
		else { //else if both are 0
			currTableMinAmp = -1;
			currTableMaxAmp = 1;
		}
	}

	if (currTableMaxAmp > globalMaxAmp)
		globalMaxAmp = currTableMaxAmp;
	if (currTableMinAmp < globalMinAmp)
		globalMinAmp = currTableMinAmp;

	globalAmpRange = globalMaxAmp - globalMinAmp; 

	for (int i=0; i<tables.size(); ++i) 
		tables[i]->setGlobalAmpRange(globalMaxAmp, globalMinAmp, globalAmpRange);

	tables[tableIndex]->createAmpOverviews(csndInputData);
}

void CabbageTableManager::mouseDown (const MouseEvent& e)
{	
	float distanceFromViewStart = e.x - viewport->getViewPositionX();
	float mouseClickProportional = e.x;

	//zoom in
	if ((zoom*2 <= maxZoom) && (e.mods.isLeftButtonDown())) {
		zoom *= 2;
		mouseClickProportional *= 2;
	}
	//zoom out
	else if ((zoom/2 >= 1) && (e.mods.isRightButtonDown())) {
		if (zoom != 1) {
			zoom /= 2;
			mouseClickProportional /= 2;
		}
	}

	//resetting bounds and viewport view position
	this->setBounds(0, 0, originalWidth*zoom, getHeight());
	viewport->setViewPosition(mouseClickProportional - distanceFromViewStart, 0);

	//applying zoom to child tables
	for (int i=0; i<tables.size(); ++i) {
		tables[i]->applyZoom(zoom);
		tables[i]->setBounds(0, 0, getWidth(), getHeight());
	}
}


/*
  ====================================================================================

	Table Viewer class

  ====================================================================================
*/
CabbageTableViewer::CabbageTableViewer()
{
	this->setScrollBarsShown (false, true); //only showing the h scrollbar
	tableManager = new CabbageTableManager(this);
}

CabbageTableViewer::~CabbageTableViewer()
{
}

void CabbageTableViewer::resized()
{
	tableManager->setBounds (getX(), getY(), getWidth(), getHeight());
	tableManager->setOriginalWidth(getWidth());
	addAndMakeVisible(tableManager);
	this->setViewedComponent (tableManager, false);
}

void CabbageTableViewer::addTable (String name, int tableSize, Colour colour, float alpha)
{
	tableManager->addTable (name, tableSize, colour);
}

void CabbageTableViewer::tableToTop (int tableIndex)
{
	tableManager->tableToTop(tableIndex);
}

void CabbageTableViewer::fillTable (int tableIndex, Array<float> csndInputData)
{
	tableManager->fillTable (tableIndex, csndInputData);
}

void CabbageTableViewer::setScrubberPosition(int tableIndex, float position)
{
	tableManager->getTable(tableIndex)->scrubberPosition = position;
	tableManager->getTable(tableIndex)->repaint();
}





	








