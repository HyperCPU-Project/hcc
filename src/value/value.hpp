#pragma once

#include <metadata.hpp>

namespace hcc {
  class HCC;

  class Value {
  public:
    std::string regName;

    bool isCompileTime;
    uint64_t compileTimeValue;

    uint64_t varStackAlign;
    std::string varName;
    TypeMetadata varType;

    Value();
    ~Value();

    bool isRegister();

    static Value* createAsRegister(HCC* hcc, uint64_t value, std::string reg_name = "");
    static Value* createAsCompileTimeValue(HCC* hcc, uint64_t value);
    static Value* createAsStackVar(HCC* hcc, TypeMetadata type, bool reserve = true);

    Value* use(HCC* hcc);

    Value* doCondLod(HCC* hcc, std::string load_reg = "");

  private:
    void doCondStr(HCC* hcc);

  public:
    void add(HCC* hcc, Value* other);
    void sub(HCC* hcc, Value* other);
    void mul(HCC* hcc, Value* other);
    void div(HCC* hcc, Value* other);
    void setto(HCC* hcc, Value* other);
  };
} // namespace hcc
