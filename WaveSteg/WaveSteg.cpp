#include "WaveSteg.h"

WaveSteg::WaveSteg(void) : encodeString(""), WaveItr() {
    foutName = "decoded";//init for decode to file if not overwritten by init
}
WaveSteg::~WaveSteg(void){
    if(fout.is_open()){
        fout.close();
    }
    if(dispLog){//display log on close
        log.dispLog();
    }
    if(saveLog){//log to file on close
        log.logToFile();
    }
    log.clrLog();
}
//Encode functions: file to file
void WaveSteg::init_encFToF (string setHostName, string setParasiteName){
    init_encFToF(setHostName, setParasiteName, "encoded.wav");
}
void WaveSteg::init_encFToF (string setHostName, string setParasiteName, string setDestName){
    string op = "Encode file-to-file: host name: " + setHostName + //describes the operation commenced
        ",\n\t parasite name: " + setParasiteName + ",\n\t destination name: " + setDestName;
    log.addToLog(__FUNCTION__, 0, op, false);

    if(!getExtension(setParasiteName, paraExt)){//get parasite's file type, or die
        log.addToLog(__FUNCTION__, 2, setParasiteName, true);
        return;
    }
    inGood = false;						//disable in functions; this is a write-only class
    parasite.initGet(setParasiteName);	//fileIterator, default sample size 1
    if(!parasite.good()){				//any fail sets false
        string op = "Parasite name: " + setParasiteName;
        log.addToLog(__FUNCTION__, 1, op, true);
        outGood = false;
        return;
    }
    host.initGet(setHostName);			//waveItr, sets its own sampleSize
    if(!host.good()){					//bad file name or unreadable header aborts
        string op = "Host name: " + setHostName;
        log.addToLog(__FUNCTION__, 1, op, true);
        outGood = false;
        return;
    }
    sampleSize = host.getSampleSize();	//match sampleSize
    initPut(setDestName, sampleSize);	//get this ready to write file
    if(!outGood){						//outFile fail
        string op = "Destination name: " + setDestName;
        log.addToLog(__FUNCTION__, 1, op, true);
        return;
    }
    paraSize = parasite.getInSize();	//size of file to be encoded
    encSize = paraSize*8;				//same size in bits
    if(!paraSize){						//file size zero
        string op = "Parasite name: " + setParasiteName;
        log.addToLog(__FUNCTION__, 7, op, true);
        return;
    }
    if(!checkSize()){					//insufficient host size; log error in checkSize function
        outGood = false;
    }
}
void WaveSteg::encFToF(){//Precondition: initencFToF
    copyHeader();					//wave files always have header; should be canonized prior to encoding							
    int iDataL = host.getDataStart();//Copy one second of data so encoding can't be easily deleted
    copyDataStream(iDataL, iDataL, host.get_iatOneSec()-iDataL);
    encParaStats(host.get_iatOneSec(), host.get_iatOneSec());//call private function to encode	
    encFToF_core();					//call private function to encode	
    copyDataStream(0,0);			//Copy remainder of file after encode
}
void WaveSteg::copyHeader(){
    //main header
    char * headerL = 0;
    headerL = host.getHeader();
    fout.seekp(0, ios::beg);
    int headSizeL=host.getHeadSize();
    fout.write (headerL, headSizeL);
    if(headerL){
        delete[] headerL;
    }
    //header for data block
    char dataHeader[] = {'d','a','t','a'};	//init a 4-element array
    fout.write (dataHeader, 4);				//"data" tag

    unsigned int streamLenL = host.getDataStreamLength();
    uintToCharArray(streamLenL, dataHeader, 4);//size of data block
    fout.write (dataHeader, 4);
}
//copies until end of host file, iGet and iPut can be different
void WaveSteg::copyDataStream(int iGet, int iPut){
    if(iGet > 43){//caller can defeat seek by setting this to invalid
        host.seekg(iGet);
    }
    if(iPut > 43){//caller can defeat seek by setting this to invalid
        fout.seekp(iPut, ios::beg);
    }
    char ch;
    while (host.nextSample(ch)) {
        fout.put(ch);
    }
    if(fout.tellp()%2 == 1){//wave files always have even-numbered length
        fout.put(ch);
    }
}
//copies without regard for sample size; caller should formulate length as product of sample size
void WaveSteg::copyDataStream(int iGet, int iPut, int length){
    if(iGet > 43){//caller can defeat seek by setting this to invalid
        host.seekg(iGet);
    }
    if(iPut > 43){//caller can defeat seek by setting this to invalid
        fout.seekp(iPut, ios::beg);
    }
    int end = iGet+length;
    char ch;
    while (host.nextSample(ch) && end>iGet++) {
        fout.put(ch);
    }
    fout.put(ch);
}
//This function serves both file and string encoding
void WaveSteg::encParaStats(int iGet, int iPut){
    bool bit;
    if(iGet > 43){//caller can defeat seek by setting this to invalid
        host.seekg(iGet);
    }
    if(iPut > 43){//caller can defeat seek by setting this to invalid
        fout.seekp(iPut, ios::beg);
    }
    host.nextSample(samp_int);
    {//encode magic word
        Bitr_str btr(magicWord);
        while(btr.nextBit(bit)){
            samp_int = replaceLSB(samp_int, bit);
            putSample(samp_int);
            host.nextSample(samp_int);
        }
    }
    {//encode file type
        Bitr_str btr(paraExt);
        while(btr.nextBit(bit)){
            samp_int = replaceLSB(samp_int, bit);
            putSample(samp_int);
            host.nextSample(samp_int);
        }
    }
    {//encode fileSize
        bool bit;
        Bitr_int b(paraSize);//parasize can be a file size or a string length
        while(b.nextBit(bit)){
            samp_int = replaceLSB(samp_int, bit);
            putSample(samp_int);
            host.nextSample(samp_int);
        }	
    }
}
void WaveSteg::encFToF_core(){//private function, handles file to file encoding
    bool bit;
    parasite.clearBiterator();
    while(parasite.nextBit(bit)){
        samp_int = replaceLSB(samp_int, bit);
        putSample(samp_int);
        host.nextSample(samp_int);
    }
    putSample(samp_int);//write last samp_int from loop to keep files in sync
}
//standalone function, no precondition
bool WaveSteg::verify(string fName1, string fName2, bool skipLSB, int verbosity){
    ifstream f1, f2;
    const char* fileName1 = fName1.c_str();
    const char* fileName2 = fName2.c_str();
    f1.open(fileName1, ios::in|ios::binary|ios::ate);
    f2.open(fileName2, ios::in|ios::binary|ios::ate);
    if(f1.fail() || f2.fail()){
        string op = "In file 1: " + fName1 + " or in file 2: " + fName2;
        log.addToLog(__FUNCTION__, 1, op, true);
        return false;
    }
    else{
        string op = "In file 1: " + fName1 + " == in file 2: " + fName2;
        log.addToLog(__FUNCTION__, 12, op, false);
    }
    f1.seekg(0, ios::end);//check sizes
    f2.seekg(0, ios::end);
    bool match = (f1.tellg() == f2.tellg());
    if(!match){
        string op = intToString(f1.tellg()) + " != " + intToString(f2.tellg());
        log.addToLog(__FUNCTION__, 9, op, true);
        return false;
    }
    f1.seekg(0, ios::beg);
    f2.seekg(0, ios::beg);
    char c1, c2;
    int c = 0;
    if(verbosity){
        if(skipLSB){
            cout << "Ignoring differences due to encoding." << endl;
        }
        cout << "Warnings will be displayed on first " << dec << verbosity << " mismatches..." << endl;
        cout << "index\t" << fName1 << "\t" << fName2 << endl;
    }
    while(f1.get(c1) && f2.get(c2)){
        if(skipLSB){
            c1 &= 0xFE;
            c2 &= 0xFE;
        }
        if(c1 != c2 && verbosity > c++){
            if(verbosity){
                cout  << dec << f1.tellg() << ":\t" << hex << chint(c1) << "\t!=\t"<< chint(c2) << endl;
            }
            match = false;
        }
    }
    if(match){
        log.addToLog(__FUNCTION__, 11, "Verify succeeded", false);
    }
    else{
        log.addToLog(__FUNCTION__, 10, "Verify failed", true);
    }
    return match;
}
//utility
bool WaveSteg::checkSize(){//calculates minimum size for host file; reports errors
    minSize = (12+paraSize)*sampleSize*8 + host.get_iatOneSec();
    if(minSize>host.getDataStreamLength()){
        string op = "Hose size:  " + intToString(host.getInSize() ) +
                ",\n\tNeeded size:  " + intToString(minSize+44+72);
        log.addToLog(__FUNCTION__, 3, op, true);
        return false;
    }
    return true;
}
//Encode functions: string to file
void WaveSteg::init_encSToF(string setHostName, string encodeMe){
    init_encSToF(setHostName, encodeMe, "encoded.wav");
}
void WaveSteg::init_encSToF(string setHostName, string encodeMe, string setDestName){//see init_encFToF
    string op = "Encode string-to-file: host name: " + setHostName + //describes the operation commenced
            ",\n\t destination name: " + setDestName;
    log.addToLog(__FUNCTION__, 0, op, false);
    paraExt = "ssss";					//lack of a dot file extension triggers string decode later
    inGood = false;						//disable in functions; this is a write-only class
    host.initGet(setHostName);			//waveItr, sets its own sampleSize
    if(!host.good()){
        string op = "Host name: " + setHostName;
        log.addToLog(__FUNCTION__, 1, op, true);
        outGood = false;
        return;
    }
    sampleSize = host.getSampleSize();	//match sampleSize
    initPut(setDestName, sampleSize);	//get this ready to write file
    if(!outGood){						//outFile fail
        string op = "Destination name: " + setDestName;
        log.addToLog(__FUNCTION__, 1, op, true);
        return;
    }
    encodeString = encodeMe;
    paraSize = encodeString.length();
    encSize = paraSize*8;
    if(!paraSize){						//string length 0
        string op = "Check string to be encoded";
        log.addToLog(__FUNCTION__, 7, op, true);
        return;
    }
    if(!checkSize()){					//insufficient host size; log error in checkSize function
        outGood = false;
    }
}
void WaveSteg::encSToF(){//Precondition: initencSToF
    copyHeader();					//wave files always have header; should be canonized prior to encoding							
    int iDataL = host.getDataStart();//Copy one second of data so encoding can't be easily deleted
    copyDataStream(iDataL, iDataL, host.get_iatOneSec()-iDataL);
    encParaStats(host.get_iatOneSec(), host.get_iatOneSec());//call private function to encode	
    encSToF_core();					//call private function to encode	
    copyDataStream(0,0);			//Copy remainder of file after encode
}
void WaveSteg::encSToF_core(){//private function
    bool bit;
    {//encode string
        Bitr_str btr(encodeString);
        while(btr.nextBit(bit)){
            samp_int = replaceLSB(samp_int, bit);
            putSample(samp_int);
            host.nextSample(samp_int);
        }
    }
    putSample(samp_int);//write last samp_int from loop to keep files in sync
}
//Decode functions: file to file
void WaveSteg::initDecode(string setHostName){
    string op = "Find and decode watermark: host name: " + setHostName;//describes the operation commenced
    log.addToLog(__FUNCTION__, 0, op, false);
    hostName = setHostName;
    host.initGet(setHostName);		//waveItr, sets its own sampleSize
    sampleSize = host.getSampleSize();	//match sampleSize
    inGood = false;			//disable in functions; this is a write-only class
    outGood = host.good();		//any fail sets false
    if(!good()){
        return;
    }
}
void WaveSteg::initDecode(string setHostName, string setDestName){
    cout << "initDecode:  setDestName: " << setDestName << endl;
    foutName = setDestName;//killExtension(setDestName);
    cout << "initDecode:  foutName: " << foutName << endl;
    initDecode(setHostName);
}
void WaveSteg::decode(){
    decParaStats(host.get_iatOneSec());
    if(!outGood){return;}
    if(paraExt[0] == '.'){
        decToFile();
    }
    else if(paraSize < 100){//limit screen output
        decToString();
    }
}
void WaveSteg::decParaStats(int iGet){
    bool bit;
    sampleSize = host.getSampleSize();	//match sampleSize
    int ss=host.getSampleSize();
    if(iGet > 43){//caller can defeat seek by setting this to invalid
        host.seekg(iGet);
    }
    {//get magic word
        Reshaper_str r(4);
        host.nextLSB(bit);
        while(r.putBit(bit)){
            host.nextLSB(bit);
        }
        string testMW = r.getResult();
        if(testMW != magicWord){
            log.addToLog(__FUNCTION__, 4, hostName, true);
            outGood = false;
            return;
        }
    }
    {//get file type
        Reshaper_str r(4);
        while(r.putBit(bit)){
            host.nextLSB(bit);
        }
        paraExt = r.getResult();
    }
    {//get fileSize
        Reshaper_int r;
        while(r.putBit(bit)){
            host.nextLSB(bit);
        }
        paraSize = r.getResult();
        encSize = paraSize*8;
    }
    hangBit = bit;//save last getBit to start next process
}
void WaveSteg::decToFile(){
    cout << "decToFile:  foutName: " << foutName << endl;
    foutName += paraExt;
    cout << "decToFile: += paraExt; foutName: " << foutName << endl;
    string op = "Found file ext: writing decoded data to fileName : " + foutName;// + //describes the operation commenced
    log.addToLog(__FUNCTION__, 5, op, false);
    initPut(foutName, sampleSize);	//get this ready to write file
    bool bit = hangBit;
    sampleSize = host.getSampleSize();	//match sampleSize
    int ss=host.getSampleSize();
    int i = 0;
    while(i<encSize){
        Reshaper_char r;
        while(r.putBit(bit)){
            i++;
            host.nextLSB(bit);
        }
        char res = r.getResult();
        fout.put(res);
    }	
}
void WaveSteg::decToString(){
    string op = "Found message; will output to screen";// + //describes the operation commenced
    log.addToLog(__FUNCTION__, 5, op, false);
    int i = 0;//n is next sample in int form
    bool bit = hangBit;
    encodeString = "";
    while(i<encSize){
        Reshaper_char r;
        while(r.putBit(bit)){
            i++;
            host.nextLSB(bit);
        }
        char res = r.getResult();
        encodeString += res;
    }
}
//Standalone function, no precondition
bool WaveSteg::canonize(string setHostName, string setDestName){
    hostName = setHostName;
    foutName = setDestName;
    host.initGet(setHostName);
    if(!host.good()){
        log.addToLog(__FUNCTION__, 1, setHostName, true);
        return false;
    }
    if(host.isCanon()){
        string op = setHostName + " already conforms to specs";// + //describes the operation commenced
        log.addToLog(__FUNCTION__, 6, op, false);
        return false;
    }
    string op = setHostName + " does not conform to specs.  Fixing now";// + //describes the operation commenced
    log.addToLog(__FUNCTION__, 0, op, false);
    initPut(setDestName);	//get this ready to write file
    inGood = host.good();
    if(!good()){
        return false;
    }
    char * headerL = NULL;
    headerL = host.getHeaderFixed();
    fout.seekp(0, ios::beg);
    fout.write (headerL, 36);

    char dataHeader[] = {'d','a','t','a'};
    fout.write (dataHeader, 4);

    unsigned int streamLenL = host.getDataStreamLength();
    uintToCharArray(streamLenL, dataHeader, 4);
    fout.write (dataHeader, 4);

    if(headerL){
        delete[] headerL;
    }
    copyDataStream(host.getDataStart(), 44);
    return true;
}
//getters
int WaveSteg::minHostSize(){return minSize;}
string WaveSteg::getEncodeString(){//to get result of a decode operation
    return encodeString;
}