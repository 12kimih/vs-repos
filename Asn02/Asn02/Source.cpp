#include <iostream>
#include <string>
#include <sstream>

class UserDefinedException {
public:
	bool occur;
	std::stringstream message;
	UserDefinedException() : occur(false), message() {}
	void Clear() {
		occur = false;
		message.str(std::string());
	}
	void Mark(std::string str) {
		occur = true;
		message << str << std::endl;
		return;
	}
};
UserDefinedException runtime_error;
enum SpecialSymbol {
	NIL, LEFT_PAREN, RIGHT_PAREN, QUOTE,
	DEFINE, LAMBDA, LIST,
	PLUS, MINUS, TIMES, DIVIDE,
	TRUE, FALSE,
	NOT, isNULL, isNUMBER, isSYMBOL, isPAIR, isEQ, isEQUAL,
	IF, COND, ELSE,
	CAR, CDR, CONS,
	NUM_OF_SPECIAL_SYMBOL
};
std::string* special_symbol = new std::string[NUM_OF_SPECIAL_SYMBOL];
void InitializeSpecialSymbol() {
	special_symbol[NIL] = std::string();
	special_symbol[LEFT_PAREN] = "(";
	special_symbol[RIGHT_PAREN] = ")";
	special_symbol[QUOTE] = "quote";
	special_symbol[DEFINE] = "define";
	special_symbol[LAMBDA] = "lambda";
	special_symbol[LIST] = "list";
	special_symbol[PLUS] = "+";
	special_symbol[MINUS] = "-";
	special_symbol[TIMES] = "*";
	special_symbol[DIVIDE] = '/';
	special_symbol[TRUE] = "#t";
	special_symbol[FALSE] = "#f";
	special_symbol[NOT] = "not";
	special_symbol[isNULL] = "null?";
	special_symbol[isNUMBER] = "number?";
	special_symbol[isSYMBOL] = "symbol?";
	special_symbol[isPAIR] = "pair?";
	special_symbol[isEQ] = "eq?";
	special_symbol[isEQUAL] = "equal?";
	special_symbol[IF] = "if";
	special_symbol[COND] = "cond";
	special_symbol[ELSE] = "else";
	special_symbol[CAR] = "car";
	special_symbol[CDR] = "cdr";
	special_symbol[CONS] = "cons";
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
	if ((width - obj.size()) % 2 != 0) obj += " ";
	int padding = (width - obj.size()) / 2;
	std::stringstream temp;
	for (int i = 0; i < padding; ++i) temp << ' ';
	temp << obj;
	for (int i = 0; i < padding; ++i) temp << ' ';
	return temp.str();
}

template <class T>
class Stack {
public:
	T* stack;
	int capacity;
	int top;
	Stack() : stack(new T[10]), capacity(10), top(-1) {}
	Stack(int capacity) : stack(new T[capacity]), capacity(capacity), top(-1) {}
	~Stack() {
		delete[] stack;
	}
	bool Empty() {
		if (top == -1) return true;
		else return false;
	}
	void Push(const T& element) {
		if (top == capacity - 1) {
			T* resized_stack = new T[capacity *= 2];
			for (int i = 0; i <= top; ++i) {
				resized_stack[i] = stack[i];
			}
			delete[] stack;
			stack = resized_stack;
		}
		stack[++top] = element;
	}
	T Pop() {
		if (top == -1) {
			runtime_error.Mark("WARNING: stack underflow");
			return T();
		}
		return stack[top--];
	}
	void Clear() {
		top = -1;
		return;
	}
};
class Node {
public:
	int lchild;
	int rchild;
	bool being_used;
	Node() : lchild(NIL), rchild(NIL), being_used(false) {}
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
	Symbol(const Symbol& ref) : symbol(ref.symbol), value(ref.value) {}
	Symbol& operator=(const Symbol& ref) {
		symbol = ref.symbol;
		value = ref.value;
		return *this;
	}
	static std::string StringOfSymbol(const Symbol& entry) {
		std::stringstream temp;
		temp << CENTER_ALIGN_STR(entry.symbol, WIDTH);
		temp << COL_DELIM;
		if (entry.value == 0) temp << CENTER_ALIGN_STR(NIL_SYMBOL, WIDTH);
		else temp << CENTER_ALIGN_INT(entry.value, WIDTH);
		return temp.str();
	}
};
class Pair {
public:
	int hash;
	int value;
	Pair() : hash(NIL), value(NIL) {}
	Pair(int hash, int value) : hash(hash), value(value) {}
};

