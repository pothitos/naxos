// Part of https://github.com/pothitos/naxos

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <naxos.h>
#include <string>

using namespace std;
using namespace naxos;

int main(int argc, char* argv[])
{
        try {
                //  Checking input arguments.
                if (argc != 2 && argc != 3) {
                        cerr << "Correct syntax is:\t" << argv[0]
                             << " <flight_data_file> [precision]\n";
                        return 1;
                }
                double precision = ((argc == 3) ? atof(argv[2]) : 1.0);
                string input;
                unsigned pairing, flight;
                NsInt N;
                NsDeque<NsDeque<NsInt>> P;
                NsDeque<NsInt> C;
                ifstream fileAcs(argv[1]);
                if (!fileAcs) {
                        cerr << "Could not open `" << argv[1] << "'\n";
                        return 1;
                }
                fileAcs.exceptions(ifstream::eofbit | ifstream::failbit |
                                   ifstream::badbit);
                fileAcs >> N;
                unsigned M;
                fileAcs >> M;
                NsInt number;
                for (pairing = 0; pairing < M; ++pairing) {
                        fileAcs >> number;
                        C.push_back(number * precision + 0.5);
                        P.push_back(NsDeque<NsInt>());
                        unsigned K;
                        fileAcs >> K;
                        for (flight = 0; flight < K; ++flight) {
                                fileAcs >> number;
                                P.back().push_back(number);
                        }
                }
                fileAcs.close();
                NsProblemManager pm;
                NsDeque<NsIntVarArray> vFlightScheduledWithP;
                for (flight = 0; flight < N; ++flight)
                        vFlightScheduledWithP.push_back(NsIntVarArray());
                NsIntVarArray vCostTerms;
                NsIntVarArray vIncludePairing;
                for (pairing = 0; pairing < P.size(); ++pairing) {
                        vIncludePairing.push_back(NsIntVar(pm, 0, 1));
                        for (flight = 0; flight < P[pairing].size(); ++flight) {
                                NsInt f = P[pairing][flight] - 1;
                                vFlightScheduledWithP[f].push_back(
                                    vIncludePairing[pairing]);
                        }
                        vCostTerms.push_back(C[pairing] *
                                             vIncludePairing[pairing]);
                }
                for (flight = 0; flight < N; ++flight)
                        pm.add(NsSum(vFlightScheduledWithP[flight]) == 1);
                NsIntVar vCost = NsSum(vCostTerms);
                pm.minimize(vCost);
                pm.addGoal(new NsgLabeling(vIncludePairing));
                NsDeque<NsInt> bestIncludePairing(vIncludePairing.size());
                while (pm.nextSolution() != false) {
                        //  Recording the (current) best solution.
                        for (pairing = 0; pairing < P.size(); ++pairing)
                                bestIncludePairing[pairing] =
                                    vIncludePairing[pairing].value();
                }
                pm.printCspParameters();
        } catch (exception& exc) {
                cerr << exc.what() << "\n";
                return 1;
        } catch (...) {
                cerr << "Unknown exception"
                     << "\n";
                return 1;
        }
}
