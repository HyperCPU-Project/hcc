#include "ast.hpp"
#include <pch.hpp>

extern AstFuncDef* root;
extern FILE* yyin;
int yyparse();

void print_ast(AstNode* node, int indent = 0) {
	std::string pad(indent, ' ');

	if (auto n = dynamic_cast<AstFuncDef*>(node)) {
		std::cout << pad << "Function: " << n->name << "\n";
		for (auto child : n->body)
			print_ast(child, indent + 2);
	} else if (auto n = dynamic_cast<AstVarDeclare*>(node)) {
		std::cout << pad << "Declare: " << n->name << "\n";
	} else if (auto n = dynamic_cast<AstVarAssign*>(node)) {
		std::cout << pad << "Assign: " << n->name << " = \n";
		print_ast(n->expr, indent + 2);
	} else if (auto n = dynamic_cast<AstReturn*>(node)) {
		std::cout << pad << "Return:\n";
		print_ast(n->expr, indent + 2);
	} else if (auto n = dynamic_cast<AstBinaryOp*>(node)) {
		std::cout << pad << "BinaryOp: " << n->op << "\n";
		print_ast(n->left, indent + 2);
		print_ast(n->right, indent + 2);
	} else if (auto n = dynamic_cast<AstNumber*>(node)) {
		std::cout << pad << "Number: " << n->value << "\n";
	}
}

int main(int argc, char** argv) {
	if (argc < 2) {
		return 1;
	}

	FILE* file = fopen(argv[1], "r");
	if (!file) {
		perror("fopen");
		return 1;
	}

	yyin = file;
	yyparse();

	if (root) {
		print_ast(root);
	}

	fclose(file);
	return 0;
}
