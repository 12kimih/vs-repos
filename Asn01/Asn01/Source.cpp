#include <iostream>
#include <string>
#include <sstream>

enum SpecialSymbol {
	NIL, LEFT_PAREN, RIGHT_PAREN,
	DEFINE, LAMBDA, QUOTE,
	PLUS, MINUS, TIMES, DIVIDE,
	CAR, CDR, CONS, COND,
	isNULL, isEQ, isEQUAL, isNUMBER, isSYMBOL,
	NUM_OF_SPECIAL_SYMBOL
};
std::string* special_symbol = new std::string[NUM_OF_SPECIAL_SYMBOL];
void InitializeSpecialSymbol() {
	special_symbol[NIL] = std::string();
	special_symbol[LEFT_PAREN] = "(";
	special_symbol[RIGHT_PAREN] = ")";
	special_symbol[DEFINE] = "define";
	special_symbol[LAMBDA] = "lambda";
	special_symbol[QUOTE] = "quote";
	special_symbol[PLUS] = "+";
	special_symbol[MINUS] = "-";
	special_symbol[TIMES] = "*";
	special_symbol[DIVIDE] = '/';
	special_symbol[CAR] = "car";
	special_symbol[CDR] = "cdr";
	special_symbol[CONS] = "cons";
	special_symbol[COND] = "cond";
	special_symbol[isNULL] = "null?";
	special_symbol[isEQ] = "eq?";
	special_symbol[isEQUAL] = "equal?";
	special_symbol[isNUMBER] = "number?";
	special_symbol[isSYMBOL] = "symbol?";
	return;
}

const int WIDTH = 15;
const int INDEX_WIDTH = 10;
const int ROW_WIDTH = INDEX_WIDTH + 2 * WIDTH + 4;
const std::string NIL_SYMBOL = "NIL";
const std::string ROW_DELIM(int width) {
	std::string temp;
	for (int i = 0; i < width; ++i) temp += "-";
	return temp;
}
const std::string COL_DELIM = "|";
const std::string CENTER_ALIGN_INT(int obj, int width) {
	std::stringstream temp;
	temp << obj;
	std::string obj_str = temp.str();
	if ((width - obj_str.size()) % 2 != 0) obj_str += " ";
	int padding = (width - obj_str.size()) / 2;
	temp.str(std::string());
	for (int i = 0; i < padding; ++i) temp << ' ';
	temp << obj_str;
	for (int i = 0; i < padding; ++i) temp << ' ';
	return temp.str();
}
const std::string CENTER_ALIGN_STR(std::string obj, int width) {
	if((width - obj.size()) % 2 != 0) obj += " ";
	int padding = (width - obj.size()) / 2;
	std::stringstream temp;
	for (int i = 0; i < padding; ++i) temp << ' ';
	temp << obj;
	for (int i = 0; i < padding; ++i) temp << ' ';
	return temp.str();
}

class Node {
public:
	int lchild;
	int rchild;
	Node() : lchild(NIL), rchild(NIL) {}
	static std::string StringOfNode(const Node& entry) {
		std::stringstream temp;
		if (entry.lchild == 0) temp << CENTER_ALIGN_STR(NIL_SYMBOL, WIDTH);
		else temp << CENTER_ALIGN_INT(entry.lchild, WIDTH);
		temp << COL_DELIM;
		if (entry.rchild == 0) temp << CENTER_ALIGN_STR(NIL_SYMBOL, WIDTH);
		else temp << CENTER_ALIGN_INT(entry.rchild, WIDTH);
		return temp.str();
	}
};
class Symbol {
public:
	std::string symbol;
	int value;
	Symbol() : symbol(std::string()), value(NIL) {}
	static std::string StringOfSymbol(const Symbol& entry) {
		std::stringstream temp;
		temp << CENTER_ALIGN_STR(entry.symbol, WIDTH);
		temp << COL_DELIM;
		if (entry.value == 0) temp << CENTER_ALIGN_STR(NIL_SYMBOL, WIDTH);
		else temp << CENTER_ALIGN_INT(entry.value, WIDTH);
		return temp.str();
	}
};
class UserDefinedException {
public:
	bool occur;
	std::stringstream message;
	UserDefinedException() : occur(false), message() {}
	void Clear() {
		occur = false;
		message.str(std::string());
	}
};

