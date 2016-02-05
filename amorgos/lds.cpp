#include "lds.h"

using namespace naxos;
using namespace std;



//  `vDiscr.min()' represents the current number of discrepancies.  //


NsGoal*  goalLds::GOAL (void)
{
	NsIntVar  *vDiscr = new NsIntVar(Vars[0].manager(), 0,
	                                 remainingChoices(Vars));
	
	// if maxDiscr == 0 then it is disabled (no upper discrepancy bound)
	if(maxDiscr == 0)
		maxDiscr = remainingChoices(Vars);
	
	return (new NsgOR(new goalLdsStepping(Vars,minDiscr,*vDiscr,step,
	                                      maxDiscr, lookAhead,
	                                      varHeur, valHeur),
	                  new goalLdsDestructor(Vars, vDiscr)));
}


NsGoal* goalLdsStepping::GOAL (void)
{
	//cout << " > goalLdsStepping\t(minDiscr=" << minDiscr ;
	//cout << ", vDiscr=" << vDiscr << ",\n\t\t\t Vars=" << Vars << ")\n";

	if (minDiscr + step-1 >= maxDiscr)   {
		Vars[0].removeAll();		// reached maximum discrepancy => FAILURE
		return  0;
	}
	return new NsgOR(new goalLdsLabeling(Vars, minDiscr, vDiscr, step,
	                                     lookAhead, varHeur, valHeur) ,
	                 new goalLdsStepping(Vars, (minDiscr + step), vDiscr, step,
	                                     maxDiscr, lookAhead, varHeur,valHeur));
}



NsGoal* goalLdsLabeling::GOAL (void)
{
	//cout << " > > goalLdsLabeling\t(minDiscr=" << minDiscr;
	//cout << ", vDiscr=" << vDiscr  << ",\n\t\t\t Vars=" << Vars << ")\n";

	if (remainingChoices(Vars) + vDiscr.min() < minDiscr)   {
		//cout << " > > goalLdsLabeling:  Failure! remainingChoices(Vars)=" ;
		//cout << remainingChoices(Vars) << "\n";

		Vars[0].removeAll();	// minimum discrepancy exceeded => FAILURE
		return  0;
	}
	
	//  Above, `minDiscr' is used in order to avoid visiting the
	//   same leaves again--after a "stepping" has taken place.
	//   Thus, the method is Improved LDS, rather than plain LDS.

	int  index = varHeur->select(Vars);
	if (index == -1)
		return  0;		// all variables are bound => success

	return (new NsgAND(new goalLdsInDomain(Vars, index, minDiscr, vDiscr,
	                                       step, lookAhead, varHeur, valHeur),
	                   new goalLdsLabeling(*this) ) );
}



NsGoal* goalLdsInDomain::GOAL (void)
{
	//cout << " > > > goalLdsInDomain\t(minDiscr=" << minDiscr;
	//cout << ", vDiscr=" << vDiscr  << ",\n\t\t\t Vars=" << Vars << ")\n";

	NsIntVar&  V = Vars[index];		// renaming for readability

	if (V.isBound())
		return  0;
	NsInt  value = valHeur->select(V);
	int   remChoices = remainingChoices(Vars);


	if (remChoices - static_cast<int>(V.size()-1) + vDiscr.min()  <  minDiscr){
		//  We must "consume" some values from `V', in order not to
		//   "lose" the `minDiscr' bound, the moment the next
		//   `goalLdsLabeling' will be executed.

		//cout << " > > > goalLdsInDomain:  Consuming\n";

		return  ( new NsgAND( new goalLdsRemoveValue(V, value, vDiscr),
		                      new goalLdsInDomain(*this) ) );
	}


	if (vDiscr.min() == minDiscr + step-1)   {
		//  We have reached the limit of discrepancies, so the only thing
		//   to do is to assign a value to the variable,
		//   and not to consume another discrepancy.

		if (remChoices < lookAhead)   {
			// DFS  (we start it from here because we have already
			//  chosen a variable and a value)

			return (new NsgOR(new NsgSetValue(V, value) ,
			                  new NsgAND(new NsgRemoveValue(V, value) ,
			                             new goalDfsLabeling(Vars, varHeur,
			                                                 valHeur))));
		} else {
			//cout << " > > > goalLdsInDomain:  Setting value\n";

			return  ( new NsgSetValue(V, value) );
		}
	}

	if ( vDiscr.min() >= minDiscr + step-1 )
	           throw  logic_error("goalLdsInDomain::GOAL: Too much discrepancy");

	//cout << " > > > goalLdsInDomain:  Setting value with alternative\n";

	return  ( new NsgOR( new NsgSetValue(V, value) ,
	                     new NsgAND( new goalLdsRemoveValue(V, value, vDiscr) ,
	                                 new goalLdsInDomain(*this) ) ) );
}


// Each time we remove a value, a discrepancy is made.
NsGoal* goalLdsRemoveValue::GOAL (void)
{
	//cout << " > > > > goalLdsRemoveValue\n";

	vDiscr.remove( vDiscr.min() );	// !!! discrepancy increased by one !!! //
	V.remove(value);
	return  0;
}


int naxos::remainingChoices (const NsIntVarArray& VarArr)
{
	int  remChoices = 0;
	for (NsIntVarArray::const_iterator V=VarArr.begin(); V != VarArr.end(); ++V)
		remChoices += V->size() - 1;
		
		//  If we had a singleton variable (i.e. a variable with one only
		//   value in its domain), then the remaining values for it would be:
		//   V->size() - 1 = 1 - 1 = 0.

	return  remChoices;
}

