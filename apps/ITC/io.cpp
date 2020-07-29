// Part of https://github.com/pothitos/naxos

#include "error.h"
#include "timetabling.h"
#include <naxos.h>

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
using namespace std;
using namespace naxos;

#if defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
#define IsPosix
#endif

#ifndef IsPosix
char* strdup(const char* s)
{
        // Space for length plus nul
        char* d = new char[strlen(s) + 1];
        strcpy(d, s); // Copy the characters
        return d;     // Return the new string
}
#endif

void createFctResultsFile(const struct itcProblem_t& pr,
                          string resultsFileName);

struct RoomRank_t {

        struct itcRoom_t* room;
};

struct less_function_RoomRank
    : public binary_function<struct RoomRank_t, struct RoomRank_t, bool> {

        bool operator()(const struct RoomRank_t& X, const struct RoomRank_t& Y)
        {
                return (!(X.room->capacity < Y.room->capacity));
        }
};

void createInstance(struct itcProblem_t& pr, const int argc, char* argv[])
/* Reading input file, i.e. an instance of the timetabling problem */
{
        int i, j, k, d, h;
        if (argc < 3) {
                cerr << "Usage:  " << argv[0]
                     << " <input_file> <results_file> [<random_seed>] "
                        "[-options <options_file>]\n"
                     << "          The above command solves the timetabling "
                        "problem described by\n"
                     << "          <input_file> and writes the solution to the "
                        "<results_file>.\n"
                     //<< "          A <results_file>.plt file is also produced
                     //and can be used by the\n"
                     //<< "          program \"gnuplot\" to visualize the
                     //progress of the search methods.\n"
                     << "          To override the default search options, use "
                        "the \"-options\" parameter.\n"
                     << "          (See the provided \"optionsFile\" for an "
                        "example.)  If a random search\n"
                     << "          method is selected, a <random_seed> number "
                        "can be defined.\n"
                     << "\n"
                     << "        " << argv[0]
                     << " <input_file> <results_file> -fct\n"
                     << "          The above command takes an existing "
                        "<results_file> corresponding\n"
                     << "          to <input_file> and produces a "
                        "<results_file>.fct which can be used\n"
                     << "          to visualize the results using the external "
                        "program \"SKOPtiko.\"\n";
                exit(1);
        }
        ifstream instanceFile(argv[1]);
        assert_that(instanceFile.good(), "Could not open input file");
        string str;
        instanceFile >> str;
        assert_that(str == "Name:", "Expected token \"Name\"");
        instanceFile >> str;
        assert_that((pr.name = strdup(str.c_str())) != 0, "Out of memory");
        instanceFile >> str;
        assert_that(str == "Courses:", "Expected token \"Courses\"");
        instanceFile >> pr.ncourses;
        assert_that(pr.ncourses > 0, "Invalid courses number");
        pr.course = new struct itcCourse_t[pr.ncourses];
        pr.nprofessors = 0;
        pr.professorName = new char*[pr.ncourses];
        /* Professors' number cannot be more than the number of courses. */
        instanceFile >> str;
        assert_that(str == "Rooms:", "Expected token \"Rooms\"");
        instanceFile >> pr.nrooms;
        assert_that(pr.nrooms > 0, "Invalid rooms number");
        pr.room = new struct itcRoom_t[pr.nrooms];
        instanceFile >> str;
        assert_that(str == "Days:", "Expected token \"Days\"");
        instanceFile >> pr.days;
        assert_that(pr.days > 0, "Invalid days number");
        instanceFile >> str;
        assert_that(str == "Periods_per_day:",
                    "Expected token \"Periods_per_day\"");
        instanceFile >> pr.hours;
        assert_that(pr.hours > 0, "Invalid hours number");
        instanceFile >> str;
        assert_that(str == "Curricula:", "Expected token \"Curricula\"");
        instanceFile >> pr.ngroups;
        assert_that(pr.ngroups >= 0, "Invalid groups number");
        if (pr.ngroups > 0)
                pr.group = new struct itcGroup_t[pr.ngroups];
        instanceFile >> str;
        assert_that(str == "Constraints:", "Expected token \"Constraints\"");
        instanceFile >> pr.nConstraints;
        assert_that(pr.nConstraints >= 0, "Invalid constraints number");
        instanceFile >> str;
        assert_that(str == "COURSES:", "Expected token \"COURSES\"");
        for (i = 0; i < pr.ncourses; ++i) {
                instanceFile >> str;
                assert_that((pr.course[i].name = strdup(str.c_str())) != 0,
                            "Out of memory");
                instanceFile >> str;
                /* Searching for the course professor index `j' */
                for (j = 0; j < pr.nprofessors; ++j) {
                        if (pr.professorName[j] == str)
                                break;
                }
                if (j == pr.nprofessors) {
                        /* Read a new professor */
                        ++pr.nprofessors;
                        assert_that(
                            (pr.professorName[j] = strdup(str.c_str())) != 0,
                            "Out of memory");
                }
                /* Found index `j' */
                pr.course[i].professor = j;
                instanceFile >> pr.course[i].lectures >>
                    pr.course[i].minWorkingDays >> pr.course[i].students;
                assert_that(pr.course[i].lectures > 0 &&
                                pr.course[i].minWorkingDays > 0 &&
                                pr.course[i].students >= 0,
                            "Wrong course attribute");
                pr.course[i].nunavail = 0;
                if (pr.nConstraints > 0) {
                        pr.course[i].unavail = new int[pr.nConstraints];
                        /* The number of a course unavailabilities cannot be
                         * greater than the aggregate number of unavailabilities
                         * (`pr.nConstraints'). */
                }
                pr.course[i].lecturesScheduled = 0;
                pr.course[i].periodScheduled = new int[pr.course[i].lectures];
                pr.course[i].roomScheduled = new int[pr.course[i].lectures];
        }
        instanceFile >> str;
        assert_that(str == "ROOMS:", "Expected token \"ROOMS\"");
        for (i = 0; i < pr.nrooms; ++i) {
                instanceFile >> str;
                assert_that((pr.room[i].name = strdup(str.c_str())) != 0,
                            "Out of memory");
                instanceFile >> pr.room[i].capacity;
                assert_that(pr.room[i].capacity > 0, "Wrong room capacity");
        }
        instanceFile >> str;
        assert_that(str == "CURRICULA:", "Expected token \"CURRICULA\"");
        for (i = 0; i < pr.ngroups; ++i) {
                instanceFile >> str;
                assert_that((pr.group[i].name = strdup(str.c_str())) != 0,
                            "Out of memory");
                instanceFile >> pr.group[i].ncourses;
                assert_that(pr.group[i].ncourses > 0,
                            "Wrong group courses number");
                pr.group[i].course = new int[pr.group[i].ncourses];
                for (j = 0; j < pr.group[i].ncourses; ++j) {
                        instanceFile >> str;
                        /* Searching for the course index `k' */
                        for (k = 0; k < pr.ncourses; ++k) {
                                if (pr.course[k].name == str)
                                        break;
                        }
                        assert_that(k < pr.ncourses,
                                    "Invalid course for a group");
                        /* Found index `k' */
                        pr.group[i].course[j] = k;
                }
        }
        instanceFile >> str;
        assert_that(str == "UNAVAILABILITY_CONSTRAINTS:",
                    "Expected token \"UNAVAILABILITY_CONSTRAINTS\"");
        for (i = 0; i < pr.nConstraints; ++i) {
                instanceFile >> str;
                /* Searching for the course index `j' */
                for (j = 0; j < pr.ncourses; ++j) {
                        if (pr.course[j].name == str)
                                break;
                }
                assert_that(j < pr.ncourses,
                            "Invalid course for an unavailability constraint");
                /* Found index `j' */
                instanceFile >> d >> h;
                assert_that(0 <= d && d < pr.days && 0 <= h && h < pr.hours,
                            "Invalid unavailability constraint attribute");
                ++pr.course[j].nunavail;
                pr.course[j].unavail[pr.course[j].nunavail - 1] =
                    d * pr.hours + h;
        }
        instanceFile >> str;
        assert_that(str == "END.", "Expected token \"END\"");
        instanceFile.close();
        if (argc >= 4 && strcmp(argv[3], "-fct") == 0) {
                /* Create an *.fct file from a solution file, to give
                 * the possibility of results graphical display.    */
                assert_that(argc == 4, "Too many arguments");
                ifstream resultsFile(argv[2]);
                assert_that(resultsFile.good(), "Could not open results file");
                string strRoom;
                while (resultsFile >> str >> strRoom >> d >> h) {
                        // cout << "." << str << ".\n";
                        /* Searching for the course index `i' */
                        for (i = 0; i < pr.ncourses; ++i) {
                                if (pr.course[i].name == str)
                                        break;
                        }
                        assert_that(i < pr.ncourses, "Invalid course");
                        /* Found index `i' */
                        assert_that(++pr.course[i].lecturesScheduled <=
                                        pr.course[i].lectures,
                                    "Too many lectures scheduled");
                        // assert_that( resultsFile >> str,  "No room for this
                        // lecture" );
                        /* Searching for the room index `j' */
                        for (j = 0; j < pr.nrooms; ++j) {
                                if (pr.room[j].name == strRoom)
                                        break;
                        }
                        assert_that(j < pr.nrooms,
                                    "Invalid room for this lecture");
                        /* Found index `j' */
                        pr.course[i]
                            .roomScheduled[pr.course[i].lecturesScheduled - 1] =
                            j;
                        assert_that( // resultsFile >> d >> h &&
                            0 <= d && d < pr.days && 0 <= h && h < pr.hours,
                            "Invalid lecture period");
                        pr.course[i]
                            .periodScheduled[pr.course[i].lecturesScheduled -
                                             1] = d * pr.hours + h;
                }
                assert_that(resultsFile.eof() || str == "Cost",
                            "Wrong lecture description");
                resultsFile.close();
                createFctResultsFile(pr, argv[2]);
                exit(EXIT_SUCCESS);
        }
        /* Rooms Ranks (for information purposes) */
        NsDeque<struct RoomRank_t> allRooms(pr.nrooms);
        for (i = 0; i < pr.nrooms; ++i)
                allRooms[i].room = &pr.room[i];
        sort(allRooms.begin(), allRooms.end(), less_function_RoomRank());
        int currentRank;
        for (currentRank = 0, i = 0; i < pr.nrooms; ++i) {
                if (i == 0 ||
                    allRooms[i].room->capacity < allRooms[i - 1].room->capacity)
                        currentRank = i;
                else
                        assert_that(allRooms[i].room->capacity ==
                                        allRooms[i - 1].room->capacity,
                                    "Wrong rank");
                allRooms[i].room->rank = currentRank;
                // cout << "allRooms[" << i << "].rank= " <<
                // allRooms[i].room->rank
                //	<< " .capacity= " << allRooms[i].room->capacity << "\n";
        }
        pr.nranks = currentRank + 1;
        allRooms.clear();
        // NsDeque<int>  roomCapacity(pr.nrooms);
        //
        // for (i=0;  i < pr.nrooms;  ++i)
        //	roomCapacity[i] = - pr.room[i].capacity;
        //
        // sort(  roomCapacity.begin(), roomCapacity.end());
        // unique(roomCapacity.begin(), roomCapacity.end());
        //
        // pr.nranks = roomCapacity.size();
        //
        // for (i=0;  i < pr.nrooms;  ++i)   {
        //	for (j=0;  roomCapacity[j] != - pr.room[i].capacity;  ++j)
        //		/*VOID*/;
        //	assert_that( static_cast<unsigned>(j) < roomCapacity.size(),
        //"Wrong rank" ); 	pr.room[i].rank = j;
        //}
        //
        // roomCapacity.clear();
        /* Use of the random seed. */
        if (argc >= 4 && isdigit(argv[3][0]))
                srand(atoi(argv[3]));
        /* Definitions of general options */
        /* Default options */
        pr.searchMethod = DBS;
        pr.searchMethodParameter[0] = 0;
        pr.varHeuristicType = NORMAL;
        pr.valHeuristicType = NORMAL;
        // pr.valHeuristicTransform    = false;
        pr.isActiveLocalSearch = false;
        pr.searchMethod_ls = DFS;
        pr.varHeuristicType_ls = NORMAL;
        pr.valHeuristicType_ls = NORMAL;
        // pr.valHeuristicTransform_ls    = false;
        // pr.search_roomBacktrack = false;
        pr.timeLimit = 0;
        pr.timeLimitDirectMethodRound = 60;
        pr.timeLimitIndirectMethodRound = 60;
        // pr.isCpuTime                = false;
        pr.quiet = false;
        pr.constrainingFactorExponent = 1.0;
        pr.conf = 1.0;
        pr.leastConstrainingValueHeuristicWeight = 1.0;
        pr.objectiveOrientedValueHeuristicWeight = 1.0;
        if ((argc == 4 && strcmp(argv[3], "-options") == 0) ||
            (argc == 5 && strcmp(argv[4], "-options") == 0)) {
                throw timetableException("Missing options-file");
        }
        str = "";
        if (argc == 5 && strcmp(argv[3], "-options") == 0)
                str = argv[4];
        else if (argc == 6 && isdigit(argv[3][0]) &&
                 strcmp(argv[4], "-options") == 0)
                str = argv[5];
        else if ((argc == 4 && !isdigit(argv[3][0])) || argc >= 5)
                throw timetableException("Wrong arguments.  Run without "
                                         "arguments to see the available "
                                         "options.");
        if (str != "") {
                /* Loading an options-file */
                ifstream optionsFile(str.c_str());
                assert_that(optionsFile.good(), "Could not open options-file");
                bool alreadyReadAnOption = false;
                for (;;) {
                        if (alreadyReadAnOption)
                                alreadyReadAnOption = false;
                        else if (!(optionsFile >> str))
                                break;
                        if (str[0] == '#') {
                                /* Ignore comments */
                                while (optionsFile.get() != '\n')
                                        /* VOID */;
                                continue;
                        }
                        if (str == "searchMethod") {
                                assert_that(
                                    (optionsFile >> str).good(),
                                    "Missing value for \"searchMethod\"");
                                if (str == "DFS") {
                                        pr.searchMethod = DFS;
                                        /* Default parameters */
                                        pr.varHeuristicType = NORMAL;
                                        if (!(optionsFile >> str))
                                                break;
                                        if (str != "RANDOM_VAR_HEUR" &&
                                            str != "RANDOM_VAL_HEUR") {
                                                /* Not a parameter, so continue
                                                 * with the next option. */
                                                alreadyReadAnOption = true;
                                                continue;
                                        }
                                        if (str == "RANDOM_VAR_HEUR")
                                                pr.varHeuristicType = RANDOM;
                                        else
                                                pr.valHeuristicType = RANDOM;
                                } else if (str == "LDS") {
                                        pr.searchMethod = LDS;
                                        /* Default parameters */
                                        pr.searchMethodParameter[0] = 1;
                                        pr.searchMethodParameter[1] = 0;
                                        pr.searchMethodParameter[2] = 0;
                                        pr.searchMethodParameter[3] = 0;
                                        if (!(optionsFile >> str))
                                                break;
                                        if (!isdigit(str[0])) {
                                                /* Not a parameter, so continue
                                                 * with the next option. */
                                                alreadyReadAnOption = true;
                                                continue;
                                        }
                                        pr.searchMethodParameter[0] =
                                            atoi(str.c_str());
                                        if (!(optionsFile >> str))
                                                break;
                                        if (!isdigit(str[0])) {
                                                alreadyReadAnOption = true;
                                                continue;
                                        }
                                        pr.searchMethodParameter[1] =
                                            atoi(str.c_str());
                                        if (!(optionsFile >> str))
                                                break;
                                        if (!isdigit(str[0])) {
                                                alreadyReadAnOption = true;
                                                continue;
                                        }
                                        pr.searchMethodParameter[2] =
                                            atoi(str.c_str());
                                        if (!(optionsFile >> str))
                                                break;
                                        if (!isdigit(str[0])) {
                                                alreadyReadAnOption = true;
                                                continue;
                                        }
                                        pr.searchMethodParameter[3] =
                                            atoi(str.c_str());
                                } else if (str == "CREDIT") {
                                        pr.searchMethod = CREDIT;
                                        assert_that(
                                            (optionsFile >>
                                             pr.searchMethodParameter[0])
                                                .good(),
                                            "Missing/Wrong parameter for "
                                            "search method \"CREDIT\"");
                                } else if (str == "DBS") {
                                        pr.searchMethod = DBS;
                                        /* Default parameters */
                                        pr.searchMethodParameter[0] = 0;
                                        if (!(optionsFile >> str))
                                                break;
                                        if (!isdigit(str[0])) {
                                                /* Not a parameter, so continue
                                                 * with the next option. */
                                                alreadyReadAnOption = true;
                                                continue;
                                        }
                                        pr.searchMethodParameter[0] =
                                            atoi(str.c_str());
                                } else if (str == "LAN") {
                                        pr.searchMethod = LAN;
                                        assert_that(
                                            (optionsFile >>
                                             pr.searchMethodParameter[0])
                                                .good(),
                                            "Missing/Wrong parameter for "
                                            "search method \"LAN\"");
                                } else if (str == "DBDS") {
                                        pr.searchMethod = DBDS;
                                } else if (str == "IBROAD") {
                                        pr.searchMethod = IBROAD;
                                } else if (str == "BBS") {
                                        pr.searchMethod = BBS;
                                        assert_that(
                                            (optionsFile >>
                                             pr.searchMethodParameter[0])
                                                .good(),
                                            "Missing/Wrong first parameter for "
                                            "search method \"BBS\"");
                                        /* Default parameters */
                                        pr.searchMethodParameter[1] = 0;
                                        if (!(optionsFile >> str))
                                                break;
                                        if (!isdigit(str[0])) {
                                                /* Not a parameter, so continue
                                                 * with the next option. */
                                                alreadyReadAnOption = true;
                                                continue;
                                        }
                                        pr.searchMethodParameter[1] =
                                            atoi(str.c_str());
                                } else if (str == "RDFS") {
                                        pr.searchMethod = RDFS;
                                        assert_that(
                                            (optionsFile >>
                                             pr.searchMethodParameter[0]) &&
                                                (optionsFile >>
                                                 pr.searchMethodParameter[1]),
                                            "Missing/Wrong parameter(s) for "
                                            "search method \"RDFS\"");
                                } else if (str == "GNS") {
                                        pr.searchMethod = GNS;
                                } else if (str == "FNS") {
                                        pr.searchMethod = FNS;
                                        assert_that((optionsFile >> str).good(),
                                                    "Missing parameter for "
                                                    "search method \"FNS\"");
                                        if (str == "fSample")
                                                pr.searchMethodParameter[0] =
                                                    FSAMPLE;
                                        else if (str == "fSampleMiddle")
                                                pr.searchMethodParameter[0] =
                                                    FSAMPLE_MIDDLE;
                                        else if (str == "fSampleExp")
                                                pr.searchMethodParameter[0] =
                                                    FSAMPLE_EXP;
                                        else
                                                throw timetableException(
                                                    "Missing/Wrong parameter "
                                                    "for search method "
                                                    "\"FNS\"");
                                } else if (str == "ISAMP") {
                                        pr.searchMethod = ISAMP;
                                        assert_that(
                                            (optionsFile >>
                                             pr.searchMethodParameter[0])
                                                .good(),
                                            "Missing/Wrong parameter for "
                                            "search method \"ISAMP\"");
                                } else if (str == "ONESAMP") {
                                        pr.searchMethod = ONESAMP;
                                } else if (str == "DOMAIN_SPLITTING") {
                                        pr.searchMethod = DOMAIN_SPLITTING;
                                        /* Default parameters */
                                        pr.valHeuristicType = NORMAL;
                                        if (!(optionsFile >> str))
                                                break;
                                        if (str != "FAIR") {
                                                /* Not a parameter, so continue
                                                 * with the next option. */
                                                alreadyReadAnOption = true;
                                                continue;
                                        }
                                        pr.valHeuristicType = FAIR;
                                } else if (str == "PoPS") {
                                        pr.searchMethod = PoPS;
                                        /* Default parameters */
                                        pr.searchMethodParameter[0] = -1;
                                        if (!(optionsFile >> str))
                                                break;
                                        if (!isdigit(str[0])) {
                                                /* Not a parameter, so continue
                                                 * with the next option. */
                                                alreadyReadAnOption = true;
                                                continue;
                                        }
                                        pr.searchMethodParameter[0] =
                                            atoi(str.c_str());
                                        // assert_that( ( optionsFile >>
                                        // pr.searchMethodParameter[0] ) ,
                                        //	   "Missing/Wrong parameter for
                                        //search method \"PoPS\"" );  if ( !(
                                        // optionsFile >> str ) ) 	break;
                                        // if ( str != "TRANSFORM" )   {
                                        //	/* Not a parameter, so continue
                                        //with the next option. */
                                        //	alreadyReadAnOption = true;
                                        //	continue;
                                        //}
                                        // pr.valHeuristicTransform  =  true;
                                } else {
                                        throw timetableException(
                                            "Unknown \"searchMethod\" in "
                                            "options-file");
                                }
                        } else if (str == "metaSearchMethod") {
                                assert_that(
                                    (optionsFile >> str).good(),
                                    "Missing value for \"metaSearchMethod\"");
                                if (str == "NONE") {
                                        pr.searchMethod_ls = NONE;
                                } else if (str == "DFS") {
                                        pr.searchMethod_ls = DFS;
                                        /* Default parameters */
                                        pr.varHeuristicType_ls = NORMAL;
                                        if (!(optionsFile >> str))
                                                break;
                                        if (str != "RANDOM_VAR_HEUR" &&
                                            str != "RANDOM_VAL_HEUR") {
                                                /* Not a parameter, so continue
                                                 * with the next option. */
                                                alreadyReadAnOption = true;
                                                continue;
                                        }
                                        if (str == "RANDOM_VAR_HEUR")
                                                pr.varHeuristicType_ls = RANDOM;
                                        else
                                                pr.valHeuristicType_ls = RANDOM;
                                } else if (str == "LDS") {
                                        pr.searchMethod_ls = LDS;
                                        /* Default parameters */
                                        pr.searchMethodParameter_ls[0] = 1;
                                        pr.searchMethodParameter_ls[1] = 0;
                                        pr.searchMethodParameter_ls[2] = 0;
                                        pr.searchMethodParameter_ls[3] = 0;
                                        if (!(optionsFile >> str))
                                                break;
                                        if (!isdigit(str[0])) {
                                                /* Not a parameter, so continue
                                                 * with the next option. */
                                                alreadyReadAnOption = true;
                                                continue;
                                        }
                                        pr.searchMethodParameter_ls[0] =
                                            atoi(str.c_str());
                                        if (!(optionsFile >> str))
                                                break;
                                        if (!isdigit(str[0])) {
                                                alreadyReadAnOption = true;
                                                continue;
                                        }
                                        pr.searchMethodParameter_ls[1] =
                                            atoi(str.c_str());
                                        if (!(optionsFile >> str))
                                                break;
                                        if (!isdigit(str[0])) {
                                                alreadyReadAnOption = true;
                                                continue;
                                        }
                                        pr.searchMethodParameter_ls[2] =
                                            atoi(str.c_str());
                                        if (!(optionsFile >> str))
                                                break;
                                        if (!isdigit(str[0])) {
                                                alreadyReadAnOption = true;
                                                continue;
                                        }
                                        pr.searchMethodParameter_ls[3] =
                                            atoi(str.c_str());
                                } else if (str == "CREDIT") {
                                        pr.searchMethod_ls = CREDIT;
                                        assert_that(
                                            (optionsFile >>
                                             pr.searchMethodParameter_ls[0])
                                                .good(),
                                            "Missing/Wrong parameter for "
                                            "search method \"CREDIT\"");
                                } else if (str == "DBS") {
                                        pr.searchMethod_ls = DBS;
                                        /* Default parameters */
                                        pr.searchMethodParameter_ls[0] = 0;
                                        if (!(optionsFile >> str))
                                                break;
                                        if (!isdigit(str[0])) {
                                                /* Not a parameter, so continue
                                                 * with the next option. */
                                                alreadyReadAnOption = true;
                                                continue;
                                        }
                                        pr.searchMethodParameter_ls[0] =
                                            atoi(str.c_str());
                                } else if (str == "LAN") {
                                        pr.searchMethod_ls = LAN;
                                        assert_that(
                                            (optionsFile >>
                                             pr.searchMethodParameter_ls[0])
                                                .good(),
                                            "Missing/Wrong parameter for "
                                            "search method \"LAN\"");
                                } else if (str == "DBDS") {
                                        pr.searchMethod_ls = DBDS;
                                } else if (str == "IBROAD") {
                                        pr.searchMethod_ls = IBROAD;
                                } else if (str == "BBS") {
                                        pr.searchMethod_ls = BBS;
                                        assert_that(
                                            (optionsFile >>
                                             pr.searchMethodParameter_ls[0])
                                                .good(),
                                            "Missing/Wrong first parameter for "
                                            "search method \"BBS\"");
                                        /* Default parameters */
                                        pr.searchMethodParameter_ls[1] = 0;
                                        if (!(optionsFile >> str))
                                                break;
                                        if (!isdigit(str[0])) {
                                                /* Not a parameter, so continue
                                                 * with the next option. */
                                                alreadyReadAnOption = true;
                                                continue;
                                        }
                                        pr.searchMethodParameter_ls[1] =
                                            atoi(str.c_str());
                                } else if (str == "RDFS") {
                                        pr.searchMethod_ls = RDFS;
                                        assert_that(
                                            (optionsFile >>
                                             pr.searchMethodParameter_ls[0]) &&
                                                (optionsFile >>
                                                 pr.searchMethodParameter_ls
                                                     [1]),
                                            "Missing/Wrong parameter(s) for "
                                            "search method \"RDFS\"");
                                } else if (str == "GNS") {
                                        pr.searchMethod_ls = GNS;
                                } else if (str == "FNS") {
                                        pr.searchMethod_ls = FNS;
                                        assert_that((optionsFile >> str).good(),
                                                    "Missing parameter for "
                                                    "search method \"FNS\"");
                                        if (str == "fSample")
                                                pr.searchMethodParameter_ls[0] =
                                                    FSAMPLE;
                                        else if (str == "fSampleMiddle")
                                                pr.searchMethodParameter_ls[0] =
                                                    FSAMPLE_MIDDLE;
                                        else if (str == "fSampleExp")
                                                pr.searchMethodParameter_ls[0] =
                                                    FSAMPLE_EXP;
                                        else
                                                throw timetableException(
                                                    "Missing/Wrong parameter "
                                                    "for search method "
                                                    "\"FNS\"");
                                } else if (str == "ISAMP") {
                                        pr.searchMethod_ls = ISAMP;
                                        assert_that(
                                            (optionsFile >>
                                             pr.searchMethodParameter_ls[0])
                                                .good(),
                                            "Missing/Wrong parameter for "
                                            "search method \"ISAMP\"");
                                } else if (str == "ONESAMP") {
                                        pr.searchMethod_ls = ONESAMP;
                                } else if (str == "DOMAIN_SPLITTING") {
                                        pr.searchMethod_ls = DOMAIN_SPLITTING;
                                        /* Default parameters */
                                        pr.varHeuristicType_ls = NORMAL;
                                        if (!(optionsFile >> str))
                                                break;
                                        if (str != "FAIR") {
                                                /* Not a parameter, so continue
                                                 * with the next option. */
                                                alreadyReadAnOption = true;
                                                continue;
                                        }
                                        pr.valHeuristicType_ls = FAIR;
                                } else if (str == "PoPS") {
                                        pr.searchMethod_ls = PoPS;
                                        /* Default parameters */
                                        pr.searchMethodParameter_ls[0] = -1;
                                        if (!(optionsFile >> str))
                                                break;
                                        if (!isdigit(str[0])) {
                                                /* Not a parameter, so continue
                                                 * with the next option. */
                                                alreadyReadAnOption = true;
                                                continue;
                                        }
                                        pr.searchMethodParameter_ls[0] =
                                            atoi(str.c_str());
                                        // assert_that( ( optionsFile >>
                                        // pr.searchMethodParameter_ls[0] ) ,
                                        //	   "Missing/Wrong parameter for
                                        //search method \"PoPS\"" );  if ( !(
                                        // optionsFile >> str ) ) 	break;
                                        // if ( str != "TRANSFORM" )   {
                                        //	/* Not a parameter, so continue
                                        //with the next option. */
                                        //	alreadyReadAnOption = true;
                                        //	continue;
                                        //}
                                        // pr.valHeuristicTransform_ls  =  true;
                                } else {
                                        throw timetableException(
                                            "Unknown \"metaSearchMethod\" in "
                                            "options-file");
                                }
                        } else if (str == "timeLimit") {
                                assert_that(
                                    (optionsFile >> pr.timeLimit).good(),
                                    "Missing value for \"timeLimit\"");
                        } else if (str == "timeLimitDirectMethodRound") {
                                assert_that((optionsFile >>
                                             pr.timeLimitDirectMethodRound)
                                                .good(),
                                            "Missing value for "
                                            "\"timeLimitDirectMethodRound\"");
                        } else if (str == "timeLimitIndirectMethodRound") {
                                assert_that((optionsFile >>
                                             pr.timeLimitIndirectMethodRound)
                                                .good(),
                                            "Missing value for "
                                            "\"timeLimitIndirectMethodRound\"");
                        } else if (str == "quiet") {
                                pr.quiet = true;
                                //#   If `timeType' is `CPU', then the
                                //`timeLimit' will be treated as       # #
                                //CPU time. # #
                                //# #      timeType  [REAL|CPU]
                                //# #
                                //# #
                                //# #
                                //# } else if ( str == "timeType" )   {
                                //
                                //		assert_that( optionsFile >> str
                                //,
                                //				"Missing value for
                                //\"timeType\"" );
                                //		if ( str == "REAL" )
                                //			pr.isCpuTime = false;
                                //		else if ( str == "CPU" )
                                //			pr.isCpuTime = true;
                                //		else
                                //			throw
                                //timetableException("Wrong value for
                                //\"timeType\"");
                        } else if (str == "constrainingFactorExponent") {
                                assert_that((optionsFile >>
                                             pr.constrainingFactorExponent)
                                                .good(),
                                            "Missing value for "
                                            "\"constrainingFactorExponent\"");
                        } else if (str == "conf") {
                                assert_that((optionsFile >> pr.conf).good(),
                                            "Missing value for \"conf\"");
                        } else if (str ==
                                   "leastConstrainingValueHeuristicWeight") {
                                assert_that(
                                    (optionsFile >>
                                     pr.leastConstrainingValueHeuristicWeight)
                                        .good(),
                                    "Missing value for "
                                    "\"leastConstrainingValueHeuristicWeight"
                                    "\"");
                        } else if (str ==
                                   "objectiveOrientedValueHeuristicWeight") {
                                assert_that(
                                    (optionsFile >>
                                     pr.objectiveOrientedValueHeuristicWeight)
                                        .good(),
                                    "Missing value for "
                                    "\"objectiveOrientedValueHeuristicWeight"
                                    "\"");
                        } else {
                                throw timetableException(
                                    "Unknown option in options-file");
                        }
                }
                optionsFile.close();
        }
}

