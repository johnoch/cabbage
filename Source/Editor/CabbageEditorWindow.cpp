#include "CabbageEditorWindow.h"

//==============================================================================
CabbageEditorWindow::CabbageEditorWindow()
    : DocumentWindow (T("Cabbage Csound Editor"),
                      Colours::black,
                      DocumentWindow::allButtons)
{
    centreWithSize (800, 600);
    setVisible (true);
	setResizable(true, true);
	csoundEditor = new CsoundEditor ();
	csoundEditor->addActionListener(this);
	csoundEditor->setBounds(0, 0, getWidth(), getHeight());
	setContentOwned(csoundEditor, true);


#if JUCE_MAC
setMacMainMenu (0);
#else
setMenuBar (csoundEditor, 25);
#endif
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
	String existingText = csoundEditor->output->getText();
	csoundEditor->output->setText(existingText+text);
	csoundEditor->output->setCaretPosition(csoundEditor->output->getText().length());
}