Node* memory_table;
Symbol* hash_table;
int memory_table_size = 30;
const int HASH_TABLE_SIZE = NUM_OF_SPECIAL_SYMBOL + 997; // 997 is a prime number, 3 entries for special symbols
int free_list;
std::stringstream buf;
Stack<Pair> function_stack;
Stack<int> command_stack;
Stack<int> evaluation_stack;

void Initialize() {
	// node array initialisation
	memory_table = new Node[memory_table_size];
	for (int i = 1; i < memory_table_size - 1; ++i) memory_table[i].rchild = i + 1;
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
	buf.clear();
	return;
}

const int HashFunction(std::string str) {
	int sum = 0;
	for (size_t i = 0; i < str.size(); ++i) {
		sum += str.at(i);
	}
	return NUM_OF_SPECIAL_SYMBOL + sum % (HASH_TABLE_SIZE - NUM_OF_SPECIAL_SYMBOL);
}

int IsNumber(std::string token);

int GetHashValue(std::string str) {
	for (int i = 0; i < NUM_OF_SPECIAL_SYMBOL; ++i) if (str == special_symbol[i]) return -i;
	if (IsNumber(str) == 0);
	else if (IsNumber(str) == 1) str = std::to_string(std::stoi(str));
	else str = std::to_string(std::stod(str));
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
		}
		if (i > 0) {
			buf.seekg(buf.tellg() - std::streampos(temp.size() - i));
			return temp.substr(0, i);
		}
		else {
			buf.seekg(buf.tellg() - std::streampos(temp.size() - 1));
			return temp.substr(0, 1);
		}
	}
	else return std::string();
}

void PushBack() {
	buf.seekg(buf.tellg() - std::streampos(1));
	return;
}

void MarkBeingUsed(int root) {
	if (root <= 0) return;
	memory_table[root].being_used = true;
	MarkBeingUsed(memory_table[root].lchild);
	MarkBeingUsed(memory_table[root].rchild);
	return;
}

void GarbageCollect() {
	for (int i = 1; i < memory_table_size; ++i) memory_table[i].being_used = false;
	for (int i = 0; i < HASH_TABLE_SIZE; ++i) MarkBeingUsed(hash_table[i].value);
	for (int i = 0; i <= function_stack.top; ++i) MarkBeingUsed(function_stack.stack[i].value);
	for (int i = 0; i <= command_stack.top; ++i) MarkBeingUsed(command_stack.stack[i]);
	for (int i = 0; i <= evaluation_stack.top; ++i) MarkBeingUsed(evaluation_stack.stack[i]);
	for (int i = 1; i < memory_table_size; ++i) {
		if (!memory_table[i].being_used) {
			memory_table[i].lchild = NIL;
			memory_table[i].rchild = free_list;
			free_list = i;
		}
	}
	return;
}

