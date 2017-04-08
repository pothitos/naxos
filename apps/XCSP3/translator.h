/// @file
/// Interface between XCSP3 definitions and Naxos
///
/// Part of https://github.com/pothitos/naxos

/*
 * parser for CSP instances represented in XCSP3 Format
 *
 * Copyright (c) 2015 xcsp3.org (contact <at> xcsp3.org)
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

/// The XCSP3 parser invokes the functions of this class
class Xcsp3_to_Naxos : public XCSP3Core::XCSP3CoreCallbacks {

    public:
        /// @{
        /// @name Mark where the tags start or end

        virtual void beginInstance(XCSP3Core::InstanceType type) override;

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

        /// @}

        /// @{
        /// @name Constrained variables definition

        virtual void buildVariableInteger(string id, int minValue,
                                          int maxValue) override;

        virtual void buildVariableInteger(string id,
                                          vector<int>& values) override;

        /// @}

        /// @{
        /// @name Intensional Constraints

        virtual void buildConstraintIntension(string id, string expr) override;

        virtual void buildConstraintPrimitive(string id,
                                              XCSP3Core::OrderType op,
                                              XCSP3Core::XVariable* x, int k,
                                              XCSP3Core::XVariable* y) override;

        /// @}

        /// @{
        /// @name Extensional Constraints

        virtual void
        buildConstraintExtension(string id, vector<XCSP3Core::XVariable*> list,
                                 vector<vector<int>>& tuples, bool support,
                                 bool hasStar) override;

        virtual void buildConstraintExtension(string id,
                                              XCSP3Core::XVariable* variable,
                                              vector<int>& tuples, bool support,
                                              bool hasStar) override;

        virtual void
        buildConstraintExtensionAs(string id,
                                   vector<XCSP3Core::XVariable*> list,
                                   bool support, bool hasStar) override;

        /// @}

        /// @{
        /// @name AllDifferent

        virtual void buildConstraintAlldifferent(
            string id, vector<XCSP3Core::XVariable*>& list) override;

        /// @}

        /// @{
        /// @name Sum

        virtual void buildConstraintSum(string id,
                                        vector<XCSP3Core::XVariable*>& list,
                                        vector<int>& coeffs,
                                        XCSP3Core::XCondition& cond) override;

        virtual void buildConstraintSum(string id,
                                        vector<XCSP3Core::XVariable*>& list,
                                        XCSP3Core::XCondition& cond) override;

        virtual void buildConstraintSum(string id,
                                        vector<XCSP3Core::XVariable*>& list,
                                        vector<XCSP3Core::XVariable*>& coeffs,
                                        XCSP3Core::XCondition& cond) override;

        /// @}

        /// @{
        /// @name Element

        virtual void
        buildConstraintElement(string id, vector<XCSP3Core::XVariable*>& list,
                               int startIndex, XCSP3Core::XVariable* index,
                               XCSP3Core::RankType rank, int value) override;

        virtual void
        buildConstraintElement(string id, vector<XCSP3Core::XVariable*>& list,
                               int startIndex, XCSP3Core::XVariable* index,
                               XCSP3Core::RankType rank,
                               XCSP3Core::XVariable* value) override;

        /// @}

        /// @{
        /// @name Constraint Optimization for a variable

        virtual void
        buildObjectiveMinimizeVariable(XCSP3Core::XVariable* x) override;

        virtual void
        buildObjectiveMaximizeVariable(XCSP3Core::XVariable* x) override;

        /// @}

        /// @{
        /// @name Constraint Optimization for an array of variables

        virtual void buildObjectiveMinimize(XCSP3Core::ExpressionObjective type,
                                            vector<XCSP3Core::XVariable*>& list,
                                            vector<int>& coefs) override;

        virtual void buildObjectiveMaximize(XCSP3Core::ExpressionObjective type,
                                            vector<XCSP3Core::XVariable*>& list,
                                            vector<int>& coefs) override;

        virtual void
        buildObjectiveMinimize(XCSP3Core::ExpressionObjective type,
                               vector<XCSP3Core::XVariable*>& list) override;

        virtual void
        buildObjectiveMaximize(XCSP3Core::ExpressionObjective type,
                               vector<XCSP3Core::XVariable*>& list) override;

        /// @}
};

#endif // COSOCO_XCSP3PRINTCALLBACKS_H
