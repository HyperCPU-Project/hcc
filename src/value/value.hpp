#pragma once

#include <type_metadata.hpp>
#include <value/value_stack_var.hpp>
#include <value/value_type.hpp>

namespace hcc {
  class HCC;

  class Value {
  public:
    ValueType value_type;
    std::variant<std::string, uint64_t, ValueStackVar> value;

    Value();
    ~Value();

    bool IsRegister();
    bool IsCompileTime();
    bool IsStackVar();

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
