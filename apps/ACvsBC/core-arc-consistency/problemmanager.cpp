// Part of https://github.com/pothitos/naxos

#include "naxos.h"

using namespace naxos;
using namespace std;

Ns_StackSearch::goal_iterator::goal_iterator(Ns_StackSearch& StackOfStacks_init)
  : StackOfStacks(&StackOfStacks_init), curr_Stack_it(StackOfStacks->begin())
{
        if (curr_Stack_it != StackOfStacks->end() &&
            (curr_node_it = curr_Stack_it->StackAND.begin()) ==
                curr_Stack_it->StackAND.end())
                *this = curr_Stack_it->delayed_goal;
}

Ns_StackSearch::goal_iterator& Ns_StackSearch::goal_iterator::operator++(void)
{
        assert_exc(StackOfStacks != 0,
                   "Ns_StackSearch::goal_iterator::++: Unitialized `*this'");
        assert_exc(curr_Stack_it != StackOfStacks->end() &&
                       curr_node_it != curr_Stack_it->StackAND.end(),
                   "Ns_StackSearch::goal_iterator::end: Bad request "
                   "`++(something.end())'");
        if (++curr_node_it == curr_Stack_it->StackAND.end())
                *this = curr_Stack_it->delayed_goal;
        return *this;
}

///  Writes to a file a view of the search tree in a Graphviz supported format.

///  \todo  Check if we can create classes of nodes in a dot file.

void Ns_StackSearch::searchToGraphFile(const char* fileName)
{
        fileSearchGraph.open(fileName);
        assert_exc(fileSearchGraph,
                   "Ns_StackSearch::searchToGraphFile: Could not open file");
        fileSearchGraph << "digraph  \"Search Tree\"  {\n\n"
                        << "\tnode [shape=point];\n\n"
                        << "\tedge [arrowhead=none];\n";
}

///  If it does not exist, creates a \c validHistoryId entry for the current
///  search node.

void Ns_StackSearch::push(const value_type& newNode)
{
        ++nSearchTreeNodes;
        if (!empty())
                top().has_a_child = true;
        NsStack<SearchNode>::push(newNode);
        if (validHistoryId.size() < size())
                validHistoryId.push_back(0);
}

///  Records the solution node to the goals graph file (if created).

void Ns_StackSearch::solutionNode(const NsIntVar* vObjective)
{
        if (fileSearchGraph) {
                fileSearchGraph
                    << "\n\t\"(" << size() - 1 << ","
                    << validHistoryId[size() - 1] << ")"
                    << ((top().has_a_child) ? "LastChild" : "")
                    << "\" [shape=doublecircle, height=0.1, label=\"\"];\n";
                //  If the node has children, and it is a solution, then it
                //   it is the last child of itself.  (Besides, after the
                //   success it will be popped by the backtracking
                //   algorithm.)
                if (vObjective != 0) {
                        recordObjective = true;
                        objectiveValue = vObjective->max();
                }
        }
}

///  Invalidates the \c validHistoryId for the current search node.

void Ns_StackSearch::pop(void)
{
        if (fileSearchGraph && size() - 1 > 0 && top().has_a_child) {
                fileSearchGraph << "\n\t\"(" << size() - 1 << ","
                                << validHistoryId[size() - 1]
                                << ")\"   ->   \"(" << size() - 1 << ","
                                << validHistoryId[size() - 1] << ")LastChild\"";
                if (recordObjective) {
                        recordObjective = false;
                        fileSearchGraph << "   [fontsize=9, headlabel=\""
                                        << objectiveValue << "\"]";
                }
                fileSearchGraph << ";\n";
                //  If the node has children, then it it is the last child
                //   of itself.  (Besides, the `goalNextChoice'--the second
                //   subgoal of an OR-goal--is executed one level before
                //   the execution of the first subgoal, in the search
                //   tree.)
        }
        if (fileSearchGraph && size() - 1 > 1) {
                fileSearchGraph << "\n\t\"(" << size() - 2 << ","
                                << validHistoryId[size() - 2]
                                << ")\"   ->   \"(" << size() - 1 << ","
                                << validHistoryId[size() - 1] << ")\"";
                if (recordObjective) {
                        recordObjective = false;
                        fileSearchGraph << "   [fontsize=9, headlabel=\""
                                        << objectiveValue << "\"]";
                }
                fileSearchGraph << ";\n";
        }
        ++validHistoryId[size() - 1];
        NsStack<SearchNode>::pop();
}

