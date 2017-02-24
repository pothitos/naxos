#include "rdfs.h"
#include "heuristics.h"

using namespace naxos;
using namespace std;

NsGoal* AmRdfs::GOAL(void)
{
        int* timeout = new int(timeoutLimit);
        return (new NsgOR(new AmRdfsStepping(Vars, times, timeout, timeoutLimit,
                                             varHeur, valHeur),
                          new AmRdfsDestructor(Vars, timeout)));
}

NsGoal* AmRdfsStepping::GOAL(void)
{
        if (times == 0) {
                Vars[0].removeAll(); // fail
                return 0;
        }
        // cout << "tries left: " << times << endl;
        *timeout = timeoutLimit;
        return (new NsgOR(new AmRdfsLabeling(Vars, timeout, varHeur, valHeur),
                          new AmRdfsStepping(Vars, times - 1, timeout,
                                             timeoutLimit, varHeur, valHeur)));
}

NsGoal* AmRdfsLabeling::GOAL(void)
{
        (*timeout)--;
        if (*timeout <= 0) {
                Vars[0].removeAll(); // fail
                return 0;
        }
        int index = varHeur->select(Vars);
        if (index == -1)
                return 0; // all variables are bound => success
        return (
            new NsgAND(new AmRdfsInDomain(Vars[index], timeout, valHeur),
                       new AmRdfsLabeling(Vars, timeout, varHeur, valHeur)));
}

// we need this in order to check timeout every time
NsGoal* AmRdfsInDomain::GOAL(void)
{
        (*timeout)--;
        if (*timeout <= 0) {
                Var.removeAll(); // fail
                return 0;
        }
        if (Var.isBound())
                return 0;
        NsInt value = valHeur->select(Var);
        return (new NsgOR(new NsgSetValue(Var, value),
                          new NsgAND(new NsgRemoveValue(Var, value),
                                     new AmRdfsInDomain(*this))));
}
