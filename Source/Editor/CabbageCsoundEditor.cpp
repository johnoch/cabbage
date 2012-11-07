
#include "CabbageCsoundEditor.h"
#include "../Standalone/CabbageStandaloneDialog.h"


#pragma warning(disable: 4996)
#pragma warning(disable: 4244)
//==============================================================================
CsoundEditor::CsoundEditor():hasChanged(false), unSaved(false)
{	
lookAndFeel = new CabbageLookAndFeel();
//textEditor = new CodeEditorComponent(csoundDoc, &csoundToker);
opcodes.addLines(String((BinaryData::opcodes_txt)));
csoundDoc.addListener(this);
textEditor = new CsoundCodeEditor(csoundDoc, &csoundToker);

textEditor->setColour(CodeEditorComponent::lineNumberTextId, Colours::white);
textEditor->setColour(CodeEditorComponent::lineNumberBackgroundId, Colours::black);

textEditor->setBounds(0, 0, getWidth(), getHeight());
textEditor->setFont(Font(String("Courier New"), 15, 1));
//appProperties->testWriteAccess(true, true, true);

htmlHelp = new WebBrowserComponent(false);


	
#ifndef MACOSX
String path = File::getSpecialLocation(File::currentApplicationFile).getParentDirectory().getFullPathName(); 
htmlHelp->goToURL(path+"/Docs/cabbage.html");
#else
	String path = File::getSpecialLocation(File::currentApplicationFile).getFullPathName()+"/Contents/Docs/";
	htmlHelp->goToURL(path);
#endif


setApplicationCommandManagerToWatch(&commandManager);

output = new TextEditor("output");
output->setMultiLine(true);
output->setScrollbarsShown(true);
output->setReturnKeyStartsNewLine(true);
output->setReadOnly(true);


fontSize = 15;
output->setFont(Font(String("Courier New"), 14, 0));
//background colour ID
Colour col(10, 10, 10);
output->setColour(0x1000200, col);
//text colour ID
output->setColour(0x1000201, Colours::white);

horizontalDividerBar = new StretchableLayoutResizerBar(&horizontalLayout, 1, false);
oldLookAndFeel = new OldSchoolLookAndFeel();


helpLabel = new helpContext();
helpLabel->setLookAndFeel(oldLookAndFeel);
helpLabel->setBackgroundColour(CabbageUtils::getComponentSkin());
helpLabel->setOutlineColour(Colours::grey);
helpLabel->setTextColour(Colours::white);

helpLabel->setHelpFont(Font(String("Courier New"), 14, 1));
helpLabel->setHelpText(String("Cabbage Csound Editor"));

tabComp = new TabbedComponent(TabbedButtonBar::TabsAtTop);
tabComp->addTab("Source", Colours::white, textEditor, false);
tabComp->addTab("Help", Colours::white, htmlHelp, false);

//addAndMakeVisible(textEditor);
addAndMakeVisible(tabComp);
addAndMakeVisible(horizontalDividerBar);
addAndMakeVisible(helpLabel);
addAndMakeVisible(output);



commandManager.registerAllCommandsForTarget(this);
addKeyListener(commandManager.getKeyMappings());

//tab component
horizontalLayout.setItemLayout (0,          // for item 0
    -.0, -.9,    // must be between 0 and 100 % in size
    -.7);      // and its preferred size is 70% of the total available space

// help comp
horizontalLayout.setItemLayout (1,
    15, 15,
    15);

// The resizer bar
horizontalLayout.setItemLayout (2,
    35, 45,
    55);

horizontalLayout.setItemLayout (3,          // for item 2
    -0., -.9, // size must be between 0% and 60% of the available space
    -.3);        // and its preferred size is 30% of total available space

#ifdef Cabbage_Build_Standalone

if(!appProperties->getUserSettings()->getValue("EditorColourScheme", var(0)).getIntValue()){
			textEditor->setColourScheme(csoundToker.getDefaultColourScheme());
			textEditor->setColour(CodeEditorComponent::backgroundColourId, Colours::white);
			textEditor->setColour(CaretComponent::caretColourId, Colours::black);
			textEditor->setColour(CodeEditorComponent::highlightColourId, Colours::cornflowerblue); 
}

else if(appProperties->getUserSettings()->getValue("EditorColourScheme", var(0)).getIntValue()){
			textEditor->setColourScheme(csoundToker.getDarkColourScheme());
			textEditor->setColour(CaretComponent::caretColourId, Colours::white);
			textEditor->setColour(CodeEditorComponent::backgroundColourId, Colour::fromRGB(20, 20, 20));
			textEditor->setColour(CodeEditorComponent::highlightColourId, Colours::green.withAlpha(.6f)); 
}
#endif

}
//==============================================================================
CsoundEditor::~CsoundEditor ()
{
commandManager.deleteInstance();
}
//==============================================================================
void CsoundEditor::resized()
{
Component* comps[] = { tabComp, horizontalDividerBar, helpLabel, output };
// this will position the components, one beside the other, to fit
// horizontally into the rectangle provided.
//tabComp->setBounds(0, 0, getWidth(), 24);
horizontalLayout.layOutComponents (comps, 4, 0, 0, getWidth(), getHeight(), true, true);
}

