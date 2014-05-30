#include "WaveSteg.h"

WaveSteg::WaveSteg(void) {
	foutName = "";
}
WaveSteg::~WaveSteg(void){
	if(fout.is_open()){
		fout.close();
	}
}

bool WaveSteg::checkSize(){
	minSize = (12+paraSize)*sampleSize*8 + host.get_iatOneSec();
	if(minSize>host.getDataStreamLength()){
		cout << "Host file too small to encode!" << endl;
		cout << "Use a wave file larger than " << minSize+44+72 << endl;
		return false;
	}
	return true;
}
//Encode functions: file to file
void WaveSteg::init_encFToF (string setHostName, string setParasiteName){
	init_encFToF(setHostName, setParasiteName, "encoded.wav");
}
void WaveSteg::init_encFToF (string setHostName, string setParasiteName, string setDestName){
	if(!getExtension(setParasiteName, paraExt)){//get parasite's file type, or die
		cout << "WaveSteg::initEncode: parasite fileName needs a valid extension (.txt etc)..." << endl;
		return;
	}
	parasite.initGet(setParasiteName);	//fileIterator, default sample size 1
	host.initGet(setHostName);			//waveItr, sets its own sampleSize
	sampleSize = host.getSampleSize();	//match sampleSize
	initPut(setDestName, sampleSize);	//get this ready to write file
	inGood = false;						//disable in functions; this is a write-only class
	
	outGood = parasite.good() && host.good();//any fail sets false
	if(!good()){
		cout << "WaveSteg::initEncode failed to open a write file..." << endl;
		return;
	}
	paraSize = parasite.getInSize();
	encSize = paraSize*8;

	
	outGood = outGood && paraSize && checkSize();
	cout << "minSize" << minSize << endl;

	//cout << "Parasite file type is " << paraExt << endl;
	//cout << "Parasite file size is " << paraSize << endl;
	//cout << "encSize is " << encSize << endl;
	//cout << "all good" << endl;
}
void WaveSteg::encFToF(){//Precondition: initencFToF
	copyHeader();					//wave files always have header; should be canonized prior to encoding
									//Copy one second of data so encoding can't be easily deleted
	int iDataL = host.getDataStart();
	copyDataStream(iDataL, iDataL, host.get_iatOneSec()-iDataL);
	encParaStats(host.get_iatOneSec(), host.get_iatOneSec());//call private function to encode	
	encFToF_core();//call private function to encode	
	copyDataStream(0,0);		//Copy remainder of file after encode
}
void WaveSteg::copyHeader(){
	char * headerL = 0;
	headerL = host.getHeader();
	fout.seekp(0, ios::beg);
	int headSizeL=host.getHeadSize();
	//cout << "copyHeader:  Headsize:  " << headSizeL << endl;
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
	if(fout.tellp()%2 == 1){
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
void WaveSteg::encParaStats(int iGet, int iPut){
//int i = 0;
	bool bit;
	cout << "encParaStats: sample size: " << dec << sampleSize << ", iGet=" << iGet << endl;
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
			//cout << bit;
			//if( (i+1)%8 == 0){cout << endl;}
			//i++;
			samp_int = replaceLSB(samp_int, bit);
			putSample(samp_int);
			//cout << "encode Bitr_str: sample=" << hex << n << endl;
			host.nextSample(samp_int);
		}
	}
	{//encode file type
		Bitr_str btr(paraExt);
		while(btr.nextBit(bit)){
			//cout << bit;
			//if( (i+1)%8 == 0){cout << endl;}
			//i++;
			samp_int = replaceLSB(samp_int, bit);
			putSample(samp_int);
			//cout << "encode Bitr_str: sample=" << hex << n << endl;
			host.nextSample(samp_int);
		}
	}
	{//encode fileSize
		//i=0;
		bool bit;
		Bitr_int b(paraSize);
		while(b.nextBit(bit)){
			//cout << bit;
			//if( (i+1)%8 == 0){cout << endl;}
			samp_int = replaceLSB(samp_int, bit);
			putSample(samp_int);
			//cout << "encode Bitr_int: sample=" << hex << n << endl;
			host.nextSample(samp_int);
			//i++;
		}	
	}
}
void WaveSteg::encFToF_core(){//private function
	bool bit;
	{
		//cout << "encoding bits:" << endl;
		//i=0;
		parasite.clearBiterator();
		while(parasite.nextBit(bit)){
			//cout << bit;
			//if( (i+1)%8 == 0){cout << endl;}
			samp_int = replaceLSB(samp_int, bit);
			putSample(samp_int);
			//cout << "encode Bitr_char: sample=" << hex << n << endl;
			host.nextSample(samp_int);
			//v[i]=replaceLSBt(v[i], bit);
			//i++;
		}
	}
	putSample(samp_int);//write last samp_int from loop to keep files in sync
}
//standalone function, no precondition
bool WaveSteg::verify(string fName1, string fName2, bool skipLSB, int verbosity){
	ifstream f1, f2;
	f1.open(fName1, ios::in|ios::binary|ios::ate);
	f2.open(fName2, ios::in|ios::binary|ios::ate);
	if(f1.fail() || f2.fail()){
		cout << "Function Verify:  a read file failed to open..." << endl;
		return false;
	}
	f1.seekg(0, ios::end);
	f2.seekg(0, ios::end);
	bool match = f1.tellg() == f2.tellg();
	if(!match){
		if(verbosity){
			cout << "Mismatch file sizes:  " << f1.tellg() << " != " << f2.tellg() << endl;
		}
		return false;
	}
	//match = true;
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
		//cout << "Compare " << c1 << "=" << c2 << endl;
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
	return match;
}

