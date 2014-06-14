#pragma once
#include "commons.h"

class WaveForm{
public:
	WaveForm(int max);
public:
	void single(int val);
private:
	int width, zero, maxVal;
	float scale;
};

