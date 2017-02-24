// Part of https://github.com/pothitos/naxos

#include "localSearch.h"
#include "error.h"
#include <cstdlib>

using namespace naxos;
using namespace std;

#if 0
goalLocalSearch::goalLocalSearch (naxos::NsIntVarArray& Vars_init, naxos::NsIntVarArray& classVars_init,
                                  struct course_t course_arg_init[], int ncourse_init,
                                  const naxos::NsDeque<CLectureInfo>& vi_init, struct setup_t& setup_init, int d, int h,
                                  int& cancel_init, int& progress_init)
        : Vars(Vars_init), classVars(classVars_init), course_arg(course_arg_init), ncourse(ncourse_init),
          vi(vi_init), setup(setup_init), days(d), hours(h),
          cancel(cancel_init), progress(progress_init), currentWidthBound(2)
{
        // For simplity, we pass Local Search's parameters to the ones for (plain) search.
        //  Old values are overwritten!
        setup.search_method = setup.ls_search_method;
        setup.lds_max_discr = setup.ls_lds_max_discr;
        setup.lds_step      = setup.ls_lds_step;
        setup.lds_lookahead = setup.ls_lds_lookahead;
}

NsGoal *
goalLocalSearch::GOAL (void)
{
        cout << "goalLocalSearch\n";
        if (currentWidthBound > hours) {
                Vars[0].removeAll();
                return  0;
        }
        ++currentWidthBound;
        return  ( new NsgOR( new NsgOR ( new goalLsWeek(Vars, classVars, course_arg, ncourse, vi, setup, days, hours,
                                         cancel, progress) ,
                                         new goalLsSlices(Vars, classVars, course_arg, ncourse, vi, setup, days, hours,
                                                         cancel, progress, (currentWidthBound-1)) ) ,
                             new goalLocalSearch(*this) ) );
}
#endif

NsGoal* goalLsWeek::GOAL(void)
{
        // cout << "  goalLsWeek\n";
        if (currentDay >= pr.days) {
                vLectPeriod[0].removeAll();
                return 0;
        }
        ++currentDay;
        // cout << Vars << "\n";
        // cout << classVars << "\n";
        return (new NsgOR(new goalLsDay(vLectPeriod, vLectPeriodSortedByHeur,
                                        vLectRoom, pr, varHeur, valHeur,
                                        lanAssigns, (currentDay - 1)),
                          new goalLsWeek(*this)));
}

NsGoal* goalLsDay::GOAL(void)
{
        // cout << "    goalLsDay -- (Day:" << currentDay << ")\n";
        int i, j, k;
        for (k = 0, i = 0; i < pr.ncourses; ++i) {
                for (j = 0; j < pr.course[i].lectures; ++j, ++k) {
                        if (pr.course[i].periodScheduled[j] / pr.days !=
                            currentDay) {
                                vLectPeriod[k].set(
                                    pr.course[i].periodScheduled[j]);
                                vLectRoom[k].set(pr.course[i].roomScheduled[j]);
                        }
                }
        }
        return metaSearchMethodGoal(pr, vLectPeriod, vLectPeriodSortedByHeur,
                                    varHeur, valHeur, lanAssigns);
        // if (setup.search_method == 2)
        //	return  (new AmLds(Vars, classVars, vi, setup, days, hours,
        //cancel, progress));  else 	return  (new AmDfsLabeling(Vars,
        //classVars, vi, setup, days, hours, cancel, progress));
}

#if 0
NsGoal *
goalLsSlices::GOAL (void)
{
        cout << "  goalLsSlices -- (WidthBound:" << currentWidthBound << ")\n";
        if (currentWidth > currentWidthBound  ||  currentWidth > hours/2) {
                Vars[0].removeAll();
                return  0;
        }
        int  oldCurrentPosition = currentPosition;
        int  oldCurrentWidth    = currentWidth;
        currentPosition += currentWidth;
        if (currentPosition >= hours) {
                currentPosition = 0;
                ++currentWidth;
        }
        return  ( new NsgOR( new goalLsOneSlice(Vars, classVars, course_arg, ncourse, vi, setup, days, hours,
                                                cancel, progress, oldCurrentPosition, oldCurrentWidth) ,
                             new goalLsSlices(*this) ) );
}

NsGoal *
goalLsOneSlice::GOAL (void)
{
        cout << "    goalLsOneSlice -- (Pos:" << currentPosition << ", Width:" << currentWidth << ")\n";
        int  i, j, lecture_count;
        lecture_count = 0;
        for (i=0;  i<ncourse;  ++i) {
                for (j=0;  j<course_arg[i].nlecture;  ++j) {
                        if ( !( currentPosition <= course_arg[i].oblig_start_hour[j] % hours
                                && course_arg[i].oblig_start_hour[j] % hours < currentPosition + currentWidth ) ) {
                                //// As the objective gets lower, the domains can be more constrained than before
                                //if ( !Vars[lecture_count].contains( course_arg[i].oblig_start_hour[j] )
                                //	|| !classVars[lecture_count].contains( course_arg[i].oblig_room[j] ) )
                                //{
                                //	Vars[0].removeAll();
                                //	return  0;
                                //}
                                Vars[lecture_count].set( course_arg[i].oblig_start_hour[j] );
                                classVars[lecture_count].set( course_arg[i].oblig_room[j] );
                        }
                        ++lecture_count;
                }
        }
        if (setup.search_method == 2)
                return  (new AmLds(Vars, classVars, vi, setup, days, hours, cancel, progress));
        else
                return  (new AmDfsLabeling(Vars, classVars, vi, setup, days, hours, cancel, progress));
}
#endif
