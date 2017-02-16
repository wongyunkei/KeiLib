/*
 * Omega.cpp
 *
 *  Created on: 2014¦~8¤ë24¤é
 *      Author: YunKei
 */

#include <Omega.h>

Omega::Omega(MPU6050* mMPU6050) : isMPU6500(false), _mMPU6050(mMPU6050), isValided(false){
	for(int i = 0; i < 3; i++){
		mOmegaMovingWindowAverageFilter[i] = new MovingWindowAverageFilter(5);
	}
	_FilteredOmega.setZero();
	Update();
}

Omega::Omega(MPU6500* mMPU6500) : isMPU6500(true), _mMPU6500(mMPU6500), isValided(false){
	for(int i = 0; i < 3; i++){
		mOmegaMovingWindowAverageFilter[i] = new MovingWindowAverageFilter(5);
	}
	_FilteredOmega.setZero();
	Update();
}

void Omega::Update(){
	if(isMPU6500){
		if(_mMPU6500->getIsValided()){
			_Omega = _mMPU6500->getRawOmega();
			for(int i = 0; i < 3; i++){
				mOmegaMovingWindowAverageFilter[i]->Update(_mMPU6500->getRawOmega()[i]);
			}
			isValided = true;
		}
		else{
			isValided = false;
		}
	}
	else{
		if(_mMPU6050->getIsValided()){
			_Omega = _mMPU6050->getRawOmega();
			for(int i = 0; i < 3; i++){
				mOmegaMovingWindowAverageFilter[i]->Update(_mMPU6050->getRawOmega()[i]);
			}
			isValided = true;
		}
		else{
			isValided = false;
		}
	}
}

bool Omega::getIsValided(){
	return isValided;
}


Vector3f Omega::getFilteredOmega(){
	for(int i = 0; i < 3; i++){
		_FilteredOmega[i] = mOmegaMovingWindowAverageFilter[i]->getAverage();
	}
	return _FilteredOmega;
}

Vector3f Omega::getOmega(){
	return _Omega;
}

void Omega::setOmega(Vector3f value){
	_Omega = value;
}
