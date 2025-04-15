#pragma once
#include <ncc.hpp>
#include <parser/parser.hpp>
#include <pch.hpp>

class Parser;
class NCC;

class AstNode {
public:
	std::vector<AstNode*> children;

	virtual void print() = 0;
	virtual void assemble(NCC* ncc);

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
	void assemble(NCC* ncc);
	void print();
};

class AstReturnNode : public AstNode {
public:
	static AstReturnNode* create(Parser* parser);
	void assemble(NCC* ncc);
	void print();
};