//==============================================================================
void CsoundEditor::setFontSize(String zoom)
{
if(zoom==String("in"))
textEditor->setFont(Font(String("Courier New"), ++fontSize, 1));
else
textEditor->setFont(Font(String("Courier New"), --fontSize, 1));

}

//==============================================================================

void CsoundEditor::getCommandInfo (const CommandID commandID, ApplicationCommandInfo& result)
{
	switch (commandID)
    {
    case CommandIDs::fileNew:
		result.setInfo (String("New"), String("Create a new file"), CommandCategories::file, 0);
		result.addDefaultKeypress ('n', ModifierKeys::commandModifier);
        break;
    case CommandIDs::fileOpen:
		result.setInfo (String("Open"), String("Open a file"), CommandCategories::file, 0);
		result.addDefaultKeypress ('o', ModifierKeys::commandModifier);
        break;
    case CommandIDs::fileSave:
		result.setInfo (String("Save"), String("Save a file"), CommandCategories::file, 0);
		result.addDefaultKeypress ('s', ModifierKeys::commandModifier);
        break;
    case CommandIDs::fileSaveAs:
		result.setInfo (String("Save as"), String("Save file as.."), CommandCategories::file, 0);
		result.addDefaultKeypress ('s', ModifierKeys::shiftModifier | ModifierKeys::commandModifier);
        break;
    case CommandIDs::fileExportSynth:
		result.setInfo (String("Export Synth"), String("Export Synth"), CommandCategories::file, 0);
		result.addDefaultKeypress ('i', ModifierKeys::commandModifier);
        break;
    case CommandIDs::fileExportEffect:
		result.setInfo (String("Export Effect"), String("Export Effect"), CommandCategories::file, 0);
		result.addDefaultKeypress ('e', ModifierKeys::commandModifier);
        break;
    case CommandIDs::fileUpdateGUI:
		result.setInfo (String("Update GUI"), String("Update GUI"), CommandCategories::file, 0);
		result.addDefaultKeypress ('u', ModifierKeys::commandModifier);
        break;
    case CommandIDs::editUndo:
		result.setInfo (String("Undo"), String("Undo last action"), CommandCategories::edit, 0);
		result.addDefaultKeypress ('z', ModifierKeys::commandModifier);
        break;
    case CommandIDs::editRedo:
		result.setInfo (String("Redo"), String("Redo last action"), CommandCategories::edit, 0);
		result.addDefaultKeypress ('z', ModifierKeys::shiftModifier | ModifierKeys::commandModifier);
        break;
    case CommandIDs::editCut:
		result.setInfo (String("Cut"), String("Cut selection"), CommandCategories::edit, 0);
		result.addDefaultKeypress ('x', ModifierKeys::commandModifier);
        break;
    case CommandIDs::editCopy:
		result.setInfo (String("Copy"), String("Copy selection"), CommandCategories::edit, 0);
		result.addDefaultKeypress ('c', ModifierKeys::commandModifier);
        break;
    case CommandIDs::editPaste:
		result.setInfo (String("Paste"), String("Paste selection"), CommandCategories::edit, 0);
		result.addDefaultKeypress ('v', ModifierKeys::commandModifier);
        break;
	case CommandIDs::editZoomIn:
		result.setInfo (String("Zoom in"), String("Zoom in"), CommandCategories::edit, 0);
		result.addDefaultKeypress ('=', ModifierKeys::commandModifier);
        break;
	case CommandIDs::editZoomOut:
		result.setInfo (String("Zoom out"), String("Zoom out"), CommandCategories::edit, 0);
		result.addDefaultKeypress ('-', ModifierKeys::commandModifier);
        break;
	case CommandIDs::whiteBackground:
		result.setInfo (String("White background"), String("White scheme"), CommandCategories::edit, 0);
        break;
	case CommandIDs::blackBackground:
		result.setInfo (String("Dark background"), String("Dark scheme"), CommandCategories::edit, 0);
        break;
	case CommandIDs::viewHelp:
#ifndef MACOSX
		result.setInfo (String("View Help F1"), String("View Help"), CommandCategories::help, 0);
		result.defaultKeypresses.add(KeyPress(KeyPress::F1Key));
#else
			result.setInfo (String("View Help Ctrl+1"), String("View Help"), CommandCategories::help, 0);
			result.addDefaultKeypress (String('1'), ModifierKeys::commandModifier);		
#endif
		break;
		
	}
}

