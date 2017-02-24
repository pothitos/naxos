// Part of https://github.com/pothitos/naxos

#include "heuristics.h"
#include "error.h"
#include <cfloat>
#include <climits>
#include <cmath>
#include <cstdlib>

using namespace naxos;
using namespace std;

bool intersectionEmpty2(const NsIntVar* VarY, const NsIntVar* VarZ)
{
        return (VarY->min() > VarZ->max() || VarY->max() < VarZ->min());
}

NsIndex randomizeHeuristic(NsDeque<double>& heuristic, const double conf);

int ValueOrderHeurForRooms(int& bestRoomObjectiveEstimation, const int period,
                           const NsIntVarArray& vLectRoom, const int index,
                           const struct itcProblem_t& pr,
                           const NsDeque<struct itcLectureInfo_t>& lectInfo,
                           const NsDeque<struct itcCourseInfo_t>& courseInfo,
                           const NsIntVarArray& vRooms);

//  VARIABLE ORDERING HEURISTIC  //
//
//  1st criterion:               Minimum Domain Size (First-Fail Heuristic)
//  2nd criterion (tiebreaker):  Most Constrained Lecture (Brelaz Heuristic)
//  3rd criterion (tiebreaker):  Algebraic (Objective Function-Oriented)
//  Expression
//                                = students + 5 * minWorkingDays + 2 *
//                                lectureGroups + lectures
//                They refer to the  ^1st       ^2nd                 ^3rd ^4th
//                soft constraint, and they represent an upper bound of the cost
//                that an assignment to the current lecture may have.

int VarHeurTimetabling::select(const NsIntVarArray& vLectPeriod)
{
        /*bestCourseIndex=-1,*/
        int bestIndex = -1, courseIndex, currentBrelaz = -1, maxBrelaz = -1,
            maxObjective = -1; // initializations to suppress warnings
        unsigned i;
        NsDeque<unsigned> varIndex;
        NsDeque<double> heuristic;
        for (i = 0; i < vLectPeriod.size(); ++i) {
                if (vLectPeriod[i].isBound() && !vLectRoom[i].isBound()) {
                        //  This is the case that we have not a complete
                        //  assignment     //
                        //   for the current lecture; we have an assigned
                        //   teaching      // period for it, but we did not
                        //   provide a classroom for it.  //
                        int bestRoom, bestRoomObjectiveEstimation;
                        bestRoom = ValueOrderHeurForRooms(
                            bestRoomObjectiveEstimation, vLectPeriod[i].value(),
                            vLectRoom, i, pr, lectInfo, courseInfo, vRooms);
                        vLectRoom[i].set(bestRoom);
                }
        }
        switch ((pr.isActiveLocalSearch) ? pr.varHeuristicType_ls
                                         : pr.varHeuristicType) {
        case NORMAL:
                for (i = 0; i < vLectPeriod.size(); ++i) {
                        courseIndex = lectInfo[i].courseIndex;
                        if (!vLectPeriod[i].isBound() &&
                            (bestIndex == -1 ||
                             (vLectPeriod[i].size() <
                              vLectPeriod[bestIndex].size()) ||
                             (vLectPeriod[i].size() ==
                                  vLectPeriod[bestIndex].size() &&
                              (currentBrelaz = unboundVarsConnecedTo(
                                   courseIndex, courseInfo, vLectPeriod, pr)) >
                                  maxBrelaz) ||
                             (vLectPeriod[i].size() ==
                                  vLectPeriod[bestIndex].size() &&
                              currentBrelaz == maxBrelaz &&
                              varHeurObjectiveCriterion(courseIndex, courseInfo,
                                                        pr, vBusyDays) >
                                  maxObjective))) {
                                if (((pr.isActiveLocalSearch)
                                         ? pr.searchMethod_ls
                                         : pr.searchMethod) == LAN &&
                                    lanAssigns[i] >=
                                        static_cast<unsigned>(
                                            (pr.isActiveLocalSearch)
                                                ? pr.searchMethodParameter_ls[0]
                                                : pr.searchMethodParameter
                                                      [0]) /*lanLimit*/) {
                                        continue;
                                }
                                bestIndex = i;
                                // bestCourseIndex =
                                // lectInfo[bestIndex].courseIndex;
                                maxBrelaz = unboundVarsConnecedTo(
                                    courseIndex, courseInfo, vLectPeriod, pr);
                                maxObjective = varHeurObjectiveCriterion(
                                    courseIndex, courseInfo, pr, vBusyDays);
                        }
                }
                break;
        case STATIC:
                for (i = 0; i < vLectPeriod.size(); ++i) {
                        if (!vLectPeriod[i].isBound() &&
                            (bestIndex == -1 ||
                             lectInfo[i].staticHeuristic >
                                 lectInfo[bestIndex].staticHeuristic)) {
                                bestIndex = i;
                        }
                }
                break;
        case RANDOM:
                //  Number of unbound variables
                int numUnbound;
                numUnbound = 0;
                for (i = 0; i < vLectPeriod.size(); ++i)
                        numUnbound += (!vLectPeriod[i].isBound());
                // cout << "\r" << static_cast<int>(100.0 * numUnbound /
                // vLectPeriod.size())
                //	<< "%  (" << numUnbound << "/" << vLectPeriod.size() <<
                //")\t";
                if (numUnbound == 0)
                        return -1;
                varIndex.resize(numUnbound);
                heuristic.resize(varIndex.size() + 1);
                unsigned valNum;
                valNum = 0;
                for (i = 0; i < vLectPeriod.size(); ++i) {
                        if (!vLectPeriod[i].isBound()) {
                                varIndex[valNum] = i;
                                //  MRV / BRELAZ heuristic
                                heuristic[valNum + 1] =
                                    (pr.days * pr.hours -
                                     vLectPeriod[i].size()) *
                                        pr.max2ndVarHeurCriterion +
                                    unboundVarsConnecedTo(
                                        lectInfo[i].courseIndex, courseInfo,
                                        vLectPeriod, pr);
                                // cout << "heuristic[" << valNum+1 << "]  =  "
                                // << (pr.days * pr.hours -
                                // vLectPeriod[i].size())
                                //	<< " * " << pr.max2ndVarHeurCriterion
                                //	<< " + " <<
                                //unboundVarsConnecedTo(lectInfo[i].courseIndex,courseInfo,vLectPeriod,pr)
                                //	<< " = " << heuristic[valNum+1] << "\n";
                                ++valNum;
                        }
                }
                ///  \todo  check bug for fair-domain-splitting in linux14 for
                ///  exp=16 comp01.ctt
                bestIndex = varIndex[randomizeHeuristic(heuristic, pr.conf)];
                // cout << bestIndex << ":  ";
                // cout << "Selected: " <<
                // intervalBinarySearch(heuristic,random) << "/" <<
                // varIndex.size() << "\n";  system("pause");
                ////  ROUND ROBIN HEURISTIC (FOR DOMAIN SPLITTING)
                // roundRobinVarPosition  =  (roundRobinVarPosition + 1) %
                // vLectPeriod.size();  int  oldPosition  =
                // roundRobinVarPosition;  for ( ; ; )   { 	if (
                //!vLectPeriod[roundRobinVarPosition].isBound() )    {
                //		bestIndex  =  roundRobinVarPosition;
                //		break;
                //	}
                //	roundRobinVarPosition  =  (roundRobinVarPosition + 1) %
                //vLectPeriod.size(); 	if ( roundRobinVarPosition  ==
                //oldPosition ) 		return  -1;
                //}
                ////  PURE RANDOM HEURISTIC
                ////  Number of unbound variables
                // int  numUnbound;
                // numUnbound = 0;
                // for (i=0;  i < vLectPeriod.size();  ++i)
                //	numUnbound  +=  ( !vLectPeriod[i].isBound() );
                // cout << "\r" << static_cast<int>(100.0 * numUnbound /
                // vLectPeriod.size())
                //	<< "%  (" << numUnbound << "/" << vLectPeriod.size() <<
                //")\t";  if ( numUnbound  ==  0 ) 	return  -1;
                ////  Pick variable among unbound ones
                // int  random  =  static_cast<int>( numUnbound * (rand() /
                // (RAND_MAX + 1.0)) );
                ////  Find position of chosen variable
                // numUnbound  =  0;
                // for (i=0;  i < vLectPeriod.size();  ++i)   {
                //	if ( !vLectPeriod[i].isBound()  &&  random-- == 0 )    {
                //		bestIndex  =  i;
                //		break;
                //	}
                //}
                break;
        default:
                throw timetableException("Wrong `pr.varHeuristicType'");
                break;
        }
        // cout << vLectPeriod << "\n-----------------------------\n";
        // cout << vLectRoom << "\n\n";
        return bestIndex;
}

