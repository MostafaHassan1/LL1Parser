#include<iostream>
#include<fstream>
#include<vector>
#include<string>
#include<set>
#include<stack>
using namespace std;

enum token
{
	PROGRAM_SY, BEGIN_SY, END_SY, PROC_SY, ID_SY, EQUAL_SY, ASSIGMENT_SY, SEMICOMA_SY,
	LEFTPAR_SY, RIGHTPAR_SY, CALL_SY, INT_SY, BOOL_SY, NUM_SY, TRUE_SY, FALSE_SY, NUMERAL_SY,
	ERROR_SY, $_SY, EOF_SY

};
struct table
{
	string nonTerminal;
	set<string> first;
	set<string> follows;

};
struct productionRule {
	string nonTerminal;
	vector<string> rules;
};
struct numberedProdRule {
	int num;
	string nonTerminal;
	string rule;
};
struct singleCol
{
	token Terminal;
	string rule;
};
struct ParsingTable
{
	string nonterminal;
	vector<singleCol> colomn;
};
//variables
set <string> terminal;
vector<productionRule> grammerRules;
int pos;
vector<table> firstFollow;//will holds the nonterminal/first/follows values 
vector<numberedProdRule> prodWithNumber;
vector<ParsingTable> tableVector;
token CT;
stack<string> s;

