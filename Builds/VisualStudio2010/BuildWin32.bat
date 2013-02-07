echo off
IF "%1"=="all" goto ALL
IF "%1"=="standalone" goto STANDALONE
IF "%1"=="plugins" goto PLUGINS
IF "%1"=="release" goto RELEASE

REM http://social.msdn.microsoft.com/Forums/eu/msbuild/thread/8645667d-fba3-4f87-8606-d0aa6407ab1b

:RELEASE
echo Release Builds 
msbuild CabbagePlugin.sln /t:ReBuild /p:Configuration=Debug
COPY Debug\CabbagePlugin.dll CabbagePluginEffect.dat
msbuild CabbagePlugin.sln /t:REBuild /p:Configuration=Debug /p:MyConstants="Cabbage_Plugin_Synth=1"
COPY Debug\CabbagePlugin.dll CabbagePluginSynth.dat
msbuild Cabbage.sln /t:ReBuild /p:Configuration=Debug /p:MyConstants="Cabbage_Plugin_Synth=1"
COPY Debug\Cabbage.exe Cabbage.exe
echo Pleae check current directory for Cabbage.exe, CabbagePluginSynth.dat and CabbagePluginEffect.dat. If any files are missing seomthing went wrong in the build process!  
goto END

:ALL
echo Building All 
msbuild CabbagePlugin.sln /t:ReBuild /p:Configuration=Debug
COPY Debug\CabbagePlugin.dll CabbagePluginEffect.dat
msbuild CabbagePlugin.sln /t:REBuild /p:Configuration=Debug /p:DefineConst="Cabbage_Plugin_Synth=1"
COPY Debug\CabbagePlugin.dll CabbagePluginSynth.dat
msbuild Cabbage.sln /t:ReBuild /p:Configuration=Debug /p:DefineConst="Cabbage_Plugin_Synth=1"
COPY Debug\Cabbage.exe Cabbage.exe
echo Pleae check current directory for Cabbage.exe, CabbagePluginSynth.dat and CabbagePluginEffect.dat. If any files are missing seomthing went wrong in the build process!  
goto END

:PLUGINS
echo Building Plugins 
msbuild CabbagePlugin.sln /t:Build /p:Configuration=Debug /p:UserConstant="Cabbage_Plugin_Synth"
COPY Debug\CabbagePlugin.dll CabbagePluginSynth.dat
msbuild CabbagePlugin.sln /t:ReBuild /p:Configuration=Debug
COPY Debug\CabbagePlugin.dll CabbagePluginEffect.dat
goto END

:STANDALONE
echo Building Standalone 
msbuild Cabbage.sln /t:Build /p:Configuration=Debug /p:DefineConstants="Cabbage_Plugin_Synth=1"
COPY Debug\Cabbage.exe Cabbage.exe
goto END

:END
echo Build complete