int intervalBinarySearch(const NsDeque<double>& intervalStart, const double x);

void partitionRoulette(NsDeque<double>& heuristic, const double conf)
{
        double sumHeuristic;
        NsIndex i;
        sumHeuristic = 0.0;
        for (i = 1; i < heuristic.size(); ++i)
                sumHeuristic += heuristic[i];
        for (i = 1; i < heuristic.size(); ++i) {
                heuristic[i] = pow(heuristic[i] / sumHeuristic, conf);
                assert_that(heuristic[i] >= 0.0, "Negative `heuristic[i]'");
        }
        //  [0,1) is partitioned into intervals corresponding to values.
        sumHeuristic = 0.0;
        for (i = 1; i < heuristic.size(); ++i)
                sumHeuristic += heuristic[i];
        i = 0;
        heuristic[i] = 0.0;
        ++i;
        for (; i < heuristic.size(); ++i) {
                heuristic[i] = heuristic[i] / sumHeuristic + heuristic[i - 1];
                //  `varIndex[i]' is represented by the interval
                //   [heuristic[i-1],heuristic[i]].  The greater the
                //   heuristic function value is, the larger the interval is.
        }
}

NsIndex randomizeHeuristic(NsDeque<double>& heuristic, const double conf)
{
        partitionRoulette(heuristic, conf);
        // A random number in [0,1) multiplied by heuristic[i-1].
        double random = (rand() / (RAND_MAX + 1.0)) * heuristic.back();
        //  To which interval (corresponding to a specific varIndex) does
        //   `random' belong to?  (We search it using binary search.)
        return intervalBinarySearch(heuristic, random);
}

NsDeque<NsInt> randomizeHeuristicSequence(NsDeque<NsInt> value,
                                          NsDeque<double>& heuristic,
                                          const double conf, double sumLimit)
{
        partitionRoulette(heuristic, conf);
        NsDeque<NsInt> assignmentSequence;
        NsDeque<bool> selectedElement(heuristic.size() - 1);
        while (sumLimit > 0) {
                double random = (rand() / (RAND_MAX + 1.0)) * heuristic.back();
                NsIndex i = intervalBinarySearch(heuristic, random);
                if (!selectedElement[i]) {
                        selectedElement[i] = true;
                        assignmentSequence.push_back(value[i]);
                        double probability = heuristic[i + 1] - heuristic[i];
                        sumLimit -= probability;
                        for (NsIndex j = heuristic.size() - 1; j > i; ++j)
                                heuristic[j] -= probability;
                }
        }
        return assignmentSequence;
}

