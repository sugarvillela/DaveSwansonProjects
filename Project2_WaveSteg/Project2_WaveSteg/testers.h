#pragma once
#include "WaveSteg.h"

void canonize(string fName);
void canonize(string fName1, string fName2);
void encodeFileToFile(string fName, string parasiteName);
void encodeFileToFile(string fName1, string parasiteName, string fName2);
void testFileCopy();
void testDispFile();
void testBFileCopy();
void testWSVerify();
void testWSCanonize();
void testEncDec();
void testEncDec_string();