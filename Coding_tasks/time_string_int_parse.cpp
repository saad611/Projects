/******************************************************************************

                              Online C++ Compiler.
               Code, Compile, Run and Debug C++ program online.
Write your code in this editor and press "Run" button to compile and execute it.

*******************************************************************************/

#include <iostream>
#include <limits.h>
#include <cstdlib>

using namespace std;

int main()
{
    
    char time[] = "02:23:01AM";
    int hour;
    int min;
    int sec;
   sscanf (time,"%d:%d:%d",&hour,&min,&sec);

    cout << hour << " " << min << " " << sec << endl;
    return 0;
}
