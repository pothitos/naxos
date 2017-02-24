#include <amorgos.h>
#include <naxos.h>

#include <cstdlib>
#include <iostream>

using namespace std;
using namespace naxos;

int main(int argc, char* argv[])
{
        try {
                // number of numbers
                int N = (argc > 1) ? atoi(argv[1]) : 15;
                // maximum number
                int Max = (argc > 2) ? atoi(argv[2]) : 100;
                // allowed difference
                int D = (argc > 3) ? atoi(argv[3]) : 5;
                int i;
                // construct a random sequence of N positive numbers
                srand(time(NULL));
                int* numbers = new int[N];
                for (i = 0; i < N; ++i)
                        numbers[i] = rand() % Max + 1;
                NsProblemManager pm;
                // PROBLEM STATEMENT //
                NsIntVarArray Choice, Bucket1, Bucket2;
                NsIntVar sum1(pm, N, N * Max), sum2(pm, N, N * Max);
                // 0: numbers[i] in the 1st "bucket"
                // 1: numbers[i] in the 2nd "bucket"
                for (i = 0; i < N; ++i)
                        Choice.push_back(NsIntVar(pm, 0, 1));
                for (i = 0; i < N; ++i) {
                        Bucket1.push_back(Choice[i] * numbers[i]);
                        Bucket2.push_back((1 - Choice[i]) * numbers[i]);
                }
                pm.add(NsSum(Bucket1) == sum1);
                pm.add(NsSum(Bucket2) == sum2);
                pm.add(sum1 - sum2 <= D);
                pm.add(sum2 - sum1 <= D);
                // GOALS //
                pm.addGoal(new AmDfsLabeling(Choice));
                // SOLVING //
                if (pm.nextSolution()) {
                        cout << "Numbers in 1st bucket: (sum: " << sum1 << ")"
                             << endl;
                        for (i = 0; i < N; ++i)
                                if (Choice[i].value())
                                        cout << numbers[i] << " ";
                        cout << endl;
                        cout << "Numbers in 2nd bucket: (sum: " << sum2 << ")"
                             << endl;
                        for (i = 0; i < N; ++i)
                                if (!Choice[i].value())
                                        cout << numbers[i] << " ";
                        cout << endl;
                }
        } catch (exception& exc) {
                cerr << exc.what() << "\n";
        } catch (...) {
                cerr << "Unknown exception"
                     << "\n";
        }
}