///  Returns the i, with x belonging to [intervalStart[i],intervalStart[i+1]).

int intervalBinarySearch(const NsDeque<double>& intervalStart, const double x)
{
        NsIndex i;
        NsIndex low = 0;
        NsIndex high = intervalStart.size() - 2;
        for (;;) {
                i = (low + high) / 2;
                if (intervalStart[i] <= x && x < intervalStart[i + 1])
                        break;
                if (x < intervalStart[i])
                        high = i;
                else
                        low = i + 1;
                assert_that(low <= high,
                            "intervalBinarySearch: Bad `intervalStart'");
        }
        return i;
}

int unboundVarsConnecedTo(const int courseIndex,
                          const NsDeque<struct itcCourseInfo_t>& courseInfo,
                          const NsIntVarArray& vLectPeriod,
                          const struct itcProblem_t& pr)
{
        int unboundVars = 0;
        for (int i = 0; i < pr.ncourses; ++i) {
                if (courseInfo[courseIndex].isConnectedToCourse[i]) {
                        for (int j = courseInfo[i].firstLectIndex;
                             j <= courseInfo[i].lastLectIndex; ++j)
                                unboundVars += (!vLectPeriod[j].isBound());
                }
        }
        return unboundVars;
}

int varHeurObjectiveCriterion(const int courseIndex,
                              const NsDeque<struct itcCourseInfo_t>& courseInfo,
                              const struct itcProblem_t& pr,
                              const NsIntVarArray& vBusyDays)
{
        return (pr.course[courseIndex].students +
                5 * (pr.course[courseIndex].minWorkingDays >
                     vBusyDays[courseIndex].min()) +
                2 * courseInfo[courseIndex].group.size() +
                pr.course[courseIndex].lectures);
}

double
heurEstimationForVariable(const int index, const int courseIndex,
                          const NsDeque<struct itcCourseInfo_t>& courseInfo,
                          const NsIntVarArray& vLectPeriod,
                          const struct itcProblem_t& pr,
                          const NsIntVarArray& vBusyDays)
{
        // int  unboundLecturesOfTheSameCourse = 0;
        // for (int j = courseInfo[courseIndex].firstLectIndex;   j <=
        // courseInfo[courseIndex].lastLectIndex;   ++j)
        //	unboundLecturesOfTheSameCourse  +=  ( !vLectPeriod[j].isBound()
        //);
        /*cout << "   (" << pr.days << "*" << pr.hours << " - " <<
        vLectPeriod[index].size() << ") * " << pr.max2ndVarHeurCriterion << " *
        " << pr.max3rdVarHeurCriterion << "\n"
                 << " + " <<
        unboundVarsConnecedTo(courseIndex,courseInfo,vLectPeriod,pr) << "
        * " << pr.max3rdVarHeurCriterion << "\n"
                 << " + " <<
        varHeurObjectiveCriterion(courseIndex,courseInfo,pr,vBusyDays)    <<
        "\n"; cout << "   ---------------------\n     " << ( (pr.days * pr.hours
        - vLectPeriod[index].size()) * pr.max2ndVarHeurCriterion *
        pr.max3rdVarHeurCriterion +
        unboundVarsConnecedTo(courseIndex,courseInfo,vLectPeriod,pr)
        * pr.max3rdVarHeurCriterion +
        varHeurObjectiveCriterion(courseIndex,courseInfo,pr,vBusyDays)   ) <<
        "\n\n";*/
        return (
            (pr.days * pr.hours - vLectPeriod[index].size()) *
                pr.max2ndVarHeurCriterion * pr.max3rdVarHeurCriterion +
            unboundVarsConnecedTo(courseIndex, courseInfo, vLectPeriod, pr) *
                pr.max3rdVarHeurCriterion +
            varHeurObjectiveCriterion(courseIndex, courseInfo, pr, vBusyDays));
}

double howMuchConstrainingValue(
    const int period, const NsIntVarArray& vLectPeriod,
    const NsIntVarArray& vLectRoom, const int index, const int courseIndex,
    const NsDeque<struct itcCourseInfo_t>& courseInfo,
    const NsIntVarArray& vRooms, const struct itcProblem_t& pr);

//  VALUE ORDERING HEURISTIC  //

NsInt ValHeurTimetabling::select(const NsIntVar& V)
{
        double foo;
        return select(V, foo);
}

