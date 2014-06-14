#include "TowerGame.h"
//A crude video game for people with too much time on their hands
Tower::Tower(void) : height(0), tArray(0){}
Tower::~Tower(void){
    if(tArray){
        delete[] tArray;
        tArray = 0;
    }
}
void Tower::init(int setHeight){
    height = setHeight;
    tArray = new int[height];
}
void Tower::makeStack(){
    for (int i = 0; i < height; i++){//vertical axis
        tArray[i] = (height - i - 1) * 2;
    }
}
void Tower::resetTower(){
    for(int i = 0; i < height; i++){//vertical axis
        tArray[i] = 0;
    }
}
void Tower::setT(int i, int val){
    tArray[i] = val;
}
int Tower::getT(int i){
    return tArray[i];
}
int Tower::findNuTop(){//finds empty index after top:  lowest array index that holds a zero value
    int i;
    bool empty = true;
    for(i = 0; i < height; i++){//vertical axis
        if(tArray[i] == 0){
            break;
        }
        else{empty = false;}
    }
    if(empty){return -1;}
    return i;//indicates full
}
bool Tower::tWin(){
    return tArray[height-2] != 0;
}
bool Tower::tLose(){
    for(int i = 0; i < height-1; i++){
        if(tArray[i] < tArray[i+1]){
            return true;
        }
    }
    return false;
}

TowerGame::TowerGame(void){//default constructor plays regular game
    gameWidth = 3;
    gameHeight = 4;
    setTowerWidth();
    count = 0;
    g = new Tower[gameWidth];
    resetGame();
    play ();
    dispGame();
    cout << "It took you " << count << " moves..." << endl;
}
TowerGame::TowerGame(int mode, int setHeight){//mode sets game type
    gameWidth = 3;
    gameHeight = setHeight+1;//empty line at top of tower
    setTowerWidth();
    count = 0;
    g = new Tower[gameWidth];
    resetGame();
    if(mode == 0){
       play();
    }
    else if(mode == 1){
        playAdj();
    }
    else if(mode == 2){
        playClockWise();
    }
    else if(mode == 3){
        vector<int> v;
        solution(setHeight, 3, 1, 2, v);
        if(gameHeight>5){
            cout << "For easier reading of long games, press a key to see each move" << endl;
        }
        autoPlay(v);
    }
    else {
        vector<int> v;
        solution(setHeight, 3, 1, 2, v);
        teach(v);
    }
    dispGame();
    cout << "It took you " << count << " moves..." << endl;
}
void TowerGame::autoPlay(vector<int> &v){
    int n = v.size()-1;
    for(int i = 0; i < n; i+=2){
        dispGame();
        bar();
        //cout << "------------------------------------------------------------------" << endl;
        if(gameHeight>5){system("pause");}
        cout << "Move " << v[i] << " to " << v[i+1] << endl;
        move(v[i]-1, v[i+1]-1);
        count ++;
        if(lose() || win()){
            break;
        }
    }
}
void TowerGame::teach(vector<int> &v){
    bool clue = ask("Display clue? (y/n):  ", 'y');
    int from, to, n = v.size()-1;
    for(int i = 0; i < n; i+=2){
        //system("cls");
        quickInstruct();
    dispGame();
        if(clue){
            cout << "Move " << v[i] << " to " << v[i+1] << endl;
        }
    do{//Doesn't allow invalid moves (take from empty colums, place on full, etc.)
        cout << "From:  ";
        from = getIntInput(1, 3);
        cout << "To:  ";
        to = getIntInput(1, 3);
    }
    while (!checkTeach(v[i]-1, v[i+1]-1, from - 1, to - 1) || !move(from - 1, to - 1)); 
        //move(v[i]-1, v[i+1]-1);
        count ++;
        if(lose() || win()){
                break;
        }
    }
}
bool TowerGame::checkTeach(int f1, int t1, int from, int to){
    if(f1 == from && t1 == to){
        return true;
    }
    cout << "Not the best move...try again!" << endl;
    return false;
}
void TowerGame::solution(int disks, int to, int from, int empty, vector<int> &v){
    if (disks > 0){
        solution (disks - 1, empty, from, to, v);//saves the 1-->3 moves
        v.push_back(from);
        v.push_back(to);
        solution (disks - 1, to, empty, from, v);//saves the other moves
    }
}
TowerGame::~TowerGame(void){
    if(g){
        delete[] g;
        g = 0;
    }
}
void TowerGame::resetGame(){
    g[0].init(gameHeight);
    g[0].makeStack();//sets a stack of disks on far-left column
    for(int i = 1; i < gameWidth; i++){//horizontal axis
        g[i].init(gameHeight);
        g[i].resetTower();//set all zero
    }
}
void TowerGame::dispGame (){//Calls dispLine with one array value
    for (int i = gameHeight-1; i >= 0; i--){//vertical axis
        for (int ii = 0; ii < gameWidth; ii++){//Horizontal axis
            dispLine(g[ii].getT(i));
        }
        cout << endl;//Start new line each array index
    }
    cout << endl;//end space
}
void TowerGame::dispLine (int arrayVal){//displays row of underscores or a center-justified set of O's, equal to the value at the array index
    if (arrayVal == 0){space (towerWidth);}
    else {
        int sides = (towerWidth - arrayVal)/2;
        space (sides);
        O(arrayVal);
        space (sides);
    }
}
void TowerGame::O (int rep){//Unnecessarily recusrive method
    cout << "O";
    if (rep > 1){
        O (rep - 1);
    }
}    
void TowerGame::space (int rep){//Unnecessarily recusrive method
    cout << " ";
    if (rep > 1){
        space (rep - 1);
    }
}

