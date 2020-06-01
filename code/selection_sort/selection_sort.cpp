#include <iostream>

using std::cout;
using std::endl;
using std::swap;

template <typename T>
void selection_sort(T* arr, int N) {
    for (int i = 0; i < N - 1; i++) {
        int smallest_index = i;
        // Finds the smallest value in an array
        for (int j = i + 1; j < N; j++) {
            if (arr[j] < arr[smallest_index]) {
                smallest_index = j;
            }
        }
        swap(arr[i], arr[smallest_index]);
    }
}

int main() {
    const int size = 8;
    double arr[size] = {1.2, 1.0, 3, 0, -1, 0.5, 100, -99};
    selection_sort(arr, size);

    cout << "Sorted array: ";
    for (auto num : arr) {
        cout << num << " ";
    }
    cout << endl;
}
