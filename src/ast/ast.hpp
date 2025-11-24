#pragma once
#include "ast/type.hpp"
#include "cstd_pch.hpp"
#include <memory>

namespace hcc {
  class HCC;
  class AstNode {
  public:
    virtual ~AstNode() = 0;
    virtual void Print(int indent = 0) const = 0;
    virtual bool Compile(HCC* hcc);

    std::vector<std::unique_ptr<AstNode>> children;

  protected:
    void PrintIndent(int indent) const;
  };

  class AstRootNode : public AstNode {
  public:
    void Print(int indent = 0) const override;
  };

  class AstFuncDef : public AstNode {
  public:
    std::string name;
    std::map<std::string, ParserVarType> args;

    void Print(int indent = 0) const override;
    bool Compile(HCC* hcc) override;
  };

  class AstVarDeclare : public AstNode {
  public:
    std::vector<std::string> names;
    ParserVarType type;
    void Print(int indent = 0) const override;
    bool Compile(HCC* hcc) override;
  };

  class AstVarAssign : public AstNode {
  public:
    std::string name;
    std::unique_ptr<AstNode> expr;
    void Print(int indent = 0) const override;
    bool Compile(HCC* hcc) override;
    ~AstVarAssign();
  };

  class AstNumber : public AstNode {
  public:
    int value;
    explicit AstNumber(int val)
        : value(val) {
    }
    void Print(int indent = 0) const override;
    bool Compile(HCC* hcc) override;
  };

  class AstBinaryOp : public AstNode {
  public:
    std::unique_ptr<AstNode> left;
    std::unique_ptr<AstNode> right;
    std::string op;

    void Print(int indent = 0) const override;
    bool Compile(HCC* hcc) override;

    ~AstBinaryOp();
  };

  class AstReturn : public AstNode {
  public:
    std::unique_ptr<AstNode> expr;
    void Print(int indent = 0) const override;
    bool Compile(HCC* hcc) override;

    ~AstReturn();
  };

  class AstVarRef : public AstNode {
  public:
    std::string name;
    void Print(int indent = 0) const override;
    bool Compile(HCC* hcc) override;
  };

  class AstAsm : public AstNode {
  public:
    std::string code;
    void Print(int indent = 0) const override;
    bool Compile(HCC* hcc) override;
  };

  class AstAddrof : public AstNode {
  public:
    std::string name;
    void Print(int indent = 0) const override;
    bool Compile(HCC* hcc) override;
  };

  class AstFuncCall : public AstNode {
  public:
    std::vector<std::unique_ptr<AstNode>> args;
    std::string name;

    void Print(int indent = 0) const override;
    bool Compile(HCC* hcc) override;

    ~AstFuncCall();
  };
} // namespace hcc
