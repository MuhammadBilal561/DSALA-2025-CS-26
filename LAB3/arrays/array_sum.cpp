#include <iostream>
using namespace std;

int main() {
    int size;
    cout << "How many elements in the array? ";
    cin >> size;

    if (size <= 0) {
        cout << "Sum is 0." << endl;
        return 0;
    }

    int numbers[size];
    int totalSum = 0;

    cout << "Enter " << size << " elements: " << endl;
    for (int i = 0; i < size; i++) {
        cin >> numbers[i];
        totalSum += numbers[i];
    }

    cout << "Total sum of all elements: " << totalSum << endl;

    return 0;
}
