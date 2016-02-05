#ifndef NS_LDS_H
#define NS_LDS_H


#include <naxos.h>
#include "heuristics.h"
#include "dfs.h"

namespace  naxos  {



class goalLds : public NsGoal  {
private:
	NsIntVarArray&  Vars;
	int  minDiscr;
	int step, maxDiscr, lookAhead;

	VariableHeuristic *varHeur;
	ValueHeuristic *valHeur;
public:
	goalLds(NsIntVarArray& Vars_init, int lStep=1, int lAhead=0,
	        int minDiscrepancy=0, int maxDiscrepancy=0,
	        VariableHeuristic *varHeuristic = new VarHeurMRV,
	        ValueHeuristic *valHeuristic = new ValHeurFirst)
	  : Vars(Vars_init), minDiscr(minDiscrepancy),
	    step(lStep), maxDiscr(maxDiscrepancy), lookAhead(lAhead),
		varHeur(varHeuristic), valHeur(valHeuristic) { }
   NsGoal*  GOAL (void);
};



class goalLdsStepping : public NsGoal  {
private:
	NsIntVarArray&  Vars;
	int  minDiscr;
	NsIntVar&  vDiscr;
	int step, maxDiscr, lookAhead;

	VariableHeuristic *varHeur;
	ValueHeuristic *valHeur;
public:
	goalLdsStepping(NsIntVarArray& Vars_init, int minDiscrepancy,
	                NsIntVar& vDiscrepancy, int lStep, int maxDiscrep,
					int lAhead,
					VariableHeuristic *varHeuristic = new VarHeurMRV,
	                ValueHeuristic *valHeuristic = new ValHeurFirst)
	  : Vars(Vars_init), minDiscr(minDiscrepancy), vDiscr(vDiscrepancy),
	    step(lStep), maxDiscr(maxDiscrep), lookAhead(lAhead),
		varHeur(varHeuristic), valHeur(valHeuristic) { }

	NsGoal*  GOAL (void);
};


class goalLdsLabeling : public NsGoal  {
private:
	NsIntVarArray&  Vars;
	int  minDiscr;
	NsIntVar&  vDiscr;
	int step, lookAhead;

	VariableHeuristic *varHeur;
	ValueHeuristic *valHeur;
public:
	goalLdsLabeling(NsIntVarArray& Vars_init, int minDiscrepancy,
	                NsIntVar& vDiscrepancy, int lStep = 1, int lAhead = 0,
	                VariableHeuristic *varHeuristic = new VarHeurMRV,
	                ValueHeuristic *valHeuristic = new ValHeurFirst)
	  : Vars(Vars_init), minDiscr(minDiscrepancy), vDiscr(vDiscrepancy),
	    step(lStep), lookAhead(lAhead),
		varHeur(varHeuristic), valHeur(valHeuristic) { }

	NsGoal*  GOAL (void);
};


class goalLdsInDomain : public NsGoal  {
private:
	NsIntVarArray&  Vars;
	const int  index;

	int  minDiscr;
	NsIntVar&  vDiscr;
	int step, lookAhead;

	VariableHeuristic *varHeur;
	ValueHeuristic *valHeur;
public:
	goalLdsInDomain(NsIntVarArray& Vars_init, int index_init,
	                int minDiscrepancy, NsIntVar& vDiscrepancy, int lStep,
	                int lAhead, VariableHeuristic *varHeuristic,
	                ValueHeuristic *valHeuristic)
	  : Vars(Vars_init), index(index_init), minDiscr(minDiscrepancy),
	    vDiscr(vDiscrepancy), step(lStep), lookAhead(lAhead),
		varHeur(varHeuristic), valHeur(valHeuristic) { }

	NsGoal*  GOAL (void);
};


class goalLdsRemoveValue : public NsGoal  {
private:
	NsIntVar&  V;
	const NsInt  value;
	NsIntVar&  vDiscr;

public:
	goalLdsRemoveValue (NsIntVar& V_init, const NsInt value_init,
	                    NsIntVar& vDiscrepancy)
	  : V(V_init), value(value_init), vDiscr(vDiscrepancy)    {    }

	NsGoal*  GOAL (void);
};



class goalLdsDestructor : public NsGoal  {
private:
	NsIntVarArray&  Vars;
	NsIntVar *vDiscr;

public:
	goalLdsDestructor(NsIntVarArray& Vars_init, NsIntVar *vDiscrepancy)
			: Vars(Vars_init), vDiscr(vDiscrepancy) {  }

	NsGoal*  GOAL (void)
	{
		Vars[0].removeAll();
		return  0;
	}

	~goalLdsDestructor (void)
	{
		delete  vDiscr;
	}
}; 




int  remainingChoices (const NsIntVarArray& VarArr);


} // end namespace



#endif  // NS_LDS_H
