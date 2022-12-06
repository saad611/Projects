#include <iostream>
#include <limits.h>
#include <cstdlib>

using namespace std;

int main()
{
    
    int arr[6]= {-5,1,2,3,4,5};
    int closest_to_zero =0;
    int index=0;
    
    
    for(int i =0; i<6;i++)
    {
        if(abs(arr[i])<abs(arr[closest_to_zero]))
        {
           closest_to_zero =i;
        }
    }
    
    
    cout<<" "<< arr[closest_to_zero] << " "<< endl;

    return 0;
}