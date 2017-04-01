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

        /// @{
        /// @name AllDifferent

        virtual void
        buildConstraintAlldifferent(string id,
                                    vector<XVariable*>& list) override;

        virtual void buildConstraintAlldifferentList(
            string id, vector<vector<XVariable*>>& lists) override;

        /// @}

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

        /// @{
        /// @name Constraint Optimization

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

        /// @}
};
}

#endif // COSOCO_XCSP3PRINTCALLBACKS_H
