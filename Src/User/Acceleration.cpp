/*
 * Acceleration.cpp
 *
 *  Created on: 2014¦~8¤ë24¤é
 *      Author: YunKei
 */

#include <Acceleration.h>

using namespace Inertia;

float Acceleration::Gravity = 9.80665f;

Acceleration::Acceleration(Sensors::MPU6050* mMPU6050) : isMPU6500(false), _mMPU6050(mMPU6050), isValided(false){
	FilteredAcc.setZero();
	for(int i = 0; i < 3; i++){
		AccMovingWindowAverageFilter[i] = new MovingWindowAverageFilter(20);
	}
	Update();
}

Acceleration::Acceleration(Sensors::MPU6500* mMPU6500) : isMPU6500(true), _mMPU6500(mMPU6500), isValided(false){
	FilteredAcc.setZero();
	for(int i = 0; i < 3; i++){
		AccMovingWindowAverageFilter[i] = new MovingWindowAverageFilter(20);
	}
	Update();
}

void Acceleration::Update(){
	if(isMPU6500){
		if(_mMPU6500->getIsValided()){
			Acc = _mMPU6500->getRawAcc();
			for(int i = 0; i < 3; i++){
				AccMovingWindowAverageFilter[i]->Update(Acc[i]);
			}
			isValided = true;
		}
		else{
			isValided = false;
		}
	}
	else{
		if(_mMPU6050->getIsValided()){
			Acc = _mMPU6050->getRawAcc();
			for(int i = 0; i < 3; i++){
				AccMovingWindowAverageFilter[i]->Update(Acc[i]);
			}
			isValided = true;
		}
		else{
			isValided = false;
		}
	}
}

bool Acceleration::getIsValided(){
	return isValided;
}

Vector3f Acceleration::getAcc(){
	return Acc;
}

Vector3f Acceleration::getFilteredAcc(){
	for(int i = 0; i < 3; i++){
		FilteredAcc[i] = AccMovingWindowAverageFilter[i]->getAverage();
	}
	return FilteredAcc;
}

Vector3f Acceleration::getFilteredAngle(){
	Vector3f acc = getFilteredAcc();
	Vector3f angle;
	angle[1] = atan2(acc[1], sqrtf(acc[0] * acc[0] + acc[2] * acc[2]));
	angle[0] = atan2(-acc[0], sqrtf(acc[1] * acc[1] + acc[2] * acc[2]));
	angle[2] = 0.0f;
	return angle;
}

void Acceleration::setAcc(Vector3f value){
	Acc = value;
}

Vector3f Acceleration::getAngle(){
	Vector3f angle;
	angle[1] = atan2(Acc[1], sqrtf(Acc[0] * Acc[0] + Acc[2] * Acc[2]));
	angle[0] = atan2(-Acc[0], sqrtf(Acc[1] * Acc[1] + Acc[2] * Acc[2]));
	angle[2] = 0.0f;
	return angle;
}
