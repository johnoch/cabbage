
#include "CabbageCsoundEditor.h"



#pragma warning(disable: 4996)
#pragma warning(disable: 4244)
//==============================================================================
CsoundEditor::CsoundEditor():hasChanged(false), unSaved(false)
{	
//lookAndFeel = look;//new LookAndFeel();
//this->setLookAndFeel(look);
//textEditor = new CodeEditorComponent(csoundDoc, &csoundToker);
opcodes.addLines(String((BinaryData::opcodes_txt)));
csoundDoc.addListener(this);
textEditor = new CodeEditorExtended(csoundDoc, &csoundToker);
textEditor->setBounds(0, 0, getWidth(), getHeight());
textEditor->editor->setFont(Font(T("Courier New"), 15, 1));

setApplicationCommandManagerToWatch(&commandManager);
output = new TextEditor("output");
output->setMultiLine(true);
output->setScrollbarsShown(true);
output->setReturnKeyStartsNewLine(true);
output->setReadOnly(true);


fontSize = 15;
output->setFont(Font(T("Courier New"), 14, 0));
//background colour ID
Colour col(10, 10, 10);
output->setColour(0x1000200, col);
//text colour ID
output->setColour(0x1000201, Colours::white);

horizontalDividerBar = new StretchableLayoutResizerBar(&horizontalLayout, 1, false);

helpLabel = new helpContext();
oldLookAndFeel = new OldSchoolLookAndFeel();
helpLabel->setLookAndFeel(oldLookAndFeel);
helpLabel->setBackgroundColour(CabbageUtils::componentSkin());
helpLabel->setOutlineColour(Colours::grey);
helpLabel->setTextColour(Colours::white);

helpLabel->setHelpFont(Font(T("Courier New"), 14, 1));
helpLabel->setHelpText(T("Cabbage Csound Editor"));
addAndMakeVisible(textEditor);
addAndMakeVisible(horizontalDividerBar);
addAndMakeVisible(helpLabel);
addAndMakeVisible(output);
   

commandManager.registerAllCommandsForTarget(this);
addKeyListener(commandManager.getKeyMappings());

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

}
//==============================================================================
CsoundEditor::~CsoundEditor ()
{
//csoundDoc.setPositionMaintained(false);
}
//==============================================================================
void CsoundEditor::resized()
{
Component* comps[] = { textEditor, horizontalDividerBar, helpLabel, output };
// this will position the components, one beside the other, to fit
// horizontally into the rectangle provided.
horizontalLayout.layOutComponents (comps, 4, 0, 0, getWidth(), getHeight(), true, true);
}

//==============================================================================
void CsoundEditor::setFontSize(String zoom)
{
if(zoom==T("in"))
textEditor->editor->setFont(Font::Font(T("Courier New"), ++fontSize, 1));
else
textEditor->editor->setFont(Font::Font(T("Courier New"), --fontSize, 1));

}

//==============================================================================

