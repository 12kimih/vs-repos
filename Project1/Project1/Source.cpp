// Implementation:
// recursive merge sort
// iterative merge sort
// recursive heap sort
// iterative heap sort

#include <iostream>
#include <cstdlib>
#include <ctime>

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

int main() {
	int n;
	std::cin >> n;
	int* arr = new int[n];
	std::srand(static_cast<unsigned int>(std::time(nullptr)));
	for (int i = 0; i < n; ++i) {
		arr[i] = std::rand() % n;
	}
	
	std::cout << "Before operation: ";
	print_sorted(is_sorted_asc(arr, n));
	print_arr(arr, n);

	MergeSort_ITER(arr, 0, n - 1);

	std::cout << "After operation: ";
	print_sorted(is_sorted_asc(arr, n));
	print_arr(arr, n);

	return 0;
}