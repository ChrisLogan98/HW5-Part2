//Chris Logan & Matthew Manuel
//CSC 306
//3/24/2020
//Main file for HW5
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>
using namespace std;

class SyntaxAnalyzer{
    private:
        vector<string> lexemes;
        vector<string> tokens;
        vector<string>::iterator lexitr;
        vector<string>::iterator tokitr;

        // map of variables to datatype (i.e. sum t_integer)
        // sets this up for stage 3
        map<string, string> symboltable;

        // other private methods
        bool vdec();
        int vars();
        bool stmtlist();
        int stmt();
        bool ifstmt();
        bool elsepart();
        bool whilestmt();
        bool assignstmt();
        bool inputstmt();
        bool outputstmt();
        bool expr();
        bool simpleexpr();
        bool term();
        bool logicop();
        bool arithop();
        bool relop();
        bool haveVariable(); // Written by Matthew Manuel. Called by Chris Logan
        std::istream& getline_safe(std::istream& input, std::string& output);
    public:
        SyntaxAnalyzer(istream& infile);
        // pre: 1st parameter consists of an open file containing a source code's
        //	valid scanner output.  This data must be in the form
        //			token : lexeme
        // post: the vectors have been populated

        bool parse();
        // pre: none
        // post: The lexemes/tokens have been parsed and the symboltable created.
        // If an error occurs, a message prints indicating the token/lexeme pair
        // that caused the error.  If no error occurs, the symboltable contains all
        // variables and datatypes.
};
SyntaxAnalyzer::SyntaxAnalyzer(istream& infile){
    string line, tok, lex;
    int pos;
    getline_safe(infile, line);
    bool valid = true;
    while(!infile.eof() && valid){
    	pos = line.find(":");
    	tok = line.substr(0, pos-1);
    	lex = line.substr(pos+2, line.length());
    	cout << pos << " " << tok << " " << lex << endl;
    	tokens.push_back(tok);
    	lexemes.push_back(lex);
    	getline_safe(infile, line);
    }
    tokitr = tokens.begin();
    lexitr = lexemes.begin();
}

bool SyntaxAnalyzer::parse(){
    if (vdec()){
        if (*tokitr == "t_begin"){
            tokitr++;
            lexitr++;
            if (tokitr!=tokens.end() && stmtlist()){
            	cout << *tokitr << endl;
                if (tokitr!=tokens.end())
                	if (*tokitr == "t_end"){
                		tokitr++; lexitr++;
                		if (tokitr==tokens.end()){
                			cout << "Valid source code file" << endl;
                			return true;
                		}
                		else{
                			cout << "end came too early" << endl;
                		}
                	}
                	else{
                		cout << "invalid statement ending code" << endl;
                }
                else{
                	cout << "no end" << endl;
                }
            }
            else{
            	cout << "bad/no stmtlist or no end" << endl;
            }
        }
        else{
        	cout << "no begin" << endl;
        }
    }
    else{
    	cout << "bad var list" << endl;
    }
    return false;
}

bool SyntaxAnalyzer::vdec(){

    if (*tokitr != "t_var")
		if(*tokitr == "t_begin") return true;
		else return false; // Matthew Manuel: Incomplete var list
    else{
        tokitr++; lexitr++;
        int result = 0;   // 0 - valid, 1 - done, 2 - error
        result = vars();
        if (result == 2)
            return false;
        while (result == 0){
            if (tokitr!=tokens.end())
                result = vars(); // parse vars
        }

        if (result == 1)
            return true;
        else
            return false;
    }
}

int SyntaxAnalyzer::vars(){
    int result = 0;  // 0 - valid, 1 - done, 2 - error
    string temp;
    if (*tokitr == "t_integer"){
        temp = "t_integer";
        tokitr++; lexitr++;
    }
    else if (*tokitr == "t_string"){
        temp = "t_string";
        tokitr++; lexitr++;
    }
    else // End of var declarations
        return 1;
    bool semihit = false;
    while (tokitr != tokens.end() && result == 0 && !semihit){
        if (*tokitr == "t_id"){
            symboltable[*lexitr] = temp;
            tokitr++; lexitr++;
            if (tokitr != tokens.end() && *tokitr == "s_comma"){
                tokitr++; lexitr++;
            }
            else if (tokitr != tokens.end() && *tokitr == "s_semi"){
                semihit = true;
                tokitr++; lexitr++;
            }
            else
                result = 2;
        }
        else{
            result = 2;
        }
    }
    return result;
}

bool SyntaxAnalyzer::stmtlist(){
    int result = stmt();
    while (result == 1){
        result = stmt();
    }
    if (result == 0)
        return false;
    else
        return true;
}

int SyntaxAnalyzer::stmt(){
	if (*tokitr == "t_if"){
        tokitr++; lexitr++;
        if (ifstmt()) return 1;
        else return 0;
    }
    else if (*tokitr == "t_while"){
        tokitr++; lexitr++;
        if (whilestmt()) return 1;
        else return 0;
    }
    else if (*tokitr == "t_id"){  // assignment starts with identifier
        if(!haveVariable()) {return 0;}
        tokitr++; lexitr++;
        cout << "t_id" << endl;
        if (assignstmt()) return 1;
        else return 0;
    }
    else if (*tokitr == "t_input"){
        if(!haveVariable()){return 0;}
        tokitr++; lexitr++;
        if (inputstmt()) return 1;
        else return 0;
    }
    else if (*tokitr == "t_output"){
        tokitr++; lexitr++;
        cout << "t_output" << endl;
        if (outputstmt()) return 1;
        else return 0;
    }
    return 2;  //stmtlist can be null
}

