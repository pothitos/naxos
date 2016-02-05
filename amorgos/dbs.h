#ifndef NS_DBS_H
#define NS_DBS_H


#include <naxos.h>
#include "heuristics.h"


namespace  naxos  {


class goalDbsStepping : public NsGoal  {
private:
	NsIntVarArray&  Vars;
	int depthLimit;

	VariableHeuristic *varHeur;
	ValueHeuristic *valHeur;
public:
	goalDbsStepping(NsIntVarArray& Vars_init, int limit=0,
	                VariableHeuristic *varHeuristic = new VarHeurMRV,
	                ValueHeuristic *valHeuristic = new ValHeurFirst)
			: Vars(Vars_init), varHeur(varHeuristic), valHeur(valHeuristic)
			{ depthLimit = limit; }
	NsGoal*  GOAL (void);
};


class goalDbsLabeling : public NsGoal  {
private:
	NsIntVarArray&  Vars;
	int depthLimit;

	VariableHeuristic *varHeur;
	ValueHeuristic *valHeur;
public:
	goalDbsLabeling(NsIntVarArray& Vars_init, int limit, 
	                VariableHeuristic *varHeuristic = new VarHeurMRV,
	                ValueHeuristic *valHeuristic = new ValHeurFirst)
			: Vars(Vars_init), depthLimit(limit),
			  varHeur(varHeuristic), valHeur(valHeuristic) {}
	NsGoal*  GOAL (void);
};


} // end namespace



#endif  // NS_DBS_H
