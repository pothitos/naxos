// Part of https://github.com/pothitos/naxos

#include "timetabling.h"
#include "error.h"
#include "heuristics.h"
#include "localSearch.h"

#include <cctype>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <naxos.h>
#include <string>
using namespace naxos;
using namespace std;

//  We reimplement difftime() because of MinGW issues.
double DiffTime(time_t time2, time_t time1)
{
        return (time2 - time1);
}

struct VarHeur_t {

        int index;

        double heur;
};

struct less_function_VarHeur
    : public binary_function<struct VarHeur_t, struct VarHeur_t, bool> {

        bool operator()(const struct VarHeur_t& X, const struct VarHeur_t& Y)
        {
                return (!(X.heur < Y.heur));
        }
};

int less_function_VarHeur_for_qsort(const void* X, const void* Y)
{
        if (static_cast<const struct VarHeur_t*>(X)->heur >
            static_cast<const struct VarHeur_t*>(Y)->heur)
                return -1;
        else if (static_cast<const struct VarHeur_t*>(X)->heur <
                 static_cast<const struct VarHeur_t*>(Y)->heur)
                return +1;
        else
                return 0;
}

struct PoPS_t {

        double PieceToCover;

        double conf;

        bool suspend;

        PoPS_t(void) : PieceToCover(0.0), suspend(false)
        {
        }
};

