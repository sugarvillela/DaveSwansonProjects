#include "commons.h"


//user interfaces
bool ask(string prompt, char y){//returns true for 'y', false for anything else
	cout << prompt;
	char ch = getCharInput();
	return (ch == y);
}
char getCharInput(){//Truncates any length > 1
	string userIn;
	getline(cin, userIn);
	return (userIn.length()>0)? userIn[0] : ' ';
}
bool goodRange(int test, int loBound, int hiBound){//Test between bound; displays warning if bad
	if(test < loBound || test > hiBound){
		cout << "Please type a number between " << loBound << " and " << hiBound <<endl;
		return false;
	}
	return true;
}
int getIntInput(int loBound, int hiBound){//Loops until valid input; catches out of range and non-numeric
	string in;
	int out;
	do{
		out = loBound -1;//set to invalid
		getline(cin, in);
		istringstream s;
		s.str(in);
		s >> out;//will ignore invalid input
	}
	while(!goodRange(out, loBound, hiBound));
	return out;
}
int getIntInput(){//Loops until valid input; catches non-numeric, no range
	string in;
	int out;
	do{
		out = INT_MAX;//set to invalid
		getline(cin, in);
		istringstream s;
		s.str(in);
		s >> out;//will ignore invalid input
	}
	while(out == INT_MAX);
	return out;
}
string getStringInput(){
	string in;
	getline(cin, in);
	return in;
}
string getStringInput(bool &done){
	string in;
	getline(cin, in);
	if(in == "done"){
		done=true;
		return "";
	}
	return in;
}
string enterFileName(string prompt, string extension){
	string userIn = "";
	do{
		cout << prompt;
		getline (cin, userIn);
	}
	while(!endsWith(userIn, extension));//userIn.find(".txt") == -1);
	return userIn;
}
//useful stuff
bool endsWith(string haystack, string needle){
	if (haystack.length() < needle.length()){
		return false;
	}
	return (haystack.compare(haystack.length()-needle.length(), needle.length(), needle) == 0);
}
string reverse(string normal){
	string rev(normal);
	reverse(rev.begin(), rev.end());
	return rev;
}
bool inArray(int * nums, int size, int n){//sequential search for unsorted array
	for(int i = 0; i<size; i++){
		if(nums[i] == n){
			return true;
		}
	}
	return false;
}
int usedBits(int n){
	int i=0;
	while(n>0){
		cout << n << endl;
		n/=2;
		i++;
	}
	return i;
}
//file stuff
int findFileSize(string fileName){
	//cout << "string version" << endl;
    struct stat results;
	const char* fName = fileName.c_str();
	if (stat(fName, &results) == 0){
		return results.st_size;
	}
	else{cout << "findFileSize() failed to open file :-(" << endl;}
	return 0;
}
int findFileSize(const char* fName){
	//cout << "char* version" << endl;
    struct stat results;
	if (stat(fName, &results) == 0){
		return results.st_size;
	}
	else{cout << "findFileSize() failed to open file :-(" << endl;}
	return 0;
}
bool getExtension(string in, string &ext){//true if looks like valid file name
	int pos = in.find_last_of(".");
	if(pos == -1){return false;}
	ext = in.substr(pos);
	return true;
}
string killExtension(string in){//no bool, just substr until last dot
	int pos = in.find_last_of(".");
	if(pos == -1){return in;}//on abort, return in
	return in.substr(0, pos);
}
//mathy stuff
int power(int base, int exp){
	if(exp == 0){return 1;}
	int pow = base;
	for(int i=1;i<exp;i++){
		pow *= base;
	}
	return pow;
}
int abso(int val){
	return (val>=0)? val : val*-1;
}
char charMask(int exp){//outputs 00000001, 00000010 etc. sets exp bit to 1, all others to 0
	if(exp == 0){return (char)1;}
	int base = 2;
	for(int i=1;i<exp;i++){
		base *= 2;
	}
	return (char)(base & 0xFF);
}
int fMask(int exp){//outputs all F's...00000011, 00000111 etc. sets bits to right of exp 1, including exp
	return power(2, exp) - 1;
}
int intMask(int exp){//outputs 00000011, 00000111 etc. sets all bits to right of exp 1, including exp
	return power(2, exp);
}
bool oppSigns(int a, int b){
	return(a>=0 && b<0)||(b>=0 && a<0);
}

