#pragma once
#include <pch.hpp>

namespace hcc {
class HCC;
class AstNode {
public:
	virtual ~AstNode() = 0;
	virtual void print(int indent = 0) const = 0;
	virtual bool compile(HCC* hcc);

	std::vector<AstNode*> children;

protected:
	void printIndent(int indent) const;
};

class AstExpr : public AstNode {
public:
	virtual ~AstExpr() = default;
};

class AstStatement : public AstNode {
public:
	virtual ~AstStatement() = default;
};

class AstRootNode : public AstNode {
public:
	void print(int indent = 0) const override;
};

class AstFuncDef : public AstNode {
public:
	std::string name;
	void print(int indent = 0) const override;
	bool compile(HCC* hcc) override;
};

class AstVarDeclare : public AstStatement {
public:
	std::string name;
	std::string type;
	void print(int indent = 0) const override;
	bool compile(HCC* hcc) override;
};

class AstVarAssign : public AstStatement {
public:
	std::string name;
	AstExpr* expr;
	void print(int indent = 0) const override;
	bool compile(HCC* hcc) override;
	~AstVarAssign();
};

class AstNumber : public AstExpr {
public:
	int value;
	explicit AstNumber(int val) : value(val) {
	}
	void print(int indent = 0) const override;
	bool compile(HCC* hcc) override;
};

class AstBinaryOp : public AstExpr {
public:
	AstExpr* left;
	AstExpr* right;
	std::string op;

	void print(int indent = 0) const override;
	bool compile(HCC* hcc) override;

	~AstBinaryOp();
};

class AstReturn : public AstStatement {
public:
	AstExpr* expr;
	void print(int indent = 0) const override;
	bool compile(HCC* hcc) override;

	~AstReturn();
};

class AstVarRef : public AstExpr {
public:
	std::string name;
	void print(int indent = 0) const override;
	bool compile(HCC* hcc) override;
};

class AstAsm : public AstStatement {
public:
	std::string code;
	void print(int indent = 0) const override;
	bool compile(HCC* hcc) override;
};
} // namespace hcc
