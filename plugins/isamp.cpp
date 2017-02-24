#include "isamp.h"
#include "heuristics.h"
#include "onesamp.h"

using namespace naxos;
using namespace std;

NsGoal* AmIsampStepping::GOAL(void)
{
        if (nProbes == 0) {
                Vars[0].removeAll(); // exceeded number of tries => FAILURE
                return 0;
        }
        // cout << "Tries left: " << nProbes << endl;
        /* update remaining tries */
        if (nProbes > 0)
                --nProbes;
        return (new NsgOR(new AmOnesampLabeling(Vars, varHeur, valHeur),
                          new AmIsampStepping(*this)));
}
