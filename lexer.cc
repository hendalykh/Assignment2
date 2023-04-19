/*
 * Copyright (C) Rida Bazzi, 2016
 *
 * Do not share this file with anyone
 */
#include <iostream>
#include <istream>
#include <vector>
#include <string>
#include <cctype>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stack>

#include "lexer.h"
#include "inputbuf.h"


using namespace std;



string reserved[] = { "END_OF_FILE",
		"PUBLIC", "PRIVATE",
		"EQUAL", "COLON", "COMMA", "SEMICOLON",
		"LBRACE", "RBRACE", "ID", "ERROR"
};

#define KEYWORDS_COUNT 2
string keyword[] = {"public", "private" };

void Token::Print()
{
    cout << "{" << this->lexeme << " , "
         << reserved[(int) this->token_type] << " , "
         << this->line_no << "}\n";
}
LexicalAnalyzer::LexicalAnalyzer()
{
    this->line_no = 1;
    tmp.lexeme = "";
    tmp.line_no = 1;
    tmp.token_type = ERROR;
}
bool LexicalAnalyzer::SkipSpace()
{
    char c;
    bool space_encountered = false;

    input.GetChar(c);
    line_no += (c == '\n');

    while (!input.EndOfInput() && isspace(c)) {
        space_encountered = true;
        input.GetChar(c);
        line_no += (c == '\n');
    }

    if (!input.EndOfInput()) {
        input.UngetChar(c);
    }
    return space_encountered;
}
bool LexicalAnalyzer::IsKeyword(string s)
{
    for (int i = 0; i < KEYWORDS_COUNT; i++) {
        if (s == keyword[i]) {
            return true;
        }
    }
    return false;
}
TokenType LexicalAnalyzer::FindKeywordIndex(string s)
{
    for (int i = 0; i < KEYWORDS_COUNT; i++) {
        if (s == keyword[i]) {
            return (TokenType) (i + 1);
        }
    }
    return ERROR;
}

Token LexicalAnalyzer::ScanNumber()
{
    char c, x,y,z,q,p;

    input.GetChar(c);
    if (isdigit(c)) {
        if (c == '0') {
            tmp.lexeme = "0";
        } else {
            tmp.lexeme = "";
            while (!input.EndOfInput() && isdigit(c)) {
                tmp.lexeme += c;
                input.GetChar(c);
            }
            if (!input.EndOfInput()) {
                input.UngetChar(c);
            }
        }

		tmp.token_type = NUM;
        tmp.line_no = line_no;
        return tmp;
    } else {
        if (!input.EndOfInput()) {
            input.UngetChar(c);
        }
        tmp.lexeme = "";
        tmp.token_type = ERROR;
        tmp.line_no = line_no;
        return tmp;
    }
}

Token LexicalAnalyzer::ScanIdOrKeyword()
{
    char c;
    input.GetChar(c);

    if (isalpha(c)) {
        tmp.lexeme = "";
        while (!input.EndOfInput() && isalnum(c)) {
            tmp.lexeme += c;
            input.GetChar(c);
        }
        if (!input.EndOfInput()) {
            input.UngetChar(c);
        }
        tmp.line_no = line_no;
        if (IsKeyword(tmp.lexeme))
            tmp.token_type = FindKeywordIndex(tmp.lexeme);
        else
            tmp.token_type = ID;
    } else {
        if (!input.EndOfInput()) {
            input.UngetChar(c);
        }
        tmp.lexeme = "";
        tmp.token_type = ERROR;
    }
    return tmp;
}
TokenType LexicalAnalyzer::UngetToken(Token tok)
{
    tokens.push_back(tok);;
    return tok.token_type;
}

Token LexicalAnalyzer::SkipComment()
{
    char c;

    input.GetChar(c);
    while (!input.EndOfInput() && c != '\n') {
        input.GetChar(c);

    }
    line_no++;

    if (!input.EndOfInput()) {
        input.UngetChar(c);
    }
    return GetToken();
}

Token LexicalAnalyzer::GetToken()
{
    char c,d,e;

    if (!tokens.empty()) {
        tmp = tokens.back();
        tokens.pop_back();
        return tmp;
    }

    SkipSpace();
    tmp.lexeme = "";
    tmp.line_no = line_no;
    input.GetChar(c);
	//cout << "GETTOKEN: c:" << c << endl;


	switch (c) {
	case '/':
        char a;
        input.GetChar(a);
         if (a == '/') {
             return SkipComment();
        } else {
             input.UngetChar(a);
            }
	case '=':
		tmp.token_type = EQUAL;
		return tmp;
	case ':':
		tmp.token_type = COLON;
		return tmp;
	case ',':
		tmp.token_type = COMMA;
		return tmp;
	case ';':
		tmp.token_type = SEMICOLON;
		return tmp;
	case '[':
		tmp.token_type = LBRAC;
		return tmp;
	case ']':
		tmp.token_type = RBRAC;
		return tmp;
	case '{':
		tmp.token_type = LBRACE;
		return tmp;
	case '}':
		tmp.token_type = RBRACE;
		return tmp;
	default:
		if (isdigit(c)) {
			//Find what type of digit
			input.UngetChar(c);
			return ScanNumber();
		}
		else if (isalpha(c)) {
			input.UngetChar(c);
			return ScanIdOrKeyword();
		}
		else if (input.EndOfInput())
			tmp.token_type = END_OF_FILE;
		else
			tmp.token_type = ERROR;

		return tmp;
	}
}