void CsoundEditor::getCommandInfo (const CommandID commandID, ApplicationCommandInfo& result)
{
	switch (commandID)
    {
    case CommandIDs::fileNew:
		result.setInfo (T("New"), T("Create a new file"), CommandCategories::file, 0);
		result.addDefaultKeypress (T('n'), ModifierKeys::commandModifier);
        break;
    case CommandIDs::fileOpen:
		result.setInfo (T("Open"), T("Open a file"), CommandCategories::file, 0);
		result.addDefaultKeypress (T('o'), ModifierKeys::commandModifier);
        break;
    case CommandIDs::fileSave:
		result.setInfo (T("Save"), T("Save a file"), CommandCategories::file, 0);
		result.addDefaultKeypress (T('s'), ModifierKeys::commandModifier);
        break;
    case CommandIDs::fileSaveAs:
		result.setInfo (T("Save as"), T("Save file as.."), CommandCategories::file, 0);
		result.addDefaultKeypress (T('s'), ModifierKeys::shiftModifier | ModifierKeys::commandModifier);
        break;
    case CommandIDs::fileExportSynth:
		result.setInfo (T("Export Synth"), T("Export Synth"), CommandCategories::file, 0);
		result.addDefaultKeypress (T('i'), ModifierKeys::commandModifier);
        break;
    case CommandIDs::fileExportEffect:
		result.setInfo (T("Export Effect"), T("Export Effect"), CommandCategories::file, 0);
		result.addDefaultKeypress (T('e'), ModifierKeys::commandModifier);
        break;

    case CommandIDs::editUndo:
		result.setInfo (T("Undo"), T("Undo last action"), CommandCategories::edit, 0);
		result.addDefaultKeypress (T('z'), ModifierKeys::commandModifier);
        break;
    case CommandIDs::editRedo:
		result.setInfo (T("Redo"), T("Redo last action"), CommandCategories::edit, 0);
		result.addDefaultKeypress (T('z'), ModifierKeys::shiftModifier | ModifierKeys::commandModifier);
        break;
    case CommandIDs::editCut:
		result.setInfo (T("Cut"), T("Cut selection"), CommandCategories::edit, 0);
		result.addDefaultKeypress (T('x'), ModifierKeys::commandModifier);
        break;
    case CommandIDs::editCopy:
		result.setInfo (T("Copy"), T("Copy selection"), CommandCategories::edit, 0);
		result.addDefaultKeypress (T('c'), ModifierKeys::commandModifier);
        break;
    case CommandIDs::editPaste:
		result.setInfo (T("Paste"), T("Paste selection"), CommandCategories::edit, 0);
		result.addDefaultKeypress (T('v'), ModifierKeys::commandModifier);
        break;
	case CommandIDs::editZoomIn:
		result.setInfo (T("Zoom in"), T("Zoom in"), CommandCategories::edit, 0);
		result.addDefaultKeypress (T('='), ModifierKeys::commandModifier);
        break;
	case CommandIDs::editZoomOut:
		result.setInfo (T("Zoom out"), T("Zoom out"), CommandCategories::edit, 0);
		result.addDefaultKeypress (T('-'), ModifierKeys::commandModifier);
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
								
								CommandIDs::editUndo,
								CommandIDs::editCopy,
								CommandIDs::editCut,
								CommandIDs::editPaste,
								CommandIDs::editRedo,
								CommandIDs::editZoomIn,
								CommandIDs::editZoomOut
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
			sendActionMessage(T("fileOpen"));
			break;
		}
	case CommandIDs::fileSave:
		{
			sendActionMessage("fileSaved");
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

	case CommandIDs::editUndo:
		{			
			textEditor->editor->undo();
			break;
		}
	case CommandIDs::editCut:
		{			
			textEditor->editor->cutToClipboard();
			break;
		}
	case CommandIDs::editCopy:
		{			
			textEditor->editor->copyToClipboard();
			break;
		}
	case CommandIDs::editPaste:
		{			
			textEditor->editor->pasteFromClipboard();
			break;
		}

	case CommandIDs::editRedo:
		{			
			textEditor->editor->redo();
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
	}
return true;
}

//==============================================================================
const PopupMenu CsoundEditor::getMenuForIndex (int topLevelMenuIndex, const String& menuName)
{
PopupMenu m1, m2;

if(topLevelMenuIndex==0)
	{
	 m1.addCommandItem(&commandManager, CommandIDs::fileNew);	 
	 m1.addCommandItem(&commandManager, CommandIDs::fileOpen);
	 m1.addCommandItem(&commandManager, CommandIDs::fileSave);
	 m1.addCommandItem(&commandManager, CommandIDs::fileSaveAs);
	 m1.addSeparator();
	 m2.addCommandItem(&commandManager, CommandIDs::fileExportSynth);
	 m2.addCommandItem(&commandManager, CommandIDs::fileExportEffect);
	 m1.addSubMenu(T("Export Plugin"), m2);
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
	m1.addSubMenu(T("Font Size"), m2);
	return m1;
	}
else return m1;
}
//==============================================================================
void CsoundEditor::codeDocumentChanged (const CodeDocument::Position &affectedTextStart, const CodeDocument::Position &affectedTextEnd)
{
hasChanged = true;
String lineFromCsd = textEditor->editor->getDocument().getLine(affectedTextStart.getLineNumber());
String test1, test2, opcodeName, opcodeInfo, opcodeSyntax;
for(int i=0;i<opcodes.size();i++){
	test1 = opcodes[i].substring(1, 20);
	test2 = test1.substring(0, test1.indexOf("\""));
	opcodeName = T(" ")+test2;
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
		helpLabel->setHelpText(T("  ")+opcodeName+T("- ")+opcodeInfo+String("\n")+String("   Syntax: ")+opcodeSyntax);
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
"\n"
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
