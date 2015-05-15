# Comment out the following line, for gcc versions less than 4.3.
STANDARD = -std=c++0x

# Naxos Directory
NAXOS = ..

### COMPILER ###
CXX = g++
WARNINGS = -pedantic -Wall -W -Wshadow
CXXFLAGS = $(WARNINGS) $(STANDARD) -O2
LD = $(CXX)
LDFLAGS = -s

### FILENAMES ###
PROGRAMS = numpart nqueens send_more_money
PR_OBJECTS = $(PROGRAMS:=.o)
OBJECTS = $(addprefix $(NAXOS)/, problemmanager.o expressions.o var_constraints.o array_constraints.o intvar.o bitset_domain.o)
HEADERS = $(addprefix $(NAXOS)/, naxos.h internal.h stack.h)

### BUILDING ###
.PHONY: all
all: $(PROGRAMS)
$(OBJECTS):   %.o:   %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $<  -o $@
$(PR_OBJECTS):   %.o:   %.cpp
	$(CXX) $(CXXFLAGS) -I$(NAXOS) -c  $<
$(PROGRAMS):   %:   $(OBJECTS) %.o
	$(LD) $(LDFLAGS) $^  -o $@

### CLEANING ###
.PHONY: clean
clean:
	$(RM)  $(PROGRAMS) $(PR_OBJECTS)
