#include "helloworld.h"
#include <iostream>

using namespace std;

int main(){
    const int N = 16;

    char a[N] = "Hello \0\0\0\0\0\0";
    int b[N] = {15, 10, 6, 0, -11, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    cout <<  a << "-->" << endl;

    cuda_hello(a, b, N);

    cout <<  a << endl;
}
