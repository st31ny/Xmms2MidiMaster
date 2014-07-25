# tools
CXX = g++
RM = rm -f
MKDIR = mkdir -p
DOXYGEN = doxygen

# source files
SRC = main.cpp SongIdNotifier.cpp Config.cpp XmmsClient.cpp

# version
VERSION = $(shell git log -1 --pretty=format:%h)

# directories and target
IDIR = include
ODIR = obj
SRCDIR = src
BINDIR = bin
TARGET = x2mm
DOCDIR = doc/doxygen/
DOXYFILE = Doxyfile

# compiler and linker flags
DEBUG = -g
CXXFLAGS = -std=c++11 -Wall -c -DVERSION=\"$(VERSION)\" $(DEBUG) `pkg-config --cflags xmms2-client-cpp`
CPPFLAGS = -I$(IDIR)

LDFLAGS = $(DEBUG)
LDLIBS = -lboost_program_options -lboost_regex -lportmidi `pkg-config --libs xmms2-client-cpp`

################################################################################
OBJ_ = $(SRC:%.cpp=%.o)
OBJ = $(addprefix $(ODIR)/, $(OBJ_))
TARGET_ = $(BINDIR)/$(TARGET)

all: $(TARGET_)

$(TARGET_): $(ODIR) $(BINDIR) $(OBJ)
	$(CXX) $(LDLIBS) $(LDFLAGS) -o $(TARGET_) $(OBJ)

$(ODIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -MD -o $@ $<

-include $(OBJ:%.o=%.d)

$(ODIR):
	$(MKDIR) $(ODIR)

$(BINDIR):
	$(MKDIR) $(BINDIR)

.PHONY: doc 
doc: $(DOXYFILE)
	GITHASH=$(VERSION) $(DOXYGEN) $(DOXYFILE)

.PHONY: clean
clean:
	$(RM) -r $(ODIR)
	$(RM) -r $(BINDIR)
	$(RM) -r $(DOCDIR)