NsInt ValHeurTimetabling::select(const NsIntVar& V, double& piece)
{
        piece = 1.0 / V.size();
        int bestPeriod = -1 /*, bestRoom*/; // initializations suppress warnings
        /*  Searching to find the index of `V' in the array `vLectPeriod'.  */
        int index;
        for (index = 0; &vLectPeriod[index] != &V; ++index)
                /*VOID*/;
        int p, d, h, i, j, k, objectiveEstimation;
        double howMuchConstrainingIs, leastConstraining = DBL_MAX, estimation,
                                      bestEstimation = DBL_MAX;
        int courseIndex = lectInfo[index].courseIndex;
        NsDeque<NsInt> value;
        NsDeque<double> heuristic, heuristic1, heuristic2;
        double maxHeuristic1 = DBL_MIN, minHeuristic1Difference = DBL_MAX,
               minHeuristic2Obj = DBL_MAX, maxHeuristic2Obj = DBL_MIN;
        if (((pr.isActiveLocalSearch) ? pr.valHeuristicType_ls
                                      : pr.valHeuristicType) == RANDOM) {
                value.resize(vLectPeriod[index].size());
                heuristic.resize(value.size() + 1);
                heuristic1.resize(value.size());
                heuristic2.resize(value.size());
        }
        // //  Initially, we construct an array `dayDistanceFromBusyDay' to use
        // it as as a         //
        // //   tiebreaker:  When the Value Ordering Heuristic function has to
        // choose between two  //
        // //   values (periods) with the same evaluation (with reference to the
        // first criterion)  //
        // //   represented by the variable `objectiveEstimation', it favours
        // the one connected    //
        // //   to the day with the greatest distance from the other lectures of
        // the course.       //
        //
        // NsDeque<int>  dayDistanceFromBusyDay(pr.days);
        //
        // int  previousBusyDay = -1;
        //
        // for (d=0;  d < pr.days;  ++d)   {
        //
        // 	if ( vIsBusyDay[courseIndex][d].min() == 1 )   {
        //
        // 		//  Day `d' is busy, i.e., occupied by a lecture of the
        // course.  //
        //
        // 		for (int d1=previousBusyDay+1;  d1 < d;  ++d1)
        // 			dayDistanceFromBusyDay[d1] = min(
        // dayDistanceFromBusyDay[d1], d-d1 );
        //
        // 		previousBusyDay = d;
        //
        // 	} else if ( previousBusyDay != -1 )   {
        //
        // 		dayDistanceFromBusyDay[d] = d - previousBusyDay;
        //
        // 	} else   {
        //
        // 		dayDistanceFromBusyDay[d] = INT_MAX;
        // 	}
        // }
        /*  Evaluation of all the variable values (possible periods for the
         * lecture)  */
        unsigned valNum = 0;
        for (NsIntVar::const_iterator valPeriod = vLectPeriod[index].begin();
             valPeriod != vLectPeriod[index].end(); ++valPeriod, ++valNum) {
                p = *valPeriod;
                d = p / pr.hours;
                h = p % pr.hours;
                objectiveEstimation = 0;
                /*  Minimum Working Days-Oriented Objective Function Estimation
                 */
                int workingDaysNeeded = pr.course[courseIndex].minWorkingDays -
                                        vBusyDays[courseIndex].min();
                if (workingDaysNeeded > 0 &&
                    vIsBusyDay[courseIndex][d].min() == 0) {
                        //  If the `minWorkingDays' limit has not been reached
                        //  (i.e. the course             //
                        //   lectures are not spread enough during the week), we
                        //   favour the                 // assignment of the
                        //   lecture on a day `d' that has not been already used
                        //   // (i.e. `vIsBusyDay[courseIndex][d].min() == 0').
                        //   In this case, we              // increase the
                        //   estimation of the objective function by `5', and
                        //   not by `1',      // according to the corresponding
                        //   soft constraint (Minimum Working Days) weight.  //
                        assert_that(
                            vIsBusyDay[courseIndex][d].max() == 1,
                            "It should be possible that day `d' is busy");
                        objectiveEstimation += 5;
                        int d2, dayBusy, lectureIndexInCourse,
                            objectiveEstimation_minWorkingDays = 0;
                        dayBusy = d;
                        for (i = index - 1;
                             i >= courseInfo[courseIndex].firstLectIndex &&
                             !vLectPeriod[i].isBound();
                             --i) {
                                for (d2 = dayBusy - 1; d2 >= 0; --d2) {
                                        lectureIndexInCourse =
                                            i - courseInfo[courseIndex]
                                                    .firstLectIndex;
                                        if (vLectDay[courseIndex]
                                                    [lectureIndexInCourse]
                                                        .contains(d2)) {
                                                objectiveEstimation_minWorkingDays +=
                                                    5;
                                                dayBusy = d2;
                                        }
                                }
                        }
                        dayBusy = d;
                        for (i = index + 1;
                             i <= courseInfo[courseIndex].lastLectIndex &&
                             !vLectPeriod[i].isBound();
                             ++i) {
                                for (d2 = dayBusy + 1; d2 < pr.days; ++d2) {
                                        lectureIndexInCourse =
                                            i - courseInfo[courseIndex]
                                                    .firstLectIndex;
                                        if (vLectDay[courseIndex]
                                                    [lectureIndexInCourse]
                                                        .contains(d2)) {
                                                objectiveEstimation_minWorkingDays +=
                                                    5;
                                                dayBusy = d2;
                                        }
                                }
                        }
                        objectiveEstimation +=
                            min(objectiveEstimation_minWorkingDays,
                                5 * workingDaysNeeded);
                }
                /*  Curriculum Compactness-Oriented Objective Function
                 * Estimation  */
                if (pr.hours > 1) {
                        /* We have not a trivial case. */
                        for (k = 0; static_cast<unsigned>(k) <
                                    courseInfo[courseIndex].group.size();
                             ++k) {
                                /* We iterate through all the curricula that the
                                 * course belongs to. */
                                i = courseInfo[courseIndex].group[k];
                                for (j = 0; static_cast<unsigned>(j) <
                                            vGroup[i].size();
                                     ++j) {
                                        if (&vGroup[i][j] !=
                                            &vLectPeriod[index]) {
                                                /* We iterate through all the
                                                 * other courses of the
                                                 * curriculum. */
                                                //  Checking whether period `p'
                                                //  "glues" to any other lecture
                                                //  of    //
                                                //   the curriculum.  If this is
                                                //   true, we increase the
                                                //   estimation  // we make for
                                                //   the objective function by
                                                //   2.  We did use `2'      //
                                                //   instead of `1', because
                                                //   this is the given weight
                                                //   for the      //
                                                //   corresponding soft
                                                //   constraint (Curriculum
                                                //   Compactness).       //
                                                if (h == 0 &&
                                                    p <= vGroup[i][j].min() &&
                                                    vGroup[i][j].max() <=
                                                        p + 1) {
                                                        objectiveEstimation +=
                                                            2;
                                                        assert_that(
                                                            vIsolatedLect[i][p +
                                                                             1]
                                                                    .min() == 0,
                                                            "It should be "
                                                            "possible that the "
                                                            "lecture is not "
                                                            "isolated");
                                                        //  [*] If the following
                                                        //  lecture (the other
                                                        //  lecture of the    //
                                                        //   curriculum that the
                                                        //   `index'-th lecture
                                                        //   glues to, if //
                                                        //   assigned `p') is
                                                        //   isolated, then the
                                                        //   assignment we //
                                                        //   examine will make
                                                        //   it non-isolated,
                                                        //   thus furthermore //
                                                        //   improving the
                                                        //   objective function.
                                                        //   //
                                                        if (vIsolatedLect[i][p +
                                                                             1]
                                                                .max() == 1)
                                                                objectiveEstimation +=
                                                                    2;
                                                } else if (h == pr.hours - 1 &&
                                                           p - 1 <=
                                                               vGroup[i][j]
                                                                   .min() &&
                                                           vGroup[i][j].max() <=
                                                               p) {
                                                        objectiveEstimation +=
                                                            2;
                                                        assert_that(
                                                            vIsolatedLect[i][p -
                                                                             1]
                                                                    .min() == 0,
                                                            "It should be "
                                                            "possible that the "
                                                            "lecture is not "
                                                            "isolated");
                                                        //  See [*] above.  //
                                                        if (vIsolatedLect[i][p -
                                                                             1]
                                                                .max() == 1)
                                                                objectiveEstimation +=
                                                                    2;
                                                } else if (0 < h &&
                                                           h < pr.hours - 1 &&
                                                           p - 1 <=
                                                               vGroup[i][j]
                                                                   .min() &&
                                                           vGroup[i][j].max() <=
                                                               p + 1) {
                                                        objectiveEstimation +=
                                                            2;
                                                        // if ( !(
                                                        // vIsolatedLect[i][p-1].min()
                                                        // == 0
                                                        //		&&
                                                        //vIsolatedLect[i][p+1].min()
                                                        //== 0 ) )
                                                        //{
                                                        //	cout <<
                                                        //"vGroup[" << i << "] =
                                                        //" << vGroup[i] <<
                                                        //"\n"; 	cout <<
                                                        //"vIsolatedLect[" << i
                                                        //<< "] = " <<
                                                        //vIsolatedLect[i] <<
                                                        //"\n";
                                                        //}
                                                        assert_that(
                                                            (vIsolatedLect[i][p -
                                                                              1]
                                                                     .min() ==
                                                                 0 &&
                                                             vIsolatedLect[i][p +
                                                                              1]
                                                                     .min() ==
                                                                 0),
                                                            "It should be "
                                                            "possible that the "
                                                            "lecture is not "
                                                            "isolated");
                                                        //  See [*] above.  //
                                                        if (vGroup[i][j]
                                                                .isBound() &&
                                                            vIsolatedLect
                                                                    [i]
                                                                    [vGroup[i][j]
                                                                         .value()]
                                                                        .max() ==
                                                                1) {
                                                                objectiveEstimation +=
                                                                    2;
                                                        }
                                                }
                                        }
                                }
                        }
                }
                int /*bestRoomForP,*/ bestRoomObjectiveEstimation;
                /*bestRoomForP =*/ValueOrderHeurForRooms(
                    bestRoomObjectiveEstimation, p, vLectRoom, index, pr,
                    lectInfo, courseInfo, vRooms);
                //  See [**] below.
                objectiveEstimation *= pr.nranks;
                objectiveEstimation += bestRoomObjectiveEstimation;
                howMuchConstrainingIs = howMuchConstrainingValue(
                    p, vLectPeriod, vLectRoom, index, courseIndex, courseInfo,
                    vRooms, pr);
                estimation = pr.leastConstrainingValueHeuristicWeight *
                                 howMuchConstrainingIs -
                             pr.objectiveOrientedValueHeuristicWeight *
                                 objectiveEstimation;
                if (((pr.isActiveLocalSearch)
                         ? pr.valHeuristicType_ls
                         : pr.valHeuristicType) == RANDOM) {
                        value[valNum] = p;
                        heuristic1[valNum] =
                            pr.leastConstrainingValueHeuristicWeight *
                            howMuchConstrainingIs;
                        heuristic2[valNum] =
                            pr.objectiveOrientedValueHeuristicWeight *
                            objectiveEstimation;
                        if (heuristic1[valNum] > maxHeuristic1)
                                maxHeuristic1 = heuristic1[valNum];
                        if (heuristic2[valNum] < minHeuristic2Obj)
                                minHeuristic2Obj = heuristic2[valNum];
                        if (heuristic2[valNum] > maxHeuristic2Obj)
                                maxHeuristic2Obj = heuristic2[valNum];
                        for (i = 0; static_cast<unsigned>(i) < valNum; ++i) {
                                if (heuristic1[i] != heuristic1[valNum] &&
                                    abs(heuristic1[i] - heuristic1[valNum]) <
                                        minHeuristic1Difference)
                                        minHeuristic1Difference = abs(
                                            heuristic1[i] - heuristic1[valNum]);
                        }
                }
                if ((pr.solutions == 0 &&
                     (howMuchConstrainingIs < leastConstraining ||
                      (howMuchConstrainingIs == leastConstraining &&
                       estimation < bestEstimation))) ||
                    (pr.solutions > 0 && estimation < bestEstimation)) {
                        leastConstraining = howMuchConstrainingIs;
                        bestEstimation = estimation;
                        bestPeriod = p;
                        // bestRoom   = bestRoomForP;
                }
        }
        if (((pr.isActiveLocalSearch) ? pr.valHeuristicType_ls
                                      : pr.valHeuristicType) == RANDOM) {
                for (i = 1; static_cast<unsigned>(i) < heuristic.size(); ++i) {
                        heuristic[i] = maxHeuristic1 - heuristic1[i - 1] + 1.0;
                        if (pr.solutions == 0)
                                heuristic[i] *=
                                    (1.0 / minHeuristic1Difference) *
                                    ((maxHeuristic2Obj - minHeuristic2Obj +
                                      1.0) +
                                     1.0);
                        heuristic[i] +=
                            heuristic2[i - 1] - minHeuristic2Obj + 1.0;
                        //  The term `- minHeuristic2Obj + 1.0' ensures  //
                        //   that `heuristic[i]' is a positive number. //
                        assert_that(heuristic[i] > 0.0,
                                    "Negative `heuristic[i]'");
                }
                //  Number of bound variables
                int numBound = 0;
                for (NsIntVarArray::const_iterator Var = vLectPeriod.begin();
                     Var != vLectPeriod.end(); ++Var) {
                        numBound += Var->isBound();
                }
                double conf = pr.conf + ((100.0 - pr.conf) * numBound) /
                                            vLectPeriod.size();
                NsIndex bestIndex = randomizeHeuristic(heuristic, conf);
                bestPeriod = value[bestIndex];
                piece = heuristic[bestIndex + 1] - heuristic[bestIndex];
        }
        return bestPeriod;
}

