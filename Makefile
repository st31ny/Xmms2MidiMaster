# tools
CXX = g++
RM = rm -f
MKDIR = mkdir -p
DOXYGEN = doxygen

# source files
SRC = SongIdNotifier.cpp Config.cpp XmmsClient.cpp
# extra main source (has to be excluded for tests)
SRC_MAIN = main.cpp
# testing source files
TEST_SRC = TestMain.cpp StatusTest.cpp

# version
VERSION = $(shell git log -1 --pretty=format:%h)

# directories and target
IDIR = include
ODIR = obj
TEST_ODIR = obj/test
SRCDIR = src
TEST_SRCDIR = test
BINDIR = bin
TARGET = x2mm
TEST_TARGET = x2mmtest
DOCDIR = doc/doxygen/
DOXYFILE = Doxyfile

# compiler and linker flags
DEBUG = -g
CXXFLAGS = -std=c++11 -Wall -c -DVERSION=\"$(VERSION)\" $(DEBUG) `pkg-config --cflags xmms2-client-cpp`
CPPFLAGS = -I$(IDIR)

LDFLAGS = $(DEBUG)
LDLIBS = -lboost_program_options -lboost_regex -lportmidi `pkg-config --libs xmms2-client-cpp`
TEST_LDLIBS = -lunittest++ $(LDLIBS)

################################################################################
OBJ_ = $(SRC:%.cpp=%.o)
OBJ = $(addprefix $(ODIR)/, $(OBJ_))
OBJ_MAIN_ = $(SRC_MAIN:%.cpp=%.o)
OBJ_MAIN = $(addprefix $(ODIR)/, $(OBJ_MAIN_))
TARGET_ = $(BINDIR)/$(TARGET)

TEST_OBJ_ = $(TEST_SRC:%.cpp=%.o)
TEST_OBJ = $(addprefix $(TEST_ODIR)/, $(TEST_OBJ_))
TEST_TARGET_ = $(BINDIR)/$(TEST_TARGET)

all: $(TARGET_)

test: $(TEST_TARGET_)

$(TARGET_): $(ODIR) $(BINDIR) $(OBJ) $(OBJ_MAIN)
	$(CXX) $(LDLIBS) $(LDFLAGS) -o $@ $(OBJ) $(OBJ_MAIN)

$(ODIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -MD -o $@ $<

$(TEST_TARGET_): $(TEST_ODIR) $(BINDIR) $(OBJ) $(TEST_OBJ)
	$(CXX) $(OBJ) $(TEST_OBJ) $(TEST_LDLIBS) $(LDFLAGS) -o $@

$(TEST_ODIR)/%.o: $(TEST_SRCDIR)/%.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -MD -o $@ $<

-include $(OBJ:%.o=%.d)

$(ODIR):
	$(MKDIR) $(ODIR)

$(BINDIR):
	$(MKDIR) $(BINDIR)

$(TEST_ODIR):
	$(MKDIR) $(TEST_ODIR)

.PHONY: doc 
doc: $(DOXYFILE)
	GITHASH=$(VERSION) $(DOXYGEN) $(DOXYFILE)

.PHONY: clean
clean:
	$(RM) -r $(TEST_ODIR)
	$(RM) -r $(ODIR)
	$(RM) -r $(BINDIR)
	$(RM) -r $(DOCDIR)

