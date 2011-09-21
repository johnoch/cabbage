# Automatically generated makefile, created by the Jucer
# Don't edit this file! Your changes will be overwritten when you re-save the Jucer project!

ifndef CONFIG
  CONFIG=Debug
endif

ifeq ($(TARGET_ARCH),)
  TARGET_ARCH := -march=native
endif

# (this disables dependency generation if multiple architectures are set)
DEPFLAGS := $(if $(word 2, $(TARGET_ARCH)), , -MMD)

ifeq ($(CONFIG),Debug)
  BINDIR := build
  LIBDIR := build
  OBJDIR := build/intermediate/Debug
  OUTDIR := build
  CPPFLAGS := $(DEPFLAGS) -D "LINUX=1" -D "DEBUG=1" -D "_DEBUG=1" -D "MSVC=1" -D "USE_DOUBLE=1" -D"Cabbage_GUI_Editor" -D "Cabbage_Build_Standalone" -D "Cabbage_Named_Pipe" -D "JUCER_LINUX_MAKE_7346DA2A=1" -I "/usr/include" -I "/usr/include/freetype2"
  CFLAGS += $(CPPFLAGS) $(TARGET_ARCH) -g -ggdb -O0
  CXXFLAGS += $(CFLAGS) -I "/usr/local/include/csound"
  LDFLAGS += -L$(BINDIR) -L$(LIBDIR) -L"/usr/X11R6/lib/" -L"../../../juceTip/juce/bin" -lfreetype -lpthread -lrt -lX11 -lGL -lGLU -lXinerama -lasound -lasound -lcsound64 -lm -lsndfile
  LDDEPS :=
  RESFLAGS :=  -D "LINUX=1" -D "DEBUG=1" -D "_DEBUG=1" -D "JUCER_LINUX_MAKE_7346DA2A=1" -I "/usr/include" -I "/usr/include/freetype2"
  TARGET := Cabbage
  BLDCMD = $(CXX) -o $(OUTDIR)/$(TARGET) $(OBJECTS) $(LDFLAGS) $(RESOURCES) $(TARGET_ARCH)
endif

ifeq ($(CONFIG),Release)
  BINDIR := build
  LIBDIR := build
  OBJDIR := build/intermediate/Release
  OUTDIR := build
  CPPFLAGS := $(DEPFLAGS) -D "LINUX=1" -D "NDEBUG=1" -D "JUCER_LINUX_MAKE_7346DA2A=1" -I "/usr/include" -I "/usr/include/freetype2"
  CFLAGS += $(CPPFLAGS) $(TARGET_ARCH) -Os
  CXXFLAGS += $(CFLAGS) -I "/usr/local/include/csound"
  LDFLAGS += -L$(BINDIR) -L$(LIBDIR) -L"/usr/X11R6/lib/" -L"../../../juceTip/juce/bin" -lfreetype -lpthread -lrt -lX11 -lGL -lGLU -lXinerama -lasound -lasound -lcsound64 -lm -lsndfile
  LDDEPS :=
  RESFLAGS :=  -D "LINUX=1" -D "NDEBUG=1" -D "JUCER_LINUX_MAKE_7346DA2A=1" -I "/usr/include" -I "/usr/include/freetype2"
  TARGET := Cabbage
  BLDCMD = $(CXX) -o $(OUTDIR)/$(TARGET) $(OBJECTS) $(LDFLAGS) $(RESOURCES) $(TARGET_ARCH)
endif

OBJECTS := \
  $(OBJDIR)/CabbageGUIClass_79b9049f.o \
  $(OBJDIR)/CabbageMainPanel_12c1333.o \
  $(OBJDIR)/CabbagePluginEditor_e9bade22.o \
  $(OBJDIR)/CabbagePluginProcessor_4c907dc7.o \
  $(OBJDIR)/CabbageStandaloneDialog_1f0ad88f.o \
  $(OBJDIR)/ComponentLayoutEditor_aa38c835.o \
  $(OBJDIR)/StandaloneFilterApp_e433cd25.o \
  $(OBJDIR)/JuceLibraryCode1_682c927f.o \
  $(OBJDIR)/JuceLibraryCode2_683aaa00.o \
  $(OBJDIR)/JuceLibraryCode3_6848c181.o \
  $(OBJDIR)/JuceLibraryCode4_6856d902.o \

.PHONY: clean

$(OUTDIR)/$(TARGET): $(OBJECTS) $(LDDEPS) $(RESOURCES)
	@echo Linking Cabbage
	-@mkdir -p $(BINDIR)
	-@mkdir -p $(LIBDIR)
	-@mkdir -p $(OUTDIR)
	@$(BLDCMD)

clean:
	@echo Cleaning Cabbage
	-@rm -f $(OUTDIR)/$(TARGET)
	-@rm -rf $(OBJDIR)/*
	-@rm -rf $(OBJDIR)

$(OBJDIR)/CabbageGUIClass_79b9049f.o: ../../Source/CabbageGUIClass.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling CabbageGUIClass.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/CabbageMainPanel_12c1333.o: ../../Source/CabbageMainPanel.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling CabbageMainPanel.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/CabbagePluginEditor_e9bade22.o: ../../Source/CabbagePluginEditor.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling CabbagePluginEditor.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/CabbagePluginProcessor_4c907dc7.o: ../../Source/CabbagePluginProcessor.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling CabbagePluginProcessor.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/CabbageStandaloneDialog_1f0ad88f.o: ../../Source/CabbageStandaloneDialog.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling CabbageStandaloneDialog.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/ComponentLayoutEditor_aa38c835.o: ../../Source/ComponentLayoutEditor.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling ComponentLayoutEditor.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/StandaloneFilterApp_e433cd25.o: ../../Source/StandaloneFilterApp.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling StandaloneFilterApp.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/JuceLibraryCode1_682c927f.o: ../../JuceLibraryCode/JuceLibraryCode1.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling JuceLibraryCode1.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/JuceLibraryCode2_683aaa00.o: ../../JuceLibraryCode/JuceLibraryCode2.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling JuceLibraryCode2.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/JuceLibraryCode3_6848c181.o: ../../JuceLibraryCode/JuceLibraryCode3.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling JuceLibraryCode3.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/JuceLibraryCode4_6856d902.o: ../../JuceLibraryCode/JuceLibraryCode4.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling JuceLibraryCode4.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

-include $(OBJECTS:%.o=%.d)
