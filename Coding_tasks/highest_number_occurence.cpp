/******************************************************************************

                              Online C++ Compiler.
               Code, Compile, Run and Debug C++ program online.
Write your code in this editor and press "Run" button to compile and execute it.

*******************************************************************************/

#include <iostream>
#include <limits.h>

using namespace std;

int main()
{
    
    int arr[5]= {1,1,3,4,5};
    int count = 1;
    int max_count =0;
    int index=0;
    
    
    for(int i =0; i<4;i++)
    {
        if(arr[i]==arr[i+1])
        {
           count++;
           if(count>max_count)
           {
               index=i;
               max_count=count;
           }
        }
    }
    
    
    cout<<" "<< arr[index] << " " << max_count<< endl;

    return 0;
}
