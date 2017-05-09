/// @file
/// Implementation of the interface between XCSP3 and Naxos
///
/// Part of https://github.com/pothitos/naxos

#include "translator.h"
#include <algorithm>

using namespace XCSP3Core;
using namespace naxos;
using namespace std;

template <class T>
void displayList(vector<T>& list, string separator = " ")
{
        typename vector<T>::size_type i;
        if (list.size() > 8) {
                for (i = 0; i < 3; i++)
                        cout << list[i] << separator;
                cout << "..." << separator;
                for (i = list.size() - 4; i < list.size(); i++)
                        cout << list[i] << separator;
                cout << "\n";
                return;
        }
        for (i = 0; i < list.size(); i++)
                cout << list[i] << separator;
        cout << "\n";
}

void displayList(vector<XVariable*>& list, string separator = " ")
{
        vector<XVariable*>::size_type i;
        if (list.size() > 8) {
                for (i = 0; i < 3; i++)
                        cout << list[i]->id << separator;
                cout << "..." << separator;
                for (i = list.size() - 4; i < list.size(); i++)
                        cout << list[i]->id << separator;
                cout << "\n";
                return;
        }
        for (i = 0; i < list.size(); i++)
                cout << list[i]->id << separator;
        cout << "\n";
}

void Xcsp3_to_Naxos::beginInstance(InstanceType type)
{
        if (verbose)
                cout << "start instance of type " << type << "\n";
        if (instanceAlreadyBegan)
                throw invalid_argument("Only one CSP definition can be read");
}

void Xcsp3_to_Naxos::endInstance()
{
        if (verbose)
                cout << "end SAX parsing\n";
        if (AllVars.empty())
                throw invalid_argument("No constrained variables defined");
        pm.addGoal(new NsgLabeling(AllVars));
}

void Xcsp3_to_Naxos::beginVariables()
{
        if (verbose)
                cout << "  start variables\n";
}

void Xcsp3_to_Naxos::endVariables()
{
        if (verbose)
                cout << "  end variables\n";
}

void Xcsp3_to_Naxos::beginVariableArray(string id)
{
        if (verbose)
                cout << "    array " << id << "\n";
}

void Xcsp3_to_Naxos::endVariableArray()
{
}

void Xcsp3_to_Naxos::beginConstraints()
{
        if (verbose)
                cout << "  start constraints\n";
}

void Xcsp3_to_Naxos::endConstraints()
{
        if (verbose)
                cout << "  end constraints\n";
}

void Xcsp3_to_Naxos::beginGroup(string id)
{
        if (verbose)
                cout << "    start group " << id << "\n";
}

void Xcsp3_to_Naxos::endGroup()
{
        if (verbose)
                cout << "    end group\n";
}

void Xcsp3_to_Naxos::beginBlock(string classes)
{
        if (verbose)
                cout << "    start block of class " << classes << "\n";
}

void Xcsp3_to_Naxos::endBlock()
{
        if (verbose)
                cout << "    end block\n";
}

void Xcsp3_to_Naxos::beginObjectives()
{
        if (verbose)
                cout << "  start objective\n";
}

void Xcsp3_to_Naxos::endObjectives()
{
        if (verbose)
                cout << "  end objective\n";
}

void Xcsp3_to_Naxos::buildVariableInteger(string id, int minValue, int maxValue)
{
        if (verbose) {
                cout << "    var " << id << ": " << minValue << "..."
                     << maxValue << "\n";
        }
        variable[id] = NsIntVar(pm, minValue, maxValue);
        recordVar(id, variable[id]);
}

void Xcsp3_to_Naxos::buildVariableInteger(string id, vector<int>& values)
{
        if (verbose) {
                cout << "    var " << id << ": ";
                displayList(values);
        }
        if (values.empty()) {
                throw domain_error("The domain of a constrained variable "
                                   "cannot be empty");
        }
        // Ensure that the values are ordered
        sort(values.begin(), values.end());
        // Set variable's domain to be the whole values[0]..values[N-1]
        variable[id] = NsIntVar(pm, values.front(), values.back());
        // Remove gaps from the variable's domain
        for (vector<int>::size_type i = 0; i < values.size() - 1; ++i)
                for (int val = values[i] + 1; val < values[i + 1]; ++val)
                        variable[id].remove(val);
        recordVar(id, variable[id]);
}

/// Intension constraint
void Xcsp3_to_Naxos::buildConstraintIntension(string id, string expr)
{
        if (verbose)
                cout << "    intension " << id << ": " << expr << "\n";
}

