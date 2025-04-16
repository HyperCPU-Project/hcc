#include "ast.hpp"
#include <iostream>

extern int yyparse();
extern FILE* yyin;
extern AstRootNode* root;

int main(int argc, char** argv) {
	if (argc > 1) {
		yyin = fopen(argv[1], "r");
		if (!yyin) {
			std::cerr << "Could not open file: " << argv[1] << std::endl;
			return 1;
		}
	}

	yyparse();

	if (root) {
		root->print();
		delete root;
	}

	if (yyin != stdin) {
		fclose(yyin);
	}

	return 0;
}