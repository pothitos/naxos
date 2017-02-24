#include <amorgos.h>
#include <naxos.h>

#include <cstdlib>
#include <iostream>

#define N 15

using namespace std;
using namespace naxos;

enum { attiki,
       viotia,
       fthiotida,
       fokida,
       evritania,
       etoloakarnania,
       evia,
       magnisia,
       larisa,
       karditsa,
       trikala,
       arta,
       preveza,
       ioannina,
       thesprotia };

int main()
{
        try {
                NsProblemManager pm;
                // PROBLEM STATEMENT //
                NsIntVarArray Var;
                for (int i = 0; i < N; ++i)
                        Var.push_back(NsIntVar(pm, 0, 3));
                // bordering nomoi
                pm.add(Var[etoloakarnania] != Var[arta]);
                pm.add(Var[etoloakarnania] != Var[karditsa]);
                pm.add(Var[etoloakarnania] != Var[evritania]);
                pm.add(Var[etoloakarnania] != Var[fokida]);
                pm.add(Var[etoloakarnania] != Var[fthiotida]);
                pm.add(Var[fokida] != Var[fthiotida]);
                pm.add(Var[fokida] != Var[viotia]);
                pm.add(Var[viotia] != Var[fthiotida]);
                pm.add(Var[viotia] != Var[evia]);
                pm.add(Var[viotia] != Var[attiki]);
                pm.add(Var[evritania] != Var[karditsa]);
                pm.add(Var[evritania] != Var[fthiotida]);
                pm.add(Var[fthiotida] != Var[karditsa]);
                pm.add(Var[fthiotida] != Var[magnisia]);
                pm.add(Var[magnisia] != Var[larisa]);
                pm.add(Var[larisa] != Var[karditsa]);
                pm.add(Var[larisa] != Var[trikala]);
                pm.add(Var[karditsa] != Var[arta]);
                pm.add(Var[karditsa] != Var[trikala]);
                pm.add(Var[trikala] != Var[arta]);
                pm.add(Var[trikala] != Var[ioannina]);
                pm.add(Var[arta] != Var[preveza]);
                pm.add(Var[arta] != Var[ioannina]);
                pm.add(Var[preveza] != Var[thesprotia]);
                pm.add(Var[preveza] != Var[ioannina]);
                pm.add(Var[ioannina] != Var[thesprotia]);
                // GOALS //
                pm.addGoal(new AmDfsLabeling(Var));
                // SOLVING //
                while (pm.nextSolution() != false)
                        cout << "Solution: " << Var << "\n";
        } catch (exception& exc) {
                cerr << exc.what() << "\n";
        } catch (...) {
                cerr << "Unknown exception"
                     << "\n";
        }
}
