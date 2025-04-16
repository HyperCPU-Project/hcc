#pragma once
#include <pch.hpp>

struct AstNode {
	virtual ~AstNode() = default;
};

struct AstNumber : public AstNode {
	int value;
	AstNumber(int val) : value(val) {
	}
};

struct AstBinaryOp : public AstNode {
	std::string op;
	AstNode* left;
	AstNode* right;
	AstBinaryOp(const std::string& op, AstNode* l, AstNode* r) : op(op), left(l), right(r) {
	}
};

struct AstVarDeclare : public AstNode {
	std::string name;
	AstVarDeclare(const std::string& name) : name(name) {
	}
};

struct AstVarAssign : public AstNode {
	std::string name;
	AstNode* expr;
	AstVarAssign(const std::string& name, AstNode* expr) : name(name), expr(expr) {
	}
};

struct AstReturn : public AstNode {
	AstNode* expr;
	AstReturn(AstNode* expr) : expr(expr) {
	}
};

struct AstFuncDef : public AstNode {
	std::string name;
	std::vector<AstNode*> body;
	AstFuncDef(const std::string& name) : name(name) {
	}
};
