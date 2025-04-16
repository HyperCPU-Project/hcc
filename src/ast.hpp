#ifndef AST_HPP
#define AST_HPP

#include <pch.hpp>

// Forward declarations
class AstNode;
class AstExpr;
class AstStatement;

using AstExprPtr = std::shared_ptr<AstExpr>;
using AstStatementPtr = std::shared_ptr<AstStatement>;

// Base AST node class
class AstNode {
public:
	virtual ~AstNode() = default;
	virtual void print(int indent = 0) const = 0;

protected:
	void printIndent(int indent) const;
};

using AstNodePtr = std::shared_ptr<AstNode>;

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
	std::vector<AstNodePtr> children;
	void print(int indent = 0) const override;
};

class AstFuncDef : public AstNode {
public:
	std::string name;
	std::vector<AstStatementPtr> body;
	void print(int indent = 0) const override;
};

class AstVarDeclare : public AstStatement {
public:
	std::string name;
	std::string type;
	void print(int indent = 0) const override;
};

class AstVarAssign : public AstStatement {
public:
	std::string name;
	AstExprPtr expr;
	void print(int indent = 0) const override;
};

class AstNumber : public AstExpr {
public:
	int value;
	explicit AstNumber(int val) : value(val) {
	}
	void print(int indent = 0) const override;
};

class AstBinaryOp : public AstExpr {
public:
	AstExprPtr left;
	AstExprPtr right;
	std::string op;
	void print(int indent = 0) const override;
};

class AstReturn : public AstStatement {
public:
	AstExprPtr expr;
	void print(int indent = 0) const override;
};

class AstVarRef : public AstExpr {
public:
	std::string name;
	void print(int indent = 0) const override;
};

#endif