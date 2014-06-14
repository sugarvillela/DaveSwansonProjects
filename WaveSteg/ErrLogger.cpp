#include "ErrLogger.h"

Fact::Fact(void) :
    time(""), funct(""), mCode(0), mComment(""), err(false) {}

Fact::Fact(string setTime, string setFunct, int setMCode, string setMComment, bool setErr) :
    time(setTime), funct(setFunct), mCode(setMCode), mComment(setMComment), err(setErr) {
            uiTime = tStrToInt(setTime);
    }

ErrLogger::ErrLogger(void) : client("") {}
ErrLogger::~ErrLogger(void){
    clrLog();
}
void ErrLogger::clrLog(){
    int n = v.size();
    for (int i = 0; i < n; ++i) {
        delete v[i]; //deallocates *v[i]
    }
    v.clear();
}
void ErrLogger::setClient(string setMe){
    client = setMe;
}
void ErrLogger::addToLog(string setFunct, int setMCode, string setMComment, bool setErr){
    string setTime = curTime();
    v.push_back(new Fact(setTime, setFunct, setMCode, setMComment, setErr));
}
void ErrLogger::sortByTime(){
    int n = v.size();
    for(int i=0;i<n;i++){
        unsigned int minVal = v[i]->uiTime;
        int minIndex = i;
        for(int ii=minIndex;ii<n;ii++){
            unsigned int test = v[ii]->uiTime;
            if(test < minVal){
                minVal = test;
                minIndex = ii;
            }
        }
        swap(v[i], v[minIndex]);
    }
}
void ErrLogger::sortByErr(){
    int n = v.size();
    for(int i=0;i<n;i++){
        bool minVal = v[i]->err;
        int minIndex = i;
        for(int ii=minIndex;ii<n;ii++){
            bool test = v[ii]->err;
            if(test < minVal){
                minVal = test;
                minIndex = ii;
            }
        }
        swap(v[i], v[minIndex]);
    }
}
void ErrLogger::dispLog(){
    int n = v.size();
    if(n == 0){
        return;
    }
    bar();
    cout << "Display Log:  " << curTime() << endl << endl ;
    for (int i = 0; i < n; ++i) {
        string ev = (v[i]->err)? "Error:  ":"Event:  ";
        cout << ev;
        cout << v[i]->time << endl 
            << "\t" << v[i]->funct << endl 
            << "\t" << msgs[v[i]->mCode] << endl
            << "\t" << v[i]->mComment << endl << endl;
    }
    bar();
}
void ErrLogger::logToFile(){
    int n = v.size();
    if(n == 0){
        return;
    }
    ofstream fout;
    fout.open("./ws_log/WSLog.txt", ios::app);
    if(!fout.good()){
        return;
    }
    bar(fout);
    fout << "Display Log:  " << curTime() << endl << endl ;
    for (int i = 0; i < n; ++i) {
        string ev = (v[i]->err)? "Error:  ":"Event:  ";
        fout << ev;
        fout << v[i]->time << endl 
            << "\t" << v[i]->funct << endl 
            << "\t" << msgs[v[i]->mCode] << endl
            << "\t" << v[i]->mComment << endl << endl;
    }
    bar(fout);
    fout.close();        
}