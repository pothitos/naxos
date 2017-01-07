# Comment out the following line, for gcc versions less than 4.3.
STANDARD = -std=c++0x

# Naxos Directory
NAXOS = ../../core/bounds-consistency

# Compiler Options
CXX = g++
WARNINGS = -pedantic -Wall -W -Wshadow -Werror
CXXFLAGS = $(WARNINGS) $(STANDARD) -O2 $(COVERAGE)
LD = $(CXX)
LDFLAGS = -s $(COVERAGE)

# Filenames
PROGRAMS = nqueens
PR_OBJECTS = $(PROGRAMS:=.o)
HEADERS = $(addprefix $(NAXOS)/, internal.h naxos.h stack.h)
SOURCES = $(addprefix $(NAXOS)/, array_constraints.cpp bitset_domain.cpp expressions.cpp intvar.cpp problemmanager.cpp var_constraints.cpp)
OBJECTS = $(SOURCES:.cpp=.o)

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
