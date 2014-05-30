#include "WaveForm.h"

WaveForm::WaveForm(int setMax) : width(80), maxVal(setMax){//width = console width; maxVal = signedMax from calling class;
	zero = width/2;
	scale = zero/(float)maxVal;
	cout << "Scale: " << scale << endl;
}
void WaveForm::single(int val){
	if(val>=0){
		int amplitude = val*scale;
		for(int i=0;i<zero;i++){
			cout << " ";
		}
		for(int i=0;i<amplitude;i++){
			cout << "|";
		}
	}
	else{
		int amplitude = val*scale*-1;
		for(int i=0;i<zero-amplitude;i++){
			cout << " ";
		}
		for(int i=0;i<amplitude;i++){
			cout << "|";
		}
	}
	cout << endl;
}
