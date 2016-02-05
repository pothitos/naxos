#ifndef NS_ISAMP_H
#define NS_ISAMP_H


#include <naxos.h>
#include "heuristics.h"


namespace  naxos  {


class goalIsampStepping : public NsGoal  {
private:
	NsIntVarArray&  Vars;
	int  nProbes;

	VariableHeuristic *varHeur;
	ValueHeuristic *valHeur;
public:
	goalIsampStepping (NsIntVarArray& Vars_init, int numProbes,
	                   VariableHeuristic *varHeuristic = new VarHeurMRV,
	                   ValueHeuristic *valHeuristic = new ValHeurRand)
		  : Vars(Vars_init), nProbes(numProbes), varHeur(varHeuristic),
		    valHeur(valHeuristic) {}

	NsGoal*  GOAL (void);
};


} // end namespace


#endif  // NS_ISAMP_H

