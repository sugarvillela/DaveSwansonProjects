#include "U_Interface.h"
#include "templateFunctions.h"
//2273264 file size for 128kb

U_Interface::U_Interface(void) : verbose(true), prolific(true), banner("*****"){
    setUQs();//unique chars keep files from being overwritten; string + char = unique file name
}
U_Interface::~U_Interface(void){
    saveUQs();
}
void U_Interface::setUQs(){//get chars stored from last time program was run
    ifstream fin;
    fin.open("./ws_log/uqe.txt");
    if(!fin.good()){
        return;
    }
    fin >> uqe;
    fin.close();
    fin.open("./ws_log/uqd.txt");
    if(!fin.good()){
        return;
    }
    fin >> uqd;
    fin.close();
}
void U_Interface::saveUQs(){//save chars to file to resume next time program is run
    if(uqe == 0x5B){uqe = 'a';}//keep in range a-zA-Z
    if(uqe == 0x7B){uqe = 'A';}
    if(uqd == 0x5B){uqd = 'a';}     
    if(uqd == 0x7B){uqd = 'A';}    
    ofstream fout;
    fout.open("./ws_log/uqe.txt", ios::ate);//overwrite old char with current
    if(!fout.good()){
        return;
    }
    fout << uqe;
    fout.close();
    fout.open("./ws_log/uqd.txt", ios::ate);
    if(!fout.good()){
        return;
    }
    fout << uqd;
    fout.close();    
}
void U_Interface::setPreferences(){
    verbose = ask("Display logs when operation complete? (y/n)", 'y');
    prolific = ask("Append logs to file when operation complete? (y/n)", 'y');
}

