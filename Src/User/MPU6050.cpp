/*
 * mpu6050.cpp
 *
 *  Created on: 2014¦~8¤ë23¤é
 *      Author: YunKei
 */

#include <MPU6050.h>

using namespace System;
using namespace Sensors;
using namespace Math;

MPU6050Configuration::MPU6050Configuration(Communication::I2C* i2c,
											AccConf accConf, GyroConf gyroConf,
											Vector3f AccPos, Vector3f AccNeg,
											Vector3f OmegaScale, Vector3f OmegaOffset) : _i2c(i2c),
													_accConf(accConf),
													_gyroConf(gyroConf),
													_AccPos(AccPos),
													_AccNeg(AccNeg),
													_OmegaScale(OmegaScale),
													_OmegaOffset(OmegaOffset){}

bool MPU6050::setI2CBypass(bool onState){
	uint8_t data = 0;
	if(onState){
		data = 0x02;
	}
	WaitUntilFlagChangedOrTimeoutReturn(!i2cx->Write(ADDRESS,RA_INT_PIN_CFG,data), 0.003);
	return true;
}

MPU6050::MPU6050(MPU6050Configuration* conf) : Conf(conf), i2cx(conf->_i2c), isValided(false){
	for(int i = 0; i < 3; i++){
		RawAccScale[i] = 2.0f * Inertia::Acceleration::Gravity / (conf->_AccPos[i] - conf->_AccNeg[i]);
		RawAccOffset[i] = conf->_AccPos[i] * RawAccScale[i] - Inertia::Acceleration::Gravity;
		RawOmegaScale[i] = conf->_OmegaScale[i];
		RawOmegaOffset[i] = RawOmegaScale[i] * conf->_OmegaOffset[i];
	}
	FastInitialization();
	Update();
}

bool MPU6050::FastInitialization(){
	WaitUntilFlagChangedOrTimeoutReturn(!i2cx->Write(ADDRESS,RA_PWR_MGMT_1,0x00),0.003);
	WaitUntilFlagChangedOrTimeoutReturn(!i2cx->Write(ADDRESS,RA_SMPLRT_DIV,0x07),0.003);
	WaitUntilFlagChangedOrTimeoutReturn(!i2cx->Write(ADDRESS,RA_CONFIG,0x00),0.003);
	WaitUntilFlagChangedOrTimeoutReturn(!i2cx->Write(ADDRESS,RA_GYRO_CONFIG,Conf->_gyroConf),0.003);
	WaitUntilFlagChangedOrTimeoutReturn(!i2cx->Write(ADDRESS,RA_ACCEL_CONFIG,Conf->_accConf),0.003);
	return true;
}

bool MPU6050::Update(){

	float AccScale;
	float GyroScale;

	switch(Conf->_accConf){
		case MPU6050Configuration::AccConf2G:
			AccScale = 0.00006103515625f;
			break;
		case MPU6050Configuration::AccConf4G:
			AccScale = 0.0001220703125f;
			break;
		case MPU6050Configuration::AccConf8G:
			AccScale = 0.000244140625f;
			break;
		case MPU6050Configuration::AccConf16G:
			AccScale = 0.00048828125;
			break;

	}

	switch(Conf->_gyroConf){
		case MPU6050Configuration::GyroConf250:
			GyroScale = 0.00763358778625954198473282442748f;
			break;
		case MPU6050Configuration::GyroConf500:
			GyroScale = 0.01526717557251908396946564885496f;
			break;
		case MPU6050Configuration::GyroConf1000:
			GyroScale = 0.03048780487804878048780487804878f;
			break;
		case MPU6050Configuration::GyroConf2000:
			GyroScale = 0.06097560975609756097560975609756;
			break;

	}

	uint8_t data[14];
	int16_t temp;
	if(!i2cx->BurstRead(ADDRESS, RA_ACCEL_XOUT_H, 14, data)){
		FastInitialization();
		isValided = false;
		return false;
	}

	for(int i = 0; i < 14; i += 2){
		if(i >= 0 && i <= 5){
			int j = i / 2;
			temp = (data[i + 1] | (data[i] << 8));
			RawAcc[j] = (float)temp * AccScale;
			printf("%d\r\n", temp);
		}
		else if(i >= 6 && i <= 7){
			temp = (data[i + 1] | (data[i] << 8));
			temperature = (float)temp / 340.0f + 36.53f;
		}
		else if(i >= 8 && i <= 13){
			temp = data[i + 1] | (data[i] << 8);
			RawOmega[(i - 8) / 2] = (float)temp * GyroScale;
		}
	}

	for(int i = 0; i < 3; i++){
		if((RawAcc[i] != RawAcc[i]) || (RawOmega[i] != RawOmega[i])){
			isValided = false;
			return false;
		}
	}

	for(int i = 0; i < 3; i++){
		RawAcc[i] *= RawAccScale[i];
		RawAcc[i] -= RawAccOffset[i];
//		RawOmega[i] -= getGyroTemperatureCompensation(i, temperature);
		RawOmega[i] *= RawOmegaScale[i];
		RawOmega[i] -= RawOmegaOffset[i];
		if(RawOmegaOffset[i] != 0.0f){
			RawOmega[i] = MathTools::CutOff(RawOmega[i], 0.0f, 1.0f);
		}
	}

	isValided = true;
	return true;
}

bool MPU6050::getIsValided(){
	return isValided;
}

void MPU6050::setTemperature(float value){
	temperature = value;
}

float MPU6050::getTemperature(){
	return temperature;
}

void MPU6050::setRawOmegaOffset(Vector3f value){
	RawOmegaOffset = value;
}

Vector3f MPU6050::getRawOmegaOffset(){
	return RawOmegaOffset;
}

void MPU6050::setRawOmega(Vector3f value){
	RawOmega = value;
}

Vector3f MPU6050::getRawOmega(){
	return RawOmega;
}

void MPU6050::setRawAcc(Vector3f value){
	RawAcc = value;
}

Vector3f MPU6050::getRawAcc(){
	return RawAcc;
}
