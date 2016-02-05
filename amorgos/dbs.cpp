#include "dbs.h"
#include "dfs.h"
#include "heuristics.h"
#include "onesamp.h"

using namespace naxos;
using namespace std;



NsGoal* goalDbsStepping::GOAL (void)
{
	if ( unsigned(depthLimit) > Vars.size() )   {
		Vars[0].removeAll();	// fail
		return  0;
	}

	// cout << "depth limit: " << depthLimit << endl;

	return(new NsgOR(new goalDbsLabeling(Vars,depthLimit,varHeur,valHeur) ,
	                 new goalDbsStepping(Vars,depthLimit + 1,varHeur,valHeur)));
}



NsGoal* goalDbsLabeling::GOAL (void)
{
	if (depthLimit == 0)
		return( new goalOnesampLabeling( Vars, varHeur, valHeur )) ;
	else{
		int  index = varHeur->select(Vars);

		if (index == -1)
			return  0;		// all variables are bound => success


		return(new NsgAND(new goalDfsInDomain(Vars[index], valHeur),
		                  new goalDbsLabeling(Vars, depthLimit-1,
		                                      varHeur, valHeur)));
	}
}


