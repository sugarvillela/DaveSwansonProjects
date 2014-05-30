#include "testers.h"
//#include "FileBitr.h"
//#include "commons.h"

using namespace std;

//int randToBinFileUQ(const char* fileName, int size, bool verbose);
int intBigEnd(char* charArray, int start, int length);
int uIntBigEnd(char* charArray, int start, int length);
void toIntTest();
int replaceLSBt(int n, bool bit){
	n &= 0xFFFFFFFE;
	//cout << ", p/mask=" << hex << n;
	n+= bit;
	return n;
}
bool nextLSBt(bool &lsb, vector<int> &v, int i){//sets lsb 1 or 0, based on lsb of nextSample()
	int len = v.size();
	if(i>=len){return false;}//this bool is to pass file state
	int n=v[i];
	lsb = true & n;//this bool is the return
	return true;//this bool is to pass file state
}
void testBiterators(){
	int i=0;
	bool bit;
	int paraSize = 0xabcd1234;
	cout << "original:  " << hex << paraSize << endl;
	vector<int> v;
	initVRand(v, 32*8);
	Bitr_int b(paraSize);
	while(b.nextBit(bit)){
		cout << bit;
		if( (i+1)%8 == 0){cout << endl;}
		v[i]=replaceLSBt(v[i], bit);
		i++;
	}
	Reshaper_int r;
	i=0;
	nextLSBt(bit, v, i);
	while(r.putBit(bit)){
		cout << bit;
		if( (i+1)%8 == 0){cout << endl;}
		i++;
		nextLSBt(bit, v, i);
	}
	int res = r.getResult();
	cout << "result:  " << res << endl;
	//int i=0;
	//bool bit;
	//string s = ".txt";
	//int len = s.length();
	//cout << "original:  " << s << ", length=" << len << endl;
	//vector<int> v;
	//initVRand(v, len*8);
	//Bitr_str b(s);
	//while(b.nextBit(bit)){
	//	cout << bit;
	//	if( (i+1)%8 == 0){cout << endl;}
	//	v[i]=replaceLSBt(v[i], bit);
	//	i++;

	//}
	//Reshaper_str r(len);
	//i=0;
	//nextLSBt(bit, v, i);
	//while(r.putBit(bit)){
	//	cout << bit;
	//	if( (i+1)%8 == 0){cout << endl;}
	//	i++;
	//	nextLSBt(bit, v, i);
	//}
	//string res = r.getResult();
	//len = res.length();
	//cout << "result:  " << res << ", length=" << len << endl;
}
void testFileBiterator(){
	FileItr f;
	f.initGet("nums.bin");
	int size = f.getInSize()*8;
	cout << "set vector size:  " << dec << size << endl;
	f.dispFile(0,55,5);
	vector<int> v;
	initVRand(v, size);

	int i=0;
	bool bit;
	f.clearBiterator();
	while(f.nextBit(bit)){
		cout << bit;
		if( (i+1)%8 == 0){cout << endl;}
		v[i]=replaceLSBt(v[i], bit);
		i++;
	}

	size = v.size();
	cout << "actual vector size:  " << size << endl;
	i=0;
	nextLSBt(bit, v, i);
	while(i<size){
		Reshaper_char r;
		while(r.putBit(bit)){
			cout << bit;
			if( (i+1)%8 == 0){cout << endl;}
			if( i >= size){
				cout << endl << "break inner loop" << endl;
				break;
			}
			i++;
			nextLSBt(bit, v, i);
		}
		char res = r.getResult();
		cout << "result:  " << (int)res << endl;
	}
}

