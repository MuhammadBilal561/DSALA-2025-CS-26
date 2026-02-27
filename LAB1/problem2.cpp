//2025(S)-CS-26
// Task is to print 2d array with populating specific values
#include <iostream>
using namespace std;

int main() {
    int arr[4][5];
    int value = 1;

    // main logic
    for(int j = 0; j < 5; j++) 
    {
        arr[0][j] = value++;
    }

    for(int i = 1; i < 3; i++) 
    {
        arr[i][4] = value++;
    }

    for(int j = 4; j >= 0; j--) 
    {
        arr[3][j] = value++;
    }

    for(int i = 2; i >= 1; i--) 
    {
        arr[i][0] = value++;
    }

    for(int j = 1; j < 4; j++) 
    {
        arr[1][j] = value++;
    }

    for(int j = 3; j > 0; j--)
     {
        arr[2][j] = value++;
     }
// Printing 
    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 5; j++)
            {
                cout << arr[i][j] << " ";
            }
        cout << endl;
    }

    return 0;
}