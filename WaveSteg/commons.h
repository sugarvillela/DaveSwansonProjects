#pragma once

#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include<sstream>
#include <ctime>//for rand
#include<cstdlib>
#include<limits>//for INT_MAX
#include <sys/stat.h>//for findFileSize() on unopened file
using namespace std;

const static int INT_MAX = 0x7FFFFFFF;

//user interfaces
bool ask(string prompt, char y);
char getCharInput();
bool goodRange(int test, int loBound, int hiBound);
int getIntInput(int loBound, int hiBound);
int getIntInput();
string getStringInput();
string getStringInput(bool &done);
string enterFileName(string prompt);
string enterFileName(string prompt, string extension);
bool valFileExt(string fileName);
//useful stuff
string curTime();
string findClassName(string constructr);
unsigned int tStrToInt(string in);
bool endsWith(string haystack, string needle);
string reverse(string);
string swap(string);
bool inArray(int * nums, int size, int n);
//file stuff
int findFileSize(string fileName);
int findFileSize(const char* fName);
bool getExtension(string in, string &ext);
string killExtension(string in);
//mathy stuff
int power(int base, int exp);
int abso(int val);
char charMask(int exp);
int intMask(int exp);
int fMask(int exp);//all F's
bool oppSigns(int a, int b);
int usedBits(int n);
//converters
char* toCString(string convertMe);
string intToString(int in);
void uintToCharArray(unsigned int uint, char* charArray, int size);
//cout displays
void dispVector(vector<int> &v, int line);
//void dispArray(string* s, int size, int line);
//void dispArray(int* n, int size, int line);
//void dispArray(float* n, int size, int line);
int chint(char ch);
void bar();
void bar(ofstream &fout);
//init arrays
//int* initArray(int size, int init);
//bool* initArray(int size, bool init);
int randToBinFileUQ(const char* fileName, int size, bool verbose);
void initVRand(vector<int> &v, int size);