int main(){
	//randToBinFileUQ("nums.bin", 10, false);
	//testBiterators();
	//testFileCopy();
	//testDispFile();
	//canonize("1Hz_Sine_4Sec.wav");
	//testDispFile();
	//testEncDec();
	testEncDec_string();
	//testWSVerify();
	//testWSCanonize();


	//{
	//	cout << "1Hz_Sine" << endl;
	//	WaveItr  w;
	//	w.initGet("1Hz_Sine.wav");
	//	w.dispHeader();
	//	//w.dispHeaderHex();
	//	//f.dispFile(0,0,5);
	//	w.dispFile(0, 48, 4);
	//}
	//{
	//	cout << "1Hz_Sine_4sec" << endl;
	//	WaveItr  w;
	//	w.initGet("1Hz_Sine_4sec.wav");
	//	w.dispHeader();
	//	//w.dispHeaderHex();
	//	//f.dispFile(0,0,5);
	//	w.dispFile(0, 48, 4);
	//}
	//testEncDec();
	//testWSVerify();



	//int sampSize = 2;
	//{
	//	FileItr f;
	//	f.initPut("nums2.bin", sampSize);
	//	f.copyTest("nums.bin");
	//}
	//{
	//	FileItr f;
	//	f.initGet("nums.bin", 1);
	//	//f.dispFile(0, 50, 5);
	//	f.replaceLSB_Test();
	//	//f.dispFile(0, 50, 5);
	//}
	return 0;
}
void iterateWave(){
	WaveItr w;
	w.initGet("1Hz_Sine.wav");
	if(w.good()){
		w.dispHeader();
		w.seekDataStart();
		int n, i = 0;
		while(w.nextSample(n)){
			if(i%1000 == 0){
				cout << dec << n << endl;
			}
			i++;
		}
	}
}
int intBigEnd(char* charArray, int start, int length){//reads c-string start to start+length, returns as signed int
	int n, sum = charArray[start+length-1];	//add char (byte) to LSB's of sum; int from char carries sign bit all the way to MSB
	for(int i=start+length-2;i>=start;i--){	//will fall through if length = 1
		sum <<= 8;							//shift one byte left to make room for next
		n = charArray[i];					//same as first step, but use temp variable n
		n &= 0xFF;							//adding less significant bits...carrying over the sign bit will ruin the arithmetic, so mask it
		sum += n;							//add to LSB's of sum
	}
	return sum;//identical to toUnsignedInt except first operation's sign bit is allowed to carry over
}
int uIntBigEnd(char* charArray, int start, int length){//reads c-string start to start+length, returns as unsigned int
	int n, sum = charArray[start+length-1];	//add char (byte) to LSB's of sum; int from char carries sign bit all the way to MSB
	sum &= 0xFF;							//Unsigned int...Don't want sign bit set, so mask it
	for(int i=start+length-2;i>=start;i--){	//will fall through if length = 1
		sum <<= 8;							//shift one byte left to make room for next
		n = charArray[i];					//same as first step, but use temp variable n
		n &= 0xFF;							//adding less significant bits...carrying over the sign bit will ruin the arithmetic, so mask it
		sum += n;							//add to LSB's of sum
	}
	return sum;
}
unsigned int uIntLittleEnd(char* charArray, int start, int length){//reads c-string start to start+length, returns as unsigned int
	int n, sum = charArray[start];	//add char (byte) to LSB's of sum; int from char carries sign bit all the way to MSB
	sum &= 0xFF;							//Unsigned int...Don't want sign bit set, so mask it
	for(int i=start+1;i<length;i++){	//will fall through if length = 1
		sum <<= 8;							//shift one byte left to make room for next
		n = charArray[i];					//same as first step, but use temp variable n
		n &= 0xFF;							//adding less significant bits...carrying over the sign bit will ruin the arithmetic, so mask it
		sum += n;							//add to LSB's of sum
	}
	return sum;
}
int intLittleEnd(char* charArray, int start, int length){//reads c-string start to start+length, returns as signed int
	int n, sum = charArray[start];	//add char (byte) to LSB's of sum; int from char carries sign bit all the way to MSB
	for(int i=start+1;i<length;i++){	//will fall through if length = 1
		sum <<= 8;							//shift one byte left to make room for next
		n = charArray[i];					//same as first step, but use temp variable n
		n &= 0xFF;							//adding less significant bits...carrying over the sign bit will ruin the arithmetic, so mask it
		sum += n;							//add to LSB's of sum
	}
	return sum;//identical to toUnsignedInt except first operation's sign bit is allowed to carry over
}
void toCharArray(int n, char * charArray, int start, int length){//BigEndian
	for(int i=start;i<start+length;i++){//Precondition: charArray is allocated to sufficient size
		charArray[i] = n & 0xFF;
		n>>=8;
	}
}
void toCharArray(unsigned int n, char * charArray, int start, int length){//BigEndian
	for(int i=start;i<start+length;i++){//Precondition: charArray is allocated to sufficient size
		charArray[i] = n & 0xFF;
		n>>=8;
	}
}
void toIntTest(){//
	const int size = 4;
	unsigned int j = 0xF23;
	cout << hex << j << endl;
	char samp_charArray[size];
	toCharArray(j, samp_charArray, 0, size);

	cout << dec << intBigEnd(samp_charArray, 0, size) << endl;
	//cout << hex << uIntBigEnd(samp_charArray, 0, size) << endl;

	//int j = -5259734;
	//cout << hex << j << endl;
}