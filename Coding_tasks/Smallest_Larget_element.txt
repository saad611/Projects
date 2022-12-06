#include <iostream>
#include <limits.h>

using namespace std;

int main()
{
    
    int arr[5]= {-1,1,3,4,5};
    int small = INT_MAX;
    int large = INT_MIN;
    for(int i =0; i<5;i++)
    {
        if(small>arr[i])
        {
            small = arr[i];
        }
        
        if(large < arr[i])
        {
            large= arr[i];
        }
    }
    
    
    cout<<" "<< small << " "<< large << endl;

    return 0;
}
