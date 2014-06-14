#include "WaveItr.h"

WaveItr::WaveItr(void) : header(0), headSize(36), FileItr() {
    //dispLog = true;
}//size of char array to read header
WaveItr::~WaveItr(void){
    if(samp_charArray){
        delete[] samp_charArray;
        samp_charArray = NULL;
    }
    if(header){
        delete[] header;
        header = NULL;
    }
    if(fin.is_open()){
        fin.close();
    }
    if(fout.is_open()){
        fout.close();
    }
    if(dispLog){
        log.dispLog();
    }
    if(saveLog){
        log.logToFile();
    }
    log.clrLog();
}
//Initialize fields
void WaveItr::initGet(string fileName){//call this one
    initGet(fileName, 1);
}
void WaveItr::initGet(string fileName, int setSampleSize){
    finName = fileName;
    openInFile(fileName);
    if(inGood){
        sampleSize=setSampleSize;//should be overwritten by readHeader, to whatever is specified in the wave header
        unsignedMax = power(2, sampleSize*8)-1;//highest number encodable in specified sample size
        signedMax = power(2, sampleSize*7)-1;//if data is signed
        samp_charArray = new char [sampleSize];//small array to hold single sample (1-2-3-4 bytes)
    }
    samp_charArray = 0;
    inGood = inGood && readHeader();//header is allocated in readHeader()
}
bool WaveItr::readHeader(){
    iData = findDataStart();//index of data stream
    if(iData == 0){
        return false;
    }
    streamLen = findStreamLength();
    header = fileToChArray(0, headSize);//allocate header here
    if(!isWave()){
        return false;
    }
    //values from file
    hData.fileLen = toUnsignedInt(header, 4, 4);//actual size - 8
    hData.formatLen = toUnsignedInt(header, 16, 4);
    hData.formatTag = toUnsignedInt(header, 20, 2);
    hData.numChannels = toUnsignedInt(header, 22, 2);
    hData.sampleRate = toUnsignedInt(header, 24, 4);
    hData.bytes_sec = toUnsignedInt(header, 28, 4);
    hData.blockAlign = toUnsignedInt(header, 32, 2);
    hData.bitDepth = toUnsignedInt(header, 34, 2);
    //calculated values
    sampleSize = hData.bitDepth/8;//wave file standard formats are multiples of 8
    unsignedMax = power(2, hData.bitDepth)-1;//highest number encodable in specified sample size
    signedMax = unsignedMax/2;//audio data is signed
    find_iatOneSec();
    samp_charArray = new char [sampleSize];//small array to hold single sample (2-3-4 bytes)
    return true;
}
bool WaveItr::isWave(){
    return header[0] == 'R' && header[1] == 'I' && header[2] == 'F' && header[3] == 'F' && 
            header[8] == 'W' && header[9] == 'A' && header[10] == 'V' && header[11] == 'E';
}
unsigned int WaveItr::findStreamLength(){//Reads 4-byte header of data block
    char* charArray = fileToChArray(iData-4, 4);
    unsigned int out = toUnsignedInt(charArray, 0, 4);
    delete[] charArray;
    return out;
}
//displays
void WaveItr::dispHeader(){//Public, displays data in friendly format
    cout << finName << " is a valid wave file, size:  " << dec << inSize << endl;
    cout << "@ index 0:  characters:  'RIFF'" << endl;
    cout << "@ index 4:  file length minus eight bytes:  " << dec << hData.fileLen << endl;
    cout << "@ index 8:  characters:  'WAVE'" << endl;
    cout << "@ index 12:  characters:  'fmt '" << endl;
    cout << "@ index 16:  Length of the fmt data (16 bytes):  " << hData.formatLen << endl;
    cout << "@ index 20:  Format tag: 1 = PCM:  " << hData.formatTag << endl;
    cout << "@ index 22:  Number of channels:  " << hData.numChannels << endl;
    cout << "@ index 24:  sample rate:  " << hData.sampleRate << endl;
    cout << "@ index 28:  bytes/second (sample rate * block align):  " << hData.bytes_sec << endl;
    cout << "@ index 32:  block align (channels * bits/sample/8):  " << hData.blockAlign << endl;
    cout << "@ index 34:  Bit Depth:  " << hData.bitDepth << endl;
    cout << "@ index "<< iData-4 << ":  Length of the data block:  " << streamLen << endl;
    cout << "@ index " << iData << ":  Data stream begins" << endl;
    bar();
}
void WaveItr::dispHeaderHex(){//Public, for displaying char* header: data already in memory from file
    cout << "Display Header Hex...File size = " << dec << inSize << endl;
    cout << "Index\tHex\tChar" << endl;
    for(int i=0;i<headSize;i++){
        int n = header[i];
        n = 0xFF & n;//data is unsigned, so mask leading 1's
        cout << dec << i << "\t" << hex << n ;
        cout << "\t" << header[i] << endl;//Char representation, if meaningful
    }
    bar();
}
void WaveItr::dispDataHex(int limit){//Public, displays data parsed directly from file, in multi-byte increments based on sampleSize
    cout << "Display Data Hex...File size = " << dec << inSize << endl;
    int n, i = 0, interval = streamLen/limit;
    if(interval%2 != 0){interval++;}//make sure interval is an even number
    seekDataStart();
    while(nextSample(n, interval) && 0 < limit--){
        cout << dec << (int)fin.tellg() - iData << "\t" << hex << n << endl;
        i++;
    }
    cout << endl;
    bar();
}
void WaveItr::dispWaveForm(int limit){
    WaveForm W(signedMax);
    int n, interval = streamLen/limit;
    if(interval%2 != 0){interval++;}//make sure interval is an even number
    seekDataStart();
    while(nextSample(n, interval) && 0 < limit--){
        W.single(n);
    }
    cout << endl << endl;
    bar();
}
int WaveItr::numZeroes(bool verbose){//locates and counts wave form's zero-crossings
    int n, last=-1, zeroes = 0;
    seekDataStart();
    if(verbose){
        cout << "Number of zeroes: zero crossings displayed below..." << endl;
    }
    while(nextSample(n)){
        if(oppSigns(n, last)){
            zeroes++;
            if(verbose){
                cout << "Zero near " << dec << (int)fin.tellg() - iData-1 << endl;
            }
        }
        last = n;
    }
    if(verbose){
        cout << "Detected " << zeroes/hData.numChannels << " zeroes"<< endl;
        bar();
    }
    return zeroes/hData.numChannels;
}
//Getters
char* WaveItr::getHeader(){//Returns a pointer to a copy of header array
    char* headerCopy = new char[headSize];
    for(int i=0;i<headSize;i++){
        headerCopy[i] = header[i];
    }
    return headerCopy;
}
char* WaveItr::getHeaderFixed(){//For canonizing: Returns a pointer to a fixed copy of header array
    char* headerCopy = new char[headSize];
    for(int i=0;i<headSize;i++){
        headerCopy[i] = header[i];
    }
    unsigned int fSizeMinus8 = 44 + streamLen + 72;//why 72? idk.  Unaccounted data
    toCharArray(fSizeMinus8, headerCopy, 4, 4);//fix file size
    toCharArray(16, headerCopy, 16, 4);//fix length of format data to 16
    return headerCopy;
}
int WaveItr::getHeadSize(){
    return headSize;
}
unsigned int WaveItr::getDataStreamLength(){
    return streamLen;
}
int WaveItr::getSampleSize(){
    return sampleSize;
}
int WaveItr::getDataStart(){
    return iData;
}
int WaveItr::get_iatOneSec(){
    return iatOneSec;
}
headerData* WaveItr::getHeaderData(){//Returns pointer to structure, valid while WaveItr object in scope
    headerData* p = &hData;
    return p;
}
bool WaveItr::isCanon(){
    return iData==44;
}
//iterators
void WaveItr::seekDataStart(){
    fin.clear();
    fin.seekg (iData, ios::beg);
}

