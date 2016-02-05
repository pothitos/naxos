#ifndef NS_DFS_H
#define NS_DFS_H


#include <naxos.h>
#include "heuristics.h"


namespace  naxos  {



class goalDfsLabeling : public NsGoal  {
private:
	NsIntVarArray&  Vars;

	VariableHeuristic *varHeur;
	ValueHeuristic *valHeur;
public:
	goalDfsLabeling(NsIntVarArray& Vars_init,
	                VariableHeuristic *varHeuristic = new VarHeurMRV,
					ValueHeuristic *valHeuristic = new ValHeurFirst)
			: Vars(Vars_init), varHeur(varHeuristic), valHeur(valHeuristic) {}
	NsGoal*  GOAL (void);
};


class goalDfsInDomain : public NsGoal {
private:
	NsIntVar& Var;

	ValueHeuristic *valHeur;
public:
	goalDfsInDomain (NsIntVar& Var_init,
	                 ValueHeuristic *valHeuristic = new ValHeurFirst)
			: Var(Var_init), valHeur(valHeuristic) {}
	NsGoal* GOAL(void);
};


} // end namespace

#endif  // NS_DFS_H
