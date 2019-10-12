// Implementation:
// recursive merge sort
// iterative merge sort
// recursive heap sort
// iterative heap sort

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <sstream>
#include <stdexcept>

bool is_sorted_asc(int* arr, int size) {
	for (int i = 0; i < size - 1; ++i) {
		if (arr[i] > arr[i + 1]) return false;
	}
	return true;
}

void print_sorted(bool is_sorted) {
	if (is_sorted) {
		std::cout << "Sorted" << std::endl;
	}
	else {
		std::cout << "Not sorted" << std::endl;
	}
	return;
}

inline int min(int num1, int num2) {
	return (num1 > num2) ? num2 : num1;
}

void Merge(int* arr, int start, int mid, int end) {
	int left_size = mid - start + 1;
	int right_size = end - mid;
	int* left_arr = new int[left_size];
	int* right_arr = new int[right_size];
	for (int i = 0; i < left_size; ++i) {
		left_arr[i] = arr[start + i];
	}
	for (int j = 0; j < right_size; ++j) {
		right_arr[j] = arr[mid + 1 + j];
	}
	
	int i = 0;
	int j = 0;
	for (int k = start; k <= end; ++k) {
		if (i < left_size && j < right_size) {
			if (left_arr[i] > right_arr[j]) {
				arr[k] = right_arr[j];
				++j;
			}
			else {
				arr[k] = left_arr[i];
				++i;
			}
		}
		else if (i >= left_size) {
			arr[k] = right_arr[j];
			++j;
		}
		else {
			arr[k] = left_arr[i];
			++i;
		}
	}

	delete[] left_arr;
	delete[] right_arr;
	return;
}

void MergeSort_REC(int* arr, int start, int end) {
	if (start >= end) return;
	int mid = (start + end) / 2;
	MergeSort_REC(arr, start, mid);
	MergeSort_REC(arr, mid + 1, end);
	Merge(arr, start, mid, end);
	return;
}

void MergeSort_ITER(int* arr, int start, int end) {
	int size = end - start + 1;
	for (int cur_size = 1; cur_size < size; cur_size *= 2) {
		for (int left = start; left <= end - cur_size; left += 2 * cur_size) {
			Merge(arr, left, left + cur_size - 1, min(left + 2 * cur_size - 1, end));
		}
	}
	return;
}

void print_arr(int* arr, int size) {
	int count = 0;
	for (int i = 0; i < size; ++i) {
		if (count == 10) {
			std::cout << std::endl;
			count = 0;
		}
		std::cout << arr[i] << ' ';
		++count;
	}
	std::cout << std::endl;
	return;
}

int IsNumber(std::string str) {
	std::stringstream temp(str);
	int integer;
	temp >> integer;
	std::cout << integer << std::endl;
	if (!temp.fail() && temp.eof()) return 1;
	temp.str(str);
	temp.clear();
	double floating_point;
	temp >> floating_point;
	std::cout << floating_point << std::endl;
	if(!temp.fail() && temp.eof()) return 2;
	return 0;
}

void PrintState(std::stringstream& buf) {
	std::cout << "state:" << std::endl;
	if (buf.good()) {
		std::cout << "goodbit" << std::endl;
	}
	if (buf.eof()) {
		std::cout << "eofbit" << std::endl;
	}
	if (buf.fail()) {
		std::cout << "failbit" << std::endl;
	}
	if (buf.bad()) {
		std::cout << "badbit" << std::endl;
	}
	std::cout << "state end" << std::endl << std::endl;
	return;
}

void PrintStringstream(std::stringstream& buf) {
	PrintState(buf);
	std::cout << "tellg(): " << buf.tellg() << std::endl;
	std::cout << "tellp(): " << buf.tellp() << std::endl;
	std::string temp(buf.str());
	std::cout << "----------------------" << std::endl;
	for (int i = 0; i < temp.size(); ++i) std::cout << temp.at(i) << std::endl;
	std::cout << "----------------------" << std::endl;
	return;
}

int quotient(int a, int b) {
	if (b == 0) {
		throw std::runtime_error("attempted to divide by zero");
	}
	return a / b;
}

int main() {
	int num1, num2, res;

	std::cout << "Enter two integers (EOF to end): " << std::endl;
	while (std::cin >> num1 >> num2) {
		try {
			res = quotient(num1, num2);
			std::cout << "The quotient is: " << res << std::endl;
		}
		catch (std::runtime_error& e) {
			std::cout << "Exception occurred: " << e.what() << std::endl;
		}
		std::cout << "Enter two integers (EOF to end): " << std::endl;
	}

	/*
	while (true) {
		std::string temp;
		std::getline(std::cin, temp);
		std::stringstream buf1;
		std::stringstream buf2;
		std::string temp2;

		buf1.str(temp + std::string(" "));
		std::cout << "===============buf1==================" << std::endl;
		PrintStringstream(buf1);

		buf2.str(temp);
		buf2 << ' ';
		std::cout << "===============buf2==================" << std::endl;
		PrintStringstream(buf2);
		buf2 >> temp2;
		std::cout << temp2 << std::endl;
		PrintStringstream(buf2);
		buf2 << "strong guy";
		PrintStringstream(buf2);
		buf2.str(temp);
		buf2.clear();
		PrintStringstream(buf2);
	}
	*/
	return 0;
}