//converters
char* toCString(string convertMe){
	int len = convertMe.length();
	char* out = new char[len];
	for(int i=0;i<len;i++){
		out[i] = convertMe.at(i);
		cout << out[i];
	}
	cout << endl;
	return out;
}
string intToString(int in){
    stringstream ss;
    ss << in;
    string s = ss.str();
    return s;
}
void uintToCharArray(unsigned int uint, char* charArray, int size){
	//cout << "\tuintToCharArray:  " << hex << uint << endl;
	for(int i=0;i<size;i++){
		//cout << hex << uint;
		charArray[i]= 0xFF & uint;
		uint /=256;
	}
}
//cout displays
void dispVector(vector<int> &v, int line){//
	int n = v.size();
	for(int i = 0; i<n; i++){
		cout << v[i] << "\t";
		if( (i+1)%line == 0 ){
			cout << endl;
		}
	}
}
void dispArray(string* s, int size, int line){//
	for(int i = 0; i<size; i++){
		cout << s[i] << "\t";
		if( (i+1)%line == 0 ){
			cout << endl;
		}
	}
	cout << endl;
}
void dispArray(int* n, int size, int line){//
	for(int i = 0; i<size; i++){
		cout << n[i] << "\t";
		if( (i+1)%line == 0 ){
			cout << endl;
		}
	}
	cout << endl;
}
void dispArray(float* n, int size, int line){//
	for(int i = 0; i<size; i++){
		cout << n[i] << "\t";
		if( (i+1)%line == 0 ){
			cout << endl;
		}
	}
	cout << endl;
}
int chint(char ch){
	return ch& 0xFF;
	//int n = ch;
	//return n & 0xFF;
}
//init arrays
int* initArray(int size, int init){//Allocates nums and initializes all elements to init
	int * nums = new int[size];
	for(int i = 0; i<size; i++){
		nums[i] = init;
	}
	return nums;
}
bool* initArray(int size, bool init){//Allocates bools and initializes all elements to init
	bool* bools = new bool[size];
	for(int i = 0; i<size; i++){
		bools[i] = init;
	}
	return bools;
}

int randToBinFileUQ(const char* fileName, int size, bool verbose){//random numbers to file; numbers are unique; random range > size
	srand(static_cast<unsigned int>(time(0)));
	ofstream fout;//out file
	fout.open(fileName, ios::binary |ios::ate);
	if(fout.fail()){
		cout << "Can't open output file" << endl;
		return 0;
	}
	fout.seekp(0, ios::beg);
	int * nums = initArray(size, -1);//array to log generated numbers to check for uniqueness, init to -1
	int i, r;// variables for index, random number
	const int sampleSize = 1;
	int max = sampleSize * 0xFF/2;
	int neg = -1;
	for(i = 0; i<size; i++){
		do{
			r = (rand()%max)*neg;
			cout << hex << r << endl;
		}
		while(false);//inArray(nums, size, r));//check for duplicate
		nums[i]=r;//disburse good number to array and file
		char charArray[sampleSize];
		uintToCharArray((unsigned int)r, charArray, sampleSize);
		fout.write(charArray, sampleSize);
		neg *= -1;
		//fout << r << endl;
	}
	if(verbose){
		cout << "To file" << hex << endl;
		//for(int i = 0; i<size; i++){
		//	int n = nums[i];// & intMask(sampleSize*8);
		//	cout << hex << n << "\t";
		//	if( (i+1)%10 == 0 ){
		//		cout << endl;
		//	}
		//}
		//cout << endl;
		dispArray(nums, size, 5);
	}
	delete[] nums;//Cleanup and return
	fout.close();
	return i;//returns bool false if size 0
}
void initVRand(vector<int> &v, int size){//
	srand(static_cast<unsigned int>(time(0)));
	for(int i = 0; i<size; i++){
		v.push_back(rand());
	}
}