// Remark. In implementing the queue, to reduce the number of queue
// operations, one way is simply enqueue the variable whose domain has
// shrunk, instead of enqueue all relevant arcs. When we dequeue a variable
// from the queue, we just revise all constraints pointing to this
// variable. The method is also called variable oriented propagation. This
// idea appeared in [McG79] and in [CJ98].

// Strictly speaking, in AC-3, arc (i, j) is not enqueued when arc (j, i) is
// made consistent. This optimization could be added in AC-5 by adding j as an
// argument to Enqueue and adding the constraint k != j to its definition.
NsConstraint* QueueItem::getNextConstraint(void)
{
        /*
         * NsIntVar  V1(pm,0,0), V2(pm,0,1);
         * for (int i=0;  i<640;  ++i)
         * 	pm.add( V1 < V2 );
         * pm.nextSolution();
         *
         * 	The following, commented-out code did not work
         * 	with the above code (that a solver's programmer
         * 	could develop).  We think that the culprit is
         * 	is an STL bug...
         *
         * NsConstraint  *temp;
         * for ( ;  curr_constr != var_fired->CONSTRend();  ++curr_constr)   {
         * 	temp = *curr_constr;
         * 		if (temp != constr_fired)   {
         * 			++curr_constr;
         * 			return  temp;
         * 		}
         * }
         */
        for (; curr_constr < var_fired->CONSTR.size(); ++curr_constr) {
                if (var_fired->CONSTR[curr_constr] != constr_fired)
                        return var_fired->CONSTR[curr_constr++];
        }
        return 0; // end
}

namespace {
void destroy_goal(NsGoal* g)
{
        if (g != 0) {
                if (g->isGoalAND() || g->isGoalOR()) {
                        destroy_goal(g->getFirstSubGoal());
                        destroy_goal(g->getSecondSubGoal());
                }
                delete g;
        }
}
} // end namespace

//	void
// Ns_StackSearch::pop (void)
//{
//	destroy_goal( top().GoalNextChoice );
//	Stack<SearchNode>::pop();
//}

Ns_StackSearch::Ns_StackSearch(void)
  : nSearchTreeNodes(0), recordObjective(false)
{
}

void Ns_StackSearch::clear(void)
{
        while (!empty()) {
                destroy_goal(top().GoalNextChoice);
                pop();
        }
}

Ns_StackSearch::~Ns_StackSearch(void)
{
        clear();
        if (fileSearchGraph) {
                fileSearchGraph << "}\n";
                fileSearchGraph.close();
        }
}

StackGoals::~StackGoals(void)
{
        while (!empty()) {
                destroy_goal(top());
                pop();
        }
}

NsProblemManager::~NsProblemManager(void)
{
        for (CONSTR_arr_t::iterator c = CONSTR.begin(); c != CONSTR.end(); ++c)
                delete *c;
        for (NsDeque<NsIntVar*>::iterator v = IntermediateVar.begin();
             v != IntermediateVar.end(); ++v)
                delete *v;
}

void NsProblemManager::add(const ExprConstr& expr)
{
        assert_exc(FirstNextSolution, "NsProblemManager::add: Cannot add a "
                                      "constraint because search has already "
                                      "started");
        NsConstraint* NewConstr = expr.postConstraint();
        if (NewConstr == 0)
                return; // unary constraint
        NewConstr->ArcCons();
        RecordConstraint(NewConstr);
}

// void
// NsProblemManager::add (ExprConstr *expr)
//{
//	NsConstraint  *NewConstr = expr->post();
//	NewConstr->ArcCons();
//	RecordConstraint( NewConstr );
//	delete  expr;
//}