///////////////////////////////////////////////////////////////////////////
/////////////////////reading the grammer functions ////////////////////////
void syntaxError() {
	cout << "Syntax Error" << endl;
	system("pause");
	exit(1);
}
string findNonTerm(string line) {
	char ch;
	string conc;
	pos = 0;

	while (line.at(pos) != '>')
	{
		conc += line.at(pos);
		pos++;
	}
	conc += line.at(pos);
	pos += 5;
	return conc;
}
vector<string> findRules(string line) { //return the rules of the input nonterminal
	vector <string> rule;
	string conc;
	for (; pos < line.length(); pos++)
	{
		if (pos + 1<line.length() && line.at(pos + 1) == '|') //+1 for the space //first cond so we don't have a pos+1 bigger than the line
		{
			rule.push_back(conc);
			pos++;
			conc = "";
		}
		else {
			if (!(conc.size() == 0 && isspace(line.at(pos)))) //if size on conc = 0 and the letter is space then don't add it //i.e don't add a space into the first letter
				conc += line.at(pos);
		}
	}
	rule.push_back(conc); //gets the last rule
	return rule;
}
void findTerm() {
	string rule, conc;
	vector<productionRule> temp = grammerRules;
	for (int i = 0; i < temp.size(); i++)
	{
		for (int k = 0; k < temp[i].rules.size(); k++)
		{ //loops through rule rule
			rule = temp[i].rules[k];
			for (int j = 0; j < rule.length(); j++)
			{
				if (rule.at(j) == '<')
				{
					if (!isalpha(rule.at(j + 1))) //then it's <> | <= | <
					{
						conc += rule.at(j) + rule.at(j + 1);
						if (conc == " ") //to remove the conc if it equals to space only
						{
							conc = "";
						}
						else {
							terminal.insert(conc);
							conc = "";
						}
					}
					else { //it's a nonterminal then ignore it
						while (rule.at(j) != '>') j++;
						j += 1;
						if (j >= rule.length()) break;
					}
				}
				else if (isspace(rule.at(j)))
				{
					if (conc.size() != 0) { //handels a case where it still save a space (empty object)
						if (isspace(conc.at(0)))
						{
							conc = "";
						}
						else {
							terminal.insert(conc);
							conc = "";
						}
					}

				}
				else
				{
					if (!isspace(rule.at(j)))
						conc += rule.at(j);
					if (j + 1 >= rule.length()) //end of line
					{
						terminal.insert(conc);
						conc = "";
					}
				}
			}
		}
	}//end main loop
}
void readFile(fstream &file) {
	string line;
	while (!file.eof())
	{
		productionRule singleProd;
		getline(file, line);
		singleProd.nonTerminal = findNonTerm(line);
		singleProd.rules = findRules(line);
		/////////////we got the nonterminal && its rules 
		grammerRules.push_back(singleProd);
	}
	findTerm();
}
void showProd() {
	for (int i = 0; i < grammerRules.size(); i++)
	{
		cout << "nonTerminal: " << grammerRules[i].nonTerminal << " \t Rules: ";
		for (int k = 0; k < grammerRules[i].rules.size(); k++)
		{
			cout << grammerRules[i].rules[k];
			if (k + 1 < grammerRules[i].rules.size()) cout << " | "; //just so it doesn't add | at the end 
		}
		cout << endl;
	}

}
bool isTerminal(string check) {
	bool flag = false;
	set<string> temp = terminal;
	/*for (int i = 0; i < temp.size(); i++) {
	if (temp[i] == check) flag = true;
	}*/
	for (auto singleTerm : temp) { //like a for each
		if (singleTerm == check) flag = true;
	}
	return flag;
}
//////////////////////////////Scanner////////////////////////////////////////
class Scanner {
public:
	token check_reserved(string word) {
		if (word == "program") return PROGRAM_SY;
		else if (word == "begin") return BEGIN_SY;
		else if (word == "end") return END_SY;
		else if (word == "int") return INT_SY;
		else if (word == "bool") return BOOL_SY;
		else if (word == "true") return TRUE_SY;
		else if (word == "false") return FALSE_SY;
		else if (word == "call") return CALL_SY;
		else if (word == "proc") return PROC_SY;
		else return ID_SY;
	}
	token get_token() {
		string stringHolder = "";
		char ch;
		file.get(ch);
		while (!file.eof() && isspace(ch)) {
			file.get(ch);
		}

		if (file.eof()) return EOF_SY;
		else if (ch == '$') return $_SY;
		else if (ch == ':') {
			file.get(ch);
			if (ch == '=') return ASSIGMENT_SY;
		}
		else if (ch == ';') return SEMICOMA_SY;
		else if (ch == '(') return LEFTPAR_SY;
		else if (ch == ')') return RIGHTPAR_SY;
		else if (ch == '=') return EQUAL_SY;
		else if (isdigit(ch)) { //the start if a number
			file.get(ch);
			if (isspace(ch)) return NUMERAL_SY; //the number ended
			while (!file.eof() && isdigit(ch)) //loops till the end of that number
			{
				file.get(ch);
			}
			file.putback(ch); //to leave the extra char that made us leave the loop
			return NUMERAL_SY; //it didn't find any char inside the number then i's not an error		
		}
		else if (isalpha(ch)) { //the start of ident
			stringHolder = ch;
			file.get(ch);
			if (isspace(ch)) return ID_SY;//it's only 1 char so we know it's a ident and not a reserved word so we don't nedd to check
			while (!file.eof() && isalnum(ch)) //loops till the end of that string
			{
				stringHolder += ch;
				file.get(ch);
			}
			file.putback(ch);; //to leave the extra char that made us leave the loop
			return check_reserved(stringHolder);
		}

	}
	void match(token t) {
		if (CT == t)
			CT = get_token();
		else
			syntaxError();
	}
	token getCrosToken(string x) {
		if (x == "$") return $_SY;
		else if (x == ":=") return ASSIGMENT_SY;
		else if (x == ";") return SEMICOMA_SY;
		else if (x == "(") return LEFTPAR_SY;
		else if (x == ")") return RIGHTPAR_SY;
		else if (x == "=") return EQUAL_SY;
		else if (x == "num")return NUMERAL_SY; //the number ended
		else return check_reserved(x);
	}
	Scanner(string filename)
	{
		file.open(filename);
		if (!file)
		{
			cerr << "Unable to open file" << endl;
			system("pause");
			exit(1);
		}
	}
private:ifstream file;
};
/////////////////////LLpareser////////////////////////////////////////////