void TowerGame::playAdj(){
    int from, to;
    do{
        //system("cls");
        quickInstruct();
        dispGame();
        do{//Doesn't allow invalid moves (take from empty colums, place on full, etc.)
            cout << "From:  ";
            from = getIntInput(1, 3);
            cout << "To:  ";
            to = getIntInput(1, 3);
            count ++;
        }
        while (!adj(from, to) || !move(from - 1, to - 1)); 
    }
    while(!lose() && !win());
}
void TowerGame::playClockWise(){
    int from, to;
    do{
        //system("cls");
        cout << "Enter 'from' digit:" << endl
                << "'To' digit is the location clockwise from that..." << endl;
        dispGame();
        do{//Doesn't allow invalid moves (take from empty colums, place on full, etc.)
            cout << "From:  ";
            from = getIntInput(1, 3)-1;
            to = (from == gameWidth-1)? 0 : from+1;
            count ++;
        }
        while (!move(from, to)); 
    }
    while(!lose() && !win());
}
bool TowerGame::adj(int a, int b){
	if(a-b == 1 || b-a == 1){
            return true;
	}
	cout << "Move to adjacent columns only!" << endl;
	return false;
}
void TowerGame::play(){
    int from, to;
    do{
        //system("cls");
        quickInstruct();
        dispGame();
        do{//Doesn't allow invalid moves (take from empty colums, place on full, etc.)
            cout << "From:  ";
            from = getIntInput(1, 3);
            cout << "To:  ";
            to = getIntInput(1, 3);
            count ++;
        }
        while (!move(from - 1, to - 1)); 
    }
    while(!lose() && !win());
}
bool TowerGame::move (int columnFrom, int columnTo){//this method returns false if invalid move is specified by user
    int valueMoved = 0, rowFrom = g[columnFrom].findNuTop()-1;//gets location and value of the 'from' column's top disk
    if (rowFrom < -1){//
        cout << "You cannot take from an empty column!" << endl;
        return false;
    }
    else{
        valueMoved = moveFrom(rowFrom, columnFrom );//value moved indicates size of disk being moved
    }
    int rowTo = g[columnTo].findNuTop();//Second half places valueMoved to top empty spot on 'to' column
    if (rowTo >= gameHeight-1){//Prevent array boundary exception
        cout << "You cannot place to a full column!" << endl;
        return false;
    }
    else{
        moveTo(rowTo, columnTo, valueMoved);
    }
    return true;
}
int TowerGame::moveFrom (int rowFrom, int columnFrom){//gets value of top disk and zeroes that location
    int value = g[columnFrom].getT(rowFrom);
    g[columnFrom].setT(rowFrom, 0);
    return value;
}
void TowerGame::moveTo (int rowTo, int columnTo, int value){
    if(rowTo < 0){rowTo = 0;}
    g[columnTo].setT(rowTo, value);
}
bool TowerGame::win(){
    for(int i = 1; i < gameWidth; i++){
        if(g[i].tWin()){
            cout << "Congratulations!  You win!" << endl;
            return true;
        }
    }
    return false;
}
bool TowerGame::lose(){
    bool lose = false;
    for (int i = 0; i < gameWidth; i++){//Horizontal axis
        if(g[i].tLose()){
            cout << "Sorry, wrong move!  You lose!" << endl;
            return true;
        }
    }
    return false;
}    
void TowerGame::setTowerWidth (){
    towerWidth = gameHeight * 2 + 2;
}
void TowerGame::quickInstruct (){
    cout << "Enter two digits, pressing enter after each:" << endl
        << "First digit is where you are moving a disk from." << endl
        << "Second digit is where you are moving a disk to..." << endl;
}

