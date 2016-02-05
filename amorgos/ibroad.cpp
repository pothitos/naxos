#include "ibroad.h"
#include "heuristics.h"

using namespace naxos;
using namespace std;



NsGoal* goalIbroad::GOAL (void)
{
	unsigned i, numVars, currSize, maxDomainSize;

	// compute maximum domain size
	maxDomainSize=0;
	numVars = Vars.size();
	for( i=0; i<numVars; ++i ){
		currSize = Vars[i].size();
		if( maxDomainSize < currSize )
			maxDomainSize = currSize;
	}

	return new goalIbroadStepping(Vars, 1, maxDomainSize, varHeur,valHeur);
}


NsGoal* goalIbroadStepping::GOAL (void)
{
	if (currBreadthLimit > maxBreadthLimit){
		Vars[0].removeAll();	//fail
		return 0;
	}
	
	return new NsgOR(new goalIbroadLabeling(Vars, currBreadthLimit,
	                                        varHeur, valHeur),
	                 new goalIbroadStepping(Vars, currBreadthLimit+1,
	                                        maxBreadthLimit, varHeur, valHeur));
}



NsGoal* goalIbroadLabeling::GOAL (void)
{
	int index = varHeur->select(Vars);

	if (index == -1)
		return 0;		// all variables are bound => success

	return(new NsgAND(new goalIbroadInDomain(Vars[index], breadthLimit,valHeur),
	                  new goalIbroadLabeling(Vars, breadthLimit,
	                                         varHeur, valHeur)));
}


NsGoal* goalIbroadInDomain::GOAL (void)
{
	if (breadthLimit == 0){
		V.removeAll();	//fail
		return 0;
	}
		
	if (V.isBound())
		return  0;

	NsInt  value = valHeur->select(V);

	return(new NsgOR(new NsgSetValue(V, value) ,
	                 new NsgAND(new NsgRemoveValue(V, value),
	                            new goalIbroadInDomain(V, breadthLimit-1,
													   valHeur))));
}

