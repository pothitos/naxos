# Parser's Directory
PARSER = XCSP3-CPP-Parser

# Naxos Directory
NAXOS = ../../core/bounds-consistency

# Compiler Options
CXX = g++
WARNINGS = -pedantic -Wall -W -Wshadow -Werror
CXXFLAGS = $(WARNINGS) -std=c++0x -O2 $(COVERAGE)
LD = $(CXX)
LDFLAGS = -s $(COVERAGE)

# Filenames
PROGRAMS = naxos-xcsp3
PR_OBJECTS = $(PROGRAMS:=.o)
HEADERS = $(addprefix $(NAXOS)/, internal.h naxos.h stack.h)
SOURCES = $(addprefix $(NAXOS)/, array_constraints.cpp bitset_domain.cpp expressions.cpp intvar.cpp problemmanager.cpp var_constraints.cpp)
OBJECTS = $(SOURCES:.cpp=.o)

.PHONY: all
all: $(PROGRAMS) parser
$(OBJECTS): %.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@
$(PR_OBJECTS): %.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -I$(NAXOS) -c $<
$(PROGRAMS): %: $(OBJECTS) %.o
	$(LD) $(LDFLAGS) $^ -o $@

PARSER_SAMPLE = $(PARSER)/samples
PARSER_INSTANCES = $(PARSER)/instances
.PHONY: parser
parser:
	# Execute the internal XCSP3 Parser's Makefile
	$(MAKE) -C $(PARSER_SAMPLE)

.PHONY: clean
clean:
	$(RM) $(PROGRAMS) $(PR_OBJECTS)

.PHONY: test
test:
	# Ensure that Naxos Solver can be considered as a "Mini-Solver".
	# According to the First International XCSP3 Competition: "A
	# mini-solver is a solver whose code must be composed of
	# less than 8,000 lines of at most 160 characters (while
	# discarding code for parsing XCSP3, comments and code of
	# standard libraries)."
	SLOC=$$(cat $(HEADERS) $(SOURCES) | grep -v "^$$" | grep -v "^ *//" | wc -l) && \
        echo "$$SLOC pure source lines of code" && \
        test $$SLOC -lt 8000
	# Temporarily disable the maximum line width test
	! grep ".\{161\}" $(HEADERS) $(SOURCES) || true
	# Test the parser for TSP instance
	$(PARSER_SAMPLE)/test $(PARSER_INSTANCES)/tsp-25-843.xml > /dev/null
	# Test the parser for a Constraint Optimisation instance
	$(PARSER_SAMPLE)/test $(PARSER_INSTANCES)/obj.xml > /dev/null
	# Test the parser for an instance with all the kinds of constraints:
	# The test fails as the parser's sample solver doesn't support every constraint
	! $(PARSER_SAMPLE)/test $(PARSER_INSTANCES)/example.xml > /dev/null
