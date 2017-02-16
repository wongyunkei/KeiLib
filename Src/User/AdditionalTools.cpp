/*
 * AdditionalTools.cpp
 *
 *  Created on: 2014¦~8¤ë7¤é
 *      Author: YunKei
 */

#include <AdditionalTools.h>

using namespace Utility;

float buffer[10][10];
float m[10][10][10];

void AdditionalTools::setBuffer(int index, float* buf, int length){
	for(int i = 0; i < length; i++){
		buffer[index][i] = buf[i];
	}
}

void setBuffer(int index, VectorXf buf, int length){
	for(int i = 0; i < length; i++){
		buffer[index][i] = buf[i];
	}
}

float* AdditionalTools::getBuffer(int index){
	return buffer[index];
}

VectorXf AdditionalTools::getVector(int index, int length){
	VectorXf v(length);
	for(int i = 0; i < length; i++){
		v[i] = buffer[index][i];
	}
	return v;
}

float** AdditionalTools::getMatrix(int index){
	return (float**)m[index];
}

MatrixXf AdditionalTools::getMatrix(int index, int rows, int cols){
	MatrixXf M(rows, cols);
	for(int i = 0; i < rows; i++){
		for(int j = 0; j < rows; j++){
			M(i,j) = m[index][i][j];
		}
	}
	return M;
}

void AdditionalTools::printfBuffer(int index, int length){
	for(int i = 0; i < length; i++){
		printf("%g", buffer[index][i]);
		if(i == length - 1){
			printf("\r\n");
		}
		else{
			printf(",");
		}
	}
}

void AdditionalTools::printMatix(char* label, MatrixXf M,int rows, int cols){
	printf("%s:\r\n", label);
	for(int i = 0; i < rows; i++){
		for(int j = 0; j < cols; j++){
			printf("%g\t", M(i,j));
		}
		printf("\r\n");
	}
	printf("\r\n");
}
