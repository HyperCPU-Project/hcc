#pragma once

#include <memory>
#include <type_metadata.hpp>
#include <value/value_stack_var.hpp>

namespace hcc {
  class HCC;

  class Value {
  public:
    std::variant<std::string, uint64_t, ValueStackVar> value;

    Value();
    ~Value();

    bool IsRegister();
    bool IsCompileTime();
    bool IsStackVar();

    static std::shared_ptr<Value> CreateAsRegister(HCC* hcc, uint64_t value);
    static std::shared_ptr<Value> CreateAsCompileTimeValue(HCC* hcc, uint64_t value);
    static std::shared_ptr<Value> CreateAsStackVar(HCC* hcc, TypeMetadata type, bool reserve = true);

    std::shared_ptr<Value> Use(HCC* hcc);

    std::shared_ptr<Value> DoCondLod(HCC* hcc);

  private:
    void DoCondStr(HCC* hcc);

  public:
    void Add(HCC* hcc, std::shared_ptr<Value> other);
    void Sub(HCC* hcc, std::shared_ptr<Value> other);
    void Mul(HCC* hcc, std::shared_ptr<Value> other);
    void Div(HCC* hcc, std::shared_ptr<Value> other);
    void SetTo(HCC* hcc, std::shared_ptr<Value> other);
  };
} // namespace hcc