Node* memory_table;
Symbol* hash_table;
const int MEMORY_TABLE_SIZE = 1000;
const int HASH_TABLE_SIZE = NUM_OF_SPECIAL_SYMBOL + 997; // 997 is a prime number, 3 entries for special symbols
int free_list;
std::stringstream buf;
UserDefinedException runtime_error;

void Initialize() {
	// node array initialisation
	memory_table = new Node[MEMORY_TABLE_SIZE];
	for (int i = 1; i < MEMORY_TABLE_SIZE - 1; ++i) {
		memory_table[i].rchild = i + 1;
	}
	free_list = 1;
	// hash table initialisation
	hash_table = new Symbol[HASH_TABLE_SIZE];
	for (int i = 0; i < NUM_OF_SPECIAL_SYMBOL; ++i) hash_table[i].symbol = special_symbol[i];
	return;
}

void ReadLine() {
	std::cout << "> ";
	std::string temp;
	std::getline(std::cin, temp);
	buf.str(temp + std::string(" "));
	return;
}

const int HashFunction(std::string str) {
	int sum = 0;
	for (size_t i = 0; i < str.size(); ++i) {
		sum += str.at(i);
	}
	return NUM_OF_SPECIAL_SYMBOL + sum % (HASH_TABLE_SIZE - NUM_OF_SPECIAL_SYMBOL);
}

int GetHashValue(std::string str) {
	for (int i = 0; i < NUM_OF_SPECIAL_SYMBOL; ++i) if (str == special_symbol[i]) return -i;
	int str_hash = HashFunction(str);
	for (; !hash_table[str_hash].symbol.empty() && hash_table[str_hash].symbol != str; ) {
		if (str_hash < HASH_TABLE_SIZE - 1) ++str_hash;
		else str_hash = NUM_OF_SPECIAL_SYMBOL;
	}
	if (hash_table[str_hash].symbol.empty()) {
		hash_table[str_hash].symbol = str;
	}
	return -str_hash;
}

void ASCIILower(std::string& str) {
	for (auto iter = str.begin(); iter != str.end(); ++iter) {
		if (*iter >= 'A' && *iter <= 'Z') {
			*iter += 'a' - 'A';
		}
	}
	return;
}

std::string GetNextToken() {
	std::string temp;
	if (buf >> temp) {
		ASCIILower(temp);
		size_t i = 0;
		for (; i < temp.size(); ++i) {
			if (temp.at(i) == '(') break;
			if (temp.at(i) == ')') break;
			if (temp.at(i) == '\'') break;
		};
		if (i > 0) {
			buf.seekg(buf.tellg() - std::streampos(temp.size() - i));
			return temp.substr(0, i);
		}
		else {
			buf.seekg(buf.tellg() - std::streampos(temp.size() - 1));
			return temp.substr(0, 1);
		}
	}
	else {
		buf.clear();
		return std::string();
	}
}

void PushBack() {
	buf.seekg(buf.tellg() - std::streampos(1));
	return;
}

void CursorBack(std::string token) {
	buf.seekg(buf.tellg() - std::streampos(token.size()));
	return;
}

int Alloc() {
	int temp = free_list;
	free_list = memory_table[free_list].rchild;
	return temp;
}

/*
// Top-down deallocation
void Dealloc(int root) {
	if (root <= NIL) return;
	int cur = root;
	while (cur != NIL) {
		if (memory_table[cur].lchild > 0) Dealloc(memory_table[cur].lchild);
		int temp = memory_table[cur].rchild;
		memory_table[cur].lchild = 0;
		memory_table[cur].rchild = free_list;
		free_list = cur;
		cur = temp;
	}
	return;
}
*/

// Bottom-up deallocation
void Dealloc(int root) {
	if (root <= NIL) return;
	Dealloc(memory_table[root].rchild);
	if (memory_table[root].lchild > NIL) Dealloc(memory_table[root].lchild);
	memory_table[root].lchild = NIL;
	memory_table[root].rchild = free_list;
	free_list = root;
	return;
}

void PrintFreeList() {
	std::cout << "Free list's root = " << free_list << std::endl;
	return;
}

void PrintRootOfList(int root) {
	std::cout << "List's root = " << root << std::endl;
	return;
}

