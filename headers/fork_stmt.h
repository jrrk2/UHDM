/*
 Do not modify, auto-generated by model_gen.tcl

 Copyright 2019 Alain Dargelas

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
 * File:   fork_stmt.h
 * Author:
 *
 * Created on December 14, 2019, 10:03 PM
 */

#ifndef FORK_STMT_H
#define FORK_STMT_H

namespace UHDM {

  class fork_stmt : public scope {
  public:
    // Implicit constructor used to initialize all members,
    // comment: fork_stmt();
    ~fork_stmt() final {}
    
    BaseClass* get_vpiParent() const { return vpiParent_; }

    void set_vpiParent(BaseClass* data) { vpiParent_ = data; }

    unsigned int get_uhdmParentType() const { return uhdmParentType_; }

    void set_uhdmParentType(unsigned int data) { uhdmParentType_ = data; }

    std::string get_vpiFile() const { return SymbolFactory::getSymbol(vpiFile_); }

    void set_vpiFile(std::string data) { vpiFile_ = SymbolFactory::make(data); }

    unsigned int get_vpiLineNo() const { return vpiLineNo_; }

    void set_vpiLineNo(unsigned int data) { vpiLineNo_ = data; }

    int get_vpiJoinType() const { return vpiJoinType_; }

    void set_vpiJoinType(int data) { vpiJoinType_ = data; }

    virtual unsigned int getUhdmType() { return uhdmfork_stmt; }   
  private:
    
    BaseClass* vpiParent_;

    unsigned int uhdmParentType_;

    unsigned int vpiFile_;

    unsigned int vpiLineNo_;

    int vpiJoinType_;

  };

  class fork_stmtFactory {
  friend Serializer;
  public:
  static fork_stmt* make() {
    fork_stmt* obj = new fork_stmt();
    objects_.push_back(obj);
    return obj;
  }
  private:
    static std::vector<fork_stmt*> objects_;
  };
 	      
  class VectorOffork_stmtFactory {
  friend Serializer;
  public:
  static std::vector<fork_stmt*>* make() {
    std::vector<fork_stmt*>* obj = new std::vector<fork_stmt*>();
    objects_.push_back(obj);
    return obj;
  }
  private:
  static std::vector<std::vector<fork_stmt*>*> objects_;
  };

};

#endif