int Alloc() {
	if (free_list == NIL) GarbageCollect();
	if (free_list == NIL) {
		Node* doubled_table = new Node[2 * memory_table_size];
		for (int i = 0; i < memory_table_size; ++i) doubled_table[i] = memory_table[i];
		for (int i = memory_table_size; i < 2 * memory_table_size - 1; ++i) doubled_table[i].rchild = i + 1;
		delete[] memory_table;
		memory_table = doubled_table;
		free_list = memory_table_size;
		memory_table_size *= 2;
	}
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
	std::cout << "Memory table size = " << memory_table_size << std::endl;
	std::cout << "Memory table = " << std::endl;
	std::cout << ROW_DELIM(ROW_WIDTH) << std::endl;
	for (int i = 0; i < memory_table_size; ++i) {
		std::cout << COL_DELIM << CENTER_ALIGN_INT(i, INDEX_WIDTH) << COL_DELIM;
		std::cout << Node::StringOfNode(memory_table[i]) << COL_DELIM << std::endl;
		std::cout << ROW_DELIM(ROW_WIDTH) << std::endl;
	}
	std::cout << std::endl;
	std::cout << "Hash table size = " << HASH_TABLE_SIZE << std::endl;
	std::cout << "Hash table = " << std::endl;
	std::cout << ROW_DELIM(ROW_WIDTH) << std::endl;
	for (int i = 1; i < HASH_TABLE_SIZE; ++i) {
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
		runtime_error.Mark("The number of left and right parentheses does not match");
		return NIL;
	}
	if (-token_hash == RIGHT_PAREN) return NIL;
	int root = Alloc();
	command_stack.Push(root);
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
			runtime_error.Mark("The number of left and right parentheses does not match");
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

bool StructureTest(int hash1, int hash2) {
	if (hash1 <= NIL) {
		if (hash2 <= NIL) {
			if (hash1 == hash2) return true;
			else return false;
		}
		else return false;
	}
	else {
		if (hash2 <= NIL) return false;
		else {
			if (!StructureTest(memory_table[hash1].lchild, memory_table[hash2].lchild)) return false;
			if (StructureTest(memory_table[hash1].rchild, memory_table[hash2].rchild)) return true;
			else return false;
		}
	}
}

int Evaluate(int root);

void Substitute(int parameter_list, int argument_list, int& number_of_param) {
	if (parameter_list == NIL) {
		if (argument_list == NIL) return;
		runtime_error.Mark("The number of arguments is greater than the number of parameters");
		return;
	}
	if (argument_list == NIL) {
		runtime_error.Mark("The number of parameters is greater than the number of arguments");
		return;
	}
	int parameter = memory_table[parameter_list].lchild;
	if (parameter > NIL) {
		runtime_error.Mark("A list cannot be a parameter name");
		return;
	}
	if (-parameter < NUM_OF_SPECIAL_SYMBOL) {
		runtime_error.Mark("A pre-defined keyword cannot be a parameter name");
		return;
	}
	if (IsNumber(hash_table[-parameter].symbol)) {
		runtime_error.Mark("A number cannot be a parameter name");
		return;
	}
	function_stack.Push(Pair(parameter, hash_table[-parameter].value));
	hash_table[-parameter].value = memory_table[argument_list].lchild;
	++number_of_param;
	Substitute(memory_table[parameter_list].rchild, memory_table[argument_list].rchild, number_of_param);
	return;
}

void Unsubstitute(int number_of_param) {
	for (int i = 0; i < number_of_param; ++i) {
		Pair pair = function_stack.Pop();
		hash_table[-pair.hash].value = pair.value;
	}
	return;
}

int EvaluateArgumentList(int argument_list) {
	if (argument_list == NIL) return NIL;
	int list = Alloc();
	evaluation_stack.Push(list);
	memory_table[list].lchild = Evaluate(memory_table[argument_list].lchild);
	memory_table[list].rchild = EvaluateArgumentList(memory_table[argument_list].rchild);
	return list;
}

int Evaluate(int root) {
	// symbol
	if (root <= NIL) {
		if (-root < NUM_OF_SPECIAL_SYMBOL) return root;
		if (IsNumber(hash_table[-root].symbol)) return root;
		return hash_table[-root].value;
	}
	// command
	int function_hash = Evaluate(memory_table[root].lchild);
	if (function_hash > NIL) {
		if (memory_table[function_hash].lchild != -LAMBDA) {
			runtime_error.Mark("You tried to call a list as a function");
			return NIL;
		}
		function_hash = Evaluate(function_hash); // to check whether it fits the criteria of LAMBDA function
		if (runtime_error.occur) return NIL;
		int evaluated_argument_list = EvaluateArgumentList(memory_table[root].rchild);
		int number_of_param = 0;
		Substitute(memory_table[memory_table[function_hash].rchild].lchild, evaluated_argument_list, number_of_param);
		evaluation_stack.Clear();
		if (runtime_error.occur) {
			Unsubstitute(number_of_param);
			return NIL;
		}
		int result = Evaluate(memory_table[memory_table[memory_table[function_hash].rchild].rchild].lchild);
		Unsubstitute(number_of_param);
		return result;
	}
	if (function_hash == -DEFINE) {
		if (memory_table[memory_table[memory_table[root].rchild].rchild].rchild > NIL) {
			runtime_error.Mark("2 arguments are expected for DEFINE but more than 2 arguments were passed");
			return NIL;
		}
		int arg_hash = memory_table[memory_table[root].rchild].lchild;
		if (arg_hash > NIL) {
			runtime_error.Mark("You tried to define a list which is not a symbol");
			return NIL;
		}
		if (-arg_hash < NUM_OF_SPECIAL_SYMBOL) {
			runtime_error.Mark("You tried to define a pre-defined keyword");
			return NIL;
		}
		if (IsNumber(hash_table[-arg_hash].symbol)) {
			runtime_error.Mark("You tried to define a number");
			return NIL;
		}
		hash_table[-arg_hash].value = Evaluate(memory_table[memory_table[memory_table[root].rchild].rchild].lchild);
		return hash_table[-arg_hash].value;
	}
	if (function_hash == -LAMBDA) {
		if (memory_table[memory_table[memory_table[root].rchild].rchild].rchild > NIL) {
			runtime_error.Mark("2 arguments are expected for LAMBDA but more than 2 arguments were passed");
			return NIL;
		}
		if (memory_table[memory_table[root].rchild].lchild < NIL) {
			runtime_error.Mark("A list of arguments should be passed to a LAMBDA function");
			return NIL;
		}
		return root;
	}
	if (function_hash == -LIST) {
		if (memory_table[memory_table[root].rchild].rchild > NIL) {
			runtime_error.Mark("1 argument is expected for LIST but more than 1 argument were passed");
			return NIL;
		}
		int arg_link = Evaluate(memory_table[memory_table[root].rchild].lchild);
		if (arg_link < 0 && -NUM_OF_SPECIAL_SYMBOL < arg_link) {
			runtime_error.Mark("TYPE ERROR: a pre-defined keyword cannot be made a list");
			return NIL;
		}
		int root = Alloc();
		command_stack.Push(root);
		memory_table[root].lchild = arg_link;
		memory_table[root].rchild = NIL;
		return root;
	}
	if (function_hash == -QUOTE) {
		if (memory_table[memory_table[root].rchild].rchild > NIL) {
			runtime_error.Mark("1 argument is expected for QUOTE but more than 1 argument were passed");
			return NIL;
		}
		return memory_table[memory_table[root].rchild].lchild;
	}
	if (function_hash == -PLUS) {
		if (memory_table[memory_table[memory_table[root].rchild].rchild].rchild > NIL) {
			runtime_error.Mark("2 arguments are expected for PLUS but more than 2 arguments were passed");
			return NIL;
		}
		int arg1_hash = Evaluate(memory_table[memory_table[root].rchild].lchild);
		int arg2_hash = Evaluate(memory_table[memory_table[memory_table[root].rchild].rchild].lchild);
		if (arg1_hash > NIL || arg2_hash > NIL) {
			runtime_error.Mark("You tried to add a list which is not a number");
			return NIL;
		}
		std::string arg1_symbol = hash_table[-arg1_hash].symbol;
		std::string arg2_symbol = hash_table[-arg2_hash].symbol;
		int arg1_isnum = IsNumber(arg1_symbol);
		int arg2_isnum = IsNumber(arg2_symbol);
		if (!arg1_isnum || !arg2_isnum) {
			runtime_error.Mark("You tried to add a non-numeric variable");
			return NIL;
		}
		if (arg1_isnum == 1 && arg2_isnum == 1) return GetHashValue(std::to_string(std::stoi(arg1_symbol) + std::stoi(arg2_symbol)));
		return GetHashValue(std::to_string(std::stod(arg1_symbol) + std::stod(arg2_symbol)));
	}
	if (function_hash == -MINUS) {
		if (memory_table[memory_table[memory_table[root].rchild].rchild].rchild > NIL) {
			runtime_error.Mark("2 arguments are expected for MINUS but more than 2 arguments were passed");
			return NIL;
		}
		int arg1_hash = Evaluate(memory_table[memory_table[root].rchild].lchild);
		int arg2_hash = Evaluate(memory_table[memory_table[memory_table[root].rchild].rchild].lchild);
		if (arg1_hash > NIL || arg2_hash > NIL) {
			runtime_error.Mark("You tried to subtract a list which is not a number");
			return NIL;
		}
		std::string arg1_symbol = hash_table[-arg1_hash].symbol;
		std::string arg2_symbol = hash_table[-arg2_hash].symbol;
		int arg1_isnum = IsNumber(arg1_symbol);
		int arg2_isnum = IsNumber(arg2_symbol);
		if (!arg1_isnum || !arg2_isnum) {
			runtime_error.Mark("You tried to subtract a non-numeric variable");
			return NIL;
		}
		if (arg1_isnum == 1 && arg2_isnum == 1) return GetHashValue(std::to_string(std::stoi(arg1_symbol) - std::stoi(arg2_symbol)));
		return GetHashValue(std::to_string(std::stod(arg1_symbol) - std::stod(arg2_symbol)));
	}
	if (function_hash == -TIMES) {
		if (memory_table[memory_table[memory_table[root].rchild].rchild].rchild > NIL) {
			runtime_error.Mark("2 arguments are expected for TIMES but more than 2 arguments were passed");
			return NIL;
		}
		int arg1_hash = Evaluate(memory_table[memory_table[root].rchild].lchild);
		int arg2_hash = Evaluate(memory_table[memory_table[memory_table[root].rchild].rchild].lchild);
		if (arg1_hash > NIL || arg2_hash > NIL) {
			runtime_error.Mark("You tried to multiply a list which is not a number");
			return NIL;
		}
		std::string arg1_symbol = hash_table[-arg1_hash].symbol;
		std::string arg2_symbol = hash_table[-arg2_hash].symbol;
		int arg1_isnum = IsNumber(arg1_symbol);
		int arg2_isnum = IsNumber(arg2_symbol);
		if (!arg1_isnum || !arg2_isnum) {
			runtime_error.Mark("You tried to multiply a non-numeric variable");
			return NIL;
		}
		if (arg1_isnum == 1 && arg2_isnum == 1) return GetHashValue(std::to_string(std::stoi(arg1_symbol) * std::stoi(arg2_symbol)));
		return GetHashValue(std::to_string(std::stod(arg1_symbol) * std::stod(arg2_symbol)));
	}
	if (function_hash == -DIVIDE) {
		if (memory_table[memory_table[memory_table[root].rchild].rchild].rchild > NIL) {
			runtime_error.Mark("2 arguments are expected for DIVIDE but more than 2 arguments were passed");
			return NIL;
		}
		int arg1_hash = Evaluate(memory_table[memory_table[root].rchild].lchild);
		int arg2_hash = Evaluate(memory_table[memory_table[memory_table[root].rchild].rchild].lchild);
		if (arg1_hash > NIL || arg2_hash > NIL) {
			runtime_error.Mark("You tried to divide (by) a list which is not a number");
			return NIL;
		}
		std::string arg1_symbol = hash_table[-arg1_hash].symbol;
		std::string arg2_symbol = hash_table[-arg2_hash].symbol;
		int arg1_isnum = IsNumber(arg1_symbol);
		int arg2_isnum = IsNumber(arg2_symbol);
		if (!arg1_isnum || !arg2_isnum) {
			runtime_error.Mark("You tried to divide (by) a non-numeric variable");
			return NIL;
		}
		if (std::stod(arg2_symbol) == 0.0) {
			runtime_error.Mark("You tried to divide by zero");
			return NIL;
		}
		if (arg1_isnum == 1 && arg2_isnum == 1) return GetHashValue(std::to_string(std::stoi(arg1_symbol) / std::stoi(arg2_symbol)));
		return GetHashValue(std::to_string(std::stod(arg1_symbol) / std::stod(arg2_symbol)));
	}
	if (function_hash == -CAR) {
		if (memory_table[memory_table[root].rchild].rchild > NIL) {
			runtime_error.Mark("1 argument is expected for CAR but more than 1 argument were passed");
			return NIL;
		}
		int arg_hash = Evaluate(memory_table[memory_table[root].rchild].lchild);
		if (arg_hash <= NIL) {
			runtime_error.Mark("The argument to CAR is not the correct type");
			return NIL;
		}
		return memory_table[arg_hash].lchild;
	}
	if (function_hash == -CDR) {
		if (memory_table[memory_table[root].rchild].rchild > NIL) {
			runtime_error.Mark("1 argument is expected for CDR but more than 1 argument were passed");
			return NIL;
		}
		int arg_hash = Evaluate(memory_table[memory_table[root].rchild].lchild);
		if (arg_hash <= NIL) {
			runtime_error.Mark("The argument to CDR is not the correct type");
			return NIL;
		}
		return memory_table[arg_hash].rchild;
	}
	if (function_hash == -CONS) {
		if (memory_table[memory_table[memory_table[root].rchild].rchild].rchild > NIL) {
			runtime_error.Mark("2 arguments are expected for CONS but more than 2 arguments were passed");
			return NIL;
		}
		int arg1_hash = Evaluate(memory_table[memory_table[root].rchild].lchild);
		int arg2_hash = Evaluate(memory_table[memory_table[memory_table[root].rchild].rchild].lchild);
		if (arg2_hash < NIL) {
			runtime_error.Mark("The second argument to CONS cannot be a symbol");
			return NIL;
		}
		int list = Alloc();
		command_stack.Push(list);
		memory_table[list].lchild = arg1_hash;
		memory_table[list].rchild = arg2_hash;
		return list;
	}
	if (function_hash == -IF) {
		if (memory_table[memory_table[memory_table[memory_table[root].rchild].rchild].rchild].rchild > NIL) {
			runtime_error.Mark("3 arguments are expected for IF but more than 3 argument were passed");
			return NIL;
		}
		int predicate = Evaluate(memory_table[memory_table[root].rchild].lchild);
		if (predicate == -TRUE) return Evaluate(memory_table[memory_table[memory_table[root].rchild].rchild].lchild);
		else if (predicate == -FALSE) return Evaluate(memory_table[memory_table[memory_table[memory_table[root].rchild].rchild].rchild].lchild);
		else {
			runtime_error.Mark("Predicate of IF does not evaluate to boolean value");
			return NIL;
		}
	}
	if (function_hash == -COND) {
		if (memory_table[root].rchild == NIL) {
			runtime_error.Mark("There is no argument to COND");
			return NIL;
		}
		while (memory_table[memory_table[root].rchild].rchild > NIL) {
			root = memory_table[root].rchild;
			if (memory_table[root].lchild <= NIL) {
				runtime_error.Mark("Undesired syntax for COND statement");
				return NIL;
			}
			if (memory_table[memory_table[memory_table[root].lchild].rchild].rchild > NIL) {
				runtime_error.Mark("2 elements are expected for a pair of predicate and value but more than 2 elements were passed");
				return NIL;
			}
			if (Evaluate(memory_table[memory_table[root].lchild].lchild) == -TRUE) return Evaluate(memory_table[memory_table[memory_table[root].lchild].rchild].lchild);
		}
		root = memory_table[root].rchild;
		if (memory_table[root].lchild <= NIL) {
			runtime_error.Mark("Undesired syntax for COND statement");
			return NIL;
		}
		if (memory_table[memory_table[root].lchild].lchild != -ELSE) {
			runtime_error.Mark("There is no else statement to COND");
			return NIL;
		}
		if (memory_table[memory_table[memory_table[root].lchild].rchild].rchild > NIL) {
			runtime_error.Mark("2 elements are expected for a pair of predicate and value but more than 2 elements were passed");
			return NIL;
		}
		return Evaluate(memory_table[memory_table[memory_table[root].lchild].rchild].lchild);
	}
	if (function_hash == -NOT) {
		if (memory_table[memory_table[root].rchild].rchild > NIL) {
			runtime_error.Mark("1 argument is expected for NOT but more than 1 argument were passed");
			return NIL;
		}
		int arg_link = Evaluate(memory_table[memory_table[root].rchild].lchild);
		if (arg_link == -TRUE) return -FALSE;
		else if (arg_link == -FALSE) return -TRUE;
		else {
			runtime_error.Mark("The argument to NOT does not evaluate to a boolean value");
			return NIL;
		}
	}
	if (function_hash == -isNULL) {
		if (memory_table[memory_table[root].rchild].rchild > NIL) {
			runtime_error.Mark("1 argument is expected for isNULL but more than 1 argument were passed");
			return NIL;
		}
		if (Evaluate(memory_table[memory_table[root].rchild].lchild) == NIL) return -TRUE;
		else return -FALSE;
	}
	if (function_hash == -isEQ) {
		if (memory_table[memory_table[memory_table[root].rchild].rchild].rchild > NIL) {
			runtime_error.Mark("2 arguments are expected for isEQ but more than 2 arguments were passed");
			return NIL;
		}
		if (Evaluate(memory_table[memory_table[root].rchild].lchild) == Evaluate(memory_table[memory_table[memory_table[root].rchild].rchild].lchild)) return -TRUE;
		else return -FALSE;
	}
	if (function_hash == -isEQUAL) {
		if (memory_table[memory_table[memory_table[root].rchild].rchild].rchild > NIL) {
			runtime_error.Mark("2 arguments are expected for isEQUAL but more than 2 arguments were passed");
			return NIL;
		}
		if (StructureTest(Evaluate(memory_table[memory_table[root].rchild].lchild), Evaluate(memory_table[memory_table[memory_table[root].rchild].rchild].lchild))) return -TRUE;
		else return -FALSE;
	}
	if (function_hash == -isNUMBER) {
		if (memory_table[memory_table[root].rchild].rchild > NIL) {
			runtime_error.Mark("1 argument is expected for isNUMBER but more than 1 argument were passed");
			return NIL;
		}
		int arg_hash = Evaluate(memory_table[memory_table[root].rchild].lchild);
		if (arg_hash > NIL) return -FALSE;
		if (IsNumber(hash_table[-arg_hash].symbol)) return -TRUE;
		else return -FALSE;
	}
	if (function_hash == -isSYMBOL) {
		if (memory_table[memory_table[root].rchild].rchild > NIL) {
			runtime_error.Mark("1 argument is expected for isSYMBOL but more than 1 argument were passed");
			return NIL;
		}
		int arg_hash = Evaluate(memory_table[memory_table[root].rchild].lchild);
		if (arg_hash > NIL) return -FALSE;
		if (-arg_hash < NUM_OF_SPECIAL_SYMBOL) return -FALSE;
		if (IsNumber(hash_table[-arg_hash].symbol)) return -FALSE;
		else return -TRUE;
	}
	if (function_hash == -isPAIR) {
		if (memory_table[memory_table[root].rchild].rchild > NIL) {
			runtime_error.Mark("1 argument is expected for isPAIR but more than 1 argument were passed");
			return NIL;
		}
		int arg_link = Evaluate(memory_table[memory_table[root].rchild].lchild);
		if (arg_link <= 0) return -FALSE;
		if (memory_table[arg_link].lchild == -LAMBDA) return -FALSE;
		return -TRUE;
	}
	runtime_error.Mark("You tried to call a symbol as a function");
	return NIL;
}

int main() {
	InitializeSpecialSymbol();
	Initialize();
	while (true) {
		ReadLine();
		buf.str(Preprocess());
		buf.clear();

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