#include "isamp.h"
#include "onesamp.h"
#include "heuristics.h"

using namespace naxos;
using namespace std;



NsGoal* goalIsampStepping::GOAL (void)
{
	if (nProbes == 0) {
		Vars[0].removeAll();		// exceeded number of tries => FAILURE
		return  0;
	}

	// cout << "Tries left: " << nProbes << endl;

	/* update remaining tries */
	if (nProbes > 0) --nProbes;

	return (new NsgOR( new goalOnesampLabeling(Vars, varHeur, valHeur),
	                   new goalIsampStepping( *this )));
}

