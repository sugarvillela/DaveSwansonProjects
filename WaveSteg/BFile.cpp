#include "BFile.h"
/*
Notes:
This is the base class for a file iteration family of classes.  It
handles read and write opening, closing, status, remembering the names
of the files, and a hex file display.
It contains virtual functions, so use a subclass to instantiate.
*/

BFile::BFile(void)  : inSize(0), outSize(0), inGood(false), outGood(false), 
    finName(""), foutName(""), dispLog(true), saveLog(true) {}

void BFile::openInFile(string fileName){
    
    inSize = (unsigned int)fin.tellg();
    finName = fileName;
    const char* fName = fileName.c_str();
    fin.open(fName, ios::in|ios::binary|ios::ate);
    inGood = fin.is_open();
    if(inGood){
        findInSize();
    }
}
void BFile::openOutFile(string fileName){
    const char* fName = fileName.c_str();
    fout.open(fName, ios::out | ios::binary | ios::ate);
    outGood = fout.is_open();
}
//setter getters
void BFile::showLogs(bool show){
    dispLog = show;
}
void BFile::saveLogs(bool save){
    saveLog = save;
}
void BFile::findInSize(){//preconditions: file is open //called from openInFile() //protected function
    fin.clear();
    fin.seekg(0, ios::end);
    inSize = fin.tellg();
}
unsigned int BFile::getInSize(){//Public get
    return inSize;
}
unsigned int BFile::getOutSize(){//Returns meaningful data (non-zero) only after a write
    return outSize;
}
bool BFile::good(){//using one or the other, so whichever is good returns true
    return inGood || outGood;
}
//through function to protected object
void BFile::seekg(streampos index){
    fin.seekg(index, ios::beg);
}
//debug displays
void BFile::dispFile(int start, int length, int line){//Public, for displaying part or all of a file
    cout << "Display File:  " << finName << endl;
    cout << "Size = " << dec << inSize << endl;
    char ch;
    fin.clear();
    fin.seekg (start, ios::beg);
    while (fin.get(ch) && start++ < length){
        int n = ch;
        cout << hex << n << "\t";
        if( (start)%line == 0 ){
            cout << endl;
        }
    }
    cout << endl;
    bar();
}
void BFile::copyBlock(int start){
    fin.seekg(start, ios::beg);
    char ch;
    while (fin.get(ch)) {
        fout.put(ch);
    }
}
void BFile::copyBlock(int start, int length){
    int end = start+length;
    fin.seekg(start, ios::beg);
    char ch;
    while (end>start++ && fin.get(ch)) {
        fout.put(ch);
    }
}