void CsoundEditor::getAllCommands (Array <CommandID>& commands)
{
    const CommandID ids[] = {
								CommandIDs::fileNew,
								CommandIDs::fileOpen,
								CommandIDs::fileSave,
								CommandIDs::fileSaveAs,
								CommandIDs::fileExportSynth,
								CommandIDs::fileExportEffect,
								CommandIDs::fileUpdateGUI,
								
								CommandIDs::editUndo,
								CommandIDs::editCopy,
								CommandIDs::editCut,
								CommandIDs::editPaste,
								CommandIDs::editRedo,
								CommandIDs::editZoomIn,
								CommandIDs::editZoomOut,
								CommandIDs::whiteBackground,
								CommandIDs::blackBackground,

								CommandIDs::viewHelp
	};
	commands.addArray (ids, sizeof (ids) / sizeof (ids [0]));
}

bool CsoundEditor::perform (const InvocationInfo& info)
{
	switch (info.commandID)
    {
	//---------------------------------------------------------------------------------------------
	case CommandIDs::fileNew:
		{
			newFile("effect");
			break;
		}
	case CommandIDs::fileOpen:
		{			
			sendActionMessage(String("fileOpen"));
			break;
		}
	case CommandIDs::fileSave:
		{
#ifdef Cabbage_Build_Standalone
			sendActionMessage("fileSaved");
#else
			openCsdFile.replaceWithText(csoundDoc.getAllContent());
			this->getParentComponent()->setVisible(false);
#endif
			break;
		}
	case CommandIDs::fileSaveAs:
		{
			sendActionMessage("fileSaveAs");
			break;
		}

	case CommandIDs::fileExportSynth:
		{
			sendActionMessage("fileExportSynth");
			break;
		}

	case CommandIDs::fileExportEffect:
		{
			sendActionMessage("fileExportEffect");
			break;
		}

	case CommandIDs::fileUpdateGUI:
		{
			sendActionMessage("fileUpdateGUI");
			break;
		}

	case CommandIDs::editUndo:
		{			
			textEditor->undo();
			break;
		}
	case CommandIDs::editCut:
		{			
			textEditor->cutToClipboard();
			break;
		}
	case CommandIDs::editCopy:
		{			
			textEditor->copyToClipboard();
			break;
		}
	case CommandIDs::editPaste:
		{			
			textEditor->pasteFromClipboard();
			break;
		}

	case CommandIDs::editRedo:
		{			
			textEditor->redo();
			break;
		}

	case CommandIDs::editZoomIn:
		{			
			setFontSize("in");
			break;
		}
	case CommandIDs::editZoomOut:
		{			
			setFontSize("out");
			break;
		}

#ifdef Cabbage_Build_Standalone
	case CommandIDs::whiteBackground:
		{			
			textEditor->setColourScheme(csoundToker.getDefaultColourScheme());
			textEditor->setColour(CodeEditorComponent::backgroundColourId, Colours::white);
			textEditor->setColour(CodeEditorComponent::highlightColourId, Colours::cornflowerblue); 
			textEditor->setColour(CaretComponent::caretColourId, Colours::black);
			appProperties->getUserSettings()->setValue("EditorColourScheme", 0);
			break;
		}

	case CommandIDs::blackBackground:
		{			
			
			textEditor->setColourScheme(csoundToker.getDarkColourScheme());
			textEditor->setColour(CodeEditorComponent::backgroundColourId, Colour::fromRGB(20, 20, 20));
			textEditor->setColour(CodeEditorComponent::highlightColourId, Colours::green.withAlpha(.6f)); 
			textEditor->setColour(CaretComponent::caretColourId, Colours::white);
			appProperties->getUserSettings()->setValue("EditorColourScheme", 1);
			break;
		}
#endif

	case CommandIDs::viewHelp:
		{			
			//showMessage(appProperties->getUserSettings()->getValue("htmlHelp", "string"), lookAndFeel);
#ifdef Cabbage_Build_Standalone
			String helpDir = appProperties->getUserSettings()->getValue("CsoundHelpDir", "");
			if(helpDir.length()<2)
				showMessage("Please set the Csound manual directory in the Preference menu", lookAndFeel);		
			else{
			
			CodeDocument::Position pos1, pos2;
			pos1 = textEditor->getDocument().findWordBreakBefore(textEditor->getCaretPos());
			pos2 = textEditor->getDocument().findWordBreakAfter(textEditor->getCaretPos());
			String opcode = csoundDoc.getTextBetween(pos1, pos2);
			URL urlCsound(helpDir+"/"+opcode.trim()+String(".html"));
			String urlCabbage;
			//showMessage(urlCsound.toString(false), lookAndFeel);
			String OSXPath = File::getSpecialLocation(File::currentApplicationFile).getFullPathName()+"/Contents/Docs/";	
			String WINPath = File::getSpecialLocation(File::currentApplicationFile).getParentDirectory().getFullPathName()+"/Docs/"; 
	
			if((opcode.trim()==String("rslider"))
				||(opcode==String("hslider"))
				||(opcode==String("vslider")))
#ifndef MACOSX
			urlCabbage = String(WINPath+"cabbage.html#_sliders");
			else
			urlCabbage = String(WINPath+"cabbage.html#_")+opcode.trim();				
#else
			urlCabbage = String(OSXPath+"cabbage.html#_sliders");
			else
			urlCabbage = String(OSXPath+"cabbage.html#_")+opcode.trim();				
#endif

			//showMessage(urlCabbage);

			File temp1(urlCsound.toString(false));
			if(temp1.exists()){
			htmlHelp->goToURL(urlCsound.toString(false));
			tabComp->setCurrentTabIndex(1);
			tabComp->setCurrentTabIndex(0);
			tabComp->setCurrentTabIndex(1);
			}
			else{
				htmlHelp->goToURL(urlCabbage);
				tabComp->setCurrentTabIndex(1);
				tabComp->setCurrentTabIndex(0);
				tabComp->setCurrentTabIndex(1);
				}
			}
			break;
#endif
		}

	}
return true;
}