void PrintMemory() {
	std::cout << "Memory table size = " << MEMORY_TABLE_SIZE << std::endl;
	std::cout << "Memory table = " << std::endl;
	std::cout << ROW_DELIM(ROW_WIDTH) << std::endl;
	for (int i = 0; i < free_list; ++i) {
		std::cout << COL_DELIM << CENTER_ALIGN_INT(i, INDEX_WIDTH) << COL_DELIM;
		std::cout << Node::StringOfNode(memory_table[i]) << COL_DELIM << std::endl;
		std::cout << ROW_DELIM(ROW_WIDTH) << std::endl;
	}
	std::cout << std::endl;
	std::cout << "Hash table size = " << HASH_TABLE_SIZE << std::endl;
	std::cout << "Hash table = " << std::endl;
	std::cout << ROW_DELIM(ROW_WIDTH) << std::endl;
	for (int i = 1; i < HASH_TABLE_SIZE; ++i) {\
		if (hash_table[i].symbol != std::string()) {
			std::cout << COL_DELIM << CENTER_ALIGN_INT(i, INDEX_WIDTH) << COL_DELIM;
			std::cout << Symbol::StringOfSymbol(hash_table[i]) << COL_DELIM << std::endl;
			std::cout << ROW_DELIM(ROW_WIDTH) << std::endl;
		}
	}
	std::cout << std::endl;
	return;
}

std::string StringOfData(int root, bool list_paren = true) {
	if (root == NIL) return "()";
	if (root < 0) return hash_table[-root].symbol;
	std::stringstream temp;
	if (list_paren == true) temp << '(';
	temp << StringOfData(memory_table[root].lchild);
	if (memory_table[root].rchild == NIL) temp << ')';
	else {
		temp << ' ';
		temp << StringOfData(memory_table[root].rchild, false);
	}
	return temp.str();
}

int Read(bool tail = false) {
	int token_hash;
	if (tail == false) {
		token_hash = GetHashValue(GetNextToken());
		if (-token_hash != LEFT_PAREN) return token_hash;
	}
	token_hash = GetHashValue(GetNextToken());
	if (token_hash == NIL) {
		runtime_error.occur = true;
		runtime_error.message << "The number of left and right parentheses does not match\n";
		return NIL;
	}
	if (-token_hash == RIGHT_PAREN) return NIL;
	int root = Alloc();
	if (-token_hash == LEFT_PAREN) memory_table[root].lchild = Read(true);
	else memory_table[root].lchild = token_hash;
	memory_table[root].rchild = Read(true);
	return root;
}

/*
int Read() {
	int token_hash = GetHashValue(GetNextToken());
	if (-token_hash != LEFT_PAREN) return token_hash;
	int root = NIL;
	bool first = true;
	int temp;
	while (-(token_hash = GetHashValue(GetNextToken())) != RIGHT_PAREN) {
		if (first) {
			root = Alloc();
			temp = root;
			first = false;
		}
		else {
			memory_table[temp].rchild = Alloc();
			temp = memory_table[temp].rchild;
		}
		if (-token_hash == LEFT_PAREN) {
			PushBack();
			memory_table[temp].lchild = Read();
		}
		else memory_table[temp].lchild = token_hash;
		memory_table[temp].rchild = NIL;
		if (-token_hash == NIL) {
			runtime_error.occur = true;
			runtime_error.message << "The number of left and right parentheses does not match\n";
			return root;
		}
	}
	return root;
}
*/

std::string Preprocess() {
	std::stringstream pre_buf;
	std::string token;
	while ((token = GetNextToken()) != std::string()) {
		if (token == "define") {
			pre_buf << token << ' ';
			token = GetNextToken();
			if (token == "(") {
				token = GetNextToken();
				pre_buf << token << ' ' << "( lambda ( " << Preprocess() << ") ";
			}
			else pre_buf << token << ' ';
		}
		else if (token == "'") {
			pre_buf << "( quote ";
			int num_of_left_paren = 0;
			do {
				token = GetNextToken();
				pre_buf << token << ' ';
				if (token == "(") num_of_left_paren += 1;
				else if (token == ")") num_of_left_paren -= 1;
			} while (num_of_left_paren > 0);
			pre_buf << ") ";
		}
		else pre_buf << token << ' ';
	}
	return pre_buf.str();
}

// return 0 if non-numeric, 1 if int, 2 if double
int IsNumber(std::string token) {
	std::stringstream temp(token);
	int num1;
	temp >> num1;
	if (!temp.fail() && temp.eof()) return 1;
	temp.clear();
	temp.str(token);
	double num2;
	temp >> num2;
	if (!temp.fail() && temp.eof()) return 2;
	return 0;
}