void writeSolution(const struct itcProblem_t& pr, NsIntVarArray& vLectPeriod,
                   NsIntVarArray& vLectRoom, const char* solutionFileName)
/* Writing to solution file */
{
        int i, j, k;
        for (k = 0, i = 0; i < pr.ncourses; ++i) {
                pr.course[i].lecturesScheduled = 0;
                for (j = 0; j < pr.course[i].lectures; ++j, ++k) {
                        ++pr.course[i].lecturesScheduled;
                        pr.course[i]
                            .periodScheduled[pr.course[i].lecturesScheduled -
                                             1] = vLectPeriod[k].value();
                        pr.course[i]
                            .roomScheduled[pr.course[i].lecturesScheduled - 1] =
                            vLectRoom[k].value();
                }
        }
        ofstream solutionFile(solutionFileName);
        assert_that(solutionFile.good(),
                    "Could not open solution file for writing");
        for (i = 0; i < pr.ncourses; ++i) {
                for (j = 0; j < pr.course[i].lecturesScheduled; ++j) {
                        solutionFile
                            << pr.course[i].name << " "
                            << pr.room[pr.course[i].roomScheduled[j]].name
                            << " " << pr.course[i].periodScheduled[j] / pr.hours
                            << " " << pr.course[i].periodScheduled[j] % pr.hours
                            << "\n";
                }
        }
        solutionFile.close();
}

