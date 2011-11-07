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
			options.applicationName     = "Cabbage Standalone host";
			options.filenameSuffix      = "settings";
			options.osxLibrarySubFolder = "Preferences";

			appProperties = new ApplicationProperties();
			appProperties->setStorageParameters (options);

			 filterWindow = new StandaloneFilterWindow (T("Cabbage"), Colours::black);
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
             return T("cabbage");
          }
          
          const String getApplicationVersion()
          {
             return T("0.0");
          }

		  bool moreThanOneInstanceAllowed()       
		  { 
			  return true; 
		  }

       private:
         ScopedPointer<StandaloneFilterWindow> filterWindow;
    };

    START_JUCE_APPLICATION (CabbageStandalone)
