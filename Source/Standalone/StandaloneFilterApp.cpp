#include "CabbageStandaloneDialog.h"

ApplicationProperties* appProperties = nullptr;

class CabbageStandalone : public JUCEApplication
    {
       public:
          CabbageStandalone()
          {
          }

          void initialise(const String& /*commandLineParameters*/)
          {
			// initialise our settings file..
			PropertiesFile::Options options;
			options.applicationName     = "Cabbage";
			options.filenameSuffix      = "settings";
			options.osxLibrarySubFolder = "Preferences";

			appProperties = new ApplicationProperties();
			appProperties->setStorageParameters (options);

			filterWindow = new StandaloneFilterWindow (String("Cabbage"), Colours::black);
            filterWindow->setTitleBarButtonsRequired (DocumentWindow::allButtons, false);
            filterWindow->setVisible (true);
            //turn off resizeable...
			//filterWindow->setResizable (true, true);
            juce::Thread::setCurrentThreadName("cabbage");
          }

          void shutdown()
          {
			filterWindow = 0;// = nullptr;
			appProperties->closeFiles();
			deleteAndZero(appProperties);
          }

          const String getApplicationName()
          {
             return String("cabbage");
          }
          
          const String getApplicationVersion()
          {
             return String("0.0");
          }

		  bool moreThanOneInstanceAllowed()       
		  { 
			  return true; 
		  }

       private:
         ScopedPointer<StandaloneFilterWindow> filterWindow;
    };

    START_JUCE_APPLICATION (CabbageStandalone)
