#ifndef NS_GNS_H
#define NS_GNS_H


#include <naxos.h>
#include "heuristics.h"


namespace  naxos  {



class goalGnsLabeling : public NsGoal  {
private:
	NsIntVarArray&  Vars;

	VariableHeuristic *varHeur;
	ValueHeuristic *valHeur;
public:
	goalGnsLabeling(NsIntVarArray& Vars_init,
	                VariableHeuristic *varHeuristic = new VarHeurMRV,
					ValueHeuristic *valHeuristic = new ValHeurFirst)
			: Vars(Vars_init), varHeur(varHeuristic), valHeur(valHeuristic) {}
	NsGoal*  GOAL (void);
};


} // end namespace

#endif  // NS_GNS_H