// void
// NsProblemManager::add_XlessthanY (NsIntVar& X, NsIntVar& Y)
//{
//	//assert_exc( !initialized ,  "NsProblemManager::add_XlessthanY: Search
//has been initialized");
//
//	NsConstraint  *NewConstr = new ConstrXlessthanY(&X, &Y);
//	X.addConstraint(NewConstr);
//	Y.addConstraint(NewConstr);
//	NewConstr->ArcCons();
//	RecordConstraint( NewConstr );
//}
//
//
//
// void
// NsProblemManager::add_XeqYplusC (NsIntVar& X, NsIntVar& Y, const NsInt C)
//{
//	//assert_exc( !initialized ,  "NsProblemManager::add_XeqYplusC: Search
//has been initialized");
//
//	NsConstraint  *NewConstr = new ConstrXeqYplusC(&X, &Y, C);
//	X.addConstraint(NewConstr);
//	Y.addConstraint(NewConstr);
//	NewConstr->ArcCons();
//	RecordConstraint( NewConstr );
//}
//
//
//
// void
// NsProblemManager::add_XeqYplusCZspecial (NsIntVar& X, NsIntVar& Y, const
// NsInt C, NsIntVar& Z)
//{
//	//assert_exc( !initialized ,  "NsProblemManager::add_XeqYplusCZspecial:
//Search has been initialized");
//
//	NsConstraint  *NewConstr = new ConstrXeqYplusCZspecial(&X, &Y, C, &Z);
//	X.addConstraint(NewConstr);
//	Y.addConstraint(NewConstr);
//	Z.addConstraint(NewConstr);
//	NewConstr->ArcCons();
//	RecordConstraint( NewConstr );
//}
//
//
//
// void
// NsProblemManager::add_XeqYdivC (NsIntVar& X, NsIntVar& Y, const NsInt C)
//{
//	//assert_exc( !initialized ,  "NsProblemManager::add_XeqYdivC: Search
//has been initialized");
//
//	NsConstraint  *NewConstr = new ConstrXeqYdivC(&X, &Y, C);
//	X.addConstraint(NewConstr);
//	Y.addConstraint(NewConstr);
//	NewConstr->ArcCons();
//	RecordConstraint( NewConstr );
//}
//
//
//
// void
// NsProblemManager::add_XneqY (NsIntVar& X, NsIntVar& Y)
//{
//	//assert_exc( !initialized ,  "NsProblemManager::add_XneqY: Search has
//been initialized");
//
//	NsConstraint  *NewConstr = new ConstrXneqY(&X, &Y);
//	X.addConstraint(NewConstr);
//	Y.addConstraint(NewConstr);
//	NewConstr->ArcCons();
//	RecordConstraint( NewConstr );
//}

// bool
// NsProblemManager::Initialize (void)
//{
//	assert( VarGoal != 0 );
//	assert_exc( !initialized ,  "NsProblemManager::Initialize: Search has
//been already initialized"); 	initialized      = true; 	just_initialized
//= true;
//
//
//	//for (CONSTR_arr_t::iterator c = CONSTR.begin();  c != CONSTR.end();
//++c)   {
//	//	if ( !(*c)->ArcCons() )
//	//		return  false;
//	//}
//
//	if ( !ArcConsinstent() )   {
//		finished = true;
//		return  false;
//	}
//	//searchNodes.pop();                  // ... because there would be no
//backtracking
//	//  `SearchNode(0,false)' means non-backtrackable SearchNode.
//
//	if (VarMinObj != 0)
//		BestMinObj = VarMinObj->max() + 1;
//
//	searchNodes.push( SearchNode(0,false) );  // "obsolete" push of a node
//...
//	//  It will contain the first variable of VarGoal (obsolete domain)
//	//   See NsProblemManager::nextSolution()
//
//	return  true;
//}

bool NsProblemManager::ArcConsinstent(void)
{
        // assert_exc(initialized, "NsProblemManager::ArcConsinstent: Search has
        // not been initialized");
        if (FoundInconsistency) {
                FoundInconsistency = false;
                Q.clear();
                ++nFailures;
                return false;
        }
        NsConstraint* c;
        while (!Q.empty()) {
                assert_exc(!Q.top().getVarFired()->contains(Q.top().getW()),
                           "NsProblemManager::ArcConsinstent: A variable "
                           "contains a removed value");
                while ((c = Q.top().getNextConstraint()) != 0) {
                        // start while
                        c->LocalArcCons(Q.top());
                        ++nConstraintChecks;
                        if (FoundInconsistency) {
                                FoundInconsistency = false;
                                Q.clear();
                                ++nFailures;
                                return false;
                        }
                        // here it will break
                }
                Q.pop();
        }
        return true;
}

