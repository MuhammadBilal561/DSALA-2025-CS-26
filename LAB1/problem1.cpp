// 2025(S)-CS-26
#include <iostream>
using namespace std;

int main() {
    int n;
    cout << "Enter size: ";
    cin >> n;

    for(int row = 1; row <= 2*n-1; row++) 
    {
        
        int stars;
        if(row <= n)
         {
            stars = row;          
        } else
         {
            stars = 2*n - row;    
        }
        
        int spaces = 2 * (n - stars);   
        
        for(int j = 1; j <= stars; j++) 
        {
            cout << "*";
        }
        
        for(int j = 1; j <= spaces; j++) 
        {
            cout << " ";
        }
        
        for(int j = 1; j <= stars; j++) 
        {
            cout << "*";
        }
        cout << endl;
    }

    return 0;
}