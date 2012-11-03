To build Cabbage for Linux you need to have a version of Csound (doubles) installed on your machine. 
If Csound is not installed in the usual place, i.e., "/usr/local/include/csound" you will need to
hack the makefiles so that the compiler can find the Csound header files. 

To build just run the following command from the Linux directory:

./buildCabbage

This should build a Cabbage binary, as well as two plugin libraries. If you move the Cabbage binary
to another location please make sure to also move the plugin libs, as they MUST reside in the same
folder as the main Cabbage binary. 




The following are dependencies you may need:

sudo apt-get -y install libfreetype6-dev
sudo apt-get -y install libx11-dev
sudo apt-get -y install libxinerama-dev
sudo apt-get -y install libxcursor-dev
sudo apt-get -y install mesa-common-dev
sudo apt-get -y install libasound2-dev
sudo apt-get -y install freeglut3-dev
sudo apt-get -y install libxcomposite-dev


