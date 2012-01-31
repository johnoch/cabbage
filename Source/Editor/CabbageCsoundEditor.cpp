
#include "CabbageCsoundEditor.h"
#include "CabbageEditorCommandManager.h"


#pragma warning(disable: 4996)
#pragma warning(disable: 4244)
//==============================================================================
CsoundEditor::CsoundEditor()
{	

textEditor = new CodeEditorComponent(csoundDoc, &csoundToker);
textEditor->setBounds(0, 0, getWidth(), getHeight());
addAndMakeVisible(textEditor);
textEditor->setWantsKeyboardFocus(true);
CommandManager* commandManager = CommandManager::getInstance();
commandManager->registerAllCommandsForTarget(this);
commandManager->getKeyMappings()->resetToDefaultMappings();
addKeyListener(commandManager->getKeyMappings());

}
//==============================================================================
CsoundEditor::~CsoundEditor ()
{
//csoundDoc.setPositionMaintained(false);
}
//==============================================================================
void CsoundEditor::resized()
{
textEditor->setBounds(0, 0, getWidth(), getHeight());
}


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
	}
}

void CsoundEditor::getAllCommands (Array <CommandID>& commands)
{
    const CommandID ids[] = {
								CommandIDs::fileNew,
								CommandIDs::fileOpen,
								CommandIDs::fileSave,
								CommandIDs::fileSaveAs,
								
								CommandIDs::editUndo,
								CommandIDs::editCopy,
								CommandIDs::editCut,
								CommandIDs::editPaste,
								CommandIDs::editRedo,
	};
	commands.addArray (ids, sizeof (ids) / sizeof (ids [0]));
}

bool CsoundEditor::perform (const InvocationInfo& info)
{
//	int tabIndex = tabComponent->getCurrentTabIndex();
//	toggleButton = dynamic_cast <StartStopButton*> (info.originatingComponent);
    switch (info.commandID)
    {
	//---------------------------------------------------------------------------------------------
	case CommandIDs::fileNew:
		{
			//NewFile();
			break;
		}
	case CommandIDs::fileOpen:
		{			
			openFile();
			break;
		}
	case CommandIDs::fileSave:
		{
			saveFile();
			//toFront(true);
			break;
		}
	case CommandIDs::fileSaveAs:
		{
			break;
		}

	case CommandIDs::editUndo:
		{			
			break;
		}
	case CommandIDs::editCut:
		{			
			break;
		}
	case CommandIDs::editCopy:
		{			
			break;
		}
	case CommandIDs::editPaste:
		{			
			break;
		}

	case CommandIDs::editRedo:
		{			
			break;
		}
	}
return true;
}

//==============================================================================
const PopupMenu CsoundEditor::getMenuForIndex (int topLevelMenuIndex, const String& menuName)
{
CommandManager* commandManager = CommandManager::getInstance();
PopupMenu m1;
if(topLevelMenuIndex==0)
	{
	 m1.addCommandItem(commandManager, CommandIDs::fileNew);	 
	 m1.addCommandItem(commandManager, CommandIDs::fileOpen);
	 m1.addCommandItem(commandManager, CommandIDs::fileSave);
	 m1.addCommandItem(commandManager, CommandIDs::fileSaveAs);
	 m1.addSeparator();
	 return m1;
	}
else if(topLevelMenuIndex==1)
	{
	m1.addCommandItem(commandManager, CommandIDs::editUndo);
	m1.addCommandItem(commandManager, CommandIDs::editRedo);
	m1.addCommandItem(commandManager, CommandIDs::editCut);
	m1.addCommandItem(commandManager, CommandIDs::editCopy);
	m1.addCommandItem(commandManager, CommandIDs::editPaste);
	return m1;
	}
else return m1;
}
//==============================================================================


//==============================================================================
void CsoundEditor::openFile()
{
FileChooser openFC(T("Open a .csd file..."), File::nonexistent, T("*.csd"));
if(openFC.browseForFileToOpen())
	csoundDoc.replaceAllContent(openFC.getResult().loadFileAsString());
	openCsdFile = openFC.getResult();
}

void CsoundEditor::openFile(File input)
{
	csoundDoc.replaceAllContent(input.loadFileAsString());
	openCsdFile = input;
}

//==============================================================================
void CsoundEditor::saveFile()
{
openCsdFile.replaceWithText(csoundDoc.getAllContent());
sendActionMessage("fileSaved");
}

//==============================================================================
void CsoundEditor::saveFileAs()
{
FileChooser openFC(T("Save a .csd file..."), File::nonexistent, T("*.csd"));

if(openFC.browseForFileToSave(true)){
	File saveFile(openFC.getResult().getFullPathName());
	saveFile.replaceWithText(csoundDoc.getAllContent());
}
	
}

//==============================================================================