#pragma once
#include "WaveSteg.h"

class U_Interface
{
public:
    U_Interface(void);
    ~U_Interface(void);
    //user interface functions
    void setPreferences();
    void encodeFileToFile();
    void encodeStringToFile();
    void decode();
    void canonize();
    void testStereo();
    void verify();
    void dispFile();
    void copyFile();
    void dispHeader();
    void dispWaveData();
    void dispZeroes();
    //action functions
    bool encodeFileToFile(string host, string parasite, string dest);
    bool encodeStringToFile(string host, string encodeMe, string dest);
    void decode(string host, string dest);
    void verify(string fName1, string fName2, bool skipLSB, int verbosity);
    void canonize(string fName);
    void canonize(string fName1, string fName2);
    void testStereo(string fName);
    void dispFile(string fileName, int start, int limit, int line);
    void dispHeader(string fName, bool hexed);
    void dispWaveData(string fName, int resolution, bool hexed);
    void dispZeroes(string fName);
    void copyFile(string f1, string f2);
private:
    //user interface functions
    //string getFileChoice();
    //string getFileChoice(string prompt);
    string chooseWav(string prompt);
    string choosePara(string prompt);
    string chooseFile(string prompt);
    void setUQs();
    void saveUQs();


    bool verbose, prolific;
    string banner;
    char uqe, uqd;
};

