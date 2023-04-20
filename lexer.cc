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
    char c;

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



Token LexicalAnalyzer::GetToken1()
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
		{
		//	input.UngetChar(c);
			return tmp;
		}
	case ';':
		tmp.token_type = SEMICOLON;
		{
			//input.UngetChar(c);
			return tmp;
		}
	case '[':
		tmp.token_type = LBRAC;
		return tmp;
	case ']':
		tmp.token_type = RBRAC;
		return tmp;
	case '{':
		tmp.token_type = LBRACE;
		{
			tmp.lexeme = "{";
			return tmp;
		}

	case '}':
		tmp.token_type = RBRACE;
		{
			tmp.lexeme = "}";
			return tmp;
		}
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
	string scope1;
	string scope2;
};


//Globals
vector< struct store> storeList;//List of all Assignments made from MakeAssignment()
struct sTable* SymbolTablePointer;
struct sTable* SymbolTableTemp;
struct sTable* SymbolTableNew;
char* sScope = "::";
int SymbolPermission = 0;


void MakeAssignment(Token tok1, Token tok2) {
	struct sTable* temp = SymbolTableNew;
	struct sTable* temp1 = SymbolTableNew;
	string scope1, scope2;

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
	if (scope1.length() == 0) {
		scope1 = new char[2];
		scope1 = '?';
	}
	if (scope2.length() == 0) {
		scope2 = new char[2];
		scope2 = '?';
	}
	struct store temp_node;
	temp_node.lhs = tok1.lexeme;
	temp_node.rhs = tok2.lexeme;
	temp_node.scope1 = scope1;
	temp_node.scope2 = scope2;
	storeList.push_back(temp_node);//Add to the store of assignments to print

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
Token token1, token2, token3, token4, token5, token6;
string braces;


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

	if (token2.token_type == LBRACE)
		braces += "{";

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
		braces += "{";
		string slexeme = token1.lexeme;
		sScope = const_cast<char*>(slexeme.c_str());//New scope with name of ID
		Parse_Public_Vars();
		Parse_Private_Vars();
		Parse_Stmt_List();
		token3 = lexer.GetToken();
		if (token3.token_type == RBRACE) {

			braces += "}";
			DeleteScope(sScope);//Close current scope
			token4 = lexer.GetToken();
			braces += token4.lexeme;
			if (token4.token_type == END_OF_FILE) {
				DeleteScope(sScope);//Close Global Scope
			}
			else {
				lexer.UngetToken(token4);
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
		braces += "}";
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

//string read_input_to_string() {
//    InputBuffer input_buffer;
//    string input_string;
//    char c;
//
//    while (!input_buffer.EndOfInput()) {
//    	input_buffer.GetChar(c);
//        input_string.push_back(c);
//
//    }
//
//    cout << input_string << endl;
//
//    return input_string;
//}

bool is_syntax_error(std::string input_string) {
    // Find the position of the last closing brace
    size_t last_brace_position = input_string.rfind("}");

    // If there is no closing brace in the input string, return false
    if (last_brace_position == std::string::npos) {
        return false;
    }

    // Check if there is any code after the last closing brace
    std::string code_after_last_brace = input_string.substr(last_brace_position + 1);
    size_t non_space_position = code_after_last_brace.find_first_not_of(" \t\n\r\f\v");

    // If there is no code after the last closing brace, return false
    if (non_space_position == std::string::npos) {
        return false;
    }

    // If there is code after the last closing brace, return true
    return true;
}

int main() //     END_OF_FILE
{

	lexer.Parse_Program();

	string globalScope = "::";
	LexicalAnalyzer lexer1;
	string input_string;
	char c;

//	while (lexer1.input.EndOfInput() ) {
//		 input_string += lexer1.GetToken1().lexeme;
//
//	}

//	while (!lexer1.input.EndOfInput() && !cin.eof() )
//	{
//		lexer1.input.GetChar(c);
//		input_string.push_back(c);
//
//	}

	//cout << input_string <<endl;

	if(is_syntax_error(braces) == true)
	{
		syntax_error();

	}


	for (int i = 0; i < storeList.size(); i++)
	{
			cout << storeList[i].scope1;
			if (storeList[i].scope1 != globalScope)
				cout << ".";
			cout << storeList[i].lhs << " = " << storeList[i].scope2;
			if (storeList[i].scope2 != globalScope)
				cout << ".";
			cout << storeList[i].rhs << endl;
	}



}
