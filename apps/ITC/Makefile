### COMPILING AND LINKING OPTIONS ###
# Uncomment the following line, for gcc versions greater than 4.2.
STANDARD = -std=c++0x

NDIR = ../../core/bounds-consistency
NDIR_AC = ../../naxos-AC
ADIR = ../../amorgos

CC = $(CPATH)g++
WFLAGS = -pedantic -Wall -W -Wshadow -Werror
COMPFLAGS = $(WFLAGS) $(STANDARD) -I$(NDIR) -I$(ADIR)
COMPFLAGS_AC = $(WFLAGS) $(STANDARD) -I$(NDIR_AC) -I$(ADIR)
CFLAGS = $(COMPFLAGS) -O2 $(COVERAGE)
CFLAGS_AC = $(COMPFLAGS_AC) -O2 $(COVERAGE)
CDEBFLAGS = $(COMPFLAGS) -g3
NCFLAGS = $(WFLAGS) $(STANDARD) -O2 $(COVERAGE)
ACFLAGS = $(WFLAGS) $(STANDARD) -I$(NDIR) -O2 $(COVERAGE)
ACFLAGS_AC = $(WFLAGS) $(STANDARD) -I$(NDIR_AC) -O2 $(COVERAGE)
NCDEBFLAGS = $(WFLAGS) $(STANDARD) -g3
ACDEBFLAGS = $(WFLAGS) $(STANDARD) -I$(NDIR) -g3
CPROFFLAGS = $(CDEBFLAGS) -pg

LD = $(CC)
LDFLAGS = -s $(COVERAGE)
LDPROFFLAGS = -pg

RM = \rm -f


### SOURCE AND OUTPUT FILENAMES ###
NHDRS_AC = $(NDIR_AC)/naxos.h $(NDIR_AC)/queue.h $(NDIR_AC)/stack.h
NHDRS = $(NDIR)/naxos.h $(NDIR)/internal.h $(NDIR)/stack.h
NSRCS_AC = $(NDIR_AC)/problemmanager.cpp $(NDIR_AC)/intvar.cpp $(NDIR_AC)/expressions.cpp $(NDIR_AC)/constraints.cpp
NSRCS = $(NDIR)/problemmanager.cpp $(NDIR)/intvar.cpp $(NDIR)/bitset_domain.cpp $(NDIR)/expressions.cpp $(NDIR)/var_constraints.cpp $(NDIR)/array_constraints.cpp

AHDRS = $(ADIR)/pops.h $(ADIR)/domain_splitting.h $(ADIR)/amorgos.h $(ADIR)/boundedbt.h $(ADIR)/credit.h $(ADIR)/dbds.h $(ADIR)/dbs.h $(ADIR)/dfs.h $(ADIR)/fns.h $(ADIR)/gns.h $(ADIR)/heuristics.h $(ADIR)/ibroad.h $(ADIR)/isamp.h $(ADIR)/lan.h $(ADIR)/lds.h $(ADIR)/onesamp.h $(ADIR)/rdfs.h
ASRCS = $(ADIR)/pops.cpp $(ADIR)/domain_splitting.cpp $(ADIR)/boundedbt.cpp $(ADIR)/credit.cpp $(ADIR)/dbds.cpp $(ADIR)/dbs.cpp $(ADIR)/dfs.cpp $(ADIR)/fns.cpp $(ADIR)/gns.cpp $(ADIR)/heuristics.cpp $(ADIR)/ibroad.cpp $(ADIR)/isamp.cpp $(ADIR)/lan.cpp $(ADIR)/lds.cpp $(ADIR)/onesamp.cpp $(ADIR)/rdfs.cpp

HDRS = $(NHDRS) $(AHDRS) timetabling.h heuristics.h error.h localSearch.h
HDRS_AC = $(NHDRS_AC) $(AHDRS) timetabling.h heuristics.h error.h localSearch.h
SRCS = timetabling.cpp heuristics.cpp io.cpp localSearch.cpp

OBJS = $(NSRCS:.cpp=.o) $(ASRCS:.cpp=.o) $(SRCS:.cpp=.o)
OBJS_AC = $(NSRCS_AC:.cpp=.AC.o) $(ASRCS:.cpp=.AC.o) $(SRCS:.cpp=.AC.o)

PROG = itc_solver


### BUILDING ###
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


### BUILDING AC ###
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


### DEBUGGING ###
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


### PROFILING ###
PROFPROG = $(PROG).prof
PROFOBJS = $(OBJS:.o=.prof.o)

.PHONY: prof
prof: $(PROFPROG)

$(PROFPROG): $(PROFOBJS)
	$(LD) $(LDPROFFLAGS) $(PROFOBJS) -o $@

%.prof.o: %.cpp $(HDRS)
	$(CC) $(CPROFFLAGS) -c $< -o $@


### CLEANING UP ###
TODEL = $(PROG) $(PROG).exe $(OBJS) $(PROG_AC) $(PROG_AC).exe $(OBJS_AC) $(DEBPROG) $(DEBPROG).exe $(DEBOBJS) $(PROFPROG) $(PROFPROG).exe $(PROFOBJS)

.PHONY: clean
clean:
	$(RM) $(TODEL)

### TESTING ###
.PHONY: test
test:
	# Compile the official validator
	g++ verification/validator.cc -o verification/validator
	# Find and configure the available time for searching a solution
	AVAILABLE_SECONDS=$(shell cd datasets && \
		yes | ./benchmark_my_linux_machine | \
		grep "seconds" | grep -o "[[:digit:]]*") && \
	sed -i "s/^\(timeLimit\) .*/\1 $$AVAILABLE_SECONDS/" optionsFile && \
	sed -i "s/^\(timeLimitDirectMethodRound\) .*/\1 $$AVAILABLE_SECONDS/" optionsFile
	# Execute the curriculum based course timetabling solver
	./itc_solver datasets/ITC01.ctt ITC01.sol -options optionsFile
	# Validate the last solution found
	verification/validator datasets/ITC01.ctt ITC01.sol
