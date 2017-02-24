#include "lds.h"

using namespace naxos;
using namespace std;

//  `vDiscr.min()' represents the current number of discrepancies.  //

NsGoal* AmLds::GOAL(void)
{
        NsIntVar* vDiscr =
            new NsIntVar(Vars[0].manager(), 0, remainingChoices(Vars));
        // if maxDiscr == 0 then it is disabled (no upper discrepancy bound)
        if (maxDiscr == 0)
                maxDiscr = remainingChoices(Vars);
        return (
            new NsgOR(new AmLdsStepping(Vars, minDiscr, *vDiscr, step, maxDiscr,
                                        lookAhead, varHeur, valHeur),
                      new AmLdsDestructor(Vars, vDiscr)));
}

NsGoal* AmLdsStepping::GOAL(void)
{
        // cout << " > AmLdsStepping\t(minDiscr=" << minDiscr ;
        // cout << ", vDiscr=" << vDiscr << ",\n\t\t\t Vars=" << Vars << ")\n";
        if (minDiscr + step - 1 >= maxDiscr) {
                Vars[0].removeAll(); // reached maximum discrepancy => FAILURE
                return 0;
        }
        return new NsgOR(new AmLdsLabeling(Vars, minDiscr, vDiscr, step,
                                           lookAhead, varHeur, valHeur),
                         new AmLdsStepping(Vars, (minDiscr + step), vDiscr,
                                           step, maxDiscr, lookAhead, varHeur,
                                           valHeur));
}

NsGoal* AmLdsLabeling::GOAL(void)
{
        // cout << " > > AmLdsLabeling\t(minDiscr=" << minDiscr;
        // cout << ", vDiscr=" << vDiscr  << ",\n\t\t\t Vars=" << Vars << ")\n";
        if (remainingChoices(Vars) + vDiscr.min() < minDiscr) {
                // cout << " > > AmLdsLabeling:  Failure!
                // remainingChoices(Vars)=" ;  cout << remainingChoices(Vars) <<
                // "\n";
                Vars[0].removeAll(); // minimum discrepancy exceeded => FAILURE
                return 0;
        }
        //  Above, `minDiscr' is used in order to avoid visiting the
        //   same leaves again--after a "stepping" has taken place.
        //   Thus, the method is Improved LDS, rather than plain LDS.
        int index = varHeur->select(Vars);
        if (index == -1)
                return 0; // all variables are bound => success
        return (new NsgAND(new AmLdsInDomain(Vars, index, minDiscr, vDiscr,
                                             step, lookAhead, varHeur, valHeur),
                           new AmLdsLabeling(*this)));
}

NsGoal* AmLdsInDomain::GOAL(void)
{
        // cout << " > > > AmLdsInDomain\t(minDiscr=" << minDiscr;
        // cout << ", vDiscr=" << vDiscr  << ",\n\t\t\t Vars=" << Vars << ")\n";
        NsIntVar& V = Vars[index]; // renaming for readability
        if (V.isBound())
                return 0;
        NsInt value = valHeur->select(V);
        int remChoices = remainingChoices(Vars);
        if (remChoices - static_cast<int>(V.size() - 1) + vDiscr.min() <
            minDiscr) {
                //  We must "consume" some values from `V', in order not to
                //   "lose" the `minDiscr' bound, the moment the next
                //   `AmLdsLabeling' will be executed.
                // cout << " > > > AmLdsInDomain:  Consuming\n";
                return (new NsgAND(new AmLdsRemoveValue(V, value, vDiscr),
                                   new AmLdsInDomain(*this)));
        }
        if (vDiscr.min() == minDiscr + step - 1) {
                //  We have reached the limit of discrepancies, so the only
                //  thing
                //   to do is to assign a value to the variable,
                //   and not to consume another discrepancy.
                if (remChoices < lookAhead) {
                        // DFS  (we start it from here because we have already
                        //  chosen a variable and a value)
                        return (
                            new NsgOR(new NsgSetValue(V, value),
                                      new NsgAND(new NsgRemoveValue(V, value),
                                                 new AmDfsLabeling(
                                                     Vars, varHeur, valHeur))));
                } else {
                        // cout << " > > > AmLdsInDomain:  Setting value\n";
                        return (new NsgSetValue(V, value));
                }
        }
        if (vDiscr.min() >= minDiscr + step - 1)
                throw logic_error("AmLdsInDomain::GOAL: Too much discrepancy");
        // cout << " > > > AmLdsInDomain:  Setting value with alternative\n";
        return (new NsgOR(new NsgSetValue(V, value),
                          new NsgAND(new AmLdsRemoveValue(V, value, vDiscr),
                                     new AmLdsInDomain(*this))));
}

// Each time we remove a value, a discrepancy is made.
NsGoal* AmLdsRemoveValue::GOAL(void)
{
        // cout << " > > > > AmLdsRemoveValue\n";
        // !!! discrepancy increased by one !!! //
        vDiscr.remove(vDiscr.min());
        V.remove(value);
        return 0;
}

int naxos::remainingChoices(const NsIntVarArray& VarArr)
{
        int remChoices = 0;
        for (NsIntVarArray::const_iterator V = VarArr.begin();
             V != VarArr.end(); ++V)
                remChoices += V->size() - 1;
        //  If we had a singleton variable (i.e. a variable with one only
        //   value in its domain), then the remaining values for it would be:
        //   V->size() - 1 = 1 - 1 = 0.
        return remChoices;
}