struct sTableItem {
	char* name;
	char* scope;
	int permission;
};
struct sTable {
	sTableItem* item;
	sTable* prev;
	sTable* next;
};
struct store {
	string lhs;
	string rhs;
	char* scope1;
	char* scope2;
};


//Globals
struct sTable* SymbolTablePointer;
struct sTable* SymbolTableTemp;
struct sTable* SymbolTableNew;
char* sScope = "::";
int SymbolPermission = 0;

void MakeAssignment(Token tok1, Token tok2) {
	struct sTable* temp = SymbolTableNew;
	struct sTable* temp1 = SymbolTableNew;
	char* scope1 = (char*)malloc(sizeof(char) * 50);
	char* scope2 = (char*)malloc(sizeof(char) * 50);

	while (temp != NULL) {//Scope not null
		if (temp->item->name == tok1.lexeme) {//Find the correct upper level Scope item
			if (temp->item->permission == 2 && sScope != temp->item->scope) {
				temp = temp->prev;
				continue;
			}
			else {
				scope1 = temp->item->scope;//Set LHS Scope 
				break;
			}
		}
		temp = temp->prev;
	}
	while (temp1 != NULL) {//Scope not null
		if (temp1->item->name == tok2.lexeme) {//Find the correct upper level Scope item
			if (temp1->item->permission == 2 && sScope != temp1->item->scope) {
				temp1 = temp1->prev;
				continue;
			}
			else {
				scope2 = temp1->item->scope;//Set RHS Scope 
				break;
			}
		}
		temp1 = temp1->prev;
	}
	if (strlen(scope1) == 0) {
		scope1 = new char[2];
		scope1[0] = '?';
		scope1[1] = '\0';
	}
	if (strlen(scope2) == 0) {
		scope2 = new char[2];
		scope2[0] = '?';
		scope2[1] = '\0';
	}
	struct store temp_node;
	temp_node.lhs = tok1.lexeme;
	temp_node.rhs = tok2.lexeme;
	temp_node.scope1 = scope1;
	temp_node.scope2 = scope2;

	cout << temp_node.scope1;
	if (temp_node.scope1 != "::")
		cout << ".";
	cout << temp_node.lhs << " = " << temp_node.scope2;
	if (temp_node.scope2 != "::")
		cout << ".";
	cout << temp_node.rhs << endl;


}

void DeleteScope(char* lexeme) {
	if (SymbolTablePointer == NULL) {
		return;
	}
	while (SymbolTableNew->item->scope == lexeme) {
		SymbolTableTemp->next = NULL;
		SymbolTableNew->prev = NULL;
		if (SymbolTableTemp->prev != NULL) {
			SymbolTableTemp = SymbolTableTemp->prev;
			SymbolTableNew = SymbolTableTemp->next;
			SymbolTableNew->next = NULL;
			return;
		}
		else if (SymbolTableTemp == SymbolTableNew) {
			SymbolTableTemp = NULL;
			SymbolTableNew = NULL;
			return;
		}
		else {
			SymbolTableTemp->next = NULL;
			SymbolTableNew->prev = NULL;
			SymbolTableNew = SymbolTableTemp;
			SymbolTableTemp->next = NULL;
		}
	}
	sScope = SymbolTableNew->item->scope;
}
void AddScope(char* lexeme) {
	if (SymbolTablePointer == NULL) {
		SymbolTablePointer = (sTable*)malloc(sizeof(sTable));
		struct sTableItem* newItem = (sTableItem*)malloc(sizeof(sTableItem));
		SymbolTablePointer->item = newItem;
		SymbolTablePointer->next = NULL;
		SymbolTablePointer->prev = NULL;

		int len = strlen(lexeme);
		SymbolTablePointer->item->name = new char[len + 1];
		strcpy(SymbolTablePointer->item->name, lexeme);
		SymbolTablePointer->item->name[len] = '\0';
		SymbolTablePointer->item->scope = sScope;
		SymbolTablePointer->item->permission = SymbolPermission;

		SymbolTableNew = SymbolTablePointer;
		SymbolTableTemp = SymbolTablePointer;
	}
	else {
		SymbolTableTemp = SymbolTablePointer;
		while (SymbolTableTemp->next != NULL) {
			SymbolTableTemp = SymbolTableTemp->next;
		}
		SymbolTableNew = (sTable*)malloc(sizeof(sTable));
		struct sTableItem* newItem = (sTableItem*)malloc(sizeof(sTableItem));
		SymbolTableNew->item = newItem;
		SymbolTableNew->next = NULL;
		SymbolTableNew->prev = SymbolTableTemp;
		SymbolTableTemp->next = SymbolTableNew;
		int len = strlen(lexeme);
		SymbolTableNew->item->name = new char[len + 1];
		strcpy(SymbolTableNew->item->name, lexeme);
		SymbolTableNew->item->name[len] = '\0';
		SymbolTableNew->item->scope = sScope;
		SymbolTableNew->item->permission = SymbolPermission;
	}
}