//  SPLIT VALUE ORDERING HEURISTIC  //

double SplitValHeurTimetabling::select(const NsIntVar& V)
{
        /*  Searching to find the index of `V' in the array `vLectPeriod'.  */
        int index;
        for (index = 0; &vLectPeriod[index] != &V; ++index)
                /*VOID*/;
        int i, j;
        int courseIndex = lectInfo[index].courseIndex;
        //  FAIR DOMAIN SPLITTING.
        double splitValue, meanMiddleValue = 0.0, weightSum = 0.0;
        // cout << "X = " << V << ".\n";
        for (i = courseInfo[courseIndex].firstLectIndex;
             i <= courseInfo[courseIndex].lastLectIndex; ++i) {
                if (i != index && !vLectPeriod[i].isBound() &&
                    !intersectionEmpty2(&vLectPeriod[index], &vLectPeriod[i])) {
                        splitValue = splitValueConstrLess(
                            vLectPeriod[index], vLectPeriod[i], (index > i));
                        meanMiddleValue +=
                            (1.0 / vLectPeriod[i].size()) * splitValue;
                        weightSum += 1.0 / vLectPeriod[i].size();
                        // cout << "X " << ((index > i)? ">" : "<") << " Y " <<
                        // vLectPeriod[i]
                        //	<< "    MiddleValue = "
                        //	<< splitValueConstrLess(vLectPeriod[index],
                        //vLectPeriod[i], (index > i))
                        //	<< ".\n";
                        assert_that(V.min() <= splitValue &&
                                        splitValue <= V.max(),
                                    "SplitValHeurTimetabling::select:  Wrong "
                                    "split value");
                } //  else  {
                //	cout << "X = " << vLectPeriod[index] << ".\n";
                //}
        }
        for (i = 0; i < pr.ncourses; ++i) {
                if (i != courseIndex) {
                        if (courseInfo[courseIndex].isConnectedToCourse[i]) {
                                for (j = courseInfo[i].firstLectIndex;
                                     j <= courseInfo[i].lastLectIndex; ++j) {
                                        if (!intersectionEmpty2(
                                                &vLectPeriod[index],
                                                &vLectPeriod[j])) {
                                                splitValue =
                                                    splitValueConstrNeq(
                                                        vLectPeriod[index],
                                                        vLectPeriod[j]);
                                                meanMiddleValue +=
                                                    (1.0 /
                                                     vLectPeriod[j].size()) *
                                                    splitValue;
                                                weightSum +=
                                                    1.0 / vLectPeriod[j].size();
                                                // cout << "X != Y " <<
                                                // vLectPeriod[j]
                                                //	<< "    MiddleValue = "
                                                //	<<
                                                //splitValueConstrNeq(vLectPeriod[index],
                                                //vLectPeriod[j])
                                                //	<< ".\n";
                                                assert_that(
                                                    V.min() <= splitValue &&
                                                        splitValue <= V.max(),
                                                    "SplitValHeurTimetabling::"
                                                    "select:  Wrong split "
                                                    "value");
                                        }
                                        //  If the course is connected through a
                                        //  mutual exclusion constraint
                                        //   with a lecture `j', then there will
                                        //   be a value removal if
                                        //   `vLectPeriod[j].contains(period)',
                                        //   so we add the corresponding value
                                        //   percentage.
                                }
                        } else {
                                for (j = courseInfo[i].firstLectIndex;
                                     j <= courseInfo[i].lastLectIndex; ++j) {
                                        if (!intersectionEmpty2(
                                                &vLectPeriod[index],
                                                &vLectPeriod[j])) {
                                                int availableRooms = 0;
                                                for (NsIntVar::const_iterator
                                                         valRoom = vLectRoom[j]
                                                                       .begin();
                                                     valRoom !=
                                                     vLectRoom[j].end();
                                                     ++valRoom) {
                                                        availableRooms +=
                                                            (vRooms[j].next(
                                                                 pr.days *
                                                                     pr.hours *
                                                                     *valRoom +
                                                                 max(vLectPeriod
                                                                         [index]
                                                                             .min(),
                                                                     vLectPeriod[j]
                                                                         .min()) -
                                                                 1) <=
                                                             pr.days *
                                                                     pr.hours *
                                                                     *valRoom +
                                                                 min(vLectPeriod
                                                                         [index]
                                                                             .max(),
                                                                     vLectPeriod[j]
                                                                         .max()));
                                                }
                                                if (availableRooms > 0) {
                                                        splitValue =
                                                            splitValueConstrNeq(
                                                                vLectPeriod
                                                                    [index],
                                                                vLectPeriod[j]);
                                                        meanMiddleValue +=
                                                            ((1.0 /
                                                              availableRooms) /
                                                             vLectPeriod[j]
                                                                 .size()) *
                                                            splitValue;
                                                        weightSum +=
                                                            (1.0 /
                                                             availableRooms) /
                                                            vLectPeriod[j]
                                                                .size();
                                                        // cout << "X != Y " <<
                                                        // vLectPeriod[j]
                                                        //	<< "
                                                        //(availableRooms = " <<
                                                        //availableRooms
                                                        //	<< ")"
                                                        //	<< "
                                                        //MiddleValue = "
                                                        //	<<
                                                        //splitValueConstrNeq(vLectPeriod[index],
                                                        //vLectPeriod[j])
                                                        //	<< ".\n";
                                                        assert_that(
                                                            V.min() <=
                                                                    splitValue &&
                                                                splitValue <=
                                                                    V.max(),
                                                            "SplitValHeurTimeta"
                                                            "bling::select:  "
                                                            "Wrong split "
                                                            "value");
                                                }
                                        }
                                        //  We used `1.0', because, obviously,
                                        //  `vLectPeriod[j].contains(period)==true'.
                                        //  A slightly different case to the
                                        //  above.  There is not a mutual
                                        //  exclusion
                                        //   constraint between the two
                                        //   lectures; however, they share the
                                        //   same rooms. So, an assignment
                                        //   vLectPeriod[i]=period limits the
                                        //   assignment vLectPeriod[j]=period by
                                        //   the percentage `1 /
                                        //   availableRooms'.
                                }
                        }
                }
        }
        // cout << "Average = " << ( meanMiddleValue / weightSum ) << ".\n";
        // system("pause");
        splitValue = ((weightSum > 0.0) ? meanMiddleValue / weightSum
                                        : ValHeurMiddle().select(V));
        assert_that(
            V.min() <= splitValue && splitValue <= V.max(),
            "SplitValHeurTimetabling::select:  Wrong average split value");
        return splitValue;
}

