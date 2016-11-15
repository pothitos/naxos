#  Comment out the following line, for gcc versions less than 4.3.
STANDARD = -std=c++0x

# Naxos Directory
NAXOS = ../../core/bounds-consistency
# Amorgos Directory
AMORGOS = ..



####  COMPILER  ####

CXX      = $(CPATH)g++
WARNINGS = -pedantic -Wall -W -Wshadow -Werror
CXXFLAGS = $(WARNINGS) $(STANDARD) -O2 $(COVERAGE)

LD = $(CXX)
LDFLAGS = -s $(COVERAGE)



####  FILENAMES  ####

PROGRAMS = cryptmult golfer graphcolor knump magic mapcolor nqueens placement sudoku vertexcover
PR_OBJECTS = $(PROGRAMS:=.o)

OBJECTS_NAXOS = $(addprefix $(NAXOS)/, problemmanager.o expressions.o var_constraints.o array_constraints.o intvar.o bitset_domain.o)
OBJECTS_AMORGOS = $(addprefix $(AMORGOS)/, boundedbt.o credit.o dbds.o dbs.o dfs.o fns.o gns.o heuristics.o ibroad.o isamp.o lan.o lds.o onesamp.o rdfs.o)
OBJECTS = $(OBJECTS_NAXOS) $(OBJECTS_AMORGOS)

HEADERS_NAXOS = $(addprefix $(NAXOS)/, naxos.h stack.h internal.h)
HEADERS_AMORGOS = $(addprefix $(AMORGOS)/, heuristics.h onesamp.h dfs.h)



####  BUILDING  ####

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



####  CLEANING  ####

.PHONY: clean
clean:
	$(RM)  $(PROGRAMS) $(PR_OBJECTS)
