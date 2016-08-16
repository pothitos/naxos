#  Comment out the following line, for gcc versions less than 4.3.
STANDARD = -std=c++0x

### COMPILER ###
CXX = g++
WARNINGS = -pedantic -Wall -W -Wshadow
CXXFLAGS = $(WARNINGS) $(STANDARD) -O2

### FILENAMES ###
HEADERS = naxos.h stack.h internal.h
SOURCES = problemmanager.cpp expressions.cpp var_constraints.cpp array_constraints.cpp intvar.cpp bitset_domain.cpp
OBJECTS = $(SOURCES:.cpp=.o)

### BUILDING ###
.PHONY: all
all: $(OBJECTS)
%.o:  %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c  $<

### CLEANING ###
.PHONY: clean
clean:
	$(RM)  $(OBJECTS)

### TESTING ###
.PHONY: test
test: ;
