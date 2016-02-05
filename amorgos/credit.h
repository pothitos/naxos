#ifndef CREDIT_H
#define CREDIT_H

#include <naxos.h>
#include "heuristics.h"


namespace  naxos  {


class goalCredit : public NsGoal  {
private:
	NsIntVarArray&  Vars;
	int  credit;

	VariableHeuristic *varHeur;
	ValueHeuristic *valHeur;
public:
	goalCredit(NsIntVarArray& Vars_init, int cred,
	           VariableHeuristic *varHeuristic = new VarHeurMRV,
			   ValueHeuristic *valHeuristic = new ValHeurFirst)
		: Vars(Vars_init), credit(cred),
		  varHeur(varHeuristic), valHeur(valHeuristic) { }
	NsGoal*  GOAL (void);
};



} // end namespace


#endif  // CREDIT_H
