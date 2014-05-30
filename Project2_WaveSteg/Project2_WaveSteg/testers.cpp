#include "testers.h"

void canonize(string fName){
	{
		WaveSteg  e;
		e.canonize(fName, "temp.wav");
	}
	{
		FileItr f;
		f.copyFile("temp.wav", fName);
	}
}
void canonize(string fName1, string fName2){
	if(fName1 == fName2){
		{
			WaveSteg  e;
			e.canonize(fName1, "temp.wav");
		}
		{
			FileItr f;
			f.copyFile("temp.wav", fName1);
		}
	}
	else{
		{
			WaveSteg  e;
			e.canonize(fName1, fName2);
		}
	}
}
void encodeFileToFile(string fName, string parasiteName){
	canonize(fName);
	{
		WaveSteg  e;
		e.init_encFToF(fName, parasiteName);
		e.encFToF();
	}
}
void encodeFileToFile(string fName1, string parasiteName, string fName2){}
void testFileCopy(){
	{
		WaveItr  w;
		w.initGet("temp.wav");
		w.dispHeader();
	}
	{
		FileItr f;
		f.copyFile("temp.wav", "1Hz_Sine_4Sec.wav");
	}
	{
		WaveItr  w;
		w.initGet("1Hz_Sine_4Sec.wav");
		w.dispHeader();
	}
}
void testBFileCopy(){
	{
		FileItr f;
		f.initGet("nums.bin");
		f.dispFile(0, 500, 5);
	}
	{
		FileItr f;
		f.initGetPut("nums.bin","numsCopy.bin");
		f.copyBlock(0);
	}
	{
		FileItr f;
		f.initGet("numsCopy.bin");
		f.dispFile(0, 500, 5);
	}
}
void testDispFile(){
	{
		FileItr f;
		f.initGet("1Hz_Sine_4Sec.wav");
		f.dispFile(40, 60, 4);
	}
}
void testWSCanonize(){
	{
		WaveItr  w;
		w.initGet("1Hz_Sine_4Sec.wav");
		w.dispHeader();
	}
	{
		WaveSteg w;
		w.canonize("1Hz_Sine_4Sec.wav", "temp.wav");
	}
	//{
	//	FileItr f;
	//	f.copyFile("temp.wav", "1Hz_Sine_4Sec.wav");
	//}
	{
		WaveItr  w;
		w.initGet("temp.wav");
		w.dispHeader();
	}
}
void testWSVerify(){
	{
		WaveSteg w;
		cout << "Verifying..." << endl;
		if(w.verify("1Hz_Sine_4Sec.wav", "dest.wav", true, 10)){
			cout << "No errors reported...input and output files match" << endl;
		}
		else{
			cout << "Errors reported...input and output files do not match" << endl;
		}//352917
	}
	//{
	//	FileItr f;
	//	f.initGet("1Hz_Sine_4Sec.wav");
	//	f.dispFile(352910, 352920, 5);
	//}
	//{
	//	FileItr f;
	//	f.initGet("dest.wav");
	//	f.dispFile(352910, 352920, 5);
	//}
}
void testEncDec(){
	{
		FileItr  f;
		f.initGet("nums.bin");
		f.dispFile(0,55,5);
	}
	{
		WaveSteg  e;
		e.init_encFToF("1Hz_Sine_4Sec.wav", "nums.bin", "dest.wav");
		//e.init_encFToF("1Hz_Sine.wav", "nums.bin", "dest.wav");
		if(!e.good()){
			return;
		}
		e.encFToF();
	}
	{
		WaveSteg  d;
		d.initDecode("dest.wav", "nums4");
		d.decode();
	}
	{
		FileItr  f;
		f.initGet("nums4.bin");
		f.dispFile(0,55,5);
	}
}
void testEncDec_string(){
	{
		cout << "Enter a string of any length to encode:" << endl;
		string encodeMe = getStringInput();
		WaveSteg  e;
		e.init_encSToF("1Hz_Sine_4Sec.wav", encodeMe, "dest.wav");
		if(!e.good()){
			return;
		}
		e.encSToF();
	}
	{
		WaveSteg  d;
		d.initDecode("dest.wav");
		d.decode();
	}
	{
		FileItr  f;
		f.initGet("nums2.bin");
		f.dispFile(0,55,5);
	}
}
