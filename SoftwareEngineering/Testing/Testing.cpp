// Testing.cpp : Defines the entry point for the console application.
//
// This is just a testproject you can use for any small tests
//

#include "stdafx.h"

using namespace std;

int main()   
{

    cout << "Test Start" << endl;

    // find possible screen sizes for 11x16 pixel letters with 16/9 ratio
    int lw = 11;
    int lh = 16;
    int rw = 5;
    int rh = 4;
    float r = (float)rw / rh;

    for (float sw = lw; sw < 1920; sw += lw)
    {
        float sh = sw / r;

        if ((sh / lh) != int(sh / lh))
            continue;
        cout << "Possible resolution: " << sw << "x" << sh;
        cout << " has " << sw / lw << "x" << sh / lh << " chars" << endl;
    }

    cout << "Test End!" << endl;

    system("pause");

    return 0;
}