/// Primitive constraint x +- k op y
void Xcsp3_to_Naxos::buildConstraintPrimitive(string id, OrderType op,
                                              XVariable* x, int k, XVariable* y)
{
        if (verbose) {
                cout << "    intension " << id << ": " << x->id
                     << (k >= 0 ? "+" : "") << k << " op " << y->id << "\n";
        }
        if (op == IN) {
                throw invalid_argument("Membership operator 'in' is not "
                                       "accepted");
        }
}

/// Extension constraint
void Xcsp3_to_Naxos::buildConstraintExtension(string id,
                                              vector<XVariable*> list,
                                              vector<vector<int>>& tuples,
                                              bool support, bool hasStar)
{
        if (verbose) {
                cout << "    extension " << (support ? "support" : "conflict")
                     << " " << id << "\n"
                     << "      "
                     << "arity: " << list.size() << ", "
                     << "tuples: " << tuples.size() << ", "
                     << "star: " << hasStar << "\n"
                     << "      ";
                displayList(list);
        }
        if (hasStar) {
                throw invalid_argument("Short tables, i.e. tables with tuples "
                                       "containing '*', are not accepted");
        }
}

/// Unary extension constraint
void Xcsp3_to_Naxos::buildConstraintExtension(string id, XVariable* var,
                                              vector<int>& tuples, bool support,
                                              bool hasStar)
{
        if (verbose) {
                cout << "    extension " << (support ? "support" : "conflict")
                     << " with one variable " << id << "\n"
                     << "      "
                     << "tuples: " << tuples.size() << ", "
                     << "star: " << hasStar << "\n"
                     << "      " << *var << "\n";
        }
        if (hasStar) {
                throw invalid_argument("Short tables, i.e. tables with tuples "
                                       "containing '*', are not accepted");
        }
}

/// Extension constraint with exactly the same tuples as the previous one
void Xcsp3_to_Naxos::buildConstraintExtensionAs(string id,
                                                vector<XVariable*> list,
                                                bool support, bool hasStar)
{
        if (verbose)
                cout << "    extension same as above " << id << "\n";
        if (hasStar) {
                throw invalid_argument("Short tables, i.e. tables with tuples "
                                       "containing '*', are not accepted");
        }
}

void Xcsp3_to_Naxos::buildConstraintAlldifferent(string id,
                                                 vector<XVariable*>& list)
{
        if (verbose) {
                cout << "    allDifferent " << id << ": ";
                displayList(list);
        }
        collectArray(list);
        pm.add(NsAllDiff(arrays.back()));
}

/// Enforces sum constraint for every possible condition
template <class T>
void Xcsp3_to_Naxos::unfoldSumConstraintCondition(NsIntVarArray& array,
                                                  const OrderType condition,
                                                  T& operand)
{
        switch (condition) {
        case EQ:
                pm.add(NsSum(array) == operand);
                break;
        case NE:
                pm.add(NsSum(array) != operand);
                break;
        case LT:
                pm.add(NsSum(array) < operand);
                break;
        case LE:
                pm.add(NsSum(array) <= operand);
                break;
        case GT:
                pm.add(NsSum(array) > operand);
                break;
        case GE:
                pm.add(NsSum(array) >= operand);
                break;
        default:
                throw invalid_argument("Unsupported sum condition");
                break;
        }
}

/// Enforces sum constraint for every possible operand
void Xcsp3_to_Naxos::unfoldSumConstraintOperand(NsIntVarArray& array,
                                                const XCondition& cond)
{
        switch (cond.operandType) {
        case INTEGER:
                unfoldSumConstraintCondition(array, cond.op, cond.val);
                break;
        case VARIABLE:
                unfoldSumConstraintCondition(array, cond.op,
                                             variable[cond.var]);
                break;
        default:
                throw invalid_argument("Unsupported operand for sum");
                break;
        }
}

/// Unweighted sum constraint
void Xcsp3_to_Naxos::buildConstraintSum(string id, vector<XVariable*>& list,
                                        XCondition& cond)
{
        if (verbose) {
                cout << "    sum " << id << ": ";
                displayList(list, "+");
                cout << cond << "\n";
        }
        collectArray(list);
        unfoldSumConstraintOperand(arrays.back(), cond);
}

