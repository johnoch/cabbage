#include "CabbageStandaloneDialog.h"


    class CabbageStandalone : public JUCEApplication
    {
       public:
          CabbageStandalone()
          {
          }   

          void initialise(const String& /*commandLineParameters*/)
          {
             ApplicationProperties::getInstance()->setStorageParameters (T("Cabbage"), T("userSettings"), T("Cabbage"), 400, PropertiesFile::storeAsXML);

			 filterWindow = new StandaloneFilterWindow (T("Cabbage"), Colours::white);
             filterWindow->setTitleBarButtonsRequired (DocumentWindow::allButtons, false);
             filterWindow->setVisible (true);
             filterWindow->setResizable (true, true);
             juce::Thread::setCurrentThreadName("cabbage");
           //  juce::Thread::setCurrentThreadName("cabbage");
          }

          void shutdown()
          {
			filterWindow = 0;// = nullptr;
			ApplicationProperties::getInstance()->closeFiles();
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