int main(int argc, char* argv[])
{
        try {
                time_t timeStarted = time(0);
                int i, j, k, l, d, h;
                struct itcProblem_t pr;
                createInstance(pr, argc, argv);
                /* Constraints Declaration */
                //  We double-state the problem.  In the second statement we
                //  have the  //
                //   extension "_ls" to the variables and problem manager
                //   (`pm_ls'),   // because we will use them to implement a
                //   Local Search technique.   //
                NsProblemManager pm;
                if (saveSearchTree)
                        pm.searchToGraphFile(
                            (string(argv[2]) + ".dot").c_str());
                NsProblemManager pm_ls;
                if (saveSearchTree &&
                    pr.timeLimitDirectMethodRound < pr.timeLimit)
                        pm_ls.searchToGraphFile(
                            (string(argv[2]) + ".indirect.dot").c_str());
                NsIntVarArray vLectPeriod, vLectRoom, vRooms;
                NsIntVarArray vLectPeriod_ls, vLectRoom_ls, vRooms_ls;
                NsDeque<NsIntVarArray> vProfessor(pr.nprofessors);
                NsDeque<NsIntVarArray> vProfessor_ls(pr.nprofessors);
                NsDeque<struct itcCourseInfo_t> courseInfo(pr.ncourses);
                NsDeque<struct itcLectureInfo_t> lectInfo;
                for (i = 0; i < pr.ncourses; ++i)
                        courseInfo[i].isConnectedToCourse.resize(pr.ncourses);
                /* Hard Constraints Declaration */
                for (k = 0, i = 0; i < pr.ncourses; ++i) {
                        courseInfo[i].firstLectIndex = k;
                        courseInfo[i].lastLectIndex =
                            k + pr.course[i].lectures - 1;
                        for (j = 0; j < pr.course[i].lectures; ++j, ++k) {
                                lectInfo.push_back(itcLectureInfo_t());
                                lectInfo[k].courseIndex = i;
                                vLectPeriod.push_back(
                                    NsIntVar(pm, 0, pr.days * pr.hours - 1));
                                vLectPeriod_ls.push_back(
                                    NsIntVar(pm_ls, 0, pr.days * pr.hours - 1));
                                /* Availability hard constraint */
                                for (l = 0; l < pr.course[i].nunavail; ++l) {
                                        vLectPeriod[k].remove(
                                            pr.course[i].unavail[l]);
                                        vLectPeriod_ls[k].remove(
                                            pr.course[i].unavail[l]);
                                }
                                /* Lecture distinct period assignment hard
                                 * constraint */
                                if (j > 0) {
                                        pm.add(vLectPeriod[k - 1] <
                                               vLectPeriod[k]);
                                        pm_ls.add(vLectPeriod_ls[k - 1] <
                                                  vLectPeriod_ls[k]);
                                }
                                /* Room Occupancy hard constraint */
                                vLectRoom.push_back(
                                    NsIntVar(pm, 0, pr.nrooms - 1));
                                vLectRoom_ls.push_back(
                                    NsIntVar(pm_ls, 0, pr.nrooms - 1));
                                vRooms.push_back(NsYplusCZspecial(
                                    vLectPeriod[k], pr.days * pr.hours,
                                    vLectRoom[k]));
                                vRooms_ls.push_back(NsYplusCZspecial(
                                    vLectPeriod_ls[k], pr.days * pr.hours,
                                    vLectRoom_ls[k]));
                                // vRooms.push_back( vLectRoom[k] * pr.days *
                                // pr.hours + vLectPeriod[k] );
                                // vRooms_ls.push_back( vLectRoom_ls[k] *
                                // pr.days * pr.hours + vLectPeriod_ls[k] );
                                /* Conflict (of Teachers) hard constraint */
                                vProfessor[pr.course[i].professor].push_back(
                                    vLectPeriod[k]);
                                vProfessor_ls[pr.course[i].professor].push_back(
                                    vLectPeriod_ls[k]);
                        }
                        for (j = 0; j <= i; ++j) {
                                // In purpose, we used greater or *equal* in
                                // order to connect each course with itself
                                if (pr.course[j].professor ==
                                    pr.course[i].professor)
                                        courseInfo[i].isConnectedToCourse[j] =
                                            courseInfo[j]
                                                .isConnectedToCourse[i] = true;
                        }
                }
                pm.add(NsAllDiff(vRooms));
                pm_ls.add(NsAllDiff(vRooms_ls));
                for (i = 0; i < pr.nprofessors; ++i) {
                        pm.add(NsAllDiff(vProfessor[i]));
                        pm_ls.add(NsAllDiff(vProfessor_ls[i]));
                }
                /* Conflict (of Curricula) hard constraint */
                NsDeque<NsIntVarArray> vGroup(pr.ngroups);
                NsDeque<NsIntVarArray> vGroup_ls(pr.ngroups);
                for (i = 0; i < pr.ngroups; ++i) {
                        for (j = 0; j < pr.group[i].ncourses; ++j) {
                                courseInfo[pr.group[i].course[j]]
                                    .group.push_back(i);
                                for (k = courseInfo[pr.group[i].course[j]]
                                             .firstLectIndex;
                                     k <= courseInfo[pr.group[i].course[j]]
                                              .lastLectIndex;
                                     ++k) {
                                        vGroup[i].push_back(vLectPeriod[k]);
                                        vGroup_ls[i].push_back(
                                            vLectPeriod_ls[k]);
                                }
                                for (k = 0; k < j; ++k) {
                                        l = pr.group[i].course[k];
                                        courseInfo[l].isConnectedToCourse[j] =
                                            courseInfo[j]
                                                .isConnectedToCourse[l] = true;
                                }
                        }
                        pm.add(NsAllDiff(vGroup[i]));
                        pm_ls.add(NsAllDiff(vGroup_ls[i]));
                }
                /* Objective function construction */
                NsDeque<NsIntVarArray> vCLectRoom(pr.ncourses),
                    vCRoomLect(pr.ncourses), vLectDay(pr.ncourses),
                    vDayLect(pr.ncourses), vIsBusyDay(pr.ncourses),
                    vObjectiveCapacity(vLectPeriod.size());
                NsIntVarArray vObjectiveCapacityTerms, vBusyRooms, vBusyDays,
                    vObjectiveMinWorkingDays, vObjectiveTerms;
                NsDeque<NsIntVarArray> vCLectRoom_ls(pr.ncourses),
                    vCRoomLect_ls(pr.ncourses), vLectDay_ls(pr.ncourses),
                    vDayLect_ls(pr.ncourses), vIsBusyDay_ls(pr.ncourses),
                    vObjectiveCapacity_ls(vLectPeriod.size());
                NsIntVarArray vObjectiveCapacityTerms_ls, vBusyRooms_ls,
                    vBusyDays_ls, vObjectiveMinWorkingDays_ls,
                    vObjectiveTerms_ls;
                for (i = 0; i < pr.ncourses; ++i) {
                        /* Room Capacity soft constraint */
                        for (j = courseInfo[i].firstLectIndex;
                             j <= courseInfo[i].lastLectIndex; ++j) {
                                for (k = 0; k < pr.nrooms; ++k) {
                                        if (pr.course[i].students >
                                            pr.room[k].capacity) {
                                                vObjectiveCapacity[j].push_back(
                                                    (pr.course[i].students -
                                                     pr.room[k].capacity) *
                                                    (vLectRoom[j] == k));
                                                vObjectiveCapacity_ls[j]
                                                    .push_back(
                                                        (pr.course[i].students -
                                                         pr.room[k].capacity) *
                                                        (vLectRoom_ls[j] == k));
                                        }
                                }
                                if (vObjectiveCapacity[j].size() > 0) {
                                        vObjectiveCapacityTerms.push_back(
                                            NsMax(vObjectiveCapacity[j]));
                                        vObjectiveCapacityTerms_ls.push_back(
                                            NsMax(vObjectiveCapacity_ls[j]));
                                }
                                /* We could use `NsSum' instead of `NsMax'.  We
                                 * did use the latter, because it provides a
                                 * better upper bound of the objective function.
                                 */
                        }
                        /* Room Stability soft constraint */
                        for (j = courseInfo[i].firstLectIndex;
                             j <= courseInfo[i].lastLectIndex; ++j) {
                                vCLectRoom[i].push_back(vLectRoom[j]);
                                vCLectRoom_ls[i].push_back(vLectRoom_ls[j]);
                        }
                        vCRoomLect[i] = NsInverse(vCLectRoom[i]);
                        vCRoomLect_ls[i] = NsInverse(vCLectRoom_ls[i]);
                        for (j = 0;
                             static_cast<unsigned>(j) < vCRoomLect[i].size();
                             ++j) {
                                vBusyRooms.push_back(vCRoomLect[i][j] >= 0);
                                vBusyRooms_ls.push_back(vCRoomLect_ls[i][j] >=
                                                        0);
                        }
                        // vObjectiveTerms.push_back( NsSum( vBusyRoom[i] ) - 1
                        // );
                        /* Minimum Working Days soft constraint */
                        for (j = courseInfo[i].firstLectIndex;
                             j <= courseInfo[i].lastLectIndex; ++j) {
                                vLectDay[i].push_back(vLectPeriod[j] /
                                                      pr.hours);
                                vLectDay_ls[i].push_back(vLectPeriod_ls[j] /
                                                         pr.hours);
                        }
                        vDayLect[i] = NsInverse(vLectDay[i], pr.days);
                        vDayLect_ls[i] = NsInverse(vLectDay_ls[i], pr.days);
                        for (d = 0; d < pr.days; ++d) {
                                vIsBusyDay[i].push_back(vDayLect[i][d] >= 0);
                                vIsBusyDay_ls[i].push_back(vDayLect_ls[i][d] >=
                                                           0);
                        }
                        vBusyDays.push_back(NsSum(vIsBusyDay[i]));
                        vBusyDays_ls.push_back(NsSum(vIsBusyDay_ls[i]));
                        vObjectiveMinWorkingDays.push_back(
                            (pr.course[i].minWorkingDays > vBusyDays[i]) *
                            (pr.course[i].minWorkingDays - vBusyDays[i]));
                        vObjectiveMinWorkingDays_ls.push_back(
                            (pr.course[i].minWorkingDays > vBusyDays_ls[i]) *
                            (pr.course[i].minWorkingDays - vBusyDays_ls[i]));
                }
                vObjectiveTerms.push_back(NsSum(vObjectiveCapacityTerms));
                vObjectiveTerms_ls.push_back(NsSum(vObjectiveCapacityTerms_ls));
                vObjectiveTerms.push_back(5 * NsSum(vObjectiveMinWorkingDays));
                vObjectiveTerms_ls.push_back(
                    5 * NsSum(vObjectiveMinWorkingDays_ls));
                /* Curriculum Compactness soft constraint */
                NsDeque<NsIntVarArray> vPeriodLect(pr.ngroups),
                    vIsolatedLect(pr.ngroups);
                NsDeque<NsIntVarArray> vPeriodLect_ls(pr.ngroups),
                    vIsolatedLect_ls(pr.ngroups);
                NsIntVarArray vIsolatedLectSums;
                NsIntVarArray vIsolatedLectSums_ls;
                for (i = 0; i < pr.ngroups; ++i) {
                        vPeriodLect[i] =
                            NsInverse(vGroup[i], pr.days * pr.hours - 1);
                        vPeriodLect_ls[i] =
                            NsInverse(vGroup_ls[i], pr.days * pr.hours - 1);
                        for (d = 0; d < pr.days; ++d) {
                                h = 0;
                                if (pr.hours == 1) {
                                        vIsolatedLect[i].push_back(
                                            vPeriodLect[i][d * pr.hours + h] >=
                                            0);
                                        vIsolatedLect_ls[i].push_back(
                                            vPeriodLect_ls[i][d * pr.hours +
                                                              h] >= 0);
                                }
                                if (pr.hours > 1) {
                                        vIsolatedLect[i].push_back(
                                            vPeriodLect[i][d * pr.hours + h] >=
                                                0 &&
                                            vPeriodLect[i][d * pr.hours + h +
                                                           1] < 0);
                                        vIsolatedLect_ls[i].push_back(
                                            vPeriodLect_ls[i][d * pr.hours +
                                                              h] >= 0 &&
                                            vPeriodLect_ls[i][d * pr.hours + h +
                                                              1] < 0);
                                }
                                ++h;
                                for (; h < pr.hours - 1; ++h) {
                                        vIsolatedLect[i].push_back(
                                            vPeriodLect[i][d * pr.hours + h] >=
                                                0 &&
                                            vPeriodLect[i][d * pr.hours + h -
                                                           1] < 0 &&
                                            vPeriodLect[i][d * pr.hours + h +
                                                           1] < 0);
                                        vIsolatedLect_ls[i].push_back(
                                            vPeriodLect_ls[i][d * pr.hours +
                                                              h] >= 0 &&
                                            vPeriodLect_ls[i][d * pr.hours + h -
                                                              1] < 0 &&
                                            vPeriodLect_ls[i][d * pr.hours + h +
                                                              1] < 0);
                                }
                                if (pr.hours > 1) {
                                        vIsolatedLect[i].push_back(
                                            vPeriodLect[i][d * pr.hours + h] >=
                                                0 &&
                                            vPeriodLect[i][d * pr.hours + h -
                                                           1] < 0);
                                        vIsolatedLect_ls[i].push_back(
                                            vPeriodLect_ls[i][d * pr.hours +
                                                              h] >= 0 &&
                                            vPeriodLect_ls[i][d * pr.hours + h -
                                                              1] < 0);
                                }
                        }
                        vIsolatedLectSums.push_back(NsSum(vIsolatedLect[i]));
                        vIsolatedLectSums_ls.push_back(
                            NsSum(vIsolatedLect_ls[i]));
                }
                vObjectiveTerms.push_back(2 * NsSum(vIsolatedLectSums));
                vObjectiveTerms_ls.push_back(2 * NsSum(vIsolatedLectSums_ls));
                // for the `Room Stability soft constraint'
                vObjectiveTerms.push_back(NsSum(vBusyRooms) - pr.ncourses);
                vObjectiveTerms_ls.push_back(NsSum(vBusyRooms_ls) -
                                             pr.ncourses);
                NsIntVar vObjective = NsSum(vObjectiveTerms);
                NsIntVar vObjective_ls = NsSum(vObjectiveTerms_ls);
                pm.minimize(vObjective);
                pm_ls.minimize(vObjective_ls);
                NsInt costBest = -1;
                unsigned timeNow, timeBest = 0;
                int timeLeft;
                if (pm.nextSolution() == false)
                        throw timetableException("No solution found - Initial "
                                                 "propagation detected an "
                                                 "inconsistency");
                //  Computation of the maximum values that the second and  //
                //   third value ordering heuristic criteria can take.     //
                pr.max2ndVarHeurCriterion = pr.max3rdVarHeurCriterion = 0.0;
                for (i = 0; i < pr.ncourses; ++i) {
                        double current2ndVarHeurCriterion =
                            unboundVarsConnecedTo(i, courseInfo, vLectPeriod,
                                                  pr);
                        double current3rdVarHeurCriterion =
                            varHeurObjectiveCriterion(i, courseInfo, pr,
                                                      vBusyDays);
                        if (current2ndVarHeurCriterion >
                            pr.max2ndVarHeurCriterion)
                                pr.max2ndVarHeurCriterion =
                                    current2ndVarHeurCriterion;
                        if (current3rdVarHeurCriterion >
                            pr.max3rdVarHeurCriterion)
                                pr.max3rdVarHeurCriterion =
                                    current3rdVarHeurCriterion;
                }
                pr.max2ndVarHeurCriterion += 1.0;
                pr.max3rdVarHeurCriterion += 1.0;
                //  When a static variable ordering heuristic is used by some
                //  methods, we take for     //
                //   granted that the assignments are made according to the
                //   order that each variable   // appears in the variables
                //   array (i.e. the first variable is chosen).  So we sort   //
                //   this array according to the values that the variable
                //   ordering heuristic function  // initially gives; an array
                //   `vLectPeriodSortedByHeur' is produced.                  //
                struct VarHeur_t* allVarsLectPeriod =
                    new struct VarHeur_t[vLectPeriod.size()];
                for (k = 0, i = 0; i < pr.ncourses; ++i) {
                        for (j = courseInfo[i].firstLectIndex;
                             j <= courseInfo[i].lastLectIndex; ++j, ++k) {
                                allVarsLectPeriod[k].index = k;
                                allVarsLectPeriod[k].heur =
                                    heurEstimationForVariable(k, i, courseInfo,
                                                              vLectPeriod, pr,
                                                              vBusyDays);
                                lectInfo[k].staticHeuristic =
                                    allVarsLectPeriod[k].heur;
                        }
                }
                //  Using `qsort()' instead of STL `sort()', because of a bug of
                //  the   //
                //   latter (on input `Test3.ctt'), that appeared on Linux
                //   machines.   //
                qsort(allVarsLectPeriod, vLectPeriod.size(),
                      sizeof(struct VarHeur_t),
                      less_function_VarHeur_for_qsort);
                // sort(allVarsLectPeriod.begin(), allVarsLectPeriod.end(),
                // less_function_VarHeur());
                NsIntVarArray vLectPeriodSortedByHeur, vLectRoomSortedByHeur;
                NsIntVarArray vLectPeriodSortedByHeur_ls,
                    vLectRoomSortedByHeur_ls;
                for (i = 0; static_cast<unsigned>(i) < vLectPeriod.size();
                     ++i) {
                        vLectPeriodSortedByHeur.push_back(
                            vLectPeriod[allVarsLectPeriod[i].index]);
                        vLectPeriodSortedByHeur_ls.push_back(
                            vLectPeriod_ls[allVarsLectPeriod[i].index]);
                        vLectRoomSortedByHeur.push_back(
                            vLectRoom[allVarsLectPeriod[i].index]);
                        vLectRoomSortedByHeur_ls.push_back(
                            vLectRoom_ls[allVarsLectPeriod[i].index]);
                }
                delete[] allVarsLectPeriod;
                // allVarsLectPeriod.clear();
                // used only for the LAN search method
                NsDeque<unsigned> lanAssigns;
                NsDeque<unsigned> lanAssigns_ls;
                VarHeurTimetabling varHeur(
                    ((pr.searchMethod == DBDS) ? vLectRoomSortedByHeur
                                               : vLectRoom),
                    pr, lectInfo, courseInfo, lanAssigns, vRooms, vBusyDays);
                VarHeurTimetabling varHeur_ls(
                    ((pr.searchMethod_ls == DBDS) ? vLectRoomSortedByHeur_ls
                                                  : vLectRoom_ls),
                    pr, lectInfo, courseInfo, lanAssigns_ls, vRooms_ls,
                    vBusyDays_ls);
                ValHeurTimetabling valHeur(
                    vLectPeriod, vLectRoom, pr, lectInfo, courseInfo, vRooms,
                    vLectDay, vIsBusyDay, vBusyDays, vGroup, vIsolatedLect);
                ValHeurTimetabling valHeur_ls(
                    vLectPeriod_ls, vLectRoom_ls, pr, lectInfo, courseInfo,
                    vRooms_ls, vLectDay_ls, vIsBusyDay_ls, vBusyDays_ls,
                    vGroup_ls, vIsolatedLect_ls);
                SplitValHeurTimetabling splitValHeur(
                    vLectPeriod, vLectRoom, pr, lectInfo, courseInfo, vRooms);
                SplitValHeurTimetabling splitValHeur_ls(
                    vLectPeriod_ls, vLectRoom_ls, pr, lectInfo, courseInfo,
                    vRooms_ls);
                bool isPoPS = false;
                switch (pr.searchMethod) {
                case DFS:
                        pm.addGoal(
                            new AmDfsLabeling(vLectPeriod, &varHeur, &valHeur));
                        break;
                case LDS:
                        pm.addGoal(new AmLds(
                            vLectPeriod, pr.searchMethodParameter[0],
                            pr.searchMethodParameter[1],
                            pr.searchMethodParameter[2],
                            pr.searchMethodParameter[3], &varHeur, &valHeur));
                        break;
                case CREDIT:
                        pm.addGoal(new AmCredit(vLectPeriod,
                                                pr.searchMethodParameter[0],
                                                &varHeur, &valHeur));
                        break;
                case DBS:
                        pm.addGoal(new AmDbsStepping(
                            vLectPeriod, pr.searchMethodParameter[0], &varHeur,
                            &valHeur));
                        break;
                case LAN:
                        lanAssigns.resize(vLectPeriod.size());
                        pm.addGoal(new AmLanLabeling(
                            vLectPeriod, lanAssigns,
                            pr.searchMethodParameter[0], &varHeur, &valHeur));
                        //  Original `AmLan' does not take a `varHeur' argument,
                        //  because it has its own    //
                        //   variable ordering heuristic, that uses an array
                        //   named `assigns'.  In our case,  // the variable
                        //   ordering heuristic is parameterized in order to
                        //   make assignments   // to the `vLectRoom' variables.
                        //   So we need to use our heuristic to ensure that   //
                        //   the variables of both `vLectPeriod' and `vLectRoom'
                        //   arrays are instantiated.    // We constructed our
                        //   `lanAssigns' array to use it with our `varHeur'.
                        //   We used    // a slightly changed version of
                        //   `AmLanLabeling' instead of `AmLan'.          //
                        break;
                case DBDS:
                        pr.varHeuristicType = STATIC;
                        pm.addGoal(new AmDbds(vLectPeriodSortedByHeur, &varHeur,
                                              &valHeur));
                        //  Original `AmDbds' does not take a `varHeur'
                        //  argument.  It uses a static    //
                        //   variable ordering heuristic, i.e. it uses the order
                        //   that the variables      // appear in their array.
                        //   (So we sorted them according the values that gives
                        //   // the heuristic function before search starts, and
                        //   we constructed the         //
                        //   `vLectPeriodSortedByHeur' array.)  We used our
                        //   `varHeur' for the same       // reasons as in
                        //   `AmLan' (see the corresponding comments above). //
                        break;
                case IBROAD:
                        pm.addGoal(
                            new AmIbroad(vLectPeriod, &varHeur, &valHeur));
                        break;
                case BBS:
                        pr.varHeuristicType = STATIC;
                        pr.valHeuristicType = RANDOM;
                        pm.addGoal(new AmBbs(
                            vLectPeriod, pr.searchMethodParameter[0],
                            pr.searchMethodParameter[1], &varHeur, &valHeur));
                        break;
                case RDFS:
                        pr.valHeuristicType = RANDOM;
                        pm.addGoal(new AmRdfs(
                            vLectPeriod, pr.searchMethodParameter[0],
                            pr.searchMethodParameter[1], &varHeur, &valHeur));
                        break;
                case GNS:
                        pm.addGoal(
                            new AmGnsLabeling(vLectPeriod, &varHeur, &valHeur));
                        break;
                case FNS:
                        switch (pr.searchMethodParameter[0]) {
                        case FSAMPLE:
                                pm.addGoal(new AmFnsLabeling(
                                    vLectPeriod, fSample, &varHeur, &valHeur));
                                break;
                        case FSAMPLE_MIDDLE:
                                pm.addGoal(new AmFnsLabeling(
                                    vLectPeriod, fSampleMiddle, &varHeur,
                                    &valHeur));
                                break;
                        case FSAMPLE_EXP:
                                pm.addGoal(
                                    new AmFnsLabeling(vLectPeriod, fSampleExp,
                                                      &varHeur, &valHeur));
                                break;
                        default:
                                throw timetableException(
                                    "Wrong `pr.searchMethodParameter[0]' for "
                                    "FNS");
                                break;
                        }
                        break;
                case ISAMP:
                        pr.valHeuristicType = RANDOM;
                        pm.addGoal(new AmIsampStepping(
                            vLectPeriod, pr.searchMethodParameter[0], &varHeur,
                            &valHeur));
                        break;
                case ONESAMP:
                        pr.varHeuristicType = STATIC;
                        pm.addGoal(new AmOnesampLabeling(vLectPeriod, &varHeur,
                                                         &valHeur));
                        break;
                case DOMAIN_SPLITTING:
                        pr.varHeuristicType = RANDOM;
                        if (pr.valHeuristicType == NORMAL)
                                //  Default middle-value heuristic is used.
                                pm.addGoal(new AmDomainSplittingLabeling(
                                    vLectPeriod, &varHeur));
                        else //  fair-domain-splitting
                                pm.addGoal(new AmDomainSplittingLabeling(
                                    vLectPeriod, &varHeur, &splitValHeur));
                        break;
                case PoPS:
                        pr.valHeuristicType = RANDOM;
                        if (pr.searchMethodParameter[0] == -1)
                                isPoPS = true;
                        if (!isPoPS)
                                // PopsSample
                                pm.addGoal(new AmPopsLabeling(
                                    vLectPeriod,
                                    pr.searchMethodParameter[0] / 100.0,
                                    &varHeur, &valHeur));
                        break;
                default:
                        throw timetableException("Wrong `pr.searchMethod'");
                        break;
                }
                if ((pr.varHeuristicType != RANDOM &&
                     pr.valHeuristicType != RANDOM) &&
                    argc >= 4 && isdigit(argv[3][0]))
                        cerr << "Warning: Unnecessary random seed (while using "
                                "a non-random search method)."
                             << "\n";
                assert_that(pm_ls.nextSolution() == true,
                            "Local Search initial propagation failed");
                pr.solutions = 0;
                if (isPoPS) {
                        pm.realTimeLimit(pr.timeLimit -
                                         DiffTime(time(0), timeStarted));
                        double N = vLectPeriod.size();
                        double domainsSizeSum = 0.0;
                        for (i = 0; i < N; ++i)
                                domainsSizeSum += vLectPeriod[i].size();
                        double D = domainsSizeSum / N;
                        const double confmax = 100.0;
                        const int samples = 5;
                        NsDeque<PoPS_t> popsSample(samples);
                        for (i = 0; i < samples; ++i)
                                popsSample[i].conf =
                                    confmax * i / (samples - 1.0);
                        bool searchEnded = false;
                        while (!searchEnded) {
                                bool foundSolutionInThisRound = false;
                                for (i = 0; i < samples && !searchEnded; ++i) {
                                        if (popsSample[i].suspend == true)
                                                continue;
                                        popsSample[i].suspend = true;
                                        time_t timeStartedPops = time(0);
                                        srand(1);
                                        pr.conf = popsSample[i].conf;
                                        if (!pr.quiet)
                                                cout << "PoPS("
                                                     << popsSample[i]
                                                            .PieceToCover
                                                     << ")\n"
                                                     << "conf = " << pr.conf
                                                     << "\n";
                                        pm.addGoal(new AmPopsLabeling(
                                            vLectPeriod,
                                            popsSample[i].PieceToCover,
                                            &varHeur, &valHeur));
                                        while (pm.nextSolution() != false) {
                                                ++pr.solutions;
                                                timeBest = DiffTime(
                                                    time(0), timeStarted);
                                                costBest = vObjective.max();
                                                writeSolution(pr, vLectPeriod,
                                                              vLectRoom,
                                                              argv[2]);
                                                foundSolutionInThisRound = true;
                                                popsSample[i].suspend = false;
                                                if (!pr.quiet)
                                                        cout << "Solution with "
                                                                "cost "
                                                             << vObjective.max()
                                                             << "\n";
                                        }
                                        if (!pr.quiet)
                                                cout
                                                    << "time = "
                                                    << DiffTime(time(0),
                                                                timeStartedPops)
                                                    << "\n"
                                                    << "backtracks = "
                                                    << pm.numBacktracks()
                                                    << "\n"
                                                    << "\n";
                                        searchEnded =
                                            (DiffTime(time(0), timeStarted) >=
                                             pr.timeLimit) ||
                                            (popsSample[i].PieceToCover >= 1.0);
                                        popsSample[i].PieceToCover += 1.0 / D;
                                        if (popsSample[i].PieceToCover > 1.0)
                                                popsSample[i].PieceToCover =
                                                    1.0;
                                        pm.restart();
                                }
                                if (!foundSolutionInThisRound) {
                                        for (i = 0; i < samples; ++i)
                                                popsSample[i].suspend = false;
                                }
                                if (!pr.quiet)
                                        cout << "\n";
                        }
                }
                bool foundNewSolution;
                int currentDay = 0;
                while (!isPoPS) {
                        pr.isActiveLocalSearch = false;
                        timeNow = DiffTime(time(0), timeStarted);
                        timeLeft = pr.timeLimit - timeNow;
                        if (timeLeft <= 0)
                                break;
                        if (pr.timeLimitDirectMethodRound != 0) {
                                pr.timeLimitDirectMethodRound =
                                    min(pr.timeLimitDirectMethodRound,
                                        static_cast<int>(timeLeft));
                                pm.realTimeLimit(pr.timeLimitDirectMethodRound);
                        }
                        if (saveLog && !pr.quiet) {
                                cout << "# " << setw(2) << setfill('0')
                                     << timeNow / 60 << ":" << setw(2)
                                     << setfill('0') << timeNow % 60 << "\t"
                                     << "Using a direct method"
                                     << "\n"
                                     << flush;
                        }
                        foundNewSolution = false;
                        while (pm.nextSolution() != false) {
                                foundNewSolution = true;
                                ++pr.solutions;
                                /* Recording the solution */
                                writeSolution(pr, vLectPeriod, vLectRoom,
                                              argv[2]);
                                if (saveLog) {
                                        timeNow =
                                            DiffTime(time(0), timeStarted);
                                        if (!pr.quiet)
                                                cout << "  " << setw(2)
                                                     << setfill('0')
                                                     << timeNow / 60 << ":"
                                                     << setw(2) << setfill('0')
                                                     << timeNow % 60
                                                     << "\tcost "
                                                     << vObjective.max()
                                                     << "\t# = "
                                                     << vObjectiveTerms[0].max()
                                                     << "+"
                                                     << vObjectiveTerms[1].max()
                                                     << "+"
                                                     << vObjectiveTerms[2].max()
                                                     << "+"
                                                     << vObjectiveTerms[3].max()
                                                     << "  Constraint-checks: "
                                                     << pm.numConstraintChecks()
                                                     << "\n"
                                                     << flush;
                                        timeBest = timeNow;
                                        costBest = vObjective.max();
                                }
                        }
                        if (pr.searchMethod_ls == NONE)
                                break;
                        if (pr.solutions == 0)
                                continue;
                        pr.isActiveLocalSearch = true;
                        timeNow = DiffTime(time(0), timeStarted);
                        timeLeft = pr.timeLimit - timeNow;
                        if (timeLeft <= 0)
                                break;
                        if (pr.timeLimitIndirectMethodRound != 0) {
                                pr.timeLimitIndirectMethodRound =
                                    min(pr.timeLimitIndirectMethodRound,
                                        static_cast<int>(timeLeft));
                                pm_ls.realTimeLimit(
                                    pr.timeLimitIndirectMethodRound);
                        }
                        if (saveLog) {
                                cout << "# " << setw(2) << setfill('0')
                                     << timeNow / 60 << ":" << setw(2)
                                     << setfill('0') << timeNow % 60 << "\t"
                                     << "Using an indirect method (Local "
                                        "Search for lectures on day "
                                     << currentDay << ")"
                                     << "\n"
                                     << flush;
                        }
                        pm_ls.restart();
                        if (foundNewSolution)
                                pm_ls.objectiveUpperLimit(vObjective.max());
                        if (pr.searchMethod_ls != NONE)
                                pm_ls.addGoal(new goalLsWeek(
                                    vLectPeriod_ls, vLectPeriodSortedByHeur_ls,
                                    vLectRoom_ls, pr, varHeur_ls, valHeur_ls,
                                    lanAssigns_ls, (currentDay++) % pr.days));
                        foundNewSolution = false;
                        while (pm_ls.nextSolution() != false) {
                                foundNewSolution = true;
                                ++pr.solutions;
                                /* Recording the solution */
                                writeSolution(pr, vLectPeriod_ls, vLectRoom_ls,
                                              argv[2]);
                                if (saveLog) {
                                        timeNow =
                                            DiffTime(time(0), timeStarted);
                                        cout << "  " << setw(2) << setfill('0')
                                             << timeNow / 60 << ":" << setw(2)
                                             << setfill('0') << timeNow % 60
                                             << "\tcost " << vObjective_ls.max()
                                             << "\t# = "
                                             << vObjectiveTerms_ls[0].max()
                                             << "+"
                                             << vObjectiveTerms_ls[1].max()
                                             << "+"
                                             << vObjectiveTerms_ls[2].max()
                                             << "+"
                                             << vObjectiveTerms_ls[3].max()
                                             << "\n"
                                             << flush;
                                        timeBest = timeNow;
                                        costBest = vObjective.max();
                                }
                        }
                        if (foundNewSolution)
                                pm.objectiveUpperLimit(vObjective_ls.max());
                }
                if (saveLog) {
                        timeNow = DiffTime(time(0), timeStarted);
                        if (pr.quiet) {
                                if (pr.solutions == 0)
                                        cout << "X\tX\t";
                                else
                                        cout << timeBest << "\t" << costBest
                                             << "\t";
                                pm.printCspParameters();
                        }
                }
                destroyInstance(pr);
        } catch (exception& exc) {
                cerr << exc.what() << "\n";
                return 1;
        } catch (...) {
                cerr << "Unknown exception"
                     << "\n";
                return 1;
        }
}

