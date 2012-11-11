Build Cabbage under Linux
-------------------------

Requirements
============

The following are dependencies you may need:

sudo apt-get -y install libfreetype6-dev
sudo apt-get -y install libx11-dev
sudo apt-get -y install libxinerama-dev
sudo apt-get -y install libxcursor-dev
sudo apt-get -y install mesa-common-dev
sudo apt-get -y install libasound2-dev
sudo apt-get -y install freeglut3-dev
sudo apt-get -y install libxcomposite-dev

You also need a version of Csound (doubles) installed on your machine.
If Csound is not installed in the usual place, i.e.,
"/usr/local/include/csound" you will need to hack the makefiles so
that the compiler can find the Csound header files or pass the path in
argument of buildCabbage (see below).

Compile
=======

To build run the following command from the Linux directory:

./buildCabbage [ARGS]

where ARGS are optional arguments passed to the make commands. For
instance if you want to compile over 4 threads and set the include
Csound path to /usr/include/csound, run:

./buildCabbage -j4 CSOUND_INCLUDE=/usr/include/csound VST_INCLUDE=~/SDKs/vstsdk2.4

This should create a build directory with the Cabbage binary, as well
as two plugin libraries. If you move the Cabbage binary to another
location please make sure to also move the plugin libs, as they MUST
reside in the same folder as the main Cabbage binary.
