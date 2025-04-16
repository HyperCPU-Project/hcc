#pragma once
#include <hcc.hpp>
#include <metadata.hpp>
#include <parser/parser.hpp>
#include <pch.hpp>

namespace hcc {

class Parser;
class HCC;

class AstNode {
public:
	std::vector<AstNode*> children;

	virtual void print() = 0;
	virtual void assemble(HCC* hcc);

	virtual ~AstNode() = 0;
};

class AstRootNode : public AstNode {
public:
	void print();
};

class AstFuncDef : public AstNode {
public:
	std::string name;
	TypeMetadata return_type;

	static AstFuncDef* create(Parser* parser);
	void assemble(HCC* hcc);
	void print();
};

class AstReturnNode : public AstNode {
public:
	AstNode* return_expression;

	static AstReturnNode* create(Parser* parser);
	void assemble(HCC* hcc);
	void print();
};

class AstBinaryOpNode : public AstNode {
public:
	enum class Operation { ADD, SUB, MUL, DIV };

	Operation op;
	AstNode* left;
	AstNode* right;

	static AstBinaryOpNode* create(Parser* parser, Operation op, AstNode* left, AstNode* right);
	void assemble(HCC* hcc);
	void print();
};

class AstNumberNode : public AstNode {
public:
	int value;

	static AstNumberNode* create(Parser* parser);
	void assemble(HCC* hcc);
	void print();
};

class AstDeclareVarNode : public AstNode {
public:
	TypeMetadata type;
	std::string name;
	VariableMetadata variable;

	static AstDeclareVarNode* create(Parser* parser);
	void assemble(HCC* hcc);
	void print();
};
} // namespace hcc