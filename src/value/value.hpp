#pragma once

#include <metadata.hpp>

namespace hcc {
  class HCC;

  class Value {
  public:
    std::string reg_name;

    bool is_compile_time;
    uint64_t compile_time_value;

    uint64_t var_stack_align;
    std::string var_name;
    TypeMetadata var_type;

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