bool NsProblemManager::BackTrack(void)
{
        NsGoal* GoalNextChoice;
        for (;;) {
                if (backTrackLim != 0 && nBackTracks >= backTrackLim)
                        return false;
                ++nBackTracks;
                assert_exc(
                    !searchNodes.empty(),
                    "NsProblemManager::BackTrack: `searchNodes' is empty");
                GoalNextChoice = searchNodes.top().GoalNextChoice;
                if (GoalNextChoice == 0)
                        return false;
                StackDomains& DomsOriginal =
                    searchNodes.top().DomsStore.DomsOrig;
                while (!DomsOriginal.empty()) {
                        DomsOriginal.top().Var->setDomain(
                            DomsOriginal.top().Dom);
                        DomsOriginal.pop();
                }
                searchNodes.pop();
                searchNodes.top().StackAND.push(GoalNextChoice);
                if (VarMinObj != 0) {
                        // while ( VarMinObj->max() >= BestMinObj )   {
                        //	if ( !VarMinObj->removeSingle( VarMinObj->max(),
                        //0 ) ) 		break;	// ...
                        //FoundInconsistency here
                        //}
                        VarMinObj->removeRange(BestMinObj, NsPLUS_INF, 0);
                        if (FoundInconsistency) {
                                FoundInconsistency = false;
                                Q.clear();
                                continue;
                        }
                }
                return true;
        }
}

void NsProblemManager::restart(void)
{
        FirstNextSolution = true;
        // For any case, we clear the propagation engine's members.
        FoundInconsistency = false;
        Q.clear();
        bool FoundSecondFrame = false;
        NsGoal* GoalNextChoice;
        assert_exc(!searchNodes.empty(),
                   "NsProblemManager::restart: `searchNodes' is empty");
        do {
                GoalNextChoice = searchNodes.top().GoalNextChoice;
                if (GoalNextChoice == 0)
                        FoundSecondFrame = true;
                StackDomains& DomsOriginal =
                    searchNodes.top().DomsStore.DomsOrig;
                while (!DomsOriginal.empty()) {
                        DomsOriginal.top().Var->setDomain(
                            DomsOriginal.top().Dom);
                        DomsOriginal.pop();
                }
                searchNodes.pop();
                searchNodes.top().StackAND.push(GoalNextChoice);
                // We keeped the above line because of Memory Management reasons
                //  (in order to delete the `GoalNextChoice').
                assert_exc(!searchNodes.empty(),
                           "`restart()' call, before `nextSolution()'");
        } while (!FoundSecondFrame);
        searchNodes.pop();
        searchNodes.reset();
        searchNodes.push(SearchNode(0, searchNodes.gbegin()));
        if (VarMinObj != 0) {
                // while ( VarMinObj->max() >= BestMinObj )   {
                //	if ( !VarMinObj->removeSingle( VarMinObj->max(), 0 ) )
                //		break;	// ... FoundInconsistency here
                //}
                VarMinObj->removeRange(BestMinObj, NsPLUS_INF, 0);
        }
}

