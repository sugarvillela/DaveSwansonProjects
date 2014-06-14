/*
 * Wave Steganographer
 * Dave Swanson
 * 06/06/14
 */
#include "U_Interface.h"
#include "TowerGame.h"
using namespace std;

void dispSol(int disks, int to, int from, int empty);
void gMenu();
void play();

void menu(){
    cout << endl << "Choose a task:" << endl
        << "1:  Hide a watermark file (any type) into a wave file" << endl
        << "2:  Hide a watermark (you enter string) into a wave file" << endl
        << "3:  Decode a watermark to string or file" << endl
        << "4:  Verify that two files (any type) are identical" << endl
        << "5:  Fix (canonize) a non-standard wave file" << endl
        << "6:  Test for a false-stereo wave file" << endl
        << "7:  Display a file (any type) in hex" << endl
        << "8:  Display a wave file header" << endl
        << "9:  Display wave data as graph or hex" << endl
        << "10:  Display the number of zero crossings in wave data" << endl
        << "11:  Copy a file" << endl
        << "12:  Edit Preferences" << endl << endl
        << "13:  Play Towers of Hanoi game" << endl
        << "ANY OTHER VALUE EXITS THE PROGRAM" << endl;
}
int main(){
    U_Interface u;
    while(true){
        menu();
        int c = getIntInput();
        //system("cls");
        switch(c){
        case 1://Hide a watermark file (any type) into a wave file
                u.encodeFileToFile();
                break;
        case 2://Hide a watermark (you enter string) into a wave file
                u.encodeStringToFile();
                break;
        case 3://Decode a watermark to string or file
                u.decode();
                break;
        case 4://Verify that two files (any type) are identical
                u.verify();
                break;
        case 5://Fix a non-standard wave file
                u.canonize();
                break;
        case 6://Test for a false-stereo wave file
                u.testStereo();
                break;
        case 7://Display a file (any type) in hex
                u.dispFile();
                break;
        case 8://Display a wave file header in friendly or hex
                u.dispHeader();
                break;
        case 9://Display wave data as graph or hex
                u.dispWaveData();
                break;
        case 10://Display the number of zero crossings in wave data
                u.dispZeroes();
                break;
        case 11://Copy a file setPreferences()
                u.copyFile();
                break;
        case 12://Edit Preferences
                u.setPreferences();
                break;
        case 13://Easter egg
                play();
                break;                    
        default:
                cout << "Exiting..." << endl;
                return 0;
        }
        //system("pause");
        //system("cls");
    }
    return 0;
}

void dispSol(int disks, int to, int from, int empty){
    if (disks > 0){
        dispSol (disks - 1, empty, from, to);//Prints the 1-->3 moves
		cout << "Move " << from << " --> " << to << endl;
        dispSol (disks - 1, to, empty, from);//Prints the other moves
    }
}
void gMenu(){
    cout << "Choose a task:" << endl
        << "1:  Play regular game" << endl
        << "2:  Allow moves to adjacent towers only" << endl
        << "3:  Allow clockwise moves only" << endl
        << "4:  Display solution for regular game" << endl
        << "5:  Teach regular game" << endl
        << "6:  Autoplay regular game" << endl
        << "ANY OTHER VALUE RETURNS TO MAIN MENU" << endl;
}
void play(){
    cout << "Towers of Hanoi Game" << endl
            << "By Dave Swanson..." << endl << endl;

    while(true){
        gMenu();
        char c = getCharInput();
        //system("cls");
        switch(c){
        case '1'://Play Regular
            {
                cout << "Game is three towers wide; enter height:  ";
                TowerGame game(0,getIntInput(0, 10));
            }
            break;
        case '2'://Allow moves to adjacent towers only
            {
                cout << "Game is three towers wide; enter height:  ";
                TowerGame game(1,getIntInput(0, 10));
            }
            break;
        case '3'://Allow clockwise moves only
            {
                cout << "Game is three towers wide; enter height:  ";
                TowerGame game(2,getIntInput(0, 10));
            }
            break;
        case '4'://Display winning moves
            {
                cout << "Game is three towers wide; enter height:  ";
                int height = getIntInput(0, 10);
                dispSol(height, 3, 1, 2);
            }
            break;
        case '5'://Teach
            {
                cout << "Game is three towers wide; enter height:  ";
                TowerGame game(4,getIntInput(0, 10));
            }
            break;
        case '6'://Autoplay
            {
                cout << "Game is three towers wide; enter height:  ";
                TowerGame game(3,getIntInput(0, 10));
            }
            break;
        default:
            cout << "Exiting..." << endl;
            return;
        }
        //system("pause");
        //system("cls");
    }
}
