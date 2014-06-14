#pragma once
#include "commons.h"

template<class T>
void dispArray(T* t, int size, int line){//
    for(int i = 0; i<size; i++){
        cout << t[i] << "\t";
        if( (i+1)%line == 0 ){
            cout << endl;
        }
    }
    cout << endl;
}
template<class T>
void dispNumberedList(T* t, int size, int offset){//
    for(int i = 0; i<size; i++){
        cout << i+offset << "\t";
        cout << t[i] << endl;
    }
    //cout << endl;
}
template<class T>
//init arrays
T* initArray(int size, T init){//Allocates arr and initializes all elements to init
    T * arr = new T[size];
    for(int i = 0; i<size; i++){
        arr[i] = init;
    }
    return arr;
}
