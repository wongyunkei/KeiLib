/*
 * MathTools.cpp
 *
 *  Created on: 2014¦~8¤ë24¤é
 *      Author: YunKei
 */

#include <MathTools.h>

using namespace Math;

float MathTools::PI = 3.14159f;
float MathTools::DEGREE_PER_RADIAN = 57.2958f;
float MathTools::RADIAN_PER_DEGREE = 0.01745f;

bool MathTools::isNaN(float value){
	if(value == value){
		return false;
	}
	else{
		return true;
	}
}

float MathTools::CalcLength(float* x, int length)
{
	float sum2 = 0;
	for(int i = 0; i < length; i++){
		sum2 += x[i] * x[i];
	}
	return sqrt(sum2);
}

float MathTools::Sign(float value){

	return value / fabs(value);

}

float MathTools::DegreeToRadian(float degree){
	return degree * RADIAN_PER_DEGREE;
}

float MathTools::RadianToDegree(float radian){
	return radian * DEGREE_PER_RADIAN;
}

float MathTools::Trim(float lowerBound, float value, float upperBound){

	return (value > upperBound ? upperBound : value < lowerBound ? lowerBound : value);

}

float MathTools::CutOff(float value, float BoundValue, float Bound){

	return ((value < Bound && value > -Bound) ? BoundValue : value);

}

float MathTools::TrimResolution(float value, int precision){
	int scale = 1;
	for(int i = 0; i < precision; i++){
		scale *= 10;
	}
	return (float)((int)(value * scale)) / (float)scale;
}

bool MathTools::CheckWithInInterval(float value, float mean, float var){
	if(fabs(value - mean) < var){
		return true;
	}
	else{
		return false;
	}
}
