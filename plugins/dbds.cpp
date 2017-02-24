
#include "dbds.h"

using namespace naxos;
using namespace std;

NsGoal* AmDbds::GOAL(void)
{
        return new AmDbdsStepping(Vars, 0, varHeur, valHeur);
}

NsGoal* AmDbdsStepping::GOAL(void)
{
        if (depthLimit > Vars.size()) {
                Vars[0].removeAll(); // fail
                return 0;
        }
        // cout << "depth limit: " << depthLimit << endl;
        return new NsgOR(
            new AmDbdsLabeling(Vars, 0, depthLimit, varHeur, valHeur),
            new AmDbdsStepping(Vars, depthLimit + 1, varHeur, valHeur));
}

NsGoal* AmDbdsLabeling::GOAL(void)
{
        varHeur->select(Vars); // ITC
        if (currDepth > 0)
                assert(Vars[currDepth - 1].isBound());
        if ((unsigned)currDepth == Vars.size())
                return 0; // success
        // rename for convenience
        NsIntVar& V = Vars[currDepth];
        if (currDepth == depthLimit - 1) {
                // make a discrepancy and continue by heuristic
                assert(V.size() > 0);
                NsInt val = valHeur->select(V);
                return new NsgAND(
                    new NsgRemoveValue(V, val),
                    new NsgAND(new AmDfsInDomain(V, valHeur),
                               new AmDbdsLabeling(Vars, currDepth + 1,
                                                  depthLimit, varHeur,
                                                  valHeur)));
        } else if (currDepth >= depthLimit) {
                // follow heuristic all the way down
                assert(V.size() > 0);
                return new NsgAND(new AmOnesampInDomain(V, valHeur),
                                  new AmDbdsLabeling(Vars, currDepth + 1,
                                                     depthLimit, varHeur,
                                                     valHeur));
        } else {
                // explore all values
                return new NsgAND(new AmDfsInDomain(V, valHeur),
                                  new AmDbdsLabeling(Vars, currDepth + 1,
                                                     depthLimit, varHeur,
                                                     valHeur));
        }
}