/// Weighted sum constraint
void Xcsp3_to_Naxos::buildConstraintSum(string id, vector<XVariable*>& list,
                                        vector<int>& coeffs, XCondition& cond)
{
        if (verbose) {
                cout << "    sum " << id << ": ";
                if (list.size() > 8) {
                        for (int i = 0; i < 3; i++)
                                cout << (coeffs.size() == 0 ? 1 : coeffs[i])
                                     << "*" << *list[i] << " ";
                        cout << "... ";
                        for (unsigned int i = list.size() - 4; i < list.size();
                             i++)
                                cout << (coeffs.size() == 0 ? 1 : coeffs[i])
                                     << "*" << *list[i] << " ";
                } else {
                        for (unsigned int i = 0; i < list.size(); i++)
                                cout << (coeffs.size() == 0 ? 1 : coeffs[i])
                                     << "*" << *list[i] << " ";
                }
                cout << cond << "\n";
        }
}

/// Sum constraint with variables as weights
void Xcsp3_to_Naxos::buildConstraintSum(string id, vector<XVariable*>& list,
                                        vector<XVariable*>& coeffs,
                                        XCondition& cond)
{
        if (verbose) {
                cout << "    scalar sum " << id << ": ";
                if (list.size() > 8) {
                        for (int i = 0; i < 3; i++)
                                cout << coeffs[i]->id << "*" << *list[i] << " ";
                        cout << "... ";
                        for (unsigned int i = list.size() - 4; i < list.size();
                             i++)
                                cout << coeffs[i]->id << "*" << *list[i] << " ";
                } else {
                        for (unsigned int i = 0; i < list.size(); i++)
                                cout << coeffs[i]->id << "*" << *list[i] << " ";
                }
                cout << cond << "\n";
        }
}

/// Value is in position index inside the list
void Xcsp3_to_Naxos::buildConstraintElement(string id, vector<XVariable*>& list,
                                            int startIndex, XVariable* index,
                                            RankType rank, int value)
{
        if (verbose) {
                cout << "    element constant (with index) " << id << ": ";
                displayList(list);
                cout << "      "
                     << "value: " << value << ", "
                     << "start index: " << startIndex << ", "
                     << "index: " << *index << "\n";
        }
        if (startIndex != 0) {
                throw invalid_argument("startIndex argument of element "
                                       "constraint should be 0");
        }
        if (rank != ANY) {
                throw invalid_argument("rank argument of element "
                                       "constraint should be any");
        }
}

/// Variable is in position index inside the list
void Xcsp3_to_Naxos::buildConstraintElement(string id, vector<XVariable*>& list,
                                            int startIndex, XVariable* index,
                                            RankType rank, XVariable* value)
{
        if (verbose) {
                cout << "    element variable (with index) " << id << ": ";
                displayList(list);
                cout << "      "
                     << "value: " << *value << ", "
                     << "start index: " << startIndex << ", "
                     << "index: " << *index << "\n";
        }
        if (startIndex != 0) {
                throw invalid_argument("startIndex argument of element "
                                       "constraint should be 0");
        }
        if (rank != ANY) {
                throw invalid_argument("rank argument of element "
                                       "constraint should be any");
        }
}

void Xcsp3_to_Naxos::buildObjectiveMinimizeVariable(XVariable* x)
{
        if (verbose)
                cout << "    minimize var " << x << "\n";
        constraintOptimisationMode = true;
        objectiveSign = +1;
        pm.minimize(objectiveSign * variable[x->id]);
}

void Xcsp3_to_Naxos::buildObjectiveMaximizeVariable(XVariable* x)
{
        if (verbose)
                cout << "    maximize var " << x << "\n";
        constraintOptimisationMode = true;
        objectiveSign = -1;
        pm.minimize(objectiveSign * variable[x->id]);
}

void Xcsp3_to_Naxos::buildObjectiveMinimize(ExpressionObjective type,
                                            vector<XVariable*>& list)
{
        constraintOptimisationMode = true;
        objectiveSign = +1;
}

void Xcsp3_to_Naxos::buildObjectiveMaximize(ExpressionObjective type,
                                            vector<XVariable*>& list)
{
        constraintOptimisationMode = true;
        objectiveSign = -1;
}

void Xcsp3_to_Naxos::buildObjectiveMinimize(ExpressionObjective type,
                                            vector<XVariable*>& list,
                                            vector<int>& coefs)
{
        constraintOptimisationMode = true;
        objectiveSign = +1;
}

void Xcsp3_to_Naxos::buildObjectiveMaximize(ExpressionObjective type,
                                            vector<XVariable*>& list,
                                            vector<int>& coefs)
{
        constraintOptimisationMode = true;
        objectiveSign = -1;
}