NsGoal* metaSearchMethodGoal(struct itcProblem_t& pr,
                             NsIntVarArray& vLectPeriod,
                             NsIntVarArray& vLectPeriodSortedByHeur,
                             VariableHeuristic& varHeur,
                             ValueHeuristic& valHeur,
                             NsDeque<unsigned>& lanAssigns)
{
        switch (pr.searchMethod_ls) {
        case DFS:
                return (new AmDfsLabeling(vLectPeriod, &varHeur, &valHeur));
                break;
        case LDS:
                return (new AmLds(vLectPeriod, pr.searchMethodParameter_ls[0],
                                  pr.searchMethodParameter_ls[1],
                                  pr.searchMethodParameter_ls[2],
                                  pr.searchMethodParameter_ls[3], &varHeur,
                                  &valHeur));
                break;
        case CREDIT:
                return (new AmCredit(vLectPeriod,
                                     pr.searchMethodParameter_ls[0], &varHeur,
                                     &valHeur));
                break;
        case DBS:
                return (new AmDbsStepping(vLectPeriod,
                                          pr.searchMethodParameter_ls[0],
                                          &varHeur, &valHeur));
                break;
        case LAN:
                lanAssigns.resize(vLectPeriod.size());
                return (new AmLanLabeling(vLectPeriod, lanAssigns,
                                          pr.searchMethodParameter_ls[0],
                                          &varHeur, &valHeur));
                //  Original `AmLan' does not take a `varHeur' argument, because
                //  it has its own    //
                //   variable ordering heuristic, that uses an array named
                //   `assigns'.  In our case,  // the variable ordering
                //   heuristic is parameterized in order to make assignments //
                //   to the `vLectRoom' variables.  So we need to use our
                //   heuristic to ensure that   // the variables of both
                //   `vLectPeriod' and `vLectRoom' arrays are instantiated. //
                //   We constructed our `lanAssigns' array to use it with our
                //   `varHeur'.  We used    // a slightly changed version of
                //   `AmLanLabeling' instead of `AmLan'.          //
                break;
        case DBDS:
                pr.varHeuristicType_ls = STATIC;
                return (
                    new AmDbds(vLectPeriodSortedByHeur, &varHeur, &valHeur));
                //  Original `AmDbds' does not take a `varHeur' argument.  It
                //  uses a static    //
                //   variable ordering heuristic, i.e. it uses the order that
                //   the variables      // appear in their array.  (So we sorted
                //   them according the values that gives  // the heuristic
                //   function before search starts, and we constructed the //
                //   `vLectPeriodSortedByHeur' array.)  We used our `varHeur'
                //   for the same       // reasons as in `AmLan' (see the
                //   corresponding comments above).             //
                break;
        case IBROAD:
                return (new AmIbroad(vLectPeriod, &varHeur, &valHeur));
                break;
        case BBS:
                pr.varHeuristicType_ls = STATIC;
                pr.valHeuristicType_ls = RANDOM;
                return (new AmBbs(vLectPeriod, pr.searchMethodParameter_ls[0],
                                  pr.searchMethodParameter_ls[1], &varHeur,
                                  &valHeur));
                break;
        case RDFS:
                pr.valHeuristicType_ls = RANDOM;
                return (new AmRdfs(vLectPeriod, pr.searchMethodParameter_ls[0],
                                   pr.searchMethodParameter_ls[1], &varHeur,
                                   &valHeur));
                break;
        case GNS:
                return (new AmGnsLabeling(vLectPeriod, &varHeur, &valHeur));
                break;
        case FNS:
                switch (pr.searchMethodParameter_ls[0]) {
                case FSAMPLE:
                        return (new AmFnsLabeling(vLectPeriod, fSample,
                                                  &varHeur, &valHeur));
                        break;
                case FSAMPLE_MIDDLE:
                        return (new AmFnsLabeling(vLectPeriod, fSampleMiddle,
                                                  &varHeur, &valHeur));
                        break;
                case FSAMPLE_EXP:
                        return (new AmFnsLabeling(vLectPeriod, fSampleExp,
                                                  &varHeur, &valHeur));
                        break;
                default:
                        throw timetableException(
                            "Wrong `pr.searchMethodParameter_ls[0]' for FNS");
                        break;
                }
                break;
        case ISAMP:
                pr.valHeuristicType_ls = RANDOM;
                return (new AmIsampStepping(vLectPeriod,
                                            pr.searchMethodParameter_ls[0],
                                            &varHeur, &valHeur));
                break;
        case ONESAMP:
                pr.varHeuristicType_ls = STATIC;
                return (new AmOnesampLabeling(vLectPeriod, &varHeur, &valHeur));
                break;
        case PoPS:
                pr.valHeuristicType_ls = RANDOM;
                return (new AmPopsLabeling(
                    vLectPeriod, pr.searchMethodParameter_ls[0] / 100.0,
                    &varHeur, &valHeur));
                break;
        default:
                throw timetableException("Wrong `pr.searchMethod_ls'");
                break;
        }
}