//Encode functions: string to file
void WaveSteg::init_encSToF(string setHostName, string encodeMe){
	init_encSToF(setHostName, encodeMe, "encoded.wav");
}
void WaveSteg::init_encSToF(string setHostName, string encodeMe, string setDestName){
	paraExt = "ssss";
	host.initGet(setHostName);			//waveItr, sets its own sampleSize
	sampleSize = host.getSampleSize();	//match sampleSize
	initPut(setDestName, sampleSize);	//get this ready to write file
	inGood = false;						//disable in functions; this is a write-only class
	encodeString = encodeMe;
	paraSize = encodeString.length();
	encSize = paraSize*8;
	outGood = outGood && paraSize && host.good();	//any fail sets false
	if(!good()){
		cout << "WaveSteg::init_encSToF failed to open a file..." << endl;
		return;
	}
	outGood = outGood && paraSize && checkSize();
	cout << "minSize" << minSize << endl;
}
void WaveSteg::encSToF(){//Precondition: initencSToF
	copyHeader();					//wave files always have header; should be canonized prior to encoding
									//Copy one second of data so encoding can't be easily deleted
	int iDataL = host.getDataStart();
	copyDataStream(iDataL, iDataL, host.get_iatOneSec()-iDataL);
	encParaStats(host.get_iatOneSec(), host.get_iatOneSec());//call private function to encode	
	encSToF_core();//call private function to encode	
	copyDataStream(0,0);		//Copy remainder of file after encode
}
void WaveSteg::encSToF_core(){//private function
	///int i = 0;
	bool bit;
	{//encode string
		//i=0;
		Bitr_str btr(encodeString);
		while(btr.nextBit(bit)){
			//cout << bit;
			//if( (i+1)%8 == 0){cout << endl;}
			
			samp_int = replaceLSB(samp_int, bit);
			putSample(samp_int);
			//cout << "encode Bitr_str: sample=" << hex << n << endl;
			host.nextSample(samp_int);
			//i++;
		}
	}
	putSample(samp_int);//write last samp_int from loop to keep files in sync
}
//Decode functions: file to file
void WaveSteg::initDecode(string setHostName){
	hostName = setHostName;
	host.initGet(setHostName);			//waveItr, sets its own sampleSize
	sampleSize = host.getSampleSize();	//match sampleSize
	cout << "initDecode: sample size: " << dec << sampleSize << endl;

	inGood = false;						//disable in functions; this is a write-only class
	outGood = host.good();//any fail sets false
	if(!good()){
		return;
	}
}
void WaveSteg::initDecode(string setHostName, string setDestName){
	foutName = killExtension(setDestName);
	initDecode(setHostName);
}
void WaveSteg::decode(){
	decParaStats(host.get_iatOneSec());
	if(paraExt.length() == 4 && paraExt[0] == '.'){
		decToF_core();
		cout << "decode to file.............." << endl;
	}
	else if(paraSize < 100){
		cout << "decode to string................" << endl;
		decToS_core();
	}
	
}
void WaveSteg::decParaStats(int iGet){
	int i = 0;//n is next sample in int form
	bool bit;
	sampleSize = host.getSampleSize();	//match sampleSize
	int ss=host.getSampleSize();
	cout << "A decParaStats: sample size: " << dec << ss << ", iGet=" << iGet << endl;
	cout << "decParaStats: sample size: " << dec << sampleSize << ", iGet=" << iGet << endl;
	if(iGet > 43){//caller can defeat seek by setting this to invalid
		host.seekg(iGet);
	}
	{//get magic word
		Reshaper_str r(4);
		i=0;
		host.nextLSB(bit);
		while(r.putBit(bit)){
			//cout << bit;
			//if( (i+1)%8 == 0){cout << endl;}
			i++;
			//cout << "decode Bitr_str: sample=" << hex << host.currentSample() << endl;
			host.nextLSB(bit);
		}
		string testMW = r.getResult();
		if(testMW != magicWord){
			cout << "No encoding in file " << hostName << endl;
			return;
		}
		cout << "Found encoding in file " << hostName << endl;
	}
{//get file type
		Reshaper_str r(4);
		i=0;
		//host.nextLSB(bit);
		while(r.putBit(bit)){
			//cout << bit;
			//if( (i+1)%8 == 0){cout << endl;}
			i++;
			//cout << "decode Bitr_str: sample=" << hex << host.currentSample() << endl;
			host.nextLSB(bit);
		}
		paraExt = r.getResult();
		cout << "dec: parasiteExt " << paraExt << endl;
		

	}
	{//get fileSize
		Reshaper_int r;
		i=0;
		while(r.putBit(bit)){
			//cout << bit;
			//if( (i+1)%8 == 0){cout << endl;}
			i++;
			//cout << "decode Reshaper_int: sample=" << hex << host.currentSample() << endl;
			host.nextLSB(bit);
		}
		paraSize = r.getResult();
		encSize = paraSize*8;
		cout << "dec: paraSize=" << dec << paraSize << endl;
	}
	hangBit = bit;
}
void WaveSteg::decToF_core(){
	foutName += paraExt;
	initPut(foutName, sampleSize);	//get this ready to write file
	cout << "Writing decoded file " << foutName << endl;
	int i = 0;//n is next sample in int form
	bool bit = hangBit;
	sampleSize = host.getSampleSize();	//match sampleSize
	int ss=host.getSampleSize();
	cout << "decToF_core: sample size: " << dec << ss << endl;
	cout << "decToF_core: sample size: " << dec << sampleSize << endl;
	cout << endl << "decToF_core, encodeSize=" << dec << encSize << endl;
	i=0;
	//host.nextLSB(bit);
	while(i<encSize){
		Reshaper_char r;
		while(r.putBit(bit)){
			//cout << bit;
			//if( (i+1)%8 == 0){cout << endl;}
			if( i >= encSize){
				cout << endl << "break inner loop" << endl;
				break;
			}
			i++;
			//cout << "decode Reshaper_char: sample=" << hex << host.currentSample() << endl;
			host.nextLSB(bit);
		}
		char res = r.getResult();
		fout.put(res);
		//cout << "result:  " << hex << (int)res << endl;
	}	

}

