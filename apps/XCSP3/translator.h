/// @file
/// Interface between XCSP3 definitions and Naxos
///
/// Part of https://github.com/pothitos/naxos

/*
 * parser for CSP instances represented in XCSP3 Format
 *
 * Copyright (c) 2015 xcp3.org (contact <at> xcsp3.org)
 * Copyright (c) 2008 Olivier ROUSSEL (olivier.roussel <at> cril.univ-artois.fr)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef COSOCO_XCSP3PRINTCALLBACKS_H
#define COSOCO_XCSP3PRINTCALLBACKS_H

#include <XCSP3CoreCallbacks.h>

namespace XCSP3Core {

/// The XCSP3 parser invokes the functions of this class
class Xcsp3_to_Naxos : public XCSP3CoreCallbacks {

    public:
        /// @{
        /// @name Variables Definition

        virtual void beginInstance(InstanceType type) override;

        virtual void endInstance() override;

        virtual void beginVariables() override;

        virtual void endVariables() override;

        virtual void beginVariableArray(string id) override;

        virtual void endVariableArray() override;

        virtual void beginConstraints() override;

        virtual void endConstraints() override;

        virtual void beginGroup(string id) override;

        virtual void endGroup() override;

        virtual void beginBlock(string classes) override;

        virtual void endBlock() override;

        virtual void beginSlide(string id, bool circular) override;

        virtual void endSlide() override;

        virtual void beginObjectives() override;

        virtual void endObjectives() override;

        virtual void buildVariableInteger(string id, int minValue,
                                          int maxValue) override;

        virtual void buildVariableInteger(string id,
                                          vector<int>& values) override;

        /// @}

        /// @{
        /// @name Intensional Constraints

        virtual void buildConstraintIntension(string id, string expr) override;

        virtual void buildConstraintPrimitive(string id, OrderType op,
                                              XVariable* x, int k,
                                              XVariable* y) override;

        /// @}

        /// @{
        /// @name Extensional Constraints

        virtual void buildConstraintExtension(string id,
                                              vector<XVariable*> list,
                                              vector<vector<int>>& tuples,
                                              bool support,
                                              bool hasStar) override;

        virtual void buildConstraintExtension(string id, XVariable* variable,
                                              vector<int>& tuples, bool support,
                                              bool hasStar) override;

        virtual void buildConstraintExtensionAs(string id,
                                                vector<XVariable*> list,
                                                bool support,
                                                bool hasStar) override;

        /// @}

        virtual void
        buildConstraintRegular(string id, vector<XVariable*>& list, string st,
                               vector<string>& final,
                               vector<XTransition>& transitions) override;

        virtual void
        buildConstraintMDD(string id, vector<XVariable*>& list,
                           vector<XTransition>& transitions) override;

        /// @{
        /// @name AllDifferent

        virtual void
        buildConstraintAlldifferent(string id,
                                    vector<XVariable*>& list) override;

        virtual void
        buildConstraintAlldifferentExcept(string id, vector<XVariable*>& list,
                                          vector<int>& except) override;

        virtual void buildConstraintAlldifferentList(
            string id, vector<vector<XVariable*>>& lists) override;

        virtual void buildConstraintAlldifferentMatrix(
            string id, vector<vector<XVariable*>>& matrix) override;

        /// @}

        virtual void buildConstraintAllEqual(string id,
                                             vector<XVariable*>& list) override;

        virtual void
        buildConstraintNotAllEqual(string id,
                                   vector<XVariable*>& list) override;

        virtual void buildConstraintOrdered(string id, vector<XVariable*>& list,
                                            OrderType order) override;

        virtual void buildConstraintLex(string id,
                                        vector<vector<XVariable*>>& lists,
                                        OrderType order) override;

        virtual void
        buildConstraintLexMatrix(string id, vector<vector<XVariable*>>& matrix,
                                 OrderType order) override;

        /// @{
        /// @name Sum

        virtual void buildConstraintSum(string id, vector<XVariable*>& list,
                                        vector<int>& coeffs,
                                        XCondition& cond) override;

        virtual void buildConstraintSum(string id, vector<XVariable*>& list,
                                        XCondition& cond) override;

        virtual void buildConstraintSum(string id, vector<XVariable*>& list,
                                        vector<XVariable*>& coeffs,
                                        XCondition& cond) override;

        /// @}

        virtual void buildConstraintAtMost(string id, vector<XVariable*>& list,
                                           int value, int k) override;

        virtual void buildConstraintAtLeast(string id, vector<XVariable*>& list,
                                            int value, int k) override;

        virtual void buildConstraintExactlyK(string id,
                                             vector<XVariable*>& list,
                                             int value, int k) override;

        virtual void buildConstraintAmong(string id, vector<XVariable*>& list,
                                          vector<int>& values, int k) override;

        virtual void buildConstraintExactlyVariable(string id,
                                                    vector<XVariable*>& list,
                                                    int value,
                                                    XVariable* x) override;

        virtual void buildConstraintCount(string id, vector<XVariable*>& list,
                                          vector<int>& values,
                                          XCondition& xc) override;

        virtual void buildConstraintCount(string id, vector<XVariable*>& list,
                                          vector<XVariable*>& values,
                                          XCondition& xc) override;

        virtual void buildConstraintNValues(string id, vector<XVariable*>& list,
                                            vector<int>& except,
                                            XCondition& xc) override;

        virtual void buildConstraintNValues(string id, vector<XVariable*>& list,
                                            XCondition& xc) override;

        virtual void buildConstraintCardinality(string id,
                                                vector<XVariable*>& list,
                                                vector<int> values,
                                                vector<int>& occurs,
                                                bool closed) override;

        virtual void buildConstraintCardinality(string id,
                                                vector<XVariable*>& list,
                                                vector<int> values,
                                                vector<XVariable*>& occurs,
                                                bool closed) override;

        virtual void buildConstraintCardinality(string id,
                                                vector<XVariable*>& list,
                                                vector<int> values,
                                                vector<XInterval>& occurs,
                                                bool closed) override;

        virtual void buildConstraintCardinality(string id,
                                                vector<XVariable*>& list,
                                                vector<XVariable*> values,
                                                vector<int>& occurs,
                                                bool closed) override;

        virtual void buildConstraintCardinality(string id,
                                                vector<XVariable*>& list,
                                                vector<XVariable*> values,
                                                vector<XVariable*>& occurs,
                                                bool closed) override;

        virtual void buildConstraintCardinality(string id,
                                                vector<XVariable*>& list,
                                                vector<XVariable*> values,
                                                vector<XInterval>& occurs,
                                                bool closed) override;

        virtual void buildConstraintMinimum(string id, vector<XVariable*>& list,
                                            XCondition& xc) override;

        virtual void buildConstraintMinimum(string id, vector<XVariable*>& list,
                                            XVariable* index, int startIndex,
                                            RankType rank,
                                            XCondition& xc) override;

        virtual void buildConstraintMaximum(string id, vector<XVariable*>& list,
                                            XCondition& xc) override;

        virtual void buildConstraintMaximum(string id, vector<XVariable*>& list,
                                            XVariable* index, int startIndex,
                                            RankType rank,
                                            XCondition& xc) override;

        /// @{
        /// @name Element

        virtual void buildConstraintElement(string id, vector<XVariable*>& list,
                                            int value) override;

        virtual void buildConstraintElement(string id, vector<XVariable*>& list,
                                            XVariable* value) override;

        virtual void buildConstraintElement(string id, vector<XVariable*>& list,
                                            int startIndex, XVariable* index,
                                            RankType rank, int value) override;

        virtual void buildConstraintElement(string id, vector<XVariable*>& list,
                                            int startIndex, XVariable* index,
                                            RankType rank,
                                            XVariable* value) override;

        /// @}

        virtual void buildConstraintChannel(string id, vector<XVariable*>& list,
                                            int startIndex) override;

        virtual void buildConstraintChannel(string id,
                                            vector<XVariable*>& list1,
                                            int startIndex1,
                                            vector<XVariable*>& list2,
                                            int startIndex2) override;

        virtual void buildConstraintChannel(string id, vector<XVariable*>& list,
                                            int startIndex,
                                            XVariable* value) override;

        virtual void buildConstraintStretch(string id, vector<XVariable*>& list,
                                            vector<int>& values,
                                            vector<XInterval>& widths) override;

        virtual void
        buildConstraintStretch(string id, vector<XVariable*>& list,
                               vector<int>& values, vector<XInterval>& widths,
                               vector<vector<int>>& patterns) override;

        virtual void buildConstraintNoOverlap(string id,
                                              vector<XVariable*>& origins,
                                              vector<int>& lengths,
                                              bool zeroIgnored) override;

        virtual void buildConstraintNoOverlap(string id,
                                              vector<XVariable*>& origins,
                                              vector<XVariable*>& lengths,
                                              bool zeroIgnored) override;

        virtual void
        buildConstraintNoOverlap(string id, vector<vector<XVariable*>>& origins,
                                 vector<vector<int>>& lengths,
                                 bool zeroIgnored) override;

        virtual void
        buildConstraintNoOverlap(string id, vector<vector<XVariable*>>& origins,
                                 vector<vector<XVariable*>>& lengths,
                                 bool zeroIgnored) override;

        virtual void buildConstraintInstantiation(string id,
                                                  vector<XVariable*>& list,
                                                  vector<int>& values) override;

        virtual void buildObjectiveMinimizeExpression(string expr) override;

        virtual void buildObjectiveMaximizeExpression(string expr) override;

        virtual void buildObjectiveMinimizeVariable(XVariable* x) override;

        virtual void buildObjectiveMaximizeVariable(XVariable* x) override;

        virtual void buildObjectiveMinimize(ExpressionObjective type,
                                            vector<XVariable*>& list,
                                            vector<int>& coefs) override;

        virtual void buildObjectiveMaximize(ExpressionObjective type,
                                            vector<XVariable*>& list,
                                            vector<int>& coefs) override;

        virtual void buildObjectiveMinimize(ExpressionObjective type,
                                            vector<XVariable*>& list) override;

        virtual void buildObjectiveMaximize(ExpressionObjective type,
                                            vector<XVariable*>& list) override;
};
}

#endif // COSOCO_XCSP3PRINTCALLBACKS_H