LexicalAnalyzer lexer;
Token token1, token2, token3, token4;


void syntax_error() {
	cout << "Syntax Error" << endl;
	exit(1);
}
int LexicalAnalyzer::Parse_Program() {
	token1 = lexer.GetToken();
	if (token1.token_type != END_OF_FILE) {
		lexer.UngetToken(token1);
		Parse_Global_Vars();
		Parse_Scope();
	}
	return 0;
}
int LexicalAnalyzer::Parse_Global_Vars() {
	token1 = lexer.GetToken();
	token2 = lexer.GetToken();

	if (token1.token_type == ID && (token2.token_type == SEMICOLON || token2.token_type == COMMA)) {
		lexer.UngetToken(token2);
		lexer.UngetToken(token1);
		Parse_Var_List();
		token3 = lexer.GetToken();//Semicolon
	}
	else {//global_vars --> epsilon
		lexer.UngetToken(token2);
		lexer.UngetToken(token1);
	}

	return 0;
}
int LexicalAnalyzer::Parse_Var_List() {
	token1 = lexer.GetToken();//ID
	token2 = lexer.GetToken();//COMMA?
		
	if (token1.token_type == ID) {

		char* lexeme = (char*)malloc(sizeof(token1.lexeme));
		memcpy(lexeme, (token1.lexeme).c_str(), sizeof(token1));
		AddScope(lexeme);
		sTable* temp1 = SymbolTablePointer;
		while (temp1 != NULL) {
			temp1 = temp1->next;
		}

		if (token2.token_type == COMMA) {
			Parse_Var_List();
		}
		else {
			lexer.UngetToken(token2);
		}
	}
	else {
		syntax_error();
	}
	return 0;
}
int LexicalAnalyzer::Parse_Scope() {
	token1 = lexer.GetToken();//ID
	token2 = lexer.GetToken();//LBRACE
	if(token1.token_type == ID && token2.token_type == LBRACE){
		string slexeme = token1.lexeme;
		sScope = const_cast<char*>(slexeme.c_str());//New scope with name of ID
		Parse_Public_Vars();
		Parse_Private_Vars();
		Parse_Stmt_List();
		token3 = lexer.GetToken();
		if (token3.token_type == RBRACE) {
			DeleteScope(sScope);//Close current scope
			token4 = lexer.GetToken();
			if (token4.token_type == END_OF_FILE) {
				DeleteScope(sScope);//Close Global Scope
			}
			else {
				UngetToken(token4);
			}
		}
		else {
			syntax_error();
		}
	}
	else {
		syntax_error();
	}
	return 0;
}
int LexicalAnalyzer::Parse_Public_Vars() {
	token1 = lexer.GetToken();//PUBLIC, PRIVATE, ID
	if (token1.lexeme.compare("public")==0) {//IF PUBLIC keyword
		SymbolPermission = 1;
		token2 = lexer.GetToken();//COLON
		if (token2.token_type != COLON) syntax_error();
		Parse_Var_List();
		token2 = lexer.GetToken();//SEMICOLON
		if (token2.token_type != SEMICOLON) syntax_error();
	}
	else {
		lexer.UngetToken(token1);
	}
	return 0;
}
int LexicalAnalyzer::Parse_Private_Vars() {
	token1 = lexer.GetToken();//PRIVATE, ID
	if (token1.lexeme.compare("private") == 0) {
		SymbolPermission = 2;
		token2 = lexer.GetToken();//COLON
		if (token2.token_type != COLON) syntax_error();
		Parse_Var_List();
		token2 = lexer.GetToken();//SEMICOLON
		if (token2.token_type != SEMICOLON) syntax_error();
	}
	else {
		lexer.UngetToken(token1);
	}
	return 0;
}
int LexicalAnalyzer::Parse_Stmt_List() {
	Parse_Stmt();

	token1 = lexer.GetToken();
	if (token1.token_type == RBRACE) {//stmt_list FOLLOW set
		lexer.UngetToken(token1);
	}
	else {
		lexer.UngetToken(token1);
		Parse_Stmt_List();
	}
	return 0;
}
int LexicalAnalyzer::Parse_Stmt() {
	token1 = lexer.GetToken();
	token2 = lexer.GetToken();

	if (token2.token_type == LBRACE) {
		sScope = const_cast<char*>((token1.lexeme).c_str());//New Scope with ID of token1
		lexer.UngetToken(token2);
		lexer.UngetToken(token1);
		Parse_Scope();
	}
	else {
		token3 = lexer.GetToken();
		token4 = lexer.GetToken();
		if (token1.token_type == ID && token2.token_type == EQUAL &&
			token3.token_type == ID && token4.token_type == SEMICOLON) {
			MakeAssignment(token1, token3);
		}
		else {
			syntax_error();
		}
	}
	return 0;
}

int main()
{

	int a = lexer.Parse_Program();
}
