/*
TODO
1. runtime_error to try-throw-catch system
2. separate boolean values from pre-defined keywords
*/
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
		return;
	}
	void Mark(std::string str) {
		occur = true;
		message << str << std::endl;
		return;
	}
};
UserDefinedException runtime_error;

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
	T& operator[](int index) {
		return stack[index];
	}
	const T& operator[](int index) const {
		return stack[index];
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
		return;
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
	void Print() {
		for (int i = 0; i <= top; ++i) std::cout << stack[i] << std::endl;
		std::cout << std::endl;
		return;
	}
};
class Node {
public:
	int lchild;
	int rchild;
	bool being_used;
	Node() : lchild(0), rchild(0), being_used(false) {}
	Node(const Node& ref) : lchild(ref.lchild), rchild(ref.rchild), being_used(ref.being_used) {}
	~Node() {}
	Node& operator=(const Node& ref) {
		lchild = ref.lchild;
		rchild = ref.rchild;
		being_used = ref.being_used;
		return *this;
	}
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
	int link;
	Symbol() : symbol(std::string()), link(0) {}
	Symbol(const Symbol& ref) : symbol(ref.symbol), link(ref.link) {}
	~Symbol() {}
	Symbol& operator=(const Symbol& ref) {
		symbol = ref.symbol;
		link = ref.link;
		return *this;
	}
	static std::string StringOfSymbol(const Symbol& entry) {
		std::stringstream temp;
		temp << CENTER_ALIGN_STR(entry.symbol, WIDTH);
		temp << COL_DELIM;
		if (entry.link == 0) temp << CENTER_ALIGN_STR(NIL_SYMBOL, WIDTH);
		else temp << CENTER_ALIGN_INT(entry.link, WIDTH);
		return temp.str();
	}
};
class Pair {
public:
	int hash;
	int link;
	Pair() : hash(0), link(0) {}
	Pair(int hash, int link) : hash(hash), link(link) {}
	Pair(const Pair& ref) : hash(ref.hash), link(ref.link) {}
	~Pair() {}
	Pair& operator=(const Pair& ref) {
		hash = ref.hash;
		link = ref.link;
		return *this;
	}
};

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
class MemoryTable {
public:
	Node* memory_table;
	int size;
	MemoryTable(int size = 30) : memory_table(new Node[size]), size(size) {}
	MemoryTable(const MemoryTable& ref) : memory_table(new Node[ref.size]), size(ref.size) {
		for (int i = 0; i < size; ++i) memory_table[i] = ref.memory_table[i];
	}
	MemoryTable(MemoryTable&& ref) noexcept : memory_table(ref.memory_table), size(ref.size) {
		ref.memory_table = nullptr;
		ref.size = 0;
	}
	~MemoryTable() {
		delete[] memory_table;
	}
	MemoryTable& operator=(const MemoryTable& ref) {
		delete[] memory_table;
		memory_table = new Node[ref.size];
		size = ref.size;
		for (int i = 0; i < size; ++i) memory_table[i] = ref.memory_table[i];
		return *this;
	}
	MemoryTable& operator=(MemoryTable&& ref) noexcept {
		delete[] memory_table;
		memory_table = ref.memory_table;
		size = ref.size;
		ref.memory_table = nullptr;
		ref.size = 0;
		return *this;
	}
	Node& operator[](int index) {
		return memory_table[index];
	}
	const Node& operator[](int index) const {
		return memory_table[index];
	}
	Node& at(int index) {
		return memory_table[index];
	}
	const Node& at(int index) const {
		return memory_table[index];
	}
};
class HashTable {
public:
	Symbol* hash_table;
	int size;
	HashTable(int size = NUM_OF_SPECIAL_SYMBOL + 997) : hash_table(new Symbol[size]), size(size) {}
	HashTable(const HashTable& ref) : hash_table(new Symbol[ref.size]), size(ref.size) {
		for (int i = 0; i < size; ++i) hash_table[i] = ref.hash_table[i];
	}
	~HashTable() {
		delete[] hash_table;
	}
	HashTable& operator=(const HashTable& ref) {
		delete[] hash_table;
		hash_table = new Symbol[ref.size];
		size = ref.size;
		for (int i = 0; i < size; ++i) hash_table[i] = ref.hash_table[i];
		return *this;
	}
	Symbol& operator[](int index) {
		return hash_table[index];
	}
	const Symbol& operator[](int index) const {
		return hash_table[index];
	}
	Symbol& at(int index) {
		return hash_table[index];
	}
	const Symbol& at(int index) const {
		return hash_table[index];
	}
};

