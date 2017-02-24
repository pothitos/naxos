#include "dbs.h"
#include "dfs.h"
#include "heuristics.h"
#include "onesamp.h"

using namespace naxos;
using namespace std;

NsGoal* AmDbsStepping::GOAL(void)
{
        if (unsigned(depthLimit) > Vars.size()) {
                Vars[0].removeAll(); // fail
                return 0;
        }
        // cout << "depth limit: " << depthLimit << endl;
        return (new NsgOR(
            new AmDbsLabeling(Vars, depthLimit, varHeur, valHeur),
            new AmDbsStepping(Vars, depthLimit + 1, varHeur, valHeur)));
}

NsGoal* AmDbsLabeling::GOAL(void)
{
        if (depthLimit == 0)
                return (new AmOnesampLabeling(Vars, varHeur, valHeur));
        else {
                int index = varHeur->select(Vars);
                if (index == -1)
                        return 0; // all variables are bound => success
                return (new NsgAND(
                    new AmDfsInDomain(Vars[index], valHeur),
                    new AmDbsLabeling(Vars, depthLimit - 1, varHeur, valHeur)));
        }
}