//  VALUE ORDERING HEURISTIC for room selection for a known period of a lecture
//  //
//   (It takes into account the Room Compactness soft constraint.) //

int ValueOrderHeurForRooms(int& bestRoomObjectiveEstimation, const int period,
                           const NsIntVarArray& vLectRoom, const int index,
                           const struct itcProblem_t& pr,
                           const NsDeque<struct itcLectureInfo_t>& lectInfo,
                           const NsDeque<struct itcCourseInfo_t>& courseInfo,
                           const NsIntVarArray& vRooms)
{
        int courseIndex = lectInfo[index].courseIndex;
        int bestRoom = -1, roomObjectiveEstimation;
        bestRoomObjectiveEstimation = INT_MIN;
        //  Due to the Room Stability soft constraint, we prefer using the
        //  classrooms  //
        //   that have been already used for another lecture for the course. //
        for (int j = courseInfo[courseIndex].firstLectIndex;
             j <= courseInfo[courseIndex].lastLectIndex; ++j) {
                if (vLectRoom[j].isBound() &&
                    vRooms[index].contains(period + pr.days * pr.hours *
                                                        vLectRoom[j].value()) &&
                    pr.room[vLectRoom[j].value()].capacity >=
                        pr.course[courseIndex].students &&
                    pr.room[vLectRoom[j].value()].rank >
                        bestRoomObjectiveEstimation) {
                        bestRoomObjectiveEstimation =
                            1 * pr.nranks + pr.room[vLectRoom[j].value()].rank;
                        bestRoom = vLectRoom[j].value();
                        //  The `1' above represents the Room Stability soft
                        //  constraint weight.
                        //   See also [**] below.
                }
        }
        if (bestRoom == -1) {
                // We continue searching for a proper classroom...
                for (NsIntVar::const_iterator valRoom =
                         vLectRoom[index].begin();
                     valRoom != vLectRoom[index].end(); ++valRoom) {
                        if (vRooms[index].contains(period + pr.days * pr.hours *
                                                                *valRoom)) {
                                //  The most proper classroom for a lecture is
                                //  the one that has the  //
                                //   closest capacity to the number of the
                                //   students that attend to   // that lecture,
                                //   given that the capacity is greater or equal
                                //   to    // the number of students.  If the
                                //   last condition does not hold,   // then we
                                //   prefer the classroom with the greatest
                                //   capacity.        //
                                if (pr.room[*valRoom].capacity >=
                                    pr.course[courseIndex].students) {
                                        // (1)
                                        roomObjectiveEstimation =
                                            pr.room[*valRoom].rank;
                                } else {
                                        roomObjectiveEstimation =
                                            (pr.room[*valRoom].capacity
                                             // (2)
                                             -
                                             pr.course[courseIndex].students) *
                                            pr.nranks;
                                }
                                //  The above statement suggests that if the
                                //  room capacity does not   //
                                //   suffice for the number of students of the
                                //   course, the objective  // estimation is
                                //   decreased by the corresponding difference.
                                //   //
                                //                                                                    //
                                //  [**] Notice that in this case the objective
                                //  estimation is         //
                                //   multiplied by `pr.nranks'.  We did this
                                //   because the objective    // estimation (1)
                                //   is not guaranteed, though the one in (2)
                                //   --the    // reduction to the objective
                                //   function-- is guaranteed to take      //
                                //   place.  So, multiplying it by `pr.nranks'
                                //   makes it stronger      // than (1);
                                //   besides, (1) is limited within the range 0
                                //   to          // `pr.nranks-1'.
                                //   //
                                if (roomObjectiveEstimation >
                                    bestRoomObjectiveEstimation) {
                                        bestRoomObjectiveEstimation =
                                            roomObjectiveEstimation;
                                        bestRoom = *valRoom;
                                }
                        }
                }
        }
        return bestRoom;
}

