# Comment out the following line, for gcc versions less than 4.3.
STANDARD = -std=c++0x

# Naxos Directory
NAXOS = ../../core/bounds-consistency
NAXOS_AC = ../../core/arc-consistency



####  COMPILER  ####

CXX      = $(CPATH)g++
WARNINGS = -pedantic -Wall -W -Wshadow -Werror
CXXFLAGS = $(WARNINGS) $(STANDARD) -O2 $(COVERAGE)

LD = $(CXX)
LDFLAGS = -s $(COVERAGE)



####  FILENAMES  ####

PROGRAMS = nqueens magic TSP CREW_SCHEDULING
PROGRAMS_AC = $(PROGRAMS:=.AC)

PR_OBJECTS = $(PROGRAMS:=.o)
PR_OBJECTS_AC = $(PROGRAMS_AC:=.o)

OBJECTS = $(addprefix $(NAXOS)/, problemmanager.o expressions.o var_constraints.o array_constraints.o intvar.o bitset_domain.o)
HEADERS = $(addprefix $(NAXOS)/, naxos.h internal.h stack.h)

OBJECTS_AC = $(addprefix $(NAXOS_AC)/, problemmanager.o intvar.o expressions.o constraints.o)
HEADERS_AC = $(addprefix $(NAXOS_AC)/, naxos.h queue.h stack.h)



####  BUILDING  ####

.PHONY: all
all: $(PROGRAMS) AC

$(OBJECTS):   %.o:   %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $<  -o $@

$(PR_OBJECTS):   %.o:   %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -I$(NAXOS) -c  $<

$(PROGRAMS):   %:   $(OBJECTS) %.o
	$(LD) $(LDFLAGS) $^  -o $@



.PHONY: AC
AC: $(PROGRAMS_AC)

$(OBJECTS_AC):   %.o:   %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $<  -o $@

$(PR_OBJECTS_AC):   %.AC.o:   %.cpp
	$(CXX) $(CXXFLAGS) -I$(NAXOS_AC) -c $<  -o $@

$(PROGRAMS_AC):   %:   $(OBJECTS_AC) %.o
	$(LD) $(LDFLAGS) $^  -o $@



####  CLEANING  ####

.PHONY: clean
clean:
	$(RM)  $(PROGRAMS) $(PR_OBJECTS) $(PROGRAMS_AC) $(PR_OBJECTS_AC)

### TESTING ###
.PHONY: test
test: ;
