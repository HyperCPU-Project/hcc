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

    bool IsRegister();

    static Value* CreateAsRegister(HCC* hcc, uint64_t value, std::string reg_name = "");
    static Value* CreateAsCompileTimeValue(HCC* hcc, uint64_t value);
    static Value* CreateAsStackVar(HCC* hcc, TypeMetadata type, bool reserve = true);

    Value* Use(HCC* hcc);

    Value* DoCondLod(HCC* hcc, std::string load_reg = "");

  private:
    void DoCondStr(HCC* hcc);

  public:
    void Add(HCC* hcc, Value* other);
    void Sub(HCC* hcc, Value* other);
    void Mul(HCC* hcc, Value* other);
    void Div(HCC* hcc, Value* other);
    void SetTo(HCC* hcc, Value* other);
  };
} // namespace hcc