double howMuchConstrainingValue(
    const int period, const NsIntVarArray& vLectPeriod,
    const NsIntVarArray& vLectRoom, const int index, const int courseIndex,
    const NsDeque<struct itcCourseInfo_t>& courseInfo,
    const NsIntVarArray& vRooms, const struct itcProblem_t& pr)
{
        double howMuchConstraining = 0.0, valuesToRemove;
        NsInt i, j, limit;
        //  Counting the percentage of the values of the variables of the
        //  lectures    //
        //   of the same course that will be removed due to the examined
        //   assignment.  //
        limit = period;
        for (i = index - 1; i >= courseInfo[courseIndex].firstLectIndex; --i) {
                //  First, we iterate through the lectures that precede the
                //  lecture to be assigned the value...
                limit = vLectPeriod[i].previous(limit);
                valuesToRemove = 0;
                for (j = vLectPeriod[i].next(limit); j <= vLectPeriod[i].max();
                     j = vLectPeriod[i].next(j))
                        ++valuesToRemove;
                howMuchConstraining +=
                    pow(valuesToRemove / vLectPeriod[i].size(),
                        pr.constrainingFactorExponent);
        }
        limit = period;
        for (i = index + 1; i <= courseInfo[courseIndex].lastLectIndex; ++i) {
                //  ...then, we iterate through the lectures that are next to
                //  the lecture to be assigned the value.
                limit = vLectPeriod[i].next(limit);
                valuesToRemove = 0;
                for (j = vLectPeriod[i].previous(limit);
                     j >= vLectPeriod[i].min(); j = vLectPeriod[i].previous(j))
                        ++valuesToRemove;
                howMuchConstraining +=
                    pow(valuesToRemove / vLectPeriod[i].size(),
                        pr.constrainingFactorExponent);
        }
        for (i = 0; i < pr.ncourses; ++i) {
                if (i != courseIndex) {
                        if (courseInfo[courseIndex].isConnectedToCourse[i]) {
                                for (j = courseInfo[i].firstLectIndex;
                                     j <= courseInfo[i].lastLectIndex; ++j) {
                                        howMuchConstraining +=
                                            pow(static_cast<double>(
                                                    vLectPeriod[j].contains(
                                                        period)) /
                                                    vLectPeriod[j].size(),
                                                pr.constrainingFactorExponent);
                                        //  If the course is connected through a
                                        //  mutual exclusion constraint
                                        //   with a lecture `j', then there will
                                        //   be a value removal if
                                        //   `vLectPeriod[j].contains(period)',
                                        //   so we add the corresponding value
                                        //   percentage.
                                }
                        } else {
                                for (j = courseInfo[i].firstLectIndex;
                                     j <= courseInfo[i].lastLectIndex; ++j) {
                                        if (vLectPeriod[j].contains(period)) {
                                                int availableRooms = 0;
                                                for (NsIntVar::const_iterator
                                                         valRoom = vLectRoom[j]
                                                                       .begin();
                                                     valRoom !=
                                                     vLectRoom[j].end();
                                                     ++valRoom) {
                                                        availableRooms +=
                                                            vRooms[j].contains(
                                                                period +
                                                                pr.days *
                                                                    pr.hours *
                                                                    *valRoom);
                                                }
                                                if (availableRooms == 0)
                                                        continue;
                                                assert_that(availableRooms > 0,
                                                            "Could not find "
                                                            "available room "
                                                            "for an available "
                                                            "period slot");
                                                howMuchConstraining += pow(
                                                    (1.0 / availableRooms) /
                                                        vLectPeriod[j].size(),
                                                    pr.constrainingFactorExponent);
                                                //  We used `1.0', because,
                                                //  obviously,
                                                //  `vLectPeriod[j].contains(period)==true'.
                                                //  A slightly different case to
                                                //  the above.  There is not a
                                                //  mutual exclusion
                                                //   constraint between the two
                                                //   lectures; however, they
                                                //   share the same rooms. So,
                                                //   an assignment
                                                //   vLectPeriod[i]=period
                                                //   limits the assignment
                                                //   vLectPeriod[j]=period by
                                                //   the percentage `1 /
                                                //   availableRooms'.
                                        }
                                }
                        }
                }
        }
        return howMuchConstraining;
}