///////////////////////// helpers functions///////////////////////////////
string getFirstWord(string rule) {
	string word;
	for (int i = 0; i < rule.length(); i++)
	{
		if (isspace(rule.at(i))) break;
		else word += rule.at(i);
	}
	return word;
}
int find(string main, string sub) {
	string concat;
	for (int i = 0; i < main.length(); i++)
	{
		if (isspace(main.at(i))) {
			if (concat == sub)
				return i + 1;//position to the first letter after the word
			else concat = ""; //remove the word
		}
		else concat += main.at(i);
	}
	if (concat == sub)
		return main.length() + 1;//means it found it at the last postion
	return -1; //means he didn't find it
}
set<string> existFollow(string x) {
	for (int i = 0; i < firstFollow.size(); i++)
	{
		if (firstFollow[i].nonTerminal == x)
			return firstFollow[i].follows;
	}
	set<string> empty;
	return empty;
}
set<string> existFirst(string x) {
	for (int i = 0; i < firstFollow.size(); i++)
	{
		if (firstFollow[i].nonTerminal == x)
			return firstFollow[i].first;
	}
	set<string> empty;
	return empty;
}
token returnTokenNum(string word) {//does the same thing as getCrosToken 
	if (word == "program") return PROGRAM_SY;
	else if (word == "begin") return BEGIN_SY;
	else if (word == "end") return END_SY;
	else if (word == "=") return EQUAL_SY;
	else if (word == ":=") return ASSIGMENT_SY;
	else if (word == ";") return SEMICOMA_SY;
	else if (word == "(") return LEFTPAR_SY;
	else if (word == ")") return RIGHTPAR_SY;
	else if (word == "$") return $_SY;
	else if (word == "int") return INT_SY;
	else if (word == "bool") return BOOL_SY;
	else if (word == "true") return TRUE_SY;
	else if (word == "false") return FALSE_SY;
	else if (word == "call") return CALL_SY;
	else if (word == "proc") return PROC_SY;
	else if (word == "num") return NUMERAL_SY; //it's a number 
	else return ID_SY;
}
vector<string> wordsVector(string rule) {
	string concat;
	vector<string> returnVec;
	for (int i = 0; i <rule.length(); i++) {
		if (isspace(rule.at(i))) {
			if (concat != "") {
				returnVec.push_back(concat);
				concat = "";
			}
		}
		else concat += rule.at(i);
	}
	if (concat != "")  //gets frist word (last in reverse)
		returnVec.push_back(concat);

	return returnVec;

}
int rowExist(string nonTerminal) {//return the index of the nonterminal in the table or not found -1
	for (int i = 0; i < tableVector.size(); i++)
	{
		if (tableVector[i].nonterminal == nonTerminal)
			return i;
	}
	return -1;//didn't find it
}
//////////////////////////////////////////////////////////////////////////
vector<string> getRules(string nonTerminalHolder) {
	vector<productionRule> temp = grammerRules;
	for (int i = temp.size() - 1; i >= 0; i--)
	{
		if (temp[i].nonTerminal == nonTerminalHolder) {
			return temp[i].rules;

		}
	}
}
set<string> First(string x) {
	set<string> firstValue;
	bool flag = false;
	string firstWord;
	firstWord = getFirstWord(x);
	//set<string>existfirst = existFirst(firstWord);
	//if (!existfirst.empty())//so it doesnt compute it again
	//	return existfirst;
	if (isTerminal(firstWord)) {
		firstValue.insert(firstWord);
		return firstValue;
	}
	else {

		set<string> ruleTemp;
		vector<string>rulesHolder = getRules(firstWord);
		for (int i = 0; i < rulesHolder.size(); i++)
		{
			flag = false;//intilize
			vector<string> words = wordsVector(rulesHolder[i]);
			for (int k = 0; k < words.size(); k++)
			{
				if (words[k] == x) {//means we are trying to get the first of x in the same time twice //makes infinite loop
					flag = false;//breaks the outer loop
					break;//breaks the inner loop
				}
				ruleTemp = First(words[k]); //gets a set of first to thw words[k]

				for (auto singleFirstEle : ruleTemp) {
					if (singleFirstEle == "#")
						flag = true; //found epslon we need to get the first of the next words
					firstValue.insert(singleFirstEle);//put a single rule into the main set
				}
				if (!flag)//didn't find epslon then we don't need to get the first of the next word
					break;
			}


		}//end for
	}//end else 
	return firstValue;
}
set<string> RulesAfterX(string x) {
	//we need to loop over every rule in the grammer 
	//check if the nonterminal X is in it 
	//get the nonterminals after X OR the TERMINAL after X
	set<string> rulesAfterX;
	string concat;
	bool flag = false;

	for (int i = 0; i < grammerRules.size(); i++)
	{
		for (int k = 0; k < grammerRules[i].rules.size(); k++) //loops through rule rule
		{
			string ruleHolder = grammerRules[i].rules[k];
			concat = "";//initlize it
			if (find(ruleHolder, x) != -1) //means if the rule contains x then true
			{
				if (find(ruleHolder, x) + 1 > ruleHolder.length())//x is the last word 
				{//then after it is the left prod nonterminal
					rulesAfterX.insert("#"); //# will indicate to the Follow to get the Z rules of X 
				}
				for (int y = ruleHolder.find(x) + x.length() + 1; y < ruleHolder.length(); y++) //loops through the char of the rule 
				{//the loop start at the first letter of the next word i.e Y1//Z::= c X Y1 Y2…Yn
					if (isspace(ruleHolder.at(y))) {//we got a full word
						if (isTerminal(concat)) //we have a terminal
						{//then follows should only = that terminal
							rulesAfterX.insert(concat);
							flag = true;
							break; //we got a terminal in this rule so don't need to continue
						}
						else {//a nonTerminal i.e Y1,Y2...
							rulesAfterX.insert(concat);
							concat = "";//inilitze again
						}
					}
					else concat += ruleHolder.at(y);
				}//end single rule loop
			}//end if 
			if (concat != "") rulesAfterX.insert(concat); //insert the last rule
		}//end Rules loop
	}//end grammer for
	return rulesAfterX;
}
set<string> RulesBeforeX(string x) {
	//we need to loop over every rule in the grammer 
	//check if the nonterminal X is in it 
	set<string> rulesBeforeX;
	string concat;
	bool haveEps = false;

	for (int i = 0; i < grammerRules.size(); i++)
	{
		for (int k = 0; k < grammerRules[i].rules.size(); k++) //loops through rule rule
		{
			string ruleHolder = grammerRules[i].rules[k];
			concat = "";//initlize it
			if (find(ruleHolder, x) != -1) //means if the rule contains x then true
			{
				haveEps = false;//intilize
				if (find(ruleHolder, x) + 1 > ruleHolder.length())//x is the last word 
				{//then after it is the left prod nonterminal
					if(x!=grammerRules[i].nonTerminal)
					rulesBeforeX.insert(grammerRules[i].nonTerminal); //insert Z 
				}
				for (int y = ruleHolder.find(x) + x.length() + 1; y < ruleHolder.length(); y++) //loops through the char of the rule after x 
				{//the loop start at the first letter of the next word i.e Y1//Z::= c X Y1 Y2…Yn
					if (isspace(ruleHolder.at(y))) {//we got a full word
						if (isTerminal(concat)) //we have a terminal
						{//then we don't need Z
							haveEps = false;
							break; //we got a terminal in this rule so don't need to continue
						}
						else {//a nonTerminal i.e Y1,Y2...
							haveEps = false;//new rule to get a first for it then init the haveEps for it
							set<string>firstTemp = First(concat);
							for (auto singleFirst : firstTemp) {
								if (singleFirst == "#") 
									haveEps = true;
							}
							concat = "";//inilitze again
						}
					}
					else concat += ruleHolder.at(y);
				}//end single rule loop
				//a single stage for the last wrod in the rule
				if (concat != "") {//if we have a word
					if (isTerminal(concat)) //we have a terminal
					{//then we don't need Z
						continue; //we got a terminal in this rule so don't need to continue
					}
					else {//a nonTerminal i.e Y1,Y2...
						haveEps = false;//new rule to get a first for it then init the haveEps for it
						set<string>firstTemp = First(concat);
						for (auto singleFirst : firstTemp) {
							if (singleFirst == "#")
								haveEps = true;
						}
					}
				}
				if (haveEps) //if it have epslon in the after words then add z
					rulesBeforeX.insert(concat); //insert the last rule
			}//end if 
			
		}//end Rules loop
	}//end grammer for
	return rulesBeforeX;
}
string findZ(string z, string nonTerminal, string rule) {
	bool haveEps = false;
	vector<string>ruleWords = wordsVector(rule);//explode the rules to words
	for (int i = 0; i < ruleWords.size(); i++)
	{
		if (ruleWords[i] == nonTerminal) { //found nonTerminal place 
			if (i + 1 > ruleWords.size()) //if nonTerminal is the last word then return z
				return z;
			else {
				for (; i < ruleWords.size(); i++) //get the words after nonTerminal
				{
					haveEps = false;//intilize it for the new word
					set<string> tempFirst = First(ruleWords[i]);
					for (auto singleFirst : tempFirst) {
						if (singleFirst == "#")
							haveEps = true;
					}
					if (!haveEps) return ""; //it doesn't have eps we don't need to get Z //return empty string;
				}
				if (haveEps)//the last word have eps so we need to get z
					return z;
			}
		}
	}
}
set<string> getZ(string x) {
	set<string> ZbeforeX;
	for (int i = 0; i < grammerRules.size(); i++)
	{
		for (int k = 0; k < grammerRules[i].rules.size(); k++)
		{//loop through rule rule
			string ruleHolder = grammerRules[i].rules[k];
			if (find(ruleHolder, x) >= 0) {//we found the rule 
										   //ZbeforeX.insert(findZ(grammerRules[i].nonTerminal, x, grammerRules[i].rules[k]));
				if (x != grammerRules[i].nonTerminal) { //so we don't add the same rules and enter an infinite loop
					string tempZ = findZ(grammerRules[i].nonTerminal, x, grammerRules[i].rules[k]);
					if (tempZ != "")//not empty
						ZbeforeX.insert(tempZ); //insert Z
				}
			}
		}
	}//end outer for
	return ZbeforeX;
}
//set<string> ZbeforeX(string x) {
//	//we need to get all the rules after X i.e Y1 Y2 Y3...
//	//then check that every one of them is not a terminal // i.e not Y1 t Y2
//	//then we return Z  //z::= Y0 X Y1 y2....
//	set<string>testTerminal = RulesAfterX(x);
//	for (auto test : testTerminal) {//test every rule if it's a terminal
//		if (isTerminal(test)/*&&test!="#"*/) { //there is a terminal so we don't need Z //2nd condtion because # is considred a terminal
//			set<string> empty;
//			return empty;//return empty set
//		}
//	}//end for
//	 //if we reach this block then there isn't a single terminal // we need to get Z 
//	set<string> ZbeforeX = getZ(x);
//	//for (int i = 0; i < grammerRules.size(); i++)
//	//{
//	//	for (int k = 0; k < grammerRules[i].rules.size(); k++)
//	//	{
//	//		string ruleHolder = grammerRules[i].rules[k];
//	//		if (find(ruleHolder, x) >= 0) {//we found the rule 
//	//			if(x!=grammerRules[i].nonTerminal) //so we don't add the same rules and enter an infinite loop
//	//			ZbeforeX.insert(grammerRules[i].nonTerminal); //insert Z 
//	//		}
//	//	}
//	//}//end outer for
//	//return ZbeforeX;
//}
set<string> Follow(string x) {
	set<string> followSet;
	set<string>followExist = existFollow(x);
	if (!followExist.empty()) //so it doesn't compute it again
		return followExist;
	if (x == "<Program>") {
		followSet.insert("$"); //<Program> is the start symbol
		return followSet;
	}
	else {
		bool haveEps = false;
		set<string> rulesAfterX = RulesAfterX(x); //will holds every rule that comes after x
		if (rulesAfterX.empty())
			haveEps = true;//means there is no rules after X so we need to get Z //the same as if it have Epslon after it
		else {
			for (auto singleRule : rulesAfterX) {
				if (!isTerminal(singleRule))  //because it will make haveEps false while it have one without following it with a production rule // # , ) for declarere
					haveEps = false;//intilize it for the new rule
				set<string>temp = First(singleRule); //get the first of Y1 Y2 Y3 etc
				for (auto singleFirst : temp)
				{ //we need to check if it doesn't have Epslon so we stop getting the first of the afterRulesX
					if (singleFirst == "#") haveEps = true;
					followSet.insert(singleFirst);
				}
				if (!haveEps) break; //means that the first rlue after X doesn't have a Epslon so we don't need to get the first of Y2 Y3...
			}
		}
		if (haveEps) { //we don't need to get Z if it doesn't have Epslon
		//	set<string> rulesBeforeX = getZ(x);
			set<string> rulesBeforeX = RulesBeforeX(x);
			for (auto Z : rulesBeforeX) { //for every Z
				set<string>Zrules = Follow(Z);
				for (auto Zrule : Zrules) { //fer every element in the set 
					followSet.insert(Zrule);
				}
			}
		}
	}//end else 
	followSet.erase("#");//remove epslon 
	return followSet;
}
void getForFirstFollow() {
	vector<productionRule> temp = grammerRules;
	table singleRow;
	for (int i = 0; i < temp.size(); i++)//loops through every nonterminal in the grammer to create the table
	{
		singleRow.nonTerminal = temp[i].nonTerminal;
		singleRow.first = First(temp[i].nonTerminal); //sends the nonTerminal to the first function
		singleRow.follows = Follow(temp[i].nonTerminal);
		firstFollow.push_back(singleRow); //a single row in the table
	}

}
void showTable() {
	cout << "NonTerminal \t First \t \t Follow" << endl;
	for (int i = 0; i < firstFollow.size(); i++)
	{
		cout << firstFollow[i].nonTerminal << "  | ";
		for (auto singleFirst : firstFollow[i].first) //access the first set		
			cout << "  " << singleFirst;
		cout << " | ";
		for (auto singleFollow : firstFollow[i].follows) //access the Follows set
			cout << "  " << singleFollow;

		cout << endl;
	}
}
void constructNumberedProdRule() {
	int ruleNum = 1;
	for (int i = 0; i < grammerRules.size(); i++)
	{//get single nonTerminal production rules
		for (int k = 0; k < grammerRules[i].rules.size(); k++)
		{//we have a single rule
		 // will constuct a numbered production rule as [0].nonterminal.singleRule ...

			numberedProdRule singleProd;
			singleProd.nonTerminal = grammerRules[i].nonTerminal;
			singleProd.rule = grammerRules[i].rules[k];
			singleProd.num = ruleNum;
			prodWithNumber.push_back(singleProd);
			ruleNum++;
		}
	}
}
set<string> getStoredFirst(string x) {
	for (int i = 0; i < firstFollow.size(); i++)
	{
		if (x == firstFollow[i].nonTerminal) return firstFollow[i].first;
	}
}
void parsingTable() {
	for (int i = 0; i < prodWithNumber.size(); i++) //for each production rule A->a
	{
		//the if condition checks if we already added this nonterminal to table because the nonterminal gets repreated //the check on i for the 0 value exception
		//if (i - 1 != -1 && prodWithNumber[i].nonTerminal == prodWithNumber[i - 1].nonTerminal) continue;
		bool flag = false;
		ParsingTable singleRow;
		singleCol singlecolomn;
		vector<singleCol> colomns;
		singleRow.nonterminal = prodWithNumber[i].nonTerminal;
		set<string> AfirstSet = First(prodWithNumber[i].rule); //gets first set of the nonterminal
		for (auto singleFirst : AfirstSet) { //createing a colomn
			if (singleFirst == "#") {
				flag = true;//first have Epslon // then Add Follow
				continue;//don't add epslon to the set
			}
			singlecolomn.Terminal = returnTokenNum(singleFirst);  //will look like terminal => rule// i.e if Terminal go to rule 
			singlecolomn.rule = prodWithNumber[i].rule;
			colomns.push_back(singlecolomn);

		}
		if (flag) {//we need to add follow
			set<string>AFollowSet = Follow(prodWithNumber[i].nonTerminal);
			for (auto singleFollow : AFollowSet) {
				singlecolomn.Terminal = returnTokenNum(singleFollow);
				singlecolomn.rule = prodWithNumber[i].rule;
				colomns.push_back(singlecolomn);
			}
		}
		singleRow.colomn = colomns; //we got a full row which contains nonternial =>terminal/rule
		int index = rowExist(prodWithNumber[i].nonTerminal); //means the same nonTerminal but with diffrent rules 
		if (index != -1) {
			//then we need to add the colomns to the tableVector[index].colomn //and not to create a new row in the table
			for (auto singelcol : colomns) { //singleCol contains terminal => rule
				tableVector[index].colomn.push_back(singelcol);
			}
		}
		else tableVector.push_back(singleRow);//not the same nonTerminal //create a new row
	}
}
string getParseRule(string x, token ct) { //same as T[X][CT] return single rule
	for (int i = 0; i < tableVector.size(); i++)
	{
		if (tableVector[i].nonterminal == x) {//found the nonterminal in the table
			for (int k = 0; k < tableVector[i].colomn.size(); k++)
			{
				if (tableVector[i].colomn[k].Terminal == ct) {
					return tableVector[i].colomn[k].rule; //found the rule T[X][CT]
				}
			}
		}
	}
	syntaxError(); //didn't find a cros. rule;
}
void showStack(stack<string> s) {
	stack<string>temp = s;
	string x; 
	cout << "stack is : ";
	while (!temp.empty())
	{
		x= temp.top();
		cout << x<< " ";
		temp.pop();
	}
}
void LLparser() {
	s.push("<Program>");
	Scanner sc("program.txt");
	CT = sc.get_token();
	while (!s.empty()) {
		string X = s.top();
		showStack(s);
		cout << endl;
	//	cout << "stack top:" << X << endl;
		if (isTerminal(X)) {
			sc.match(sc.getCrosToken(X));//convert the string to a suitable enum then match it
			s.pop();
		}
		else {// is NonTerminal
			s.pop();
			string rule = getParseRule(X, CT);//get the cors. from the table
			vector<string> ruleVec = wordsVector(rule);
			for (int i = ruleVec.size() - 1; i >= 0; i--) //gets the vector in reverse
			{
				if (ruleVec[i] != "#") //so it doesn't push # epslon
					s.push(ruleVec[i]);

			}

		}
	}
	cout << "Program is correct" << endl;
}
int main() { //epslon is replaced with #
	fstream myFile;
	myFile.open("grammer.txt");
	readFile(myFile); //gets the production rules from the grammer
	getForFirstFollow(); //construct a first follow table //only useful for debugging
	cout << "////////////////////// Production rules \\\\\\\\\\\\\\\\\\\\\\\\\\\\\ " << endl;
	showProd();
	cout << endl << "////////////////////// first follow table \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\ " << endl;
	showTable();
	constructNumberedProdRule(); //construct a Numbered production rules with no | 
	parsingTable();//construct the parsing table
	cout << "///////////////////////////// Scanner \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\ " << endl;
	LLparser();
	system("pause");
	return 0;
}
