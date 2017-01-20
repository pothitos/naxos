# Naxos Directory
NAXOS = ../core/bounds-consistency

# Compiler Options
CXX = g++
WARNINGS = -pedantic -Wall -W -Wshadow -Werror
CXXFLAGS = $(WARNINGS) -std=c++0x -O2 $(COVERAGE)
LD = $(CXX)
LDFLAGS = -s $(COVERAGE)

# Filenames
PROGRAMS = nqueens send_more_money
PR_OBJECTS = $(PROGRAMS:=.o)
OBJECTS = $(addprefix $(NAXOS)/, array_constraints.o bitset_domain.o expressions.o intvar.o problemmanager.o var_constraints.o)
HEADERS = $(addprefix $(NAXOS)/, internal.h naxos.h stack.h)

.PHONY: all
all: $(PROGRAMS)
$(OBJECTS): %.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@
$(PR_OBJECTS): %.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -I$(NAXOS) -c $<
$(PROGRAMS): %: $(OBJECTS) %.o
	$(LD) $(LDFLAGS) $^ -o $@

.PHONY: clean
clean:
	$(RM) $(PROGRAMS) $(PR_OBJECTS) $(OBJECTS)

.PHONY: test
test:
	verification/test.sh
