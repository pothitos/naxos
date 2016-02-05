#ifndef NS_ONESAMP_H
#define NS_ONESAMP_H


#include <naxos.h>
#include "heuristics.h"


namespace  naxos  {



class goalOnesampLabeling : public NsGoal  {
private:
	NsIntVarArray&  Vars;

	VariableHeuristic *varHeur;
	ValueHeuristic *valHeur;
public:
	goalOnesampLabeling (NsIntVarArray& Vars_init,
	                     VariableHeuristic *varHeuristic = new VarHeurFirst,
	                     ValueHeuristic *valHeuristic = new ValHeurFirst)
			: Vars(Vars_init), varHeur(varHeuristic), valHeur(valHeuristic) {}
	NsGoal*  GOAL (void);
};


class goalOnesampInDomain : public NsGoal  {
private:
	NsIntVar&  V;

	ValueHeuristic *valHeur;
public:
	goalOnesampInDomain(NsIntVar& Var, 
	                    ValueHeuristic *valHeuristic = new ValHeurFirst)
			: V(Var), valHeur(valHeuristic) {}
	NsGoal*  GOAL (void);
};


} // end namespace

#endif  // NS_ONESAMP_H