//utility functions
char* WaveItr::fileToChArray(streampos start, streampos size){//returns charArray of data read from file, from start to start+size
    char * charArray = new char [size];
    fin.seekg (start, ios::beg);
    fin.read (reinterpret_cast<char *>(charArray), size);
    return charArray;
}
void WaveItr::find_iatOneSec(){//answer is in bytes
    iatOneSec = iData + hData.sampleRate * hData.numChannels * sampleSize;
}

//data block parsers
unsigned int WaveItr::findDataStart(){//to set iData: returns index 5 bytes after "data" tag in wave file header
    fin.seekg (0, ios::beg);
    unsigned int i = 0;
    string target = "";
    while(fin.good()){//try to build string to match tag, fails for all but complete match
        char c = fin.get();
        if(c == 'd' || c == 'a' || c == 't'){
            target+= c;
            if(target == "data"){
                return i+5;//i + 1 for data tag index; +4 for data start
            }
        }
        else{
            target = "";
        }
        i++;
    }
    return 0;//invalid, to indicate fail
}
bool WaveItr::trueStereo(){//Precondition: initGet; compares left and right samples for equality
    if(hData.numChannels != 2){return false;}
    fin.clear();
    fin.seekg (iData, ios::beg);
    int left, right, i = 0, common = 0;
    int total = streamLen/sampleSize/2;
    while(total>i++ && nextSample(left) && nextSample(right)){
        if(left==right){
            common++;
        }
    }
    int percent = common/total * 100;
    return (percent<90);//some audio plug-in outputs may color data, causing inequalities. 90% is 'mono enough'
}