bool NsProblemManager::nextSolution(void)
{
        bool IsArcCons = true;
        if (FirstNextSolution) {
                FirstNextSolution = false;
                IsArcCons = ArcConsinstent();
                // Throwing away unnesessary `DomsStore' in the first frame.
                searchNodes.top().DomsStore.clear();
                //  (A) Cutting from the StackAND of the base frame...
                StackGoals tempStackAND;
                while (!searchNodes.top().StackAND.empty()) {
                        tempStackAND.push(searchNodes.top().StackAND.top());
                        searchNodes.top().StackAND.pop();
                }
                // A push of frame, for the purposes of
                // NsProblemManager::restart().
                //  We took care placing it *after* the ArcConsinstent() call
                //  (because in future, we will not be able to revert to the
                //  current `Q').
                searchNodes.push(SearchNode(0, searchNodes.gbegin()));
                //  (B) ...and pasting to the StackAND of the new frame.
                while (!tempStackAND.empty()) {
                        searchNodes.top().StackAND.push(tempStackAND.top());
                        tempStackAND.pop();
                }
                if (searchNodes.top().StackAND.empty() &&
                    searchNodes.top().delayed_goal == searchNodes.gend())
                        return IsArcCons;
        }
        if (CalledTimeLimit && timeLim != 0) {
                CalledTimeLimit = false;
                if (isRealTime) {
                        startRealTime = time(0);
                        assert_exc(startRealTime != -1,
                                   "Could not find time for `realTimeLimit'");
                } else {
                        startTime = clock();
                        assert_exc(startTime != -1,
                                   "Could not find time for `timeLimit'");
                }
        }
        if ((!IsArcCons || !ArcConsinstent()) ||
            (searchNodes.top().StackAND.empty() &&
             searchNodes.top().delayed_goal == searchNodes.gend())) {
                if (!BackTrack())
                        return false;
        }
        NsGoal *CurrGoal, *NewGoal;
        bool poppedAgoal;
        while (timeLim == 0 ||
               (isRealTime && difftime(time(0), startRealTime) <= timeLim) ||
               (!isRealTime &&
                static_cast<unsigned long>(clock() - startTime) <=
                    timeLim * static_cast<unsigned long>(CLOCKS_PER_SEC))) {
                poppedAgoal = false;
                if (!searchNodes.top().StackAND.empty()) {
                        CurrGoal = searchNodes.top().StackAND.top();
                        searchNodes.top().StackAND.pop();
                        poppedAgoal = true;
                } else {
                        assert_exc(searchNodes.top().delayed_goal !=
                                       searchNodes.gend(),
                                   "NsProblemManager::nextSolution: No goal to "
                                   "execute");
                        CurrGoal = *searchNodes.top().delayed_goal;
                        ++searchNodes.top().delayed_goal;
                }
                assert_exc(
                    CurrGoal != 0,
                    "NsProblemManager::nextSolution: Zero goal to execute");
                if (CurrGoal->isGoalAND()) {
                        searchNodes.top().StackAND.push(
                            CurrGoal->getSecondSubGoal());
                        searchNodes.top().StackAND.push(
                            CurrGoal->getFirstSubGoal());
                        if (poppedAgoal)
                                delete CurrGoal;
                        // cout << "--- AND ---\n";
                } else if (CurrGoal->isGoalOR()) {
                        searchNodes.push(
                            SearchNode(CurrGoal->getSecondSubGoal(),
                                       searchNodes.gbegin()));
                        searchNodes.top().StackAND.push(
                            CurrGoal->getFirstSubGoal());
                        if (poppedAgoal)
                                delete CurrGoal;
                        // cout << "--- OR  ---\n";
                } else {
                        ++nGoals;
                        NewGoal = CurrGoal->GOAL();
                        if (poppedAgoal)
                                delete CurrGoal;
                        // for (unsigned i=0;  i<VarGoal->size();  ++i)
                        //	cout << " " << (*VarGoal)[i];
                        // cout << "\n";
                        if (!ArcConsinstent()) {
                                // cout << "<BACKTRACK>\n";
                                destroy_goal(NewGoal);
                                if (!BackTrack())
                                        return false;
                        } else if (NewGoal != 0) {
                                searchNodes.top().StackAND.push(NewGoal);
                        } else if (searchNodes.top().StackAND.empty() &&
                                   searchNodes.top().delayed_goal ==
                                       searchNodes.gend()) {
                                if (VarMinObj != 0) {
                                        assert_exc(
                                            BestMinObj > VarMinObj->max(),
                                            "NsProblemManager::nextSolution: "
                                            "Wrong objective value");
                                        BestMinObj = VarMinObj->max();
                                        //  We have taken care about the rare
                                        //  (and odd) case
                                        //   where the domain of VarMinObj has
                                        //   been augmented.
                                }
                                searchNodes.solutionNode(VarMinObj);
                                return true;
                        }
                }
        }
        return false;
}