class Interpreter {
public:
	static std::string* special_symbol;
	MemoryTable memory_table;
	HashTable hash_table;
	int free_list;
	std::stringstream buffer;
	Stack<Pair> function_stack;
	Stack<int> command_stack;
	Stack<int> evaluation_stack;
	Interpreter(int memory_table_size = 30, int hash_table_size = NUM_OF_SPECIAL_SYMBOL + 997) :
		memory_table(MemoryTable(memory_table_size)),
		hash_table(HashTable(hash_table_size)),
		free_list(1),
		buffer(std::stringstream()),
		function_stack(Stack<Pair>()),
		command_stack(Stack<int>()),
		evaluation_stack(Stack<int>()) {
		for (int i = 1; i < memory_table.size - 1; ++i) memory_table[i].rchild = i + 1;
		for (int i = 0; i < NUM_OF_SPECIAL_SYMBOL; ++i) hash_table[i].symbol = special_symbol[i];
	}
	~Interpreter() {}

	static void InitializeSpecialSymbol() {
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
	static void ASCIILower(std::string& str) {
		for (auto iter = str.begin(); iter != str.end(); ++iter) {
			if (*iter >= 'A' && *iter <= 'Z') {
				*iter += 'a' - 'A';
			}
		}
		return;
	}
	// return 0 if non-numeric, 1 if int, 2 if double
	static int IsNumber(std::string token) {
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

	int Allocate() {
		if (free_list == NIL) GarbageCollect();
		if (free_list == NIL) {
			MemoryTable doubled_table(2 * memory_table.size);
			for (int i = 0; i < memory_table.size; ++i) doubled_table[i] = memory_table[i];
			for (int i = memory_table.size; i < 2 * memory_table.size - 1; ++i) doubled_table[i].rchild = i + 1;
			free_list = memory_table.size;
			memory_table = std::move(doubled_table);
		}
		int temp = free_list;
		free_list = memory_table[free_list].rchild;
		return temp;
	}
	void GarbageCollect() {
		for (int i = 1; i < memory_table.size; ++i) memory_table[i].being_used = false;
		for (int i = 0; i < hash_table.size; ++i) MarkBeingUsed(hash_table[i].link);
		for (int i = 0; i <= function_stack.top; ++i) MarkBeingUsed(function_stack[i].link);
		for (int i = 0; i <= command_stack.top; ++i) MarkBeingUsed(command_stack[i]);
		for (int i = 0; i <= evaluation_stack.top; ++i) MarkBeingUsed(evaluation_stack[i]);
		for (int i = 1; i < memory_table.size; ++i) {
			if (!memory_table[i].being_used) {
				memory_table[i].lchild = NIL;
				memory_table[i].rchild = free_list;
				free_list = i;
			}
		}
		return;
	}
	void MarkBeingUsed(int root) {
		if (root <= 0) return;
		memory_table[root].being_used = true;
		MarkBeingUsed(memory_table[root].lchild);
		MarkBeingUsed(memory_table[root].rchild);
		return;
	}
	// Bottom-up deallocation
	void Deallocate(int root) {
		if (root <= 0) return;
		Deallocate(memory_table[root].rchild);
		if (memory_table[root].lchild > 0) Deallocate(memory_table[root].lchild);
		memory_table[root].lchild = NIL;
		memory_table[root].rchild = free_list;
		free_list = root;
		return;
	}
	/*
	// Top-down deallocation
	void Deallocate(int root) {
		if (root <= 0) return;
		while (root != NIL) {
			int lchild = memory_table[root].lchild;
			int rchild = memory_table[root].rchild;
			memory_table[root].lchild = NIL;
			memory_table[root].rchild = free_list;
			free_list = root;
			Deallocate(lchild);
			root = rchild;
		}
		return;
	}
	*/
	/*
	// Bottom-up deallocation
	void DeallocateExceptLchild(int root) {
		if (root <= 0) return;
		DeallocateExceptLchild(memory_table[root].rchild);
		memory_table[root].lchild = NIL;
		memory_table[root].rchild = free_list;
		free_list = root;
		return;
	}
	*/
	// Top-down deallocation
	void DeallocateExceptLchild(int root) {
		if (root <= 0) return;
		while (root != NIL) {
			memory_table[root].lchild = NIL;
			int temp = memory_table[root].rchild;
			memory_table[root].rchild = free_list;
			free_list = root;
			root = temp;
		}
		return;
	}

	void ReadLine() {
		std::cout << "> ";
		std::string temp;
		std::getline(std::cin, temp);
		buffer.str(temp + std::string(" "));
		buffer.clear();
		return;
	}
	std::string GetNextToken() {
		std::string temp;
		if (buffer >> temp) {
			ASCIILower(temp);
			size_t i = 0;
			for (; i < temp.size(); ++i) {
				if (temp.at(i) == '(') break;
				if (temp.at(i) == ')') break;
				if (temp.at(i) == '\'') break;
			}
			if (i > 0) {
				buffer.seekg(buffer.tellg() - std::streampos(temp.size() - i));
				return temp.substr(0, i);
			}
			else {
				buffer.seekg(buffer.tellg() - std::streampos(temp.size() - 1));
				return temp.substr(0, 1);
			}
		}
		else return std::string();
	}
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
	void PreprocessBuffer() {
		buffer.str(Preprocess());
		buffer.clear();
		return;
	}

	int Read(bool tail = false) {
		int token_hash;
		if (!tail) {
			token_hash = GetHashValue(GetNextToken());
			if (token_hash != -LEFT_PAREN) return token_hash;
		}
		token_hash = GetHashValue(GetNextToken());
		if (token_hash == 0) {
			runtime_error.Mark("The number of left and right parentheses does not match");
			return NIL;
		}
		if (token_hash == -RIGHT_PAREN) return NIL;
		int root = Allocate();
		command_stack.Push(root);
		if (token_hash == -LEFT_PAREN) memory_table[root].lchild = Read(true);
		else memory_table[root].lchild = token_hash;
		memory_table[root].rchild = Read(true);
		return root;
	}
	int GetHashValue(std::string str) {
		for (int i = 0; i < NUM_OF_SPECIAL_SYMBOL; ++i) if (str == special_symbol[i]) return -i;
		if (IsNumber(str) == 0);
		else if (IsNumber(str) == 1) str = std::to_string(std::stoi(str));
		else str = std::to_string(std::stod(str));
		int str_hash = HashFunction(str);
		for (; !hash_table[str_hash].symbol.empty() && hash_table[str_hash].symbol != str; ) {
			if (str_hash < hash_table.size - 1) ++str_hash;
			else str_hash = NUM_OF_SPECIAL_SYMBOL;
		}
		if (hash_table[str_hash].symbol.empty()) {
			hash_table[str_hash].symbol = str;
		}
		return -str_hash;
	}
	int HashFunction(std::string str) {
		int sum = 0;
		for (size_t i = 0; i < str.size(); ++i) {
			sum += str.at(i);
		}
		return NUM_OF_SPECIAL_SYMBOL + sum % (hash_table.size - NUM_OF_SPECIAL_SYMBOL);
	}

	int Evaluate(int root) {
		if (root <= 0) {
			if (-root < NUM_OF_SPECIAL_SYMBOL) return root;
			if (IsNumber(hash_table[-root].symbol)) return root;
			return hash_table[-root].link;
		}
		int function_link = Evaluate(memory_table[root].lchild);
		if (function_link > 0) {
			if (memory_table[function_link].lchild != -LAMBDA) {
				runtime_error.Mark("You tried to call a list as a function");
				return NIL;
			}
			function_link = Evaluate(function_link); // to check whether it fits the criteria of LAMBDA function
			if (runtime_error.occur) return NIL;
			int evaluated_argument_list = EvaluateArgumentList(memory_table[root].rchild);
			int number_of_param = 0;
			Substitute(memory_table[memory_table[function_link].rchild].lchild, evaluated_argument_list, number_of_param);
			evaluation_stack.Clear();
			DeallocateExceptLchild(evaluated_argument_list);
			if (runtime_error.occur) {
				Unsubstitute(number_of_param);
				return NIL;
			}
			int result = Evaluate(memory_table[memory_table[memory_table[function_link].rchild].rchild].lchild);
			Unsubstitute(number_of_param);
			return result;
		}
		if (function_link == -QUOTE) {
			if (memory_table[memory_table[root].rchild].rchild > 0) {
				runtime_error.Mark("1 argument is expected for QUOTE but more than 1 argument were passed");
				return NIL;
			}
			return memory_table[memory_table[root].rchild].lchild;
		}
		if (function_link == -DEFINE) {
			if (memory_table[memory_table[memory_table[root].rchild].rchild].rchild > 0) {
				runtime_error.Mark("2 arguments are expected for DEFINE but more than 2 arguments were passed");
				return NIL;
			}
			int arg_hash = memory_table[memory_table[root].rchild].lchild;
			if (arg_hash > 0) {
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
			hash_table[-arg_hash].link = Evaluate(memory_table[memory_table[memory_table[root].rchild].rchild].lchild);
			return hash_table[-arg_hash].link;
		}
		if (function_link == -LAMBDA) {
			if (memory_table[memory_table[memory_table[root].rchild].rchild].rchild > 0) {
				runtime_error.Mark("2 arguments are expected for LAMBDA but more than 2 arguments were passed");
				return NIL;
			}
			if (memory_table[memory_table[root].rchild].lchild < 0) {
				runtime_error.Mark("A list of arguments should be passed to a LAMBDA function");
				return NIL;
			}
			return root;
		}
		if (function_link == -LIST) {
			if (memory_table[memory_table[root].rchild].rchild > 0) {
				runtime_error.Mark("1 argument is expected for LIST but more than 1 argument were passed");
				return NIL;
			}
			int arg_link = Evaluate(memory_table[memory_table[root].rchild].lchild);
			int root = Allocate();
			command_stack.Push(root);
			memory_table[root].lchild = arg_link;
			memory_table[root].rchild = NIL;
			return root;
		}
		if (function_link == -PLUS) {
			if (memory_table[memory_table[memory_table[root].rchild].rchild].rchild > 0) {
				runtime_error.Mark("2 arguments are expected for PLUS but more than 2 arguments were passed");
				return NIL;
			}
			int arg1_link = Evaluate(memory_table[memory_table[root].rchild].lchild);
			int arg2_link = Evaluate(memory_table[memory_table[memory_table[root].rchild].rchild].lchild);
			if (arg1_link > 0 || arg2_link > 0) {
				runtime_error.Mark("You tried to add a list which is not a number");
				return NIL;
			}
			std::string arg1_symbol = hash_table[-arg1_link].symbol;
			std::string arg2_symbol = hash_table[-arg2_link].symbol;
			int arg1_isnum = IsNumber(arg1_symbol);
			int arg2_isnum = IsNumber(arg2_symbol);
			if (!arg1_isnum || !arg2_isnum) {
				runtime_error.Mark("You tried to add a non-numeric variable");
				return NIL;
			}
			if (arg1_isnum == 1 && arg2_isnum == 1) return GetHashValue(std::to_string(std::stoi(arg1_symbol) + std::stoi(arg2_symbol)));
			return GetHashValue(std::to_string(std::stod(arg1_symbol) + std::stod(arg2_symbol)));
		}
		if (function_link == -MINUS) {
			if (memory_table[memory_table[memory_table[root].rchild].rchild].rchild > 0) {
				runtime_error.Mark("2 arguments are expected for MINUS but more than 2 arguments were passed");
				return NIL;
			}
			int arg1_link = Evaluate(memory_table[memory_table[root].rchild].lchild);
			int arg2_link = Evaluate(memory_table[memory_table[memory_table[root].rchild].rchild].lchild);
			if (arg1_link > 0 || arg2_link > 0) {
				runtime_error.Mark("You tried to subtract a list which is not a number");
				return NIL;
			}
			std::string arg1_symbol = hash_table[-arg1_link].symbol;
			std::string arg2_symbol = hash_table[-arg2_link].symbol;
			int arg1_isnum = IsNumber(arg1_symbol);
			int arg2_isnum = IsNumber(arg2_symbol);
			if (!arg1_isnum || !arg2_isnum) {
				runtime_error.Mark("You tried to subtract a non-numeric variable");
				return NIL;
			}
			if (arg1_isnum == 1 && arg2_isnum == 1) return GetHashValue(std::to_string(std::stoi(arg1_symbol) - std::stoi(arg2_symbol)));
			return GetHashValue(std::to_string(std::stod(arg1_symbol) - std::stod(arg2_symbol)));
		}
		if (function_link == -TIMES) {
			if (memory_table[memory_table[memory_table[root].rchild].rchild].rchild > 0) {
				runtime_error.Mark("2 arguments are expected for TIMES but more than 2 arguments were passed");
				return NIL;
			}
			int arg1_link = Evaluate(memory_table[memory_table[root].rchild].lchild);
			int arg2_link = Evaluate(memory_table[memory_table[memory_table[root].rchild].rchild].lchild);
			if (arg1_link > 0 || arg2_link > 0) {
				runtime_error.Mark("You tried to multiply a list which is not a number");
				return NIL;
			}
			std::string arg1_symbol = hash_table[-arg1_link].symbol;
			std::string arg2_symbol = hash_table[-arg2_link].symbol;
			int arg1_isnum = IsNumber(arg1_symbol);
			int arg2_isnum = IsNumber(arg2_symbol);
			if (!arg1_isnum || !arg2_isnum) {
				runtime_error.Mark("You tried to multiply a non-numeric variable");
				return NIL;
			}
			if (arg1_isnum == 1 && arg2_isnum == 1) return GetHashValue(std::to_string(std::stoi(arg1_symbol) * std::stoi(arg2_symbol)));
			return GetHashValue(std::to_string(std::stod(arg1_symbol) * std::stod(arg2_symbol)));
		}
		if (function_link == -DIVIDE) {
			if (memory_table[memory_table[memory_table[root].rchild].rchild].rchild > 0) {
				runtime_error.Mark("2 arguments are expected for DIVIDE but more than 2 arguments were passed");
				return NIL;
			}
			int arg1_link = Evaluate(memory_table[memory_table[root].rchild].lchild);
			int arg2_link = Evaluate(memory_table[memory_table[memory_table[root].rchild].rchild].lchild);
			if (arg1_link > 0 || arg2_link > 0) {
				runtime_error.Mark("You tried to divide (by) a list which is not a number");
				return NIL;
			}
			std::string arg1_symbol = hash_table[-arg1_link].symbol;
			std::string arg2_symbol = hash_table[-arg2_link].symbol;
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
		if (function_link == -NOT) {
			if (memory_table[memory_table[root].rchild].rchild > 0) {
				runtime_error.Mark("1 argument is expected for NOT but more than 1 argument were passed");
				return NIL;
			}
			int arg_link = Evaluate(memory_table[memory_table[root].rchild].lchild);
			if (arg_link == -TRUE) return -FALSE;
			else if (arg_link == -FALSE) return -TRUE;
			else {
				runtime_error.Mark("TYPE ERROR: The argument passed to NOT is not a boolean value");
				return NIL;
			}
		}
		if (function_link == -isNULL) {
			if (memory_table[memory_table[root].rchild].rchild > 0) {
				runtime_error.Mark("1 argument is expected for isNULL but more than 1 argument were passed");
				return NIL;
			}
			if (Evaluate(memory_table[memory_table[root].rchild].lchild) == NIL) return -TRUE;
			return -FALSE;
		}
		if (function_link == -isNUMBER) {
			if (memory_table[memory_table[root].rchild].rchild > 0) {
				runtime_error.Mark("1 argument is expected for isNUMBER but more than 1 argument were passed");
				return NIL;
			}
			int arg_link = Evaluate(memory_table[memory_table[root].rchild].lchild);
			if (arg_link > 0) return -FALSE;
			if (IsNumber(hash_table[-arg_link].symbol)) return -TRUE;
			return -FALSE;
		}
		if (function_link == -isSYMBOL) {
			if (memory_table[memory_table[root].rchild].rchild > 0) {
				runtime_error.Mark("1 argument is expected for isSYMBOL but more than 1 argument were passed");
				return NIL;
			}
			int arg_link = Evaluate(memory_table[memory_table[root].rchild].lchild);
			if (arg_link > 0) return -FALSE;
			if (-arg_link < NUM_OF_SPECIAL_SYMBOL) return -FALSE;
			if (IsNumber(hash_table[-arg_link].symbol)) return -FALSE;
			return -TRUE;
		}
		if (function_link == -isPAIR) {
			if (memory_table[memory_table[root].rchild].rchild > 0) {
				runtime_error.Mark("1 argument is expected for isPAIR but more than 1 argument were passed");
				return NIL;
			}
			int arg_link = Evaluate(memory_table[memory_table[root].rchild].lchild);
			if (arg_link <= 0) return -FALSE;
			if (memory_table[arg_link].lchild == -LAMBDA) return -FALSE;
			return -TRUE;
		}
		if (function_link == -isEQ) {
			if (memory_table[memory_table[memory_table[root].rchild].rchild].rchild > 0) {
				runtime_error.Mark("2 arguments are expected for isEQ but more than 2 arguments were passed");
				return NIL;
			}
			if (Evaluate(memory_table[memory_table[root].rchild].lchild) == Evaluate(memory_table[memory_table[memory_table[root].rchild].rchild].lchild)) return -TRUE;
			return -FALSE;
		}
		if (function_link == -isEQUAL) {
			if (memory_table[memory_table[memory_table[root].rchild].rchild].rchild > 0) {
				runtime_error.Mark("2 arguments are expected for isEQUAL but more than 2 arguments were passed");
				return NIL;
			}
			if (StructureTest(Evaluate(memory_table[memory_table[root].rchild].lchild), Evaluate(memory_table[memory_table[memory_table[root].rchild].rchild].lchild))) return -TRUE;
			return -FALSE;
		}
		if (function_link == -IF) {
			if (memory_table[memory_table[memory_table[memory_table[root].rchild].rchild].rchild].rchild > 0) {
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
		if (function_link == -COND) {
			if (memory_table[root].rchild == NIL) {
				runtime_error.Mark("There is no argument to COND");
				return NIL;
			}
			while (memory_table[memory_table[root].rchild].rchild > 0) {
				root = memory_table[root].rchild;
				if (memory_table[root].lchild <= 0) {
					runtime_error.Mark("Undesired syntax for COND statement");
					return NIL;
				}
				if (memory_table[memory_table[memory_table[root].lchild].rchild].rchild > 0) {
					runtime_error.Mark("2 elements are expected for a pair of predicate and value but more than 2 elements were passed");
					return NIL;
				}
				if (Evaluate(memory_table[memory_table[root].lchild].lchild) == -TRUE) return Evaluate(memory_table[memory_table[memory_table[root].lchild].rchild].lchild);
			}
			root = memory_table[root].rchild;
			if (memory_table[root].lchild <= 0) {
				runtime_error.Mark("Undesired syntax for COND statement");
				return NIL;
			}
			if (memory_table[memory_table[root].lchild].lchild != -ELSE) {
				runtime_error.Mark("There is no else statement to COND");
				return NIL;
			}
			if (memory_table[memory_table[memory_table[root].lchild].rchild].rchild > 0) {
				runtime_error.Mark("2 elements are expected for a pair of predicate and value but more than 2 elements were passed");
				return NIL;
			}
			return Evaluate(memory_table[memory_table[memory_table[root].lchild].rchild].lchild);
		}
		if (function_link == -CAR) {
			if (memory_table[memory_table[root].rchild].rchild > 0) {
				runtime_error.Mark("1 argument is expected for CAR but more than 1 argument were passed");
				return NIL;
			}
			int arg_link = Evaluate(memory_table[memory_table[root].rchild].lchild);
			if (arg_link <= 0) {
				runtime_error.Mark("The argument to CAR is not the correct type");
				return NIL;
			}
			return memory_table[arg_link].lchild;
		}
		if (function_link == -CDR) {
			if (memory_table[memory_table[root].rchild].rchild > 0) {
				runtime_error.Mark("1 argument is expected for CDR but more than 1 argument were passed");
				return NIL;
			}
			int arg_link = Evaluate(memory_table[memory_table[root].rchild].lchild);
			if (arg_link <= 0) {
				runtime_error.Mark("The argument to CDR is not the correct type");
				return NIL;
			}
			return memory_table[arg_link].rchild;
		}
		if (function_link == -CONS) {
			if (memory_table[memory_table[memory_table[root].rchild].rchild].rchild > 0) {
				runtime_error.Mark("2 arguments are expected for CONS but more than 2 arguments were passed");
				return NIL;
			}
			int arg1_link = Evaluate(memory_table[memory_table[root].rchild].lchild);
			int arg2_link = Evaluate(memory_table[memory_table[memory_table[root].rchild].rchild].lchild);
			if (arg2_link < 0) {
				runtime_error.Mark("The second argument to CONS cannot be a symbol");
				return NIL;
			}
			int root = Allocate();
			command_stack.Push(root);
			memory_table[root].lchild = arg1_link;
			memory_table[root].rchild = arg2_link;
			return root;
		}
		runtime_error.Mark("You tried to call a symbol as a function");
		return NIL;
	}
	bool StructureTest(int link1, int link2) {
		if (link1 <= 0) {
			if (link2 <= 0) {
				if (link1 == link2) return true;
				else return false;
			}
			else return false;
		}
		else {
			if (link2 <= 0) return false;
			else {
				if (!StructureTest(memory_table[link1].lchild, memory_table[link2].lchild)) return false;
				if (StructureTest(memory_table[link1].rchild, memory_table[link2].rchild)) return true;
				return false;
			}
		}
	}
	int EvaluateArgumentList(int argument_list) {
		if (argument_list == NIL) return NIL;
		int root = Allocate();
		evaluation_stack.Push(root);
		memory_table[root].lchild = Evaluate(memory_table[argument_list].lchild);
		memory_table[root].rchild = EvaluateArgumentList(memory_table[argument_list].rchild);
		return root;
	}
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
		if (parameter > 0) {
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
		function_stack.Push(Pair(parameter, hash_table[-parameter].link));
		hash_table[-parameter].link = memory_table[argument_list].lchild;
		++number_of_param;
		Substitute(memory_table[parameter_list].rchild, memory_table[argument_list].rchild, number_of_param);
		return;
	}
	void Unsubstitute(int number_of_param) {
		for (int i = 0; i < number_of_param; ++i) {
			Pair pair = function_stack.Pop();
			hash_table[-pair.hash].link = pair.link;
		}
		return;
	}

	std::string StringOfData(int root, bool list_paren = true) {
		if (root == 0) return "()";
		if (root < 0) return hash_table[-root].symbol;
		std::stringstream temp;
		if (list_paren) temp << '(';
		temp << StringOfData(memory_table[root].lchild);
		if (memory_table[root].rchild == NIL) temp << ')';
		else {
			temp << ' ';
			temp << StringOfData(memory_table[root].rchild, false);
		}
		return temp.str();
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
		std::cout << "Memory table size = " << memory_table.size << std::endl;
		std::cout << "Memory table = " << std::endl;
		std::cout << ROW_DELIM(ROW_WIDTH) << std::endl;
		for (int i = 0; i < memory_table.size; ++i) {
			std::cout << COL_DELIM << CENTER_ALIGN_INT(i, INDEX_WIDTH) << COL_DELIM;
			std::cout << Node::StringOfNode(memory_table[i]) << COL_DELIM << std::endl;
			std::cout << ROW_DELIM(ROW_WIDTH) << std::endl;
		}
		std::cout << std::endl;
		std::cout << "Hash table size = " << hash_table.size << std::endl;
		std::cout << "Hash table = " << std::endl;
		std::cout << ROW_DELIM(ROW_WIDTH) << std::endl;
		for (int i = 1; i < hash_table.size; ++i) {
			if (hash_table[i].symbol != std::string()) {
				std::cout << COL_DELIM << CENTER_ALIGN_INT(i, INDEX_WIDTH) << COL_DELIM;
				std::cout << Symbol::StringOfSymbol(hash_table[i]) << COL_DELIM << std::endl;
				std::cout << ROW_DELIM(ROW_WIDTH) << std::endl;
			}
		}
		std::cout << std::endl;
		return;
	}
};
std::string* Interpreter::special_symbol = new std::string[NUM_OF_SPECIAL_SYMBOL];

int main() {
	Interpreter::InitializeSpecialSymbol();
	Interpreter scheme(10);
	while (true) {
		scheme.ReadLine();
		scheme.PreprocessBuffer();

		int root = scheme.Read();
		if (runtime_error.occur) {
			std::cout << "] ";
			scheme.PrintFreeList();
			scheme.PrintRootOfList(root);
			std::cout << std::endl;
			scheme.PrintMemory();

			std::cout << "Preprocessd: " << std::endl;
			std::cout << scheme.buffer.str() << std::endl << std::endl;
			std::cout << "Command interpreted: " << std::endl;
			std::cout << scheme.StringOfData(root) << std::endl << std::endl;
			std::cout << "Command Stack: " << std::endl;
			scheme.command_stack.Print();
			scheme.command_stack.Clear();

			std::cout << runtime_error.message.str() << std::endl;
			scheme.Deallocate(root);
			runtime_error.Clear();
			continue;
		}

		int result = scheme.Evaluate(root);
		if (runtime_error.occur) {
			std::cout << "] ";
			scheme.PrintFreeList();
			scheme.PrintRootOfList(root);
			std::cout << std::endl;
			scheme.PrintMemory();

			std::cout << "Preprocessd: " << std::endl;
			std::cout << scheme.buffer.str() << std::endl << std::endl;
			std::cout << "Command interpreted: " << std::endl;
			std::cout << scheme.StringOfData(root) << std::endl << std::endl;
			std::cout << "Command Stack: " << std::endl;
			scheme.command_stack.Print();
			scheme.command_stack.Clear();

			std::cout << runtime_error.message.str() << std::endl;
			scheme.Deallocate(root);
			runtime_error.Clear();
			continue;
		}

		std::cout << "] ";
		scheme.PrintFreeList();
		scheme.PrintRootOfList(root);
		std::cout << std::endl;
		scheme.PrintMemory();

		std::cout << "Preprocessd: " << std::endl;
		std::cout << scheme.buffer.str() << std::endl << std::endl;
		std::cout << "Command interpreted: " << std::endl;
		std::cout << scheme.StringOfData(root) << std::endl << std::endl;
		std::cout << "Result: " << std::endl;
		std::cout << scheme.StringOfData(result) << std::endl << std::endl;

		std::cout << "Command Stack: " << std::endl;
		scheme.command_stack.Print();
		scheme.command_stack.Clear();
	}
	return 0;
}