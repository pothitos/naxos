#include <amorgos.h>
#include <naxos.h>

#include <cstdlib>
#include <iostream>

using namespace std;
using namespace naxos;

struct Rectangle {
        int x, y;
        NsIntVar *minX, *minY;
};

void printSolution(Rectangle bigRect, Rectangle* rects, int N);

///  Takes care to instantiate the variables in couples.

class VarHeurCouples : public VariableHeuristic {

    public:
        int select(const naxos::NsIntVarArray& Vars)
        {
                int minIndex = -1;
                NsUInt minDom = NsUPLUS_INF;
                for (NsIndex i = 0; i < Vars.size(); i += 2) {
                        if (Vars[i].isBound() && !Vars[i + 1].isBound())
                                return (i + 1);
                        if (!Vars[i].isBound() &&
                            Vars[i].size() * Vars[i + 1].size() < minDom) {
                                minDom = Vars[i].size() * Vars[i + 1].size();
                                minIndex = i;
                        }
                }
                return minIndex;
        }
};

int main(int argc, char* argv[])
{
        try {
                Rectangle bigRect;
                // x-size of big rectangle
                bigRect.x = (argc > 1) ? atoi(argv[1]) : 20;
                // y-size of big rectangle
                bigRect.y = (argc > 2) ? atoi(argv[2]) : 10;
                // NsIntVars of big rectangle not used
                bigRect.minX = bigRect.minY = NULL;
                // number of small rectangles
                int N = (argc > 3) ? atoi(argv[3]) : 12;
                // maximum size of small rectangles
                int maxX = (argc > 4) ? atoi(argv[4]) : 5;
                int maxY = (argc > 5) ? atoi(argv[5]) : 5;
                int i, j;
                // construct N random small rectangles
                srand(time(NULL));
                Rectangle* rects = new Rectangle[N];
                for (i = 0; i < N; ++i) {
                        rects[i].x = rand() % maxX + 1;
                        rects[i].y = rand() % maxY + 1;
                }
                // compute areas and filled area ratio
                int bigRectArea = bigRect.x * bigRect.y;
                int rectsArea = 0;
                for (i = 0; i < N; ++i)
                        rectsArea += rects[i].x * rects[i].y;
                double ratio = (double)rectsArea / (double)bigRectArea;
                cout << "Filled area ratio: " << ratio * 100 << "%";
                if (ratio > 1)
                        cout << " (problem is over-constrained)";
                cout << endl;
                NsProblemManager pm;
                // PROBLEM STATEMENT //
                NsIntVarArray Vars;
                for (i = 0; i < N; ++i) {
                        rects[i].minX =
                            new NsIntVar(pm, 0, bigRect.x - rects[i].x);
                        rects[i].minY =
                            new NsIntVar(pm, 0, bigRect.y - rects[i].y);
                        // gather all variables
                        Vars.push_back(*(rects[i].minY));
                        Vars.push_back(*(rects[i].minX));
                }
                // small rectangles must not overlap
                for (i = 0; i < N; ++i)
                        for (j = 0; j < N; ++j)
                                if (i != j) {
                                        pm.add(*(rects[i].minX) + rects[i].x <=
                                                   *(rects[j].minX) ||
                                               *(rects[j].minX) + rects[j].x <=
                                                   *(rects[i].minX) ||
                                               *(rects[i].minY) + rects[i].y <=
                                                   *(rects[j].minY) ||
                                               *(rects[j].minY) + rects[j].y <=
                                                   *(rects[i].minY));
                                }
                // GOALS //
                pm.addGoal(new AmDfsLabeling(Vars, new VarHeurCouples));
                // SOLVING //
                if (pm.nextSolution())
                        printSolution(bigRect, rects, N);
        } catch (exception& exc) {
                cerr << exc.what() << "\n";
        } catch (...) {
                cerr << "Unknown exception"
                     << "\n";
        }
}

void printSolution(Rectangle bigRect, Rectangle* rects, int N)
{
        int i, j, k;
        char symbol;
        char** output;
        // +2 for borders
        output = new char*[bigRect.y + 2];
        for (i = 0; i < bigRect.y + 2; ++i)
                output[i] = new char[bigRect.x + 2];
        // fill output table with spaces
        for (i = 0; i < bigRect.y + 2; ++i)
                for (j = 0; j < bigRect.x + 2; ++j)
                        output[i][j] = ' ';
        // borders
        for (i = 0; i < bigRect.x + 2; ++i)
                output[0][i] = output[bigRect.y + 1][i] = '*';
        for (i = 0; i < bigRect.y + 2; ++i)
                output[i][0] = output[i][bigRect.x + 1] = '*';
        // insert small rectangles in output table
        for (k = 0; k < N; ++k) { // for each rectangle
                // select a symbol
                if (k < 10)
                        symbol = '0' + k;
                else
                        symbol = 'A' + k - 10;
                // insert rectangle into table
                for (i = 0; i < rects[k].y; ++i)
                        for (j = 0; j < rects[k].x; ++j)
                                output[rects[k].minY->value() + i + 1]
                                      [rects[k].minX->value() + j + 1] = symbol;
        }
        // print output table
        for (i = 0; i < bigRect.y + 2; ++i) {
                for (j = 0; j < bigRect.x + 2; ++j)
                        putchar(output[i][j]);
                putchar('\n');
        }
}
