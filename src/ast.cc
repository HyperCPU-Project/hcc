#include "ast.hpp"
#include <iostream>

void AstNode::printIndent(int indent) const {
	for (int i = 0; i < indent; ++i) {
		std::cout << "  ";
	}
}

void AstRootNode::print(int indent) const {
	printIndent(indent);
	std::cout << "AstRootNode" << std::endl;
	for (const auto& child : children) {
		child->print(indent + 1);
	}
}

void AstFuncDef::print(int indent) const {
	printIndent(indent);
	std::cout << "AstFuncDef" << std::endl;
	printIndent(indent + 1);
	std::cout << "name: " << name << std::endl;
	for (const auto& stmt : body) {
		stmt->print(indent + 1);
	}
}

void AstVarDeclare::print(int indent) const {
	printIndent(indent);
	std::cout << "AstVarDeclare" << std::endl;
	printIndent(indent + 1);
	std::cout << "name: " << name << std::endl;
	printIndent(indent + 1);
	std::cout << "type: " << type << std::endl;
}

void AstVarAssign::print(int indent) const {
	printIndent(indent);
	std::cout << "AstVarAssign" << std::endl;
	printIndent(indent + 1);
	std::cout << "name: " << name << std::endl;
	printIndent(indent + 1);
	std::cout << "expr:" << std::endl;
	expr->print(indent + 2);
}

void AstNumber::print(int indent) const {
	printIndent(indent);
	std::cout << "AstNumber" << std::endl;
	printIndent(indent + 1);
	std::cout << "value: " << value << std::endl;
}

void AstBinaryOp::print(int indent) const {
	printIndent(indent);
	std::cout << "AstBinaryOp" << std::endl;
	printIndent(indent + 1);
	std::cout << "op: " << op << std::endl;
	printIndent(indent + 1);
	std::cout << "left:" << std::endl;
	left->print(indent + 2);
	printIndent(indent + 1);
	std::cout << "right:" << std::endl;
	right->print(indent + 2);
}

void AstReturn::print(int indent) const {
	printIndent(indent);
	std::cout << "AstReturn" << std::endl;
	if (expr) {
		expr->print(indent + 1);
	}
}

void AstVarRef::print(int indent) const {
	printIndent(indent);
	std::cout << name << std::endl;
}