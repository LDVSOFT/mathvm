/*
 * VisitorSourcePrinter.cpp
 *
 *  Created on: 23.10.2011
 *      Author: niea
 */

#include "VisitorSourcePrinter.h"

VisitorSourcePrinter::VisitorSourcePrinter(std::ostream &msg_stream) :
	m_stream(msg_stream) {
	// TODO Auto-generated constructor stub
}

VisitorSourcePrinter::~VisitorSourcePrinter() {
	// TODO Auto-generated destructor stub
}

void VisitorSourcePrinter::visitIfNode(mathvm::IfNode *node) {
	m_stream << "<IF_NODE>";
	m_stream << "if (";
	node->ifExpr()->visit(this);
	m_stream << ") {" << std::endl;
	node->thenBlock()->visit(this);

	if (node->elseBlock()) {
		m_stream << std::endl << "} else {" << std::endl;
		node->elseBlock()->visit(this);
	}
	m_stream << "}" << std::endl;
}

void VisitorSourcePrinter::visitPrintNode(mathvm::PrintNode *node) {
	m_stream << "<PRINT_NODE>" << std::endl;
	m_stream << "print (";
	//node->visitChildren(this);
	for (unsigned int i = 0; i < node->operands(); ++i) {
		node->operandAt(i)->visit(this);
		if (i != node->operands() - 1) {
			m_stream << ", ";
		}
	}

	m_stream << ")";
}

void VisitorSourcePrinter::visitLoadNode(mathvm::LoadNode *node) {
	m_stream << "<LOAD_NODE> ";
	m_stream << node->var()->name();
}

void VisitorSourcePrinter::visitForNode(mathvm::ForNode *node) {
	m_stream << "<FOR_NODE> ";
	m_stream << "for (" << node->var()->name() << " in ";
	node->inExpr()->visit(this);
	m_stream << ") {" << std::endl;
	node->body()->visit(this);
	m_stream << "}" << std::endl;
}

void VisitorSourcePrinter::visitIntLiteralNode(mathvm::IntLiteralNode *node) {
	m_stream << "<INTLITERAL_NODE> ";
	m_stream << node->literal();
}

void VisitorSourcePrinter::visitUnaryOpNode(mathvm::UnaryOpNode *node) {
	m_stream << "<UNARY_OP_NODE> ";
	m_stream << tokenOp(node->kind());
	node->operand()->visit(this);
}

void VisitorSourcePrinter::visitFunctionNode(mathvm::FunctionNode *node) {
	m_stream << "<FUNCTION_NODE> " << std::endl;
	if (node->name() != "<top>") {
		m_stream << "function " << varTypeToStr(node->returnType()) << " "
				<< node->name() << " (";

		for (uint32_t i = 0; i != node->parametersNumber(); ++i) {
			m_stream << varTypeToStr(node->parameterType(i)) << " "
					<< node->parameterName(i);
			if (i != node->parametersNumber() - 1) {
				m_stream << ", ";
			}
		}

		m_stream << ") {" << std::endl;
		node->body()->visit(this);
		m_stream << "}" << std::endl;
	} else {
		node->body()->visit(this);
	}
}

void VisitorSourcePrinter::visitWhileNode(mathvm::WhileNode *node) {
	m_stream << "<WHILE_NODE> ";
	m_stream << "while (";
	node->whileExpr()->visit(this);
	m_stream << ") {" << std::endl;
	node->loopBlock()->visit(this);
	m_stream << std::endl << "}" << std::endl;
}

void VisitorSourcePrinter::visitBinaryOpNode(mathvm::BinaryOpNode *node) {
	m_stream << "<BINARY_OP_NODE> ";
	m_stream << "(";
	node->left()->visit(this);
	m_stream << tokenOp(node->kind());
	node->right()->visit(this);
	m_stream << ")";
}

void VisitorSourcePrinter::visitBlockNode(mathvm::BlockNode *node) {
	m_stream << "<BLOCK_NODE> " << std::endl;
	mathvm::Scope::FunctionIterator f_it(node->scope());
	while (f_it.hasNext()) {
		f_it.next()->node()->visit(this);
	}

	mathvm::Scope::VarIterator it(node->scope());
	while (it.hasNext()) {
		mathvm::AstVar *var = it.next();
		m_stream << varTypeToStr(var->type()) << " " << var->name() << ";" << std::endl;
	}

	for (uint32_t i = 0; i != node->nodes(); ++i) {
		node->nodeAt(i)->visit(this);
		if (!node->nodeAt(i)->isBlockNode() && !node->nodeAt(i)->isForNode()
				&& !node->nodeAt(i)->isWhileNode()
				&& !node->nodeAt(i)->isIfNode()
				&& !node->nodeAt(i)->isFunctionNode()
				//&& !node->nodeAt(i)->isStoreNode()
				) {
			m_stream << ";" << std::endl;
		}
	}
}

void VisitorSourcePrinter::visitDoubleLiteralNode(
		mathvm::DoubleLiteralNode *node) {
	m_stream << "<DOUBLW_LITERAL_NODE> ";
	m_stream << node->literal();
}

void VisitorSourcePrinter::visitStringLiteralNode(
		mathvm::StringLiteralNode *node) {
	m_stream << "<STRING_LITERAL_NODE> ";
	std::string str = node->literal();
	replaceSpecSymbols(str);
	m_stream << "'" << str << "'";
}

void VisitorSourcePrinter::visitStoreNode(mathvm::StoreNode *node) {
	m_stream << "<STORE_NODE> ";
	//node->visitChildren(this);
	m_stream << node->var()->name() << tokenOp(node->op());
	node->value()->visit(this);
}

void VisitorSourcePrinter::visitCallNode(mathvm::CallNode *node) {
	m_stream << node->name() << "(";
	for (uint32_t i = 0; i != node->parametersNumber(); ++i) {
		node->parameterAt(i)->visit(this);
		if (i != node->parametersNumber() - 1) {
			m_stream << ", ";
		}
	}
	m_stream << ")";
}

void VisitorSourcePrinter::visitReturnNode(mathvm::ReturnNode* node) {
	m_stream << "<RETURN_NODE>";
	m_stream << "return ";
	node->visitChildren(this);
}

std::string VisitorSourcePrinter::varTypeToStr(mathvm::VarType type) {
	switch (type) {
	case mathvm::VT_INVALID:
		return "<invalid var type>";
	case mathvm::VT_VOID:
		return "void";
	case mathvm::VT_DOUBLE:
		return "double";
	case mathvm::VT_INT:
		return "int";
	case mathvm::VT_STRING:
		return "string";
	}
	return "";
}

void VisitorSourcePrinter::replaceSpecSymbols(std::string &str)
{
	int n_pos;
	while ((n_pos = str.find('\n')) >= 0) {
		str.erase(n_pos, 1);
		str.insert(n_pos, "\\n");
	}

	while ((n_pos = str.find('\'')) >= 0) {
		str.erase(n_pos, 1);
		str.insert(n_pos, "\\'");
	}
}
