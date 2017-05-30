# Naxos Directory
NAXOS = ../../core
NAXOS_AC = core-arc-consistency

# Compiler Options
CXX      = $(CPATH)g++
WARNINGS = -pedantic -Wall -W -Wshadow -Werror
CXXFLAGS = $(WARNINGS) -std=c++0x -O2 $(COVERAGE)

LD = $(CXX)
LDFLAGS = -s $(COVERAGE)

# Filenames
PROGRAMS = nqueens magic_square tsp crew_scheduling
PROGRAMS_AC = $(PROGRAMS:=.AC)

PR_OBJECTS = $(PROGRAMS:=.o)
PR_OBJECTS_AC = $(PROGRAMS_AC:=.o)

OBJECTS = $(addprefix $(NAXOS)/, array_constraints.o bitset_domain.o expressions.o intvar.o problemmanager.o var_constraints.o non_mini_solver_expressions.o non_mini_solver_constraints.o)
HEADERS = $(addprefix $(NAXOS)/, internal.h naxos.h stack.h)

OBJECTS_AC = $(addprefix $(NAXOS_AC)/, constraints.o expressions.o intvar.o problemmanager.o)
HEADERS_AC = $(addprefix $(NAXOS_AC)/, naxos.h queue.h stack.h)

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

.PHONY: clean
clean:
	$(RM)  $(PROGRAMS) $(PR_OBJECTS) $(PROGRAMS_AC) $(PR_OBJECTS_AC) $(OBJECTS) $(OBJECTS_AC)

.PHONY: test
test:
	./verification.sh