void destroyInstance(const struct itcProblem_t& pr)
/* Memory de-allocation */
{
        int i;
        for (i = 0; i < pr.ncourses; ++i) {
                free(pr.course[i].name);
                if (pr.nConstraints > 0)
                        delete[] pr.course[i].unavail;
                delete[] pr.course[i].periodScheduled;
                delete[] pr.course[i].roomScheduled;
        }
        for (i = 0; i < pr.nrooms; ++i)
                free(pr.room[i].name);
        for (i = 0; i < pr.ngroups; ++i) {
                free(pr.group[i].name);
                delete[] pr.group[i].course;
        }
        for (i = 0; i < pr.nprofessors; ++i)
                free(pr.professorName[i]);
        free(pr.name);
        delete[] pr.course;
        delete[] pr.professorName;
        delete[] pr.room;
        if (pr.ngroups > 0)
                delete[] pr.group;
}

void replaceQuote(char* str, ofstream& os)
/* Used for "escaping" quotes of a string written to a file. */
{
        while (*str != '\0') {
                if (*str == '\'')
                        os << '\\';
                os << *str;
                ++str;
        }
}

void createFctResultsFile(const struct itcProblem_t& pr, string resultsFileName)
{
        int i, j, k;
        resultsFileName += ".fct";
        ofstream fctFile(resultsFileName.c_str());
        fctFile << "ITC2007\n";
        fctFile << "DAYS " << pr.days << "\n";
        fctFile << "HOURS " << pr.hours << "\n";
        fctFile << "problemname('";
        replaceQuote(pr.name, fctFile);
        fctFile << "').\n";
        for (i = 1; i <= pr.days; ++i)
                fctFile << "dayname(" << i << ",'Day " << i << "').\n";
        fctFile << "hourname(" << 1 << ",'" << 1 << "st period').\n";
        if (pr.hours >= 2)
                fctFile << "hourname(" << 2 << ",'" << 2 << "nd period').\n";
        if (pr.hours >= 3)
                fctFile << "hourname(" << 3 << ",'" << 3 << "rd period').\n";
        for (i = 4; i <= pr.hours; ++i)
                fctFile << "hourname(" << i << ",'" << i << "th period').\n";
        for (i = 1; i <= pr.nrooms; ++i) {
                fctFile << "classroom(" << i << ",'";
                replaceQuote(pr.room[i - 1].name, fctFile);
                fctFile << "'," << pr.room[i - 1].capacity << ",[]).\n";
        }
        for (i = 1; i <= pr.nprofessors; ++i) {
                fctFile << "teacher(" << i << ",'";
                replaceQuote(pr.professorName[i - 1], fctFile);
                fctFile << "',[],0,0,0).\n";
        }
        for (i = 1; i <= pr.ncourses; ++i) {
                fctFile << "subject(" << i << ",'C" << i << "','";
                replaceQuote(pr.course[i - 1].name, fctFile);
                fctFile << "'," << pr.course[i - 1].students << ","
                        << "1";
                for (j = 1; j < pr.course[i - 1].lecturesScheduled; ++j)
                        fctFile << "+1";
                fctFile << ",[" << pr.course[i - 1].professor + 1 << "],3,[";
                for (j = 0; j < pr.course[i - 1].lecturesScheduled; ++j) {
                        fctFile << pr.course[i - 1].periodScheduled[j] << " "
                                << pr.course[i - 1].roomScheduled[j] + 1;
                        if (j < pr.course[i - 1].lecturesScheduled - 1)
                                fctFile << " ";
                }
                fctFile << "],[";
                for (j = 0; j < pr.course[i - 1].lecturesScheduled; ++j) {
                        /* Searching if `roomScheduled[j]' has been already
                         * recorded */
                        for (k = 0; k < j; ++k) {
                                if (pr.course[i - 1].roomScheduled[k] ==
                                    pr.course[i - 1].roomScheduled[j])
                                        break;
                        }
                        if (k == j) {
                                /* `roomScheduled[j]' has not been recorded */
                                if (j > 0)
                                        fctFile << " ";
                                fctFile
                                    << pr.course[i - 1].roomScheduled[j] + 1;
                        }
                }
                fctFile << "],'C" << i << "',[";
                for (j = 0; j < pr.course[i - 1].nunavail; ++j) {
                        fctFile << "na("
                                << (pr.course[i - 1].unavail[j]) / pr.hours + 1
                                << ","
                                << (pr.course[i - 1].unavail[j]) % pr.hours + 1
                                << ")";
                        if (j < pr.course[i - 1].nunavail - 1)
                                fctFile << ",";
                }
                fctFile << "]).\n";
        }
        for (i = 1; i <= pr.ngroups; ++i) {
                fctFile << "cgroup(" << i << ",'";
                replaceQuote(pr.group[i - 1].name, fctFile);
                fctFile << "',[" << pr.group[i - 1].course[0] + 1;
                for (j = 1; j < pr.group[i - 1].ncourses; ++j)
                        fctFile << "," << pr.group[i - 1].course[j] + 1;
                fctFile << "],1).\n";
        }
        for (i = 1;
             i <= pr.ngroups && static_cast<unsigned>(i) <=
                                    sizeof(colorCode) / sizeof(colorCode[0]);
             ++i) {
                fctFile << "colorgroup(" << i << ",'";
                replaceQuote(pr.group[i - 1].name, fctFile);
                fctFile << "',[" << pr.group[i - 1].course[0] + 1;
                for (j = 1; j < pr.group[i - 1].ncourses; ++j)
                        fctFile << "," << pr.group[i - 1].course[j] + 1;
                fctFile << "],'" << colorCode[i - 1] << "').\n";
        }
        fctFile.close();
        // cerr << "Output written to " << resultsFileName << ".\n";
        // cerr << "Remove its first line for more compatibility with fct
        // format.\n";
}
