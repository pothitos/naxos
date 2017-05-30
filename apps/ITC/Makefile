# Compiler and Linker Options
NDIR = ../../core
NDIR_AC = ../../naxos-AC
ADIR = ../../plugins

CC = $(CPATH)g++
WFLAGS = -pedantic -Wall -W -Wshadow -Werror
COMPFLAGS = $(WFLAGS) -std=c++0x -I$(NDIR) -I$(ADIR)
COMPFLAGS_AC = $(WFLAGS) -std=c++0x -I$(NDIR_AC) -I$(ADIR)
CFLAGS = $(COMPFLAGS) -O2 $(COVERAGE)
CFLAGS_AC = $(COMPFLAGS_AC) -O2 $(COVERAGE)
CDEBFLAGS = $(COMPFLAGS) -g3
NCFLAGS = $(WFLAGS) -std=c++0x -O2 $(COVERAGE)
ACFLAGS = $(WFLAGS) -std=c++0x -I$(NDIR) -O2 $(COVERAGE)
ACFLAGS_AC = $(WFLAGS) -std=c++0x -I$(NDIR_AC) -O2 $(COVERAGE)
NCDEBFLAGS = $(WFLAGS) -std=c++0x -g3
ACDEBFLAGS = $(WFLAGS) -std=c++0x -I$(NDIR) -g3
CPROFFLAGS = $(CDEBFLAGS) -pg

LD = $(CC)
LDFLAGS = -s $(COVERAGE)
LDPROFFLAGS = -pg

RM = \rm -f

# Filenames
NHDRS_AC = $(NDIR_AC)/naxos.h $(NDIR_AC)/queue.h $(NDIR_AC)/stack.h
NHDRS = $(NDIR)/internal.h $(NDIR)/naxos.h $(NDIR)/stack.h
NSRCS_AC = $(NDIR_AC)/constraints.cpp $(NDIR_AC)/expressions.cpp $(NDIR_AC)/intvar.cpp $(NDIR_AC)/problemmanager.cpp
NSRCS = $(NDIR)/array_constraints.cpp $(NDIR)/bitset_domain.cpp $(NDIR)/expressions.cpp $(NDIR)/intvar.cpp $(NDIR)/problemmanager.cpp $(NDIR)/var_constraints.cpp $(NDIR)/non_mini_solver_expressions.cpp $(NDIR)/non_mini_solver_constraints.cpp

AHDRS = $(ADIR)/amorgos.h $(ADIR)/boundedbt.h $(ADIR)/credit.h $(ADIR)/dbds.h $(ADIR)/dbs.h $(ADIR)/dfs.h $(ADIR)/domain_splitting.h $(ADIR)/fns.h $(ADIR)/gns.h $(ADIR)/heuristics.h $(ADIR)/ibroad.h $(ADIR)/isamp.h $(ADIR)/lan.h $(ADIR)/lds.h $(ADIR)/onesamp.h $(ADIR)/pops.h $(ADIR)/rdfs.h
ASRCS = $(ADIR)/boundedbt.cpp $(ADIR)/credit.cpp $(ADIR)/dbds.cpp $(ADIR)/dbs.cpp $(ADIR)/dfs.cpp $(ADIR)/domain_splitting.cpp $(ADIR)/fns.cpp $(ADIR)/gns.cpp $(ADIR)/heuristics.cpp $(ADIR)/ibroad.cpp $(ADIR)/isamp.cpp $(ADIR)/lan.cpp $(ADIR)/lds.cpp $(ADIR)/onesamp.cpp $(ADIR)/pops.cpp $(ADIR)/rdfs.cpp

HDRS = $(NHDRS) $(AHDRS) error.h heuristics.h localSearch.h timetabling.h
HDRS_AC = $(NHDRS_AC) $(AHDRS) error.h heuristics.h localSearch.h timetabling.h
SRCS = heuristics.cpp io.cpp localSearch.cpp timetabling.cpp

OBJS = $(NSRCS:.cpp=.o) $(ASRCS:.cpp=.o) $(SRCS:.cpp=.o)
OBJS_AC = $(NSRCS_AC:.cpp=.AC.o) $(ASRCS:.cpp=.AC.o) $(SRCS:.cpp=.AC.o)

PROG = itc_solver

# Building Bounds-Consistency
EXEPROG = $(PROG)

.PHONY: exe
exe: $(EXEPROG)

$(EXEPROG): $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -o $@

$(NDIR)/%.o: $(NDIR)/%.cpp $(NHDRS)
	$(CC) $(NCFLAGS) -c $< -o $@
$(ADIR)/%.o: $(ADIR)/%.cpp $(NHDRS) $(AHDRS)
	$(CC) $(ACFLAGS) -c $< -o $@
%.o: %.cpp $(HDRS)
	$(CC) $(CFLAGS) -c $<

# Building Arc-Consistency
PROG_AC = $(PROG).AC

.PHONY: AC
AC: $(PROG_AC)

$(PROG_AC): $(OBJS_AC)
	$(LD) $(LDFLAGS) $(OBJS_AC) -o $@

$(NDIR_AC)/%.AC.o: $(NDIR_AC)/%.cpp $(NHDRS_AC)
	$(CC) $(NCFLAGS) -c $< -o $@
$(ADIR)/%.AC.o: $(ADIR)/%.cpp $(NHDRS_AC) $(AHDRS)
	$(CC) $(ACFLAGS_AC) -c $< -o $@
%.AC.o: %.cpp $(HDRS_AC)
	$(CC) $(CFLAGS_AC) -c $< -o $@

# Debugging
DEBPROG = $(PROG).debug
DEBOBJS = $(OBJS:.o=.debug.o)

.PHONY: debug
debug: $(DEBPROG)

$(DEBPROG): $(DEBOBJS)
	$(LD) $(DEBOBJS) -o $@

$(NDIR)/%.debug.o: $(NDIR)/%.cpp $(NHDRS)
	$(CC) $(NCDEBFLAGS) -c $< -o $@
$(ADIR)/%.debug.o: $(ADIR)/%.cpp $(NHDRS) $(AHDRS)
	$(CC) $(ACDEBFLAGS) -c $< -o $@
%.debug.o: %.cpp $(HDRS)
	$(CC) $(CDEBFLAGS) -c $< -o $@

# Profiling
PROFPROG = $(PROG).prof
PROFOBJS = $(OBJS:.o=.prof.o)

.PHONY: prof
prof: $(PROFPROG)

$(PROFPROG): $(PROFOBJS)
	$(LD) $(LDPROFFLAGS) $(PROFOBJS) -o $@

%.prof.o: %.cpp $(HDRS)
	$(CC) $(CPROFFLAGS) -c $< -o $@

# Cleaning Up
TODEL = $(PROG) $(PROG).exe $(OBJS) $(PROG_AC) $(PROG_AC).exe $(OBJS_AC) $(DEBPROG) $(DEBPROG).exe $(DEBOBJS) $(PROFPROG) $(PROFPROG).exe $(PROFOBJS)

.PHONY: clean
clean:
	$(RM) $(TODEL)

# Testing
.PHONY: test
test:
	verification/test.sh