//==============================================================================
PopupMenu CsoundEditor::getMenuForIndex (int topLevelMenuIndex, const String& menuName)
{
PopupMenu m1, m2;
m2.setLookAndFeel(&getLookAndFeel());
if(topLevelMenuIndex==0)
	{
	 m1.addCommandItem(&commandManager, CommandIDs::fileNew);	 
	 m1.addCommandItem(&commandManager, CommandIDs::fileOpen);
	 m1.addCommandItem(&commandManager, CommandIDs::fileSave);
	 m1.addCommandItem(&commandManager, CommandIDs::fileSaveAs);
	 m1.addSeparator();
	 m1.addCommandItem(&commandManager, CommandIDs::fileUpdateGUI);
	 m1.addCommandItem(&commandManager, CommandIDs::fileExportSynth);
	 m1.addCommandItem(&commandManager, CommandIDs::fileExportEffect);

	 //m2.addCommandItem(&commandManager, CommandIDs::fileExportSynth);
	 //m2.addCommandItem(&commandManager, CommandIDs::fileExportEffect);
	 //m1.addSubMenu(String("Export Plugin"), m2);
	 return m1;
	}
else if(topLevelMenuIndex==1)
	{
	m1.addCommandItem(&commandManager, CommandIDs::editUndo);
	m1.addCommandItem(&commandManager, CommandIDs::editRedo);
	m1.addCommandItem(&commandManager, CommandIDs::editCut);
	m1.addCommandItem(&commandManager, CommandIDs::editCopy);
	m1.addCommandItem(&commandManager, CommandIDs::editPaste);
	m1.addSeparator();
	m2.addCommandItem(&commandManager, CommandIDs::editZoomIn);
	m2.addCommandItem(&commandManager, CommandIDs::editZoomOut);
	m1.addSubMenu(String("Font Size"), m2);
	m2.clear();
	m2.addCommandItem(&commandManager, CommandIDs::whiteBackground);
	m2.addCommandItem(&commandManager, CommandIDs::blackBackground);
	m1.addSubMenu("Change editor theme", m2);
	return m1;
	}

else if(topLevelMenuIndex==2)
	{
	m1.addCommandItem(&commandManager, CommandIDs::viewHelp);
	return m1;
	}
	
else return m1;
}
//==============================================================================
void CsoundEditor::codeDocumentChanged (const CodeDocument::Position &affectedTextStart, const CodeDocument::Position &affectedTextEnd)
{
hasChanged = true;
String lineFromCsd = textEditor->getDocument().getLine(affectedTextStart.getLineNumber());
String test1, test2, opcodeName, opcodeInfo, opcodeSyntax;
for(int i=0;i<opcodes.size();i++){
	test1 = opcodes[i].substring(1, 20);
	test2 = test1.substring(0, test1.indexOf("\""));
	opcodeName = String(" ")+test2;
	if(lineFromCsd.contains("rslider"))
	helpLabel->setHelpText(String("rslider")+String(" - ")+String("\"Cabbage Rotary Slider\"")+String("\n")+String("Syntax: ")+String("rslider bounds(x, y, widht, height) [, channel(\"channel\"), range(min, max, value), caption(\"caption\"), colour(\"colour\"), midiCtrl(channel, control)]"));
	else if(lineFromCsd.contains("hslider"))
	helpLabel->setHelpText(String("hslider")+String(" - ")+String("\"Cabbage Horizontal Slider\"")+String("\n")+String("Syntax: ")+String("hslider bounds(x, y, widht, height) [, channel(\"channel\"), range(min, max, value), caption(\"caption\"), colour(\"colour\"), midiCtrl(channel, control)]"));
	else if(lineFromCsd.contains("vslider"))
	helpLabel->setHelpText(String("vslider")+String(" - ")+String("\"Cabbage Vertical Slider\"")+String("\n")+String("Syntax: ")+String("vslider bounds(x, y, widht, height) [, channel(\"channel\"), range(min, max, value), caption(\"caption\"), colour(\"colour\"), midiCtrl(channel, control)]"));
	else if(lineFromCsd.contains("button"))
	helpLabel->setHelpText(String("button")+String(" - ")+String("\"Cabbage Button\"")+String("\n")+String("Syntax: ")+String("button bounds(x, y, widht, height) [, channel(\"channel\"), text(\"ontext\", \"offtext\"), caption(\"caption\")]"));
	else if(lineFromCsd.contains("checkbox"))
	helpLabel->setHelpText(String("checkbox")+String(" - ")+String("\"Cabbage Checkbox\"")+String("\n")+String("Syntax: ")+String("checkbox bounds(x, y, widht, height) [, channel(\"channel\"), text(\"buttontext\"), caption(\"caption\")]"));
	else if(lineFromCsd.contains("combokbox"))
	helpLabel->setHelpText(String("checkbox")+String(" - ")+String("\"Cabbage Checkbox\"")+String("\n")+String("Syntax: ")+String("combobox bounds(x, y, widht, height) [, channel(\"channel\"), text(\"item1\", \"item2\", \"item3\", etc), caption(\"caption\")]"));
	else if(lineFromCsd.contains("xypad"))
	helpLabel->setHelpText(String("xypad")+String(" - ")+String("\"Cabbage XY Pad\"")+String("\n")+String("Syntax: ")+String("xypad bounds(x, y, widht, height) [, channel(\"channelX\",\"channelY\"), rangeX(min, max, value), rangeY(min, max, value), text(\"caption\")]"));


	else
	if(opcodeName.length()>2)
	if(lineFromCsd.contains(opcodeName)){
		test1 = opcodes[i].substring(opcodes[i].indexOf(";")+1, 500);
		test2 = test1.substring(test1.indexOf(";")+1, 500);
		opcodeInfo = test2.substring(0, test2.indexOf(";"));
		opcodeSyntax = test2.substring(test2.indexOf(";")+1, 1000);
		helpLabel->setHelpText(String("  ")+opcodeName+String("- ")+opcodeInfo+String("\n")+String("   Syntax: ")+opcodeSyntax);
	}
	}
}

