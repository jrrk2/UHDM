// -*- c++ -*-

/*

 Copyright 2019-2022 Alain Dargelas

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

 http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
 */

/*
 * File:   UhdmLint.cpp
 * Author: alaindargelas
 *
 * Created on Jan 3, 2022, 9:03 PM
 */
#include <string.h>

#include <uhdm/UhdmLint.h>
#include <uhdm/clone_tree.h>
#include <uhdm/uhdm.h>

namespace UHDM {

void UhdmLint::leaveBit_select(const bit_select* object,
                               const BaseClass* parent, vpiHandle handle,
                               vpiHandle parentHandle) {
  const expr* index = object->VpiIndex();
  if (index) {
    if (index->UhdmType() == uhdmref_obj) {
      ref_obj* ref = (ref_obj*)index;
      const any* act = ref->Actual_group();
      if (act && act->UhdmType() == uhdmreal_var) {
        serializer_->GetErrorHandler()(ErrorType::UHDM_REAL_TYPE_AS_SELECT,
                                       act->VpiName(), ref, nullptr);
      }
    }
  }
}

static const any* returnWithValue(const any* stmt) {
  switch (stmt->UhdmType()) {
    case uhdmreturn_stmt: {
      return_stmt* ret = (return_stmt*)stmt;
      if (const any* r = ret->VpiCondition()) return r;
      break;
    }
    case uhdmbegin: {
      begin* st = (begin*)stmt;
      for (auto s : *st->Stmts()) {
        if (const any* r = returnWithValue(s)) return r;
      }
      break;
    }
    case uhdmif_stmt: {
      if_stmt* st = (if_stmt*) stmt;
      if (const any* r = returnWithValue(st->VpiStmt())) return r;
      break;
    }
    case uhdmif_else: {
      if_else* st = (if_else*) stmt;
      if (const any* r = returnWithValue(st->VpiStmt())) return r;
      if (const any* r = returnWithValue(st->VpiElseStmt())) return r;
      break;
    }
    default:
      break;
  }
  return nullptr;
}

void UhdmLint::leaveFunction(const function* object, const BaseClass* parent,
                     vpiHandle handle, vpiHandle parentHandle) {
  if (object->Return() == nullptr) {
    if (const any* st = object->Stmt()) {
      const any* ret = returnWithValue(st);
      if (ret) {
        serializer_->GetErrorHandler()(ErrorType::UHDM_RETURN_VALUE_VOID_FUNCTION,
                                       object->VpiName(), ret, nullptr);
      }
    }
  }

}

void UhdmLint::leaveStruct_typespec(const struct_typespec* object,
                                    const BaseClass* parent, vpiHandle handle,
                                    vpiHandle parentHandle) {
  if (object->VpiPacked()) {
    if (object->Members()) {
      for (typespec_member* member : *object->Members()) {
        if (member->Default_value()) {
          serializer_->GetErrorHandler()(ErrorType::UHDM_ILLEGAL_DEFAULT_VALUE,
                                         std::string(""),
                                         member->Default_value(), nullptr);
        }
      }
    }
  }
}

void UhdmLint::leaveModule(const module* object, const BaseClass* parent,
                           vpiHandle handle, vpiHandle parentHandle) {
  if (auto assigns = object->Cont_assigns()) {
    checkMultiContAssign(assigns);
  }
}

void UhdmLint::checkMultiContAssign(
    const std::vector<UHDM::cont_assign*>* assigns) {
  for (unsigned int i = 0; i < assigns->size() - 1; i++) {
    cont_assign* cassign = assigns->at(i);
    const expr* lhs_exp = cassign->Lhs();
    const expr* rhs_exp = cassign->Rhs();
    if (rhs_exp->UhdmType() == uhdmoperation) {
      operation* op = (operation*)rhs_exp;
      bool triStatedOp = false;
      for (auto operand : *op->Operands()) {
        if (operand->UhdmType() == uhdmconstant) {
          constant* c = (constant*)operand;
          if (strstr(c->VpiValue().c_str(), "z")) {
            triStatedOp = true;
            break;
          }
        }
      }
      if (triStatedOp) continue;
    }
    for (unsigned int j = i + 1; j < assigns->size(); j++) {
      cont_assign* as = assigns->at(j);
      const UHDM::expr* lhs = as->Lhs();
      const UHDM::expr* rhs = as->Rhs();
      if (lhs->UhdmType() == uhdmref_obj) {
        const std::string& n = lhs->VpiName();
        if (n == lhs_exp->VpiName()) {
          ref_obj* ref = (ref_obj*)lhs;
          const any* actual = ref->Actual_group();
          if (actual) {
            if (actual->UhdmType() == uhdmlogic_net) {
              logic_net* net = (logic_net*)actual;
              int nettype = net->VpiNetType();
              if (nettype == vpiWor || nettype == vpiWand ||
                  nettype == vpiTri || nettype == vpiTriAnd ||
                  nettype == vpiTriOr || nettype == vpiTri0 ||
                  nettype == vpiTri1 || nettype == vpiTriReg)
                continue;
            }
          }
          if (as->VpiStrength0() || cassign->VpiStrength0()) continue;
          if (as->VpiStrength1() || cassign->VpiStrength1()) continue;
          if (rhs->UhdmType() == uhdmoperation) {
            operation* op = (operation*)rhs;
            bool triStatedOp = false;
            for (auto operand : *op->Operands()) {
              if (operand->UhdmType() == uhdmconstant) {
                constant* c = (constant*)operand;
                if (strstr(c->VpiValue().c_str(), "z")) {
                  triStatedOp = true;
                  break;
                }
              }
            }
            if (triStatedOp) continue;
          }
          serializer_->GetErrorHandler()(ErrorType::UHDM_MULTIPLE_CONT_ASSIGN,
                                         lhs_exp->VpiName(), lhs_exp, lhs);
        }
      }
    }
  }
}

}  // namespace UHDM
