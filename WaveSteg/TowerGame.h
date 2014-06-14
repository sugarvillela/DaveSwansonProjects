#pragma once
#include "Commons.h"

class Tower{
public:
	Tower(void);
	~Tower(void);
	void init(int setHeight);
	void makeStack();
	void resetTower();
	int findNuTop();
	bool tWin();
	bool tLose();
	void setT(int i, int val);
	int getT(int i);
private:
	int * tArray;
	int height;
};
class TowerGame{
public:
	TowerGame(void);
	TowerGame(int mode, int setHeight);
	//TowerGame(vector<int> &v, int setHeight);
	~TowerGame(void);
	void play();
	void playAdj();
	void playClockWise();
	void autoPlay(vector<int> &v);
	void teach(vector<int> &v);
	bool checkTeach(int f1, int t1, int from, int to);
	bool adj(int a, int b);
	bool move(int columnFrom, int columnTo);
	int moveFrom(int rowFrom, int columnFrom);
	void moveTo(int rowTo, int columnTo, int value);
	//int findTop(int column);
	bool win();
	bool lose();   
	
	void dispGame();
	void dispLine (int arrayVal);
	void O(int rep);    
	void space(int rep);
	void resetGame();
	void quickInstruct();
private:
	void setTowerWidth();
	void solution(int disks, int to, int from, int empty, vector<int> &v);
	Tower * g;
	int gameWidth, gameHeight;
	int towerWidth, count;//Width sets width of each tower's display. Relative to gameWidth
};