int Evaluate(int root) {
	// empty list
	if (root == NIL) return root;
	// variable
	if (root < NIL) {
		if (IsNumber(hash_table[-root].symbol)) return root;
		return hash_table[-root].value;
	}
	// list
	int function_hash = memory_table[root].lchild;
	if (function_hash == NIL) {
		runtime_error.occur = true;
		runtime_error.message << "You tried to call a NIL function\n";
		return NIL;
	}
	if (-function_hash == DEFINE) {
		int arg_hash = memory_table[memory_table[root].rchild].lchild;
		if (arg_hash >= NIL) {
			runtime_error.occur = true;
			runtime_error.message << "You tried to define a list which is not a symbol\n";
			return NIL;
		}
		if (-arg_hash < NUM_OF_SPECIAL_SYMBOL) {
			runtime_error.occur = true;
			runtime_error.message << "You tried to define a pre-defined keyword\n";
			return NIL;
		}
		if (IsNumber(hash_table[-arg_hash].symbol)) {
			runtime_error.occur = true;
			runtime_error.message << "You tried to define a number\n";
			return NIL;
		}
		hash_table[-arg_hash].value = Evaluate(memory_table[memory_table[memory_table[root].rchild].rchild].lchild);
		return arg_hash;
	}
	if (-function_hash == LAMBDA) return root;
	if (-function_hash == QUOTE) return memory_table[memory_table[root].rchild].lchild;
	if (-function_hash == PLUS) {
		int arg1_hash = Evaluate(memory_table[memory_table[root].rchild].lchild);
		int arg2_hash = Evaluate(memory_table[memory_table[memory_table[root].rchild].rchild].lchild);
		if (arg1_hash >= NIL || arg2_hash >= NIL) {
			runtime_error.occur = true;
			runtime_error.message << "You tried to add a list which is not a number\n";
			return NIL;
		}
		std::string arg1_symbol = hash_table[-arg1_hash].symbol;
		std::string arg2_symbol = hash_table[-arg2_hash].symbol;
		int arg1_isnum = IsNumber(arg1_symbol);
		int arg2_isnum = IsNumber(arg2_symbol);
		if (!arg1_isnum || !arg2_isnum) {
			runtime_error.occur = true;
			runtime_error.message << "You tried to add a non-numeric symbol\n";
			return NIL;
		}
		if (arg1_isnum == 1 && arg2_isnum == 1) return GetHashValue(std::to_string(std::stoi(arg1_symbol) + std::stoi(arg2_symbol)));
		return GetHashValue(std::to_string(std::stod(arg1_symbol) + std::stod(arg2_symbol)));
	}
	if (-function_hash == MINUS) {
		int arg1_hash = Evaluate(memory_table[memory_table[root].rchild].lchild);
		int arg2_hash = Evaluate(memory_table[memory_table[memory_table[root].rchild].rchild].lchild);
		if (arg1_hash >= NIL || arg2_hash >= NIL) {
			runtime_error.occur = true;
			runtime_error.message << "You tried to subtract a list which is not a number\n";
			return NIL;
		}
		std::string arg1_symbol = hash_table[-arg1_hash].symbol;
		std::string arg2_symbol = hash_table[-arg2_hash].symbol;
		int arg1_isnum = IsNumber(arg1_symbol);
		int arg2_isnum = IsNumber(arg2_symbol);
		if (!arg1_isnum || !arg2_isnum) {
			runtime_error.occur = true;
			runtime_error.message << "You tried to subtract a non-numeric symbol\n";
			return NIL;
		}
		if (arg1_isnum == 1 && arg2_isnum == 1) return GetHashValue(std::to_string(std::stoi(arg1_symbol) - std::stoi(arg2_symbol)));
		return GetHashValue(std::to_string(std::stod(arg1_symbol) - std::stod(arg2_symbol)));
	}
	if (-function_hash == TIMES) {
		int arg1_hash = Evaluate(memory_table[memory_table[root].rchild].lchild);
		int arg2_hash = Evaluate(memory_table[memory_table[memory_table[root].rchild].rchild].lchild);
		if (arg1_hash >= NIL || arg2_hash >= NIL) {
			runtime_error.occur = true;
			runtime_error.message << "You tried to multiply a list which is not a number\n";
			return NIL;
		}
		std::string arg1_symbol = hash_table[-arg1_hash].symbol;
		std::string arg2_symbol = hash_table[-arg2_hash].symbol;
		int arg1_isnum = IsNumber(arg1_symbol);
		int arg2_isnum = IsNumber(arg2_symbol);
		if (!arg1_isnum || !arg2_isnum) {
			runtime_error.occur = true;
			runtime_error.message << "You tried to multiply a non-numeric symbol\n";
			return NIL;
		}
		if (arg1_isnum == 1 && arg2_isnum == 1) return GetHashValue(std::to_string(std::stoi(arg1_symbol) * std::stoi(arg2_symbol)));
		return GetHashValue(std::to_string(std::stod(arg1_symbol) * std::stod(arg2_symbol)));
	}
	if (-function_hash == DIVIDE) {
		int arg1_hash = Evaluate(memory_table[memory_table[root].rchild].lchild);
		int arg2_hash = Evaluate(memory_table[memory_table[memory_table[root].rchild].rchild].lchild);
		if (arg1_hash >= NIL || arg2_hash >= NIL) {
			runtime_error.occur = true;
			runtime_error.message << "You tried to divide (by) a list which is not a number\n";
			return NIL;
		}
		std::string arg1_symbol = hash_table[-arg1_hash].symbol;
		std::string arg2_symbol = hash_table[-arg2_hash].symbol;
		int arg1_isnum = IsNumber(arg1_symbol);
		int arg2_isnum = IsNumber(arg2_symbol);
		if (!arg1_isnum || !arg2_isnum) {
			runtime_error.occur = true;
			runtime_error.message << "You tried to divide (by) a non-numeric symbol\n";
			return NIL;
		}
		if (std::stod(arg2_symbol) == 0.0) {
			runtime_error.occur = true;
			runtime_error.message << "You tried to divide by zero\n";
			return NIL;
		}
		if (arg1_isnum == 1 && arg2_isnum == 1) return GetHashValue(std::to_string(std::stoi(arg1_symbol) / std::stoi(arg2_symbol)));
		return GetHashValue(std::to_string(std::stod(arg1_symbol) / std::stod(arg2_symbol)));
	}
	if (-function_hash == CAR) {
		int arg_hash = Evaluate(memory_table[memory_table[root].rchild].lchild);
		if (arg_hash <= NIL) {
			runtime_error.occur = true;
			runtime_error.message << "The argument to CAR is not a list\n";
			return NIL;
		}
		return memory_table[arg_hash].lchild;
	}
	if (-function_hash == CDR) {
		int arg_hash = Evaluate(memory_table[memory_table[root].rchild].lchild);
		if (arg_hash <= NIL) {
			runtime_error.occur = true;
			runtime_error.message << "The argument to CAR is not a list\n";
			return NIL;
		}
		return memory_table[arg_hash].rchild;
	}
	if (-function_hash == CONS) {
		int arg1_hash = Evaluate(memory_table[memory_table[root].rchild].lchild);
		int arg2_hash = Evaluate(memory_table[memory_table[memory_table[root].rchild].rchild].lchild);
		int list = Alloc();
		memory_table[list].lchild = arg1_hash;
		memory_table[list].rchild = arg2_hash;
		return list;
	}
	if (-function_hash == COND) {
		return NIL;
	}
	if (-function_hash == isNULL) {
		return NIL;
	}
	if (-function_hash == isEQ) {
		return NIL;
	}
	if (-function_hash == isEQUAL) {
		return NIL;
	}
	if (-function_hash == isNUMBER) {
		return NIL;
	}
	if (-function_hash == isSYMBOL) {
		return NIL;
	}
	return function_hash;
}

int main() {
	InitializeSpecialSymbol();
	Initialize();
	while (true) {
		ReadLine();
		buf.str(Preprocess());

		int root = Read();
		if (runtime_error.occur) {
			std::cout << runtime_error.message.str() << std::endl;
			Dealloc(root);
			runtime_error.Clear();
			continue;
		}

		int result = Evaluate(root);
		if (runtime_error.occur) {
			std::cout << runtime_error.message.str() << std::endl;
			Dealloc(root);
			runtime_error.Clear();
			continue;
		}

		std::cout << "] ";
		PrintFreeList();
		PrintRootOfList(root);
		std::cout << std::endl;
		PrintMemory();

		std::cout << "Preprocessd: " << std::endl;
		std::cout << buf.str() << std::endl << std::endl;
		std::cout << "Command interpreted: " << std::endl;
		std::cout << StringOfData(root) << std::endl << std::endl;
		std::cout << "Result: " << std::endl;
		std::cout << StringOfData(result) << std::endl << std::endl;
	}
	return 0;
}