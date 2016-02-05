#ifndef NS_IBROAD_H
#define NS_IBROAD_H


#include <naxos.h>
#include "heuristics.h"

namespace naxos {


class goalIbroad : public NsGoal {
private:
	NsIntVarArray& Vars;

	VariableHeuristic *varHeur;
	ValueHeuristic *valHeur;
public:
	goalIbroad (NsIntVarArray& Vars_init,
	            VariableHeuristic *varHeuristic = new VarHeurMRV,
	            ValueHeuristic *valHeuristic = new ValHeurFirst)
		  : Vars(Vars_init), varHeur(varHeuristic), valHeur(valHeuristic) {}

	NsGoal*  GOAL (void);
};	


class goalIbroadStepping : public NsGoal {
private:
	NsIntVarArray& Vars;
	unsigned currBreadthLimit, maxBreadthLimit;

	VariableHeuristic *varHeur;
	ValueHeuristic *valHeur;
public:
	goalIbroadStepping (NsIntVarArray& Vars_init, unsigned currBLimit,
	                    unsigned maxBLimit,
	                    VariableHeuristic *varHeuristic = new VarHeurMRV,
	                    ValueHeuristic *valHeuristic = new ValHeurFirst)
		  : Vars(Vars_init), currBreadthLimit(currBLimit),
		    maxBreadthLimit(maxBLimit),
		    varHeur(varHeuristic), valHeur(valHeuristic) {}

	NsGoal*  GOAL (void);
};	



class goalIbroadLabeling : public NsGoal {
private:
	NsIntVarArray& Vars;
	unsigned breadthLimit;

	VariableHeuristic *varHeur;
	ValueHeuristic *valHeur;
public:
	goalIbroadLabeling (NsIntVarArray& Vars_init, unsigned bLimit,
	                    VariableHeuristic *varHeuristic = new VarHeurMRV,
						ValueHeuristic *valHeuristic = new ValHeurFirst)
		  : Vars(Vars_init), breadthLimit(bLimit),
		    varHeur(varHeuristic), valHeur(valHeuristic) {}

	NsGoal*  GOAL (void);
};


class goalIbroadInDomain : public NsGoal {
private:
	NsIntVar&  V;
	unsigned breadthLimit;

	ValueHeuristic *valHeur;
public:
	goalIbroadInDomain(NsIntVar& V_init, unsigned bLimit,
	                   ValueHeuristic *valHeuristic = new ValHeurFirst)
	  : V(V_init), breadthLimit(bLimit), valHeur(valHeuristic) {}

	NsGoal*  GOAL (void);
};


} // end namespace


#endif  // NS_IBROAD_H