//encodeFileToFile
void U_Interface::encodeFileToFile(){
    if(verbose){
        cout << banner << "Note: host file automatically canonized for encoding operation" << endl;
        cout << banner << "Note: overwrite not a good idea (virtually undetectable watermark)" << endl;
        bar();
    }
    string      f1 = chooseWav("Choose host file (must be .wav)"),
                f2 = choosePara("Choose parasite file (must be smaller)");
    string destName = "./encoded/encoded_";//string constructor doesn't like string+char
    destName += uqe++;//increment char for next use
    destName += ".wav";//add extension
    cout << banner << "Encode file to file, destination name:  " << destName << endl;
    if(!encodeFileToFile(f1, f2, destName)){
        return;
    }
    if(ask("Verify accuracy of encoded file? (y/n)", 'y')){
        verify(f1, destName, verbose, verbose*25);
    }
}
bool U_Interface::encodeFileToFile(string host, string parasite, string dest){
    canonize(host);
    WaveSteg  e;
    e.showLogs(verbose);
    e.saveLogs(prolific);
    e.init_encFToF(host, parasite, dest);
    if(!e.good()){
            return false;
    }
    e.encFToF();
    return e.good();
}
//encodeStringToFile
void U_Interface::encodeStringToFile(){
    if(verbose){
        cout << banner << "Note: host file automatically canonized for encoding operation" << endl;
        cout << banner << "Note: overwrite not a good idea (virtually undetectable watermark)" << endl;
        bar();
    }
    cout << "Enter a string of any length to encode:" << endl;
    string encodeMe = getStringInput();
    string	f1 = chooseWav("Choose host file (must be .wav)");
    string destName = "./encoded/encoded_";//string constructor doesn't like string+char
    destName += uqe++;//increment char for next use
    destName += ".wav";//add extension
    cout << banner << "Encode string to file, destination name:  " << destName << endl;
    if(!encodeStringToFile(f1, encodeMe, destName)){
        return;
    }
    if(ask("Verify accuracy of encoded file? (y/n)", 'y')){
        if(verbose){
            verify(f1, destName, true, 25);
        }
        else{
            verify(f1, destName, true, 0);
        }
    }
}
bool U_Interface::encodeStringToFile(string host, string encodeMe, string dest){
    canonize(host);
    WaveSteg  e;
    e.showLogs(verbose);
    e.saveLogs(prolific);
    e.init_encSToF(host, encodeMe, dest);
    if(!e.good()){
        return false;
    }
    e.encSToF();
    return e.good();
}
void U_Interface::decode(){
    if(verbose){
        cout << banner << "\tNote: Will detect string or file.  If string, will output to screen." << endl
                << "\tIf file, will output to decodedx.xxx (whatever extension is detected)" << endl;
        bar();
    }
    string  hostName = "./encoded/encoded_";
    cout << "Path and file name: " << hostName << endl << 
        "Add a character: (a-zA-Z) to select file:  ";
    hostName += getCharInput();
    hostName += ".wav";
    cout << banner << "Decode, host name:  " << hostName << endl;

    string destName = "./decoded/decoded_";//string constructor doesn't like string+char
    destName += uqd++;//increment char for next use //no extension, WaveSteg adds extension when decoding
    cout << banner << "Decode, output name (if file):  " << destName << endl;
    decode(hostName, destName);
}
void U_Interface::decode(string host, string dest){
    WaveSteg  d;
    d.showLogs(verbose);
    d.saveLogs(prolific);
    d.initDecode(host, dest);
    if(d.good()){
        d.decode();
        string result = d.getEncodeString();
        if(result != ""){
            cout << banner << "\tDecoded to string " << banner << endl;
            cout << banner << "Found this:  " << result << endl;
        }
    }
}
void U_Interface::verify(){
    string f1 = chooseFile("Choose first file to check"),
           f2 = chooseFile("Choose second file to check");
    if(verbose){
        cout << banner << "Note: Will detect watermark if skip LSB not true." << endl;
    }
    bool skipLSB = ask("Skip least significant bit when checking? (y/n)", 'y');
    int verbosity = 0;
    if(verbose){
        cout << "If errors exist, how many do you want to display? (max 300)" << endl;
        verbosity = getIntInput(1, 300);
    }
    verify(f1, f2, skipLSB, verbosity);
}
void U_Interface::verify(string fName1, string fName2, bool skipLSB, int verbosity){
    WaveSteg  v;
    v.showLogs(verbose);
    v.saveLogs(prolific);
    v.verify(fName1, fName2, skipLSB, verbosity);
}
void U_Interface::canonize(){
    string f1 = chooseWav("Choose file to canonize (must be .wav)");
    if(ask("Overwrite? (y/n)", 'y')){
        canonize(f1);
    }
    else{
        string f2 = chooseWav("Choose a destination for the corrected file (must be .wav)");
        canonize(f1, f2);
    }
}
void U_Interface::canonize(string fName){
    {
        WaveSteg  e;
        e.showLogs(verbose);
        e.saveLogs(prolific);
        if(!e.canonize(fName, "temp.wav")){
            return;
        }
    }
    {
        FileItr f;
        f.copyFile("temp.wav", fName);
    }
}
void U_Interface::canonize(string fName1, string fName2){
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
void U_Interface::testStereo(){
    if(verbose){
        cout << banner << "\tJust because a wave file has two channels doesn't mean it is stereo." << endl 
            << "\tFiles are often created with the same information in both" << endl
            << "\tchannels (dual-mono), which is a waste of space" << endl;
        bar();
    }
    string fName = chooseWav("Choose file to test true stereo");
    testStereo(fName);
}
void U_Interface::testStereo(string fName){
    WaveItr  w;
    w.showLogs(verbose);
    w.saveLogs(prolific);
    w.initGet(fName);
    if(!w.good()){
        return;
    }
    w.dispHeader();
    if(w.trueStereo()){
        cout << fName << " is a true stereo file" << endl;
    }
    else{
        cout << fName << " is a mono file" << endl;
    }
}

void U_Interface::dispFile(){
    int start = 0, limit = 50, line = 5;
    string	f1 = chooseFile("Choose a file to display");
    if(ask("Limit display size? (y/n)", 'y')){
        cout << "Start at:  ";
        start = getIntInput(0, INT_MAX);
        cout << "Stop at:  ";
        limit = getIntInput(0, start + 300);
    }
    dispFile(f1, start, limit, line);
}
void U_Interface::dispFile(string fileName, int start, int limit, int line){
    FileItr  f;
    f.showLogs(verbose);
    f.saveLogs(prolific);
    f.initGet(fileName);
    if(!f.good()){
        return;
    }
    f.dispFile(start, limit, line);
}
void U_Interface::dispHeader(){
    string	f1 = chooseWav("Choose file to display (must be .wav)");
    cout << banner << "Can be displayed in 'friendly' format or hex" << endl;
    bool hexed = ask("Hex only? (y/n)", 'y');
    dispHeader(f1, hexed);
}
void U_Interface::dispHeader(string fName, bool hexed){
    WaveItr  w;
    w.showLogs(verbose);
    w.saveLogs(prolific);
    w.initGet(fName);
    if(!w.good()){
        return;
    }
    if(hexed){
        w.dispHeader();
    }
    else{
        w.dispHeader();
    }
}
void U_Interface::dispWaveData(){
    cout << banner << "Note:  Display only makes sense for one of the short waveform files" << endl;
    string	f1 = chooseWav("Choose file to display (must be .wav)");
    cout << banner << "Note:  Resolution is #lines/size" << endl;
    cout << "How many lines do you want to display? (max 300)" << endl;
    int limit = getIntInput(1, 300);
    cout << banner << "Can be displayed in hex or bar graph" << endl;
    bool hexed = ask("Hex or graph? (h/g)", 'h');
    dispWaveData(f1, limit, hexed);
}
void U_Interface::dispWaveData(string fName, int limit, bool hexed){
    WaveItr  w;
    w.showLogs(verbose);
    w.saveLogs(prolific);
    w.initGet(fName);
    if(!w.good()){
        return;
    }
    if(hexed){
        w.dispDataHex(limit);
    }
    else{
        w.dispWaveForm(limit);
    }
}
void U_Interface::dispZeroes(){
    string	f1 = chooseWav("Choose file to analyze (must be .wav)");
    dispZeroes(f1);
}
void U_Interface::dispZeroes(string fName){
    WaveItr  w;
    w.showLogs(verbose);
    w.saveLogs(prolific);
    w.initGet(fName);
    if(!w.good()){
        return;
    }
    w.numZeroes(true);
}

void U_Interface::copyFile(){
    string	f1 = chooseFile("Choose a file to copy"),
                f2 = chooseFile("Choose a file to copy to");
    if(f1 == f2){
        cout << "Same file name: copy cancelled" << endl;
        return;
    }
    string ext1, ext2;
    if(!getExtension(f1, ext1) || !getExtension(f2, ext2) || ext1 != ext2){
        cout << "Mismatched file types: copy cancelled" << endl;
        return;
    }
    copyFile(f1, f2);
    if(ask("Verify accuracy of copied file? (y/n)", 'y')){
        verify(f1, f2, false, verbose*25);
    }
}
void U_Interface::copyFile(string f1, string f2){
    FileItr  f;
    f.showLogs(verbose);
    f.saveLogs(prolific);
    f.initGetPut(f1, f2);
    if(!f.good()){
        return;
    }
    f.copyBlock(0);
}
//In lieu of a real file chooser...
string U_Interface::chooseWav(string prompt){
    cout << prompt << endl;
    int size = 12;//adjust manually from list waveFiles
    string list[] = {
        "1Hz_Saw.wav",
        "1Hz_Sine.wav",
        "2Hz_Saw.wav",
        "1Hz_Sine_4Sec.wav",
        "2Hz_Square.wav",
        "kidRap.wav",
        "drums.wav",
        "guitar.wav",
        "fakeStereo_bass.wav",
        "dualMono_660hz.wav",
        "non-canon.wav",
        "LongSong.wav"
    };
    cout << dec;//set display to decimal before calling numbered list
    dispNumberedList<string>(list, size, 1);
    cout << size+1 << "\tEnter your own file name..." << endl;
    int choice = getIntInput(1, size+2)-1;
    string s = "./waveFiles/";
    s += (choice == size)? enterFileName("Enter file name (must be .wav):  ", ".wav") : list[choice];
    cout << "You chose:  " << s << endl;
    return s;
}
string U_Interface::choosePara(string prompt){
    cout << prompt << endl;
    int size = 5;//adjust manually from list waveFiles
    string list[] = {
        "JordanNDave.jpg",
        "Project2_WriteUp.pdf",
        "nums.bin",
        "numsText.txt",
        "prose.txt"
    };
    cout << dec;//set display to decimal before calling numbered list
    dispNumberedList<string>(list, size, 1);
    cout << size+1 << "\tEnter your own file name..." << endl;
    int choice = getIntInput(1, size+2)-1;
    string s = "./parasites/";
    s += (choice == size)? enterFileName("Enter file name:  ") : list[choice];
    cout << "You chose:  " << s << endl;
    return s;
}
string U_Interface::chooseFile(string prompt){
    cout << prompt << endl;
    int size = 17;//adjust manually from list waveFiles
    string list[] = {
        "./waveFiles/1Hz_Saw.wav",
        "./waveFiles/1Hz_Sine.wav",
        "./waveFiles/2Hz_Saw.wav",
        "./waveFiles/1Hz_Sine_4Sec.wav",
        "./waveFiles/2Hz_Square.wav",
        "./waveFiles/kidRap.wav",
        "./waveFiles/drums.wav",
        "./waveFiles/guitar.wav",
        "./waveFiles/fakeStereo_bass.wav",
        "./waveFiles/dualMono_660hz.wav",
        "./waveFiles/non-canon.wav",
        "./waveFiles/LongSong.wav",        
        "./parasites/JordanNDave.jpg",
        "./parasites/Project2_WriteUp.pdf",
        "./parasites/nums.bin",
        "./parasites/numsText.txt",
        "./parasites/prose.txt"
    };
    cout << dec;//set display to decimal before calling numbered list
    dispNumberedList<string>(list, size, 1);
    cout << size+1 << "\tEnter your own file name..." << endl;
    int choice = getIntInput(1, size+2)-1;
    string s = (choice == size)? enterFileName("Enter file name (type ./folderName/ first):  ") : list[choice];
    cout << "You chose:  " << s << endl;
    return s;
}