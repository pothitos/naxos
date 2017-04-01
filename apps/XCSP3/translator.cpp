/// @file
/// Implementation of the interface between XCSP3 and Naxos
///
/// Part of https://github.com/pothitos/naxos

#include "translator.h"

using namespace XCSP3Core;

template <class T>
void displayList(vector<T>& list, string separator = " ")
{
        if (list.size() > 8) {
                for (int i = 0; i < 3; i++)
                        cout << list[i] << separator;
                cout << " ... ";
                for (int i = list.size() - 4; i < list.size(); i++)
                        cout << list[i] << separator;
                cout << endl;
                return;
        }
        for (int i = 0; i < list.size(); i++)
                cout << list[i] << separator;
        cout << endl;
}

void displayList(vector<XVariable*>& list, string separator = " ")
{
        if (list.size() > 8) {
                for (int i = 0; i < 3; i++)
                        cout << list[i]->id << separator;
                cout << " ... ";
                for (unsigned int i = list.size() - 4; i < list.size(); i++)
                        cout << list[i]->id << separator;
                cout << endl;
                return;
        }
        for (unsigned int i = 0; i < list.size(); i++)
                cout << list[i]->id << separator;
        cout << endl;
}

void Xcsp3_to_Naxos::beginInstance(InstanceType type)
{
        cout << "Start Instance - type=" << type << endl;
}

void Xcsp3_to_Naxos::endInstance()
{
        cout << "End SAX parsing " << endl;
}

void Xcsp3_to_Naxos::beginVariables()
{
        cout << " start variables declaration" << endl;
}

void Xcsp3_to_Naxos::endVariables()
{
        cout << " end variables declaration" << endl << endl;
}

void Xcsp3_to_Naxos::beginVariableArray(string id)
{
        cout << "    array: " << id << endl;
}

void Xcsp3_to_Naxos::endVariableArray()
{
}

void Xcsp3_to_Naxos::beginConstraints()
{
        cout << " start constraints declaration" << endl;
}

void Xcsp3_to_Naxos::endConstraints()
{
        cout << "\n end constraints declaration" << endl << endl;
}

void Xcsp3_to_Naxos::beginGroup(string id)
{
        cout << "   start group of constraint " << id << endl;
}

void Xcsp3_to_Naxos::endGroup()
{
        cout << "   end group of constraint" << endl;
}

void Xcsp3_to_Naxos::beginBlock(string classes)
{
        cout << "   start block of constraint classes = " << classes << endl;
}

void Xcsp3_to_Naxos::endBlock()
{
        cout << "   end group of constraint" << endl;
}

void Xcsp3_to_Naxos::beginObjectives()
{
        cout << "   start Objective " << endl;
}

void Xcsp3_to_Naxos::endObjectives()
{
        cout << "   end Objective " << endl;
}

void Xcsp3_to_Naxos::buildVariableInteger(string id, int minValue, int maxValue)
{
        cout << "    var " << id << " : " << minValue << "..." << maxValue
             << endl;
}

void Xcsp3_to_Naxos::buildVariableInteger(string id, vector<int>& values)
{
        cout << "    var " << id << " : ";
        cout << "        ";
        displayList(values);
}

/// Intension constraint
void Xcsp3_to_Naxos::buildConstraintIntension(string id, string expr)
{
        cout << "\n    intension constraint : " << id << " : " << expr << endl;
}

/// Primitive constraint x +- k op y
void Xcsp3_to_Naxos::buildConstraintPrimitive(string id, OrderType op,
                                              XVariable* x, int k, XVariable* y)
{
        cout << "\n   intension constraint " << id << ": " << x->id
             << (k >= 0 ? "+" : "") << k << " op " << y->id << endl;
}

/// Extension constraint
void Xcsp3_to_Naxos::buildConstraintExtension(string id,
                                              vector<XVariable*> list,
                                              vector<vector<int>>& tuples,
                                              bool support, bool hasStar)
{
        cout << "\n    extension constraint : " << id << endl;
        cout << "        " << (support ? "support" : "conflict")
             << " arity:" << list.size() << " nb tuples: " << tuples.size()
             << " star: " << hasStar << endl;
        cout << "        ";
        displayList(list);
}

/// Unary extension constraint
void Xcsp3_to_Naxos::buildConstraintExtension(string id, XVariable* variable,
                                              vector<int>& tuples, bool support,
                                              bool hasStar)
{
        cout << "\n    extension constraint with one variable: " << id << endl;
        cout << "        " << (support ? "support" : "conflict")
             << " nb tuples: " << tuples.size() << " star: " << hasStar << endl;
        cout << (*variable) << endl;
}

/// Extension constraint with exactly the same tuples as the previous one
void Xcsp3_to_Naxos::buildConstraintExtensionAs(string id,
                                                vector<XVariable*> list,
                                                bool support, bool hasStar)
{
        cout << "\n    extension constraint similar as previous one: " << id
             << endl;
}

void Xcsp3_to_Naxos::buildConstraintAlldifferent(string id,
                                                 vector<XVariable*>& list)
{
        cout << "\n    allDiff constraint" << id << endl;
        cout << "        ";
        displayList(list);
}