//==============================================================================
void CsoundEditor::setCurrentFile(File input)
{
	csoundDoc.replaceAllContent(input.loadFileAsString());
	openCsdFile = input;
	this->setName(openCsdFile.getFullPathName());
}
//==============================================================================
String CsoundEditor::getCurrentText()
{
	return csoundDoc.getAllContent();
}

//===============================================================================
void CsoundEditor::newFile(String type)
{
String untitledCSD;
if(type=="effect"){
untitledCSD= 
"<Cabbage>\n"
"form size(400, 300), caption(\"Untitled\"), pluginID(\"plu1\")\n"
"\n"
"</Cabbage>\n"
"<CsoundSynthesizer>\n"
"<CsOptions>\n"
"-n -d\n"
"</CsOptions>\n"
"<CsInstruments>\n"
"sr = 44100\n"
"ksmps = 64\n"
"nchnls = 2\n"
"0dbfs=1\n"
"\n"
"instr 1\n"
"a1 inch 1\n"
"a2 inch 2\n"
"\n"
"\n"
"outs a1, a2\n"
"endin\n"
"\n"
"</CsInstruments>  \n"
"<CsScore>\n"
"f1 0 1024 10 1\n"
"i1 0 3600\n"
"</CsScore>\n"
"</CsoundSynthesizer>";
}
else if(type=="instrument")
{
untitledCSD= 
"<Cabbage>\n"
"form size(400, 300), caption(\"Untitled\"), pluginID(\"plu1\")\n"
"keyboard bounds(10, 200, 360, 100)\n"
"\n"
"</Cabbage>\n"
"<CsoundSynthesizer>\n"
"<CsOptions>\n"
"-n -d -+rtmidi=NULL -M0 --midi-key-cps=4 --midi-velocity-amp=5\n" 
"</CsOptions>\n"
"<CsInstruments>\n"
"sr = 44100\n"
"ksmps = 64\n"
"nchnls = 2\n"
"0dbfs=1\n"
"\n"
"instr 1\n"
"a1 oscili p5, p4, 1\n"
"outs a1, a1"
"\n"
"endin\n"
"\n"
"</CsInstruments>  \n"
"<CsScore>\n"
"f1 0 1024 10 1\n"
"f0 3600\n"
"</CsScore>\n"
"</CsoundSynthesizer>";
}
csoundDoc.replaceAllContent(untitledCSD);
unSaved = true;
sendActionMessage("fileSavedAs");
}