//void WaveSteg::decToS(){
//	decToS_core(host.get_iatOneSec());
//}
void WaveSteg::decToS_core(){
	int i = 0;//n is next sample in int form
	bool bit = hangBit;
	//sampleSize = host.getSampleSize();	//match sampleSize
	//int ss=host.getSampleSize();
	//cout << "A decParaStats: sample size: " << dec << ss << ", iGet=" << iGet << endl;
	//cout << "decParaStats: sample size: " << dec << sampleSize << ", iGet=" << iGet << endl;
	//if(iGet > 43){//caller can defeat seek by setting this to invalid
	//	host.seekg(iGet);
	//}
	{
		//encSize = paraSize*8;
		//cout << endl << "decode data, encodeSize=" << dec << encSize << endl;
		//host.nextLSB(bit);
		encodeString = "";
		while(i<encSize){
			Reshaper_char r;
			while(r.putBit(bit)){
				//cout << bit;
				//if( (i+1)%8 == 0){cout << endl;}
				if( i >= encSize){
					cout << endl << "break inner loop" << endl;
					break;
				}
				i++;
				//cout << "decode Reshaper_char: sample=" << hex << host.currentSample() << endl;
				host.nextLSB(bit);
			}
			char res = r.getResult();
			encodeString += res;
			//fout.put(res);
			//cout << "result:  " << hex << (int)res << endl;
		}	
	}
	cout << "result:  " << encodeString << endl;
}
//Standalone function, no precondition
void WaveSteg::canonize(string setHostName, string setDestName){
	hostName = setHostName;
	foutName = setDestName;
	host.initGet(setHostName);
	if(host.isCanon()){
		cout << "WaveSteg::canonize..." << setHostName << " conforms to specs" << endl;
	}
	else{
		cout << "WaveSteg::canonize..." << setHostName << " does not conform to specs.  Fixing now..." << endl;
	}
	initPut(setDestName);	//get this ready to write file
	inGood = host.good();
	if(!good()){
		cout << "WaveSteg::canonize failed to open a file..." << endl;
		return;
	}
	char * headerL = NULL;
	headerL = host.getHeaderFixed();
	fout.seekp(0, ios::beg);
	fout.write (headerL, 36);

	char dataHeader[] = {'d','a','t','a'};
	fout.write (dataHeader, 4);

	unsigned int streamLenL = host.getDataStreamLength();
	//cout << "copyHeader:  dataStreamLen:  " << hex << streamLenL << endl;
	uintToCharArray(streamLenL, dataHeader, 4);
	fout.write (dataHeader, 4);

	if(headerL){
		delete[] headerL;
	}
	copyDataStream(host.getDataStart(), 44);
}
//getters
int WaveSteg::minHostSize(){return minSize;}