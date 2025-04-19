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

class AstVarDeclare : public AstNode {
public:
	std::string name;
	std::string type;
	void print(int indent = 0) const override;
	bool compile(HCC* hcc) override;
};

class AstVarAssign : public AstNode {
public:
	std::string name;
	AstNode* expr;
	void print(int indent = 0) const override;
	bool compile(HCC* hcc) override;
	~AstVarAssign();
};

class AstNumber : public AstNode {
public:
	int value;
	explicit AstNumber(int val) : value(val) {
	}
	void print(int indent = 0) const override;
	bool compile(HCC* hcc) override;
};

class AstBinaryOp : public AstNode {
public:
	AstNode* left;
	AstNode* right;
	std::string op;

	void print(int indent = 0) const override;
	bool compile(HCC* hcc) override;

	~AstBinaryOp();
};

class AstReturn : public AstNode {
public:
	AstNode* expr;
	void print(int indent = 0) const override;
	bool compile(HCC* hcc) override;

	~AstReturn();
};

class AstVarRef : public AstNode {
public:
	std::string name;
	void print(int indent = 0) const override;
	bool compile(HCC* hcc) override;
};

class AstAsm : public AstNode {
public:
	std::string code;
	void print(int indent = 0) const override;
	bool compile(HCC* hcc) override;
};
} // namespace hcc