bool SyntaxAnalyzer::ifstmt(){ //written in class
	if(*tokitr == "s_lparen"){
		tokitr++; lexitr++;
		if(expr()){
			if(*tokitr == "s_rparen"){
				tokitr++; lexitr++;
				if(*tokitr == "t_then"){
					tokitr++; lexitr++;
					if(stmtlist()){
						if(elsepart()){
							if(*tokitr == "t_end"){
								tokitr++; lexitr++;
								if(*tokitr == "t_if"){
									tokitr++; lexitr++;
									return true;
								}
							}
						}
					}
				}
			}
		}
	}
	return false;
}

bool SyntaxAnalyzer::elsepart(){
    if (*tokitr == "t_else"){
        tokitr++; lexitr++;
        if (stmtlist())
            return true;
        else
            return false;
    }
    return true;   // elsepart can be null
}

//Written by Chris Logan
bool SyntaxAnalyzer::whilestmt(){
    if(*tokitr == "s_lparen"){ //encloses EXPR
        tokitr++; lexitr++;
        if(expr()){ //conditions for while
            if(*tokitr == "s_rparen"){ //encloses EXPR
                tokitr++; lexitr++;
                if(*tokitr == "t_loop"){ //begin loop
                    tokitr++; lexitr++;
                    if(stmtlist()){
                        if(*tokitr == "t_end"){ //end statement
                            tokitr++; lexitr++;
                            if(*tokitr == "t_loop"){ //end loop
                                tokitr++; lexitr++;
                                return true;
                            }
                        }
                    }
                }
            }
        }
    }
    return false;
}

// Written by Matthew Manuel
bool SyntaxAnalyzer::assignstmt(){
	if(*tokitr == "s_assign"){
		tokitr++; lexitr++;
		if(expr()){
			if(*tokitr == "s_semi"){
				tokitr++; lexitr++;
				return true;
			}
		}
	}
	return false;
}

bool SyntaxAnalyzer::inputstmt(){
    if (*tokitr == "s_lparen"){
        tokitr++; lexitr++;
        if (*tokitr == "t_id"){
            tokitr++; lexitr++;
            if (*tokitr == "s_rparen"){
                tokitr++; lexitr++;
                return true;
            }
        }
    }
    return false;
}

// Written by Matthew Manuel
bool SyntaxAnalyzer::outputstmt(){
	if(*tokitr == "s_lparen"){
		tokitr++; lexitr++;
		if(expr()){ 					// Output an expression
			if(*tokitr == "s_rparen"){ // Check if output stmt closed
				tokitr++; lexitr++;
				return true;
			}
		}
		else if(*tokitr == "t_string"){ // Output a string
			tokitr++; lexitr++;
			if(*tokitr == "s_rparen"){ // Check if output stmt closed
				tokitr++; lexitr++;
				return true;
			}
		}
	}
	return false;
}

bool SyntaxAnalyzer::expr(){
    if (simpleexpr()){
		if (logicop()){ //relop was here instead of logicop
			if (simpleexpr())
				return true;
			else
				return false;
		}
		else{ // Only one simpleexpr
			return true;
		}
	}
	else{
		return false;
	}
}
//Written by Chris Logan
bool SyntaxAnalyzer::simpleexpr(){
    if(term()){//begin term
        //checks for multiple instances of numbers or mathematical symbols
        while(arithop() || relop()){ //begin while
            if(!term()){
                return false;
            }
        }//end while
        return true;
    }//end term
    else{
        return false;
    }
    return false;
}

bool SyntaxAnalyzer::term(){
    if ((*tokitr == "t_int")
	|| (*tokitr == "t_str")
	|| (*tokitr == "t_id")){
	    if(*tokitr == "t_id" && !haveVariable()){return false;}
    	tokitr++; lexitr++;
    	return true;
    }
    else{ // Check if expr
        if (*tokitr == "s_lparen"){
            tokitr++; lexitr++;
            if (expr())
                if (*tokitr == "s_rparen"){
                    tokitr++; lexitr++;
                    return true;
                }
        }
    }
    return false;
}

bool SyntaxAnalyzer::logicop(){ //this was not called in EXPR for some reason
    if ((*tokitr == "s_and") || (*tokitr == "s_or")){
        tokitr++; lexitr++;
        return true;
    }
    else
        return false;
}

bool SyntaxAnalyzer::arithop(){
    if ((*tokitr == "s_mult") || (*tokitr == "s_plus") || (*tokitr == "s_minus")
        || (*tokitr == "s_div")	|| (*tokitr == "s_mod")){
        tokitr++; lexitr++;
        return true;
    }
    else
        return false;
}

bool SyntaxAnalyzer::relop(){
    if ((*tokitr == "s_lt") || (*tokitr == "s_gt") || (*tokitr == "s_ge")
        || (*tokitr == "s_eq") || (*tokitr == "s_ne") || (*tokitr == "s_le")){
        tokitr++; lexitr++;
        return true;
    }
    else
    	return false;
}

// Written by Matthew Manuel
// Called by Chris Logan. Collaborated via text on placement.
// Check if variable has been declared and is in the symbol table
bool SyntaxAnalyzer::haveVariable(){
	if(symboltable.count(*lexitr) == 0){
		return false;
	}
	return true;
}

std::istream& SyntaxAnalyzer::getline_safe(std::istream& input, std::string& output)
{
    char c;
    output.clear();

    input.get(c);
    while (input && c != '\n')
    {
        if (c != '\r' || input.peek() != '\n')
        {
            output += c;
        }
        input.get(c);
    }

    return input;
}

int main(){
	cout << "Begin program" << endl;
    ifstream infile("lexemes.txt");
    if(!infile){
        cout << "ERROR - FILE NOT FOUND" << endl;
        cout << "PLEASE CHECK if file is valid" << endl;
    }
    SyntaxAnalyzer sa(infile);
    sa.parse();
    return 1;
}
