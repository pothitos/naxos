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
OBJECTS = $(addprefix $(NAXOS)/, problemmanager.o expressions.o var_constraints.o array_constraints.o intvar.o bitset_domain.o)
HEADERS = $(addprefix $(NAXOS)/, naxos.h stack.h internal.h)

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
	./verification.sh
