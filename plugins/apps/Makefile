# Naxos Directory
NAXOS = ../../core

# Amorgos Directory
AMORGOS = ..

# Compiler Options
CXX      = $(CPATH)g++
WARNINGS = -pedantic -Wall -W -Wshadow -Werror
CXXFLAGS = $(WARNINGS) -std=c++0x -O2 $(COVERAGE)

LD = $(CXX)
LDFLAGS = -s $(COVERAGE)

# Filenames
PROGRAMS = cryptmult golfer graphcolor knump magic mapcolor nqueens placement sudoku vertexcover
PR_OBJECTS = $(PROGRAMS:=.o)

OBJECTS_NAXOS = $(addprefix $(NAXOS)/, array_constraints.o bitset_domain.o expressions.o intvar.o problemmanager.o var_constraints.o non_mini_solver_constraints.o)
OBJECTS_AMORGOS = $(addprefix $(AMORGOS)/, boundedbt.o credit.o dbds.o dbs.o dfs.o fns.o gns.o heuristics.o ibroad.o isamp.o lan.o lds.o onesamp.o rdfs.o)
OBJECTS = $(OBJECTS_NAXOS) $(OBJECTS_AMORGOS)

HEADERS_NAXOS = $(addprefix $(NAXOS)/, internal.h naxos.h stack.h)
HEADERS_AMORGOS = $(addprefix $(AMORGOS)/, dfs.h heuristics.h onesamp.h)

.PHONY: all
all: $(PROGRAMS)

$(OBJECTS_NAXOS):   %.o:   %.cpp $(HEADERS_NAXOS)
	$(CXX) $(CXXFLAGS) -c $<  -o $@

$(OBJECTS_AMORGOS):   %.o:   %.cpp %.h $(HEADERS_AMORGOS)
	$(CXX) $(CXXFLAGS) -I$(NAXOS) -c $<  -o $@

$(PR_OBJECTS):   %.o:   %.cpp
	$(CXX) $(CXXFLAGS) -I$(NAXOS) -I$(AMORGOS) -c  $<

$(PROGRAMS):   %:   $(OBJECTS) %.o
	$(LD) $(LDFLAGS) $^  -o $@

.PHONY: clean
clean:
	$(RM)  $(PROGRAMS) $(PR_OBJECTS) $(OBJECTS)

.PHONY: test
test: ;
