#include "CabbageStandaloneDialog.h"

ApplicationProperties* appProperties = nullptr;
PropertySet* defaultPropSet = nullptr;

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
			defaultPropSet = new PropertySet();
			ScopedPointer<XmlElement> xml = new XmlElement("PLANTS");
			defaultPropSet->setValue("DisablePluginInfo", 0);
			defaultPropSet->setValue("DisableGUIEditModeWarning", 1);
			defaultPropSet->setValue("SetAlwaysOnTop", 1);	
			defaultPropSet->setValue("PlantRepository", xml);
			defaultPropSet->setValue("EditorColourScheme", 1);
			
			appProperties->setStorageParameters (options);
			appProperties->getUserSettings()->setFallbackPropertySet(defaultPropSet);
			

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
			deleteAndZero(defaultPropSet);
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
