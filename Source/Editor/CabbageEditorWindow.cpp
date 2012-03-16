#include "CabbageEditorWindow.h"

//==============================================================================
CabbageEditorWindow::CabbageEditorWindow(LookAndFeel* look)
    : DocumentWindow (T("Cabbage Csound Editor"),
                      Colours::black,
                      DocumentWindow::allButtons)
{
    centreWithSize (800, 600);
    setVisible (true);
	setResizable(true, true);
	this->setLookAndFeel(look);
	csoundEditor = new CsoundEditor ();
	csoundEditor->addActionListener(this);
	csoundEditor->setBounds(0, 0, getWidth(), getHeight());
	setContentOwned(csoundEditor, true);


setMenuBar (csoundEditor, 25);

}

//==============================================================================
CabbageEditorWindow::~CabbageEditorWindow()
{
}

//==============================================================================
void CabbageEditorWindow::closeButtonPressed()
{
   setVisible(false);
}

//==============================================================================
void CabbageEditorWindow::actionListenerCallback (const String& message){
	sendActionMessage(message);
}

//==============================================================================
void CabbageEditorWindow::setCsoundOutputText(String text){
	const MessageManagerLock mmLock;
	String existingText = csoundEditor->output->getText();
	csoundEditor->output->setText(existingText+text);
	csoundEditor->output->setCaretPosition(csoundEditor->output->getText().length());
}

