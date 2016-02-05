#include "boundedbt.h"
#include "heuristics.h"

using namespace naxos;
using namespace std;



NsGoal*  goalBbs::GOAL (void)
{
	int *maxLevel = new int;
	return (new NsgOR(new goalBbsStepping(Vars,lookaheadLimit,times,*maxLevel,
	                                      varHeur, valHeur),
	                  new goalBbsDestructor(Vars,maxLevel)));
}



NsGoal* goalBbsStepping::GOAL (void)
{
	if (times == 0)   {
		Vars[0].removeAll();	// fail
		return  0;
	}

	maxLevel = 0;

	// cout << "tries left: " << times << endl;

	return(new NsgOR(new goalBbsLabeling(Vars, lookaheadLimit, maxLevel,
	                                     varHeur, valHeur),
	                 new goalBbsStepping(Vars,lookaheadLimit,times-1,maxLevel,
	                                     varHeur, valHeur)));
}



NsGoal* goalBbsLabeling::GOAL (void)
{
	// compute current level
	int level = 0;
	unsigned int i;
	for (i=0; i<Vars.size(); ++i)
		if(Vars[i].isBound())
			level++;

	// cout << level << " - " << maxLevel << " - " << lookahead << endl;

	if (level < maxLevel - lookahead){
		Vars[0].removeAll();	//fail
		return 0;
	}	

	if (level > maxLevel)
		maxLevel=level;


	int  index = varHeur->select(Vars);

	if (index == -1)
		return  0;		// all variables are bound => success

	return(new NsgAND(new goalDfsInDomain(Vars[index], valHeur),
	                  new goalBbsLabeling(Vars, lookahead, maxLevel,
	                                      varHeur, valHeur)));
}


