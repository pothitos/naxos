#include "dfs.h"
#include "heuristics.h"

using namespace naxos;
using namespace std;



NsGoal* goalDfsLabeling::GOAL (void)
{
	int  index = varHeur->select(Vars);

	if (index == -1)
		return  0;		// all variables are bound => success


	return(new NsgAND(new goalDfsInDomain(Vars[index],valHeur),
	                  new goalDfsLabeling( *this )));
}


NsGoal* goalDfsInDomain::GOAL(void)
{
	if (Var.isBound())
		return 0;
	NsInt value = valHeur->select(Var);
	
	return ( new NsgOR( new NsgSetValue(Var,value) ,
	                    new NsgAND( new NsgRemoveValue(Var,value) ,
	                                new goalDfsInDomain(*this))));
}

