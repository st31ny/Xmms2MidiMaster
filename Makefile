# tools
CXX = g++
RM = rm -f
MKDIR = mkdir -p
DOXYGEN = doxygen

# source files
SRC = main.cpp SongIdNotifier.cpp

# version
VERSION = $(shell git log -1 --pretty=format:%h)

# directories and target
IDIR = include
ODIR = obj
SRCDIR = src
BINDIR = bin
TARGET = x2mm
DEPENDFILE = .depend
DOCDIR = doc/doxygen/
DOXYFILE = Doxyfile

# compiler and linker flags
DEBUG = -g
CXXFLAGS = -Wall -c -DVERSION=\"$(VERSION)\" $(DEBUG)
CPPFLAGS = -I$(IDIR)

LDFLAGS = $(DEBUG)
LDLIBS =

################################################################################
OBJ_ = $(SRC:%.cpp=%.o)
OBJ = $(addprefix $(ODIR)/, $(OBJ_))

all: $(TARGET)

$(TARGET): $(ODIR) $(BINDIR) $(OBJ)
	$(CXX) $(LDLIBS) $(LDFLAGS) -o $(BINDIR)/$(TARGET) $(OBJ)

$(ODIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -MD -o $@ $<

-include $(OBJ:%.o=%.d)

$(ODIR):
	$(MKDIR) $(ODIR)

$(BINDIR):
	$(MKDIR) $(BINDIR)

.PHONY: doc 
doc: $(DOXYFILE)
	export GITHASH=$(VERSION)
	$(DOXYGEN) $(DOXYFILE)

.PHONY: clean
clean:
	$(RM) -r $(ODIR)
	$(RM) -r $(BINDIR)
	$(RM) -r $(DOCDIR)