void Xcsp3_to_Naxos::buildConstraintAlldifferentList(
    string id, vector<vector<XVariable*>>& lists)
{
        cout << "\n    allDiff list constraint" << id << endl;
        for (unsigned int i = 0; i < (lists.size() < 4 ? lists.size() : 3);
             i++) {
                cout << "        ";
                displayList(lists[i]);
        }
}

/// Weighted sum constraint
void Xcsp3_to_Naxos::buildConstraintSum(string id, vector<XVariable*>& list,
                                        vector<int>& coeffs, XCondition& cond)
{
        cout << "\n        sum constraint:";
        if (list.size() > 8) {
                for (int i = 0; i < 3; i++)
                        cout << (coeffs.size() == 0 ? 1 : coeffs[i]) << "*"
                             << *(list[i]) << " ";
                cout << " ... ";
                for (unsigned int i = list.size() - 4; i < list.size(); i++)
                        cout << (coeffs.size() == 0 ? 1 : coeffs[i]) << "*"
                             << *(list[i]) << " ";
        } else {
                for (unsigned int i = 0; i < list.size(); i++)
                        cout << (coeffs.size() == 0 ? 1 : coeffs[i]) << "*"
                             << *(list[i]) << " ";
        }
        cout << cond << endl;
}

/// Unweighted sum constraint
void Xcsp3_to_Naxos::buildConstraintSum(string id, vector<XVariable*>& list,
                                        XCondition& cond)
{
        cout << "\n        unweighted sum constraint:";
        cout << "        ";
        displayList(list, "+");
        cout << cond << endl;
}

/// Sum constraint with variables as weights
void Xcsp3_to_Naxos::buildConstraintSum(string id, vector<XVariable*>& list,
                                        vector<XVariable*>& coeffs,
                                        XCondition& cond)
{
        cout << "\n        scalar sum constraint:";
        if (list.size() > 8) {
                for (int i = 0; i < 3; i++)
                        cout << coeffs[i]->id << "*" << *(list[i]) << " ";
                cout << " ... ";
                for (unsigned int i = list.size() - 4; i < list.size(); i++)
                        cout << coeffs[i]->id << "*" << *(list[i]) << " ";
        } else {
                for (unsigned int i = 0; i < list.size(); i++)
                        cout << coeffs[i]->id << "*" << *(list[i]) << " ";
        }
        cout << cond << endl;
}

/// Value is in position index inside the list
void Xcsp3_to_Naxos::buildConstraintElement(string id, vector<XVariable*>& list,
                                            int startIndex, XVariable* index,
                                            RankType rank, int value)
{
        cout << "\n    element constant (with index) constraint" << endl;
        cout << "        ";
        displayList(list);
        cout << "        value: " << value << endl;
        cout << "        Start index : " << startIndex << endl;
        cout << "        index : " << *index << endl;
}

/// Variable is in position index inside the list
void Xcsp3_to_Naxos::buildConstraintElement(string id, vector<XVariable*>& list,
                                            int startIndex, XVariable* index,
                                            RankType rank, XVariable* value)
{
        cout << "\n    element variable (with index) constraint" << endl;
        cout << "        ";
        displayList(list);
        cout << "        value: " << *value << endl;
        cout << "        Start index : " << startIndex << endl;
        cout << "        index : " << *index << endl;
}

/// The expression can only be a variable (identifier)
void Xcsp3_to_Naxos::buildObjectiveMinimizeExpression(string expr)
{
        cout << "\n    objective: minimize" << expr << endl;
}

/// The expression can only be a variable (identifier)
void Xcsp3_to_Naxos::buildObjectiveMaximizeExpression(string expr)
{
        cout << "\n    objective: maximize" << expr << endl;
}

void Xcsp3_to_Naxos::buildObjectiveMinimizeVariable(XVariable* x)
{
        cout << "\n    objective: minimize variable " << x << endl;
}

void Xcsp3_to_Naxos::buildObjectiveMaximizeVariable(XVariable* x)
{
        cout << "\n    objective: maximize variable " << x << endl;
}

void Xcsp3_to_Naxos::buildObjectiveMinimize(ExpressionObjective type,
                                            vector<XVariable*>& list,
                                            vector<int>& coefs)
{
        XCSP3CoreCallbacks::buildObjectiveMinimize(type, list, coefs);
}

void Xcsp3_to_Naxos::buildObjectiveMaximize(ExpressionObjective type,
                                            vector<XVariable*>& list,
                                            vector<int>& coefs)
{
        XCSP3CoreCallbacks::buildObjectiveMaximize(type, list, coefs);
}

void Xcsp3_to_Naxos::buildObjectiveMinimize(ExpressionObjective type,
                                            vector<XVariable*>& list)
{
        XCSP3CoreCallbacks::buildObjectiveMinimize(type, list);
}

void Xcsp3_to_Naxos::buildObjectiveMaximize(ExpressionObjective type,
                                            vector<XVariable*>& list)
{
        XCSP3CoreCallbacks::buildObjectiveMaximize(type, list);
}
