#ifndef CABBAGE_COMMAND_H
#define CABBAGE_COMMAND_H

#include "../../JuceLibraryCode/JuceHeader.h"
//==============================================================================
namespace CommandIDs
{
    static const int fileNew			= 0x2000;
    static const int fileOpen			= 0x2001;
    static const int fileSave			= 0x2002;
    static const int fileSaveAs			= 0x2003;
    static const int fileSaveAll		= 0x2004;
	static const int fileExportSynth	= 0x2005;
	static const int fileExportEffect	= 0x2006;
	static const int fileUpdateGUI		= 0x2007;

	static const int editUndo			= 0x2011;
	static const int editRedo			= 0x2012;
	static const int editCut			= 0x2013;
	static const int editCopy			= 0x2014;
	static const int editPaste			= 0x2015;
	static const int editZoomIn			= 0x2016;
	static const int editZoomOut		= 0x2017;

	static const int viewHelp			= 0x2018;
	static const int viewSource			= 0x2019;
};

//==============================================================================
namespace CommandCategories
{
    static const char* const file = "File";
    static const char* const edit = "Edit";
	static const char* const help = "View";
}

//==============================================================================

class CommandManager : public ApplicationCommandManager
{
public:

	CommandManager ();
    ~CommandManager ();

	juce_DeclareSingleton (CommandManager, true)

};

#endif
