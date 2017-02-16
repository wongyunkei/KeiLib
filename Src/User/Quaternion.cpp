/*
 * Quaternion.cpp
 *
 *  Created on: 2014�~8��24��
 *      Author: YunKei
 */

#include <App.h>
#include <Quaternion.h>
#include <MathTools.h>
#include <math.h>
#include <Kalman.h>
#include <MPU6050.h>
#include <MPU6500.h>
#include <stdio.h>
#include <Pid.h>
#include <Task.h>
#include <HMC5883L.h>
#include <Controlling.h>
#include <Delay.h>
#include <AdditionalTools.h>
#include <Communicating.h>
#include <Led.h>
#include <UART.h>
#include <EncoderYaw.h>

using namespace Utility;
using namespace Math;

Quaternion::Quaternion(Acceleration* mAcceleration, Omega* mOmega) : _mAcceleration(mAcceleration), _mOmega(mOmega), IsUseCompass(false), IsUseEncoderYaw(false), _mCompass(0), Interval(0), Valid(false), _mEncoderYaw((EncoderYaw*)0){
	PrevT.setZero();
	PrevAT.setZero();
	PrevMT.setZero();
	Matrix3f Q;
	Q.setIdentity();
	Q *= 1e-6f;
//	Q(2,2) = 1e-12f;
	Matrix3f R;
	R.setIdentity();
	R *= 1e-4f;
	CorrectionPid[0] = new Pid(0.05,0,0,10000);
	CorrectionPid[1] = new Pid(0.05,0,0,10000);
	CorrectionPid[2] = new Pid(0.05,0,0,10000);
//	R(2,2) *= 1e-6f;
	_QuaternionKalman = new Kalman(mAcceleration->getFilteredAngle(), Q, R);
	PrevTick = App::mApp->mTicks->getTicks();
//	mean[0] = new MovingWindowAverageFilter(100);
//	mean[1] = new MovingWindowAverageFilter(100);
//	mean[2] = new MovingWindowAverageFilter(100);
}

Quaternion::Quaternion(Acceleration* mAcceleration, Omega* mOmega, Compass* mCompass) : _mAcceleration(mAcceleration), _mOmega(mOmega), IsUseCompass(true), IsUseEncoderYaw(false), Interval(0), Valid(false), _mCompass(mCompass), _mEncoderYaw((EncoderYaw*)0){
	_Quaternion << 1, 0, 0, 0;
	PrevT.setZero();
	PrevAT.setZero();
	PrevMT.setZero();
	CorrectionPid[0] = new Pid(0.05,0,0,10000);
	CorrectionPid[1] = new Pid(0.05,0,0,10000);
	CorrectionPid[2] = new Pid(0.05,0,0,10000);
	Matrix3f Q;
	Q.setIdentity();
	Q *= 1e-9f;
	MatrixXf R(6,6);
//	MatrixXf R(3,3);
	R.setIdentity();
	R *= 1e2f;
	_QuaternionKalman = new Kalman(mAcceleration->getFilteredAngle(), Q, R);
	PrevTick = App::mApp->mTicks->getTicks();
}


Quaternion::Quaternion(Acceleration* mAcceleration, Omega* mOmega, EncoderYaw* mEncoderYaw) : _mAcceleration(mAcceleration), _mOmega(mOmega), IsUseCompass(false), IsUseEncoderYaw(true), _mCompass(0), Interval(0), Valid(false), _mEncoderYaw(mEncoderYaw){
	PrevT.setZero();
	Matrix3f Q;
	Q.setIdentity();
	Q *= 1e-6f;
	Matrix3f R;
	R.setIdentity();
	R *= 1e-4f;
	CorrectionPid[0] = new Pid(0.05,0,0,10000);
	CorrectionPid[1] = new Pid(0.05,0,0,10000);
	CorrectionPid[2] = new Pid(0.05,0,0,10000);
	_QuaternionKalman = new Kalman(mAcceleration->getAngle(), Q, R);
	PrevTick = App::mApp->mTicks->getTicks();
}

Vector4f Quaternion::getConjugate(Vector4f Q){
	Vector4f P;
	P << Q[0], -Q[1], -Q[2], -Q[3];
	return P;
}

Vector3f Quaternion::QuaternionActOnVector3(Vector4f Q, Vector3f X){
	Vector3f P;
	Vector4f p;
	Vector4f x;
	x << 0, X[0], X[1], X[2];
	Vector4f Q_T = getConjugate(Q);
	p = QuaternionMultipication(QuaternionMultipication(Q, x), Q_T);
	P << p[1], p[2], p[3];

//	printf("x:  %g  %g  %g  %g\r\n", x[0], x[1], x[2], x[3]);
//	printf("Q_T:  %g  %g  %g  %g\r\n", Q_T[0], Q_T[1], Q_T[2], Q_T[3]);
//	printf("p:  %g  %g  %g  %g\r\n", p[0], p[1], p[2], p[3]);
	return P;
}

Vector4f Quaternion::QuaternionMultipication(Vector4f Q, Vector4f P){
	Vector4f QP;
	float q0 = Q[0];
	Vector3f q;
	q << Q[1], Q[2], Q[3];
	float p0 = P[0];
	Vector3f p;
	p << P[1], P[2], P[3];
	float qp0 = q0 * p0 - q.dot(p);
	Vector3f qp = q0 * p + p0 * q + q.cross(p);
	QP << qp0, qp[0], qp[1], qp[2];
	return QP;
}

bool Quaternion::Update(){
	Vector3f o;
	Vector3f a;
	Vector3f m;
	if(_mOmega->getIsValided()){
		o = _mOmega->getFilteredOmega() * MathTools::RADIAN_PER_DEGREE;
	}
	else{
		return false;
	}

	if(_mAcceleration->getIsValided()){
		a = _mAcceleration->getFilteredAcc();
	}
	else{
		return false;
	}
	if(_mCompass != 0){
		if(_mCompass->getIsValided()){
			m = _mCompass->getFilteredMag();
		}
		else{
			return false;
		}
		Interval = App::mApp->mTicks->getTicks() - PrevTick;
		PrevTick = App::mApp->mTicks->getTicks();
		Interval /= 1000.0f;
		if(Interval <= 0){
			Valid = false;
			return false;
		}
		MadgwickAHRSupdate(o[0], o[1], o[2], a[0], a[1], a[2], m[0], m[1], m[2], Interval);
		float q[4];
		getQuaternion(q);
		_Quaternion << q[0], q[1], q[2], q[3];
		_Euler = QuaternionToEuler(_Quaternion);
		Valid = true;
		return true;
	}
	else if(_mCompass == 0){
		Interval = App::mApp->mTicks->getTicks() - PrevTick;
		PrevTick = App::mApp->mTicks->getTicks();
		Interval /= 1000.0f;
		if(Interval <= 0){
			Valid = false;
			return false;
		}
		MadgwickAHRSupdate(o[0], o[1], o[2], a[0], a[1], a[2], 0,0,0, Interval);
		float q[4];
		getQuaternion(q);
		_Quaternion << q[0], q[1], q[2], q[3];
		_Euler = QuaternionToEuler(_Quaternion);
		Valid = true;
		return true;
	}

}

//bool Quaternion::Update(){
//	Vector4f Q = _Quaternion;
//	Vector4f P;
//	Vector4f q;
//	Vector4f aq;
//	Vector4f mq;
//	Vector4f p;
//	Vector4f ap;
//	Vector4f mp;
//	Vector4f _o;
//	Vector3f b;
//	Vector3f g;
//	Vector3f _b;
//	Vector3f _g;
//	Vector3f o;
//	Vector3f a;
//	Vector3f m;
//	Vector3f e;
//	Vector3f ae;
//	Vector3f me;
//	g << 0, 0, 1;
//	if(_mOmega->getIsValided()){
//		o = _mOmega->getFilteredOmega() * MathTools::RADIAN_PER_DEGREE;
//	}
//	else{
//		return false;
//	}
//
//	if(_mAcceleration->getIsValided()){
//		a = _mAcceleration->getFilteredAcc();
//		a.normalize();
//	}
//	else{
//		return false;
//	}
//
//	if(_mCompass->getIsValided()){
//		m = _mCompass->getFilteredMag();
//		m.normalize();
//		b << sqrt(m[0] * m[0] + m[1] * m[1]), 0, m[2];
//	}
//	else{
//		return false;
//	}
//
//	_o << 0, o[1], o[0], o[2];
//	_o = QuaternionMultipication(Q, _o);
//	o << _o[1], _o[2], _o[3];
//	_g = QuaternionActOnVector3(Q, g) - a;
//	_b = QuaternionActOnVector3(Q, b) - m;
////	printf("o:  %g  %g  %g\r\n", o[0], o[1], o[2]);
////	printf("_g:  %g  %g  %g\r\n", _g[0], _g[1], _g[2]);
////	printf("a:  %g  %g  %g\r\n", a[0], a[1], a[2]);
////	printf("_b:  %g  %g  %g\r\n", _b[0], _b[1], _b[2]);
////	printf("m:  %g  %g  %g\r\n", m[0], m[1], m[2]);
//
//	Vector3f s;
//	Vector3f f;
//	s << _g[0], _g[1], _b[2];
//	f = o;
//	for(int i = 0; i < 3; i++){
//		o[i] += CorrectionPid[i]->pid(o[i], s[i]);
//	}
//
////	printf("f:  %g  %g  %g\r\n", f[0], f[1], f[2]);
////	printf("o:  %g  %g  %g\r\n", o[0], o[1], o[2]);
//	Interval = App::mApp->mTicks->getTicks() - PrevTick;
//	PrevTick = App::mApp->mTicks->getTicks();
//	Interval /= 1000.0f;
//	if(Interval <= 0){
//		Valid = false;
//		return false;
//	}
//
//	p << 0, o[0], o[1], o[2];
//	p *= 0.5f * Interval;
//	q = Q + 0.5f * (p + PrevT);
//	q.normalize();
//	PrevT = p;
//	e = QuaternionToEuler(q);
//
//	ap << 0, s[0], s[1], s[2];
//	ap *= 0.5f * Interval;
//	aq = Q + 0.5f * (ap + PrevAT);
//	aq.normalize();
//	PrevAT = ap;
//	ae = QuaternionToEuler(aq);
////
////	mp << 0, _b[0], _b[1], _b[2];
////	mp *= 0.5f * Interval;
////	mq = Q + 0.5f * (mp + PrevMT);
////	mq.normalize();
////	PrevMT = mp;
////	me = QuaternionToEuler(mq);
////
////	VectorXf y(6);
////	y << ae[0], ae[1],ae[2], me[0], me[1], me[2];
////
////	Matrix3f A;
////	A.setIdentity();
//////	Matrix3f H;
//////	H.setIdentity();
////	MatrixXf H(6,3);
////	H << 1,1,1,
////		 1,1,1,
////		 1,1,1,
////		 1,1,1,
////		 1,1,1,
////		 1,1,1;
////
//////	MatrixXf H(3,3);
//////	H << 1,1,1,
//////		 1,1,1,
//////		 1,1,1;
////	//if(_QuaternionKalman->Filtering(A, _o, H, e)){
////		_QuaternionKalman->Filtering(A, e, H, y);
////		Vector3f _e = _QuaternionKalman->getCorrectedData();
//////		p << 0, _e[0], _e[1], _e[2];
//////		p << 0, o[0], o[1], o[2];
//////		ap << 0, _g[0], _g[1], _g[2];
//////		mp << 0, _b[0], _b[1], _b[2];
//////		printf("o:  %g  %g  %g\r\n", o[0], o[1], o[2]);
//////		printf("_e:  %g  %g  %g\r\n", _e[0], _e[1], _e[2]);
//////			printf("_g:  %g  %g  %g\r\n", _g[0], _g[1], _g[2]);
//////			printf("a:  %g  %g  %g\r\n", a[0], a[1], a[2]);
//static int delayCount = 0;
//if(delayCount++ == 10){
////		printf("\r\ne:  %g  %g  %g\r\n", MathTools::RadianToDegree(e[0]), MathTools::RadianToDegree(e[1]), MathTools::RadianToDegree(e[2]));
////		printf("ae:  %g  %g  %g\r\n", MathTools::RadianToDegree(ae[0]), MathTools::RadianToDegree(ae[1]), MathTools::RadianToDegree(ae[2]));
//////		printf("me:  %g  %g  %g\r\n", MathTools::RadianToDegree(me[0]), MathTools::RadianToDegree(me[1]), MathTools::RadianToDegree(me[2]));
//////		printf("_e:  %g  %g  %g\r\n", MathTools::RadianToDegree(_e[0]), MathTools::RadianToDegree(_e[1]), MathTools::RadianToDegree(_e[2]));
//		delayCount = 0;
//}
//		_Euler = e;
//		_Quaternion = EulerToQuaternion(_Euler);
//
////		return true;
////	}
////	else{
////		Vector3f _e = _QuaternionKalman->getCorrectedData();
////		p << 0, _e[0], _e[1], _e[2];
////		printf("p:  %g  %g  %g  %g\r\n", p[0], p[1], p[2], p[3]);
////	}
////	return false;
//	return true;
//
//}

//bool Quaternion::Update(){
//	if(_mOmega->getIsValided()){
////		Vector3f omega = _mOmega->getFilteredOmega() * MathTools::RADIAN_PER_DEGREE;
//		Vector3f omega = _mOmega->getOmega() * MathTools::RADIAN_PER_DEGREE;
//		Vector4f q;
//		q << 0, omega[0], omega[1], omega[2];
//		Vector4f t;
//		t[0] = -q[1]*_Quaternion[1]-q[2]*_Quaternion[2]-q[3]*_Quaternion[3];
//		t[1] = q[1]*_Quaternion[0]+q[2]*_Quaternion[3]-q[3]*_Quaternion[2];
//		t[2] = -q[1]*_Quaternion[3]+q[2]*_Quaternion[0]+q[3]*_Quaternion[1];
//		t[3] = q[1]*_Quaternion[2]-q[2]*_Quaternion[1]+q[3]*_Quaternion[0];
//		Interval = App::mApp->mTicks->getTicks() - PrevTick;
//		PrevTick = App::mApp->mTicks->getTicks();
//		Interval /= 1000.0f;
//		if(Interval <= 0){
//			Valid = false;
//			return false;
//		}
//		t *= 0.5f * Interval;
//		q = _Quaternion + 0.5f * (t + PrevT);
//		PrevT = t;
//		q.normalize();
//		Vector3f e = QuaternionToEuler(q);
//		bool valid = true;
//		bool AccValid = _mAcceleration->getIsValided() && (_mAcceleration->getFilteredAcc().norm() > Acceleration::Gravity * 0.75f) && (_mAcceleration->getFilteredAcc().norm() < Acceleration::Gravity * 1.25f);
//		Vector3f angle;
//		bool MagValid;
//
//		if(IsUseCompass){
//			MagValid = _mCompass->getIsValided() && _mCompass->getMag().norm() > 0.75f && _mCompass->getMag().norm() < 1.25f;
//		}
//		AccValid = true;
//		if(AccValid){
//			angle = _mAcceleration->getFilteredAngle();
//			if(IsUseCompass && MagValid){
//				angle[2] = _mCompass->getAngle()[2];
//			}
//			else if(IsUseEncoderYaw){
//				angle[2] = _mEncoderYaw->getYaw();
//			}
//			else{
//				angle[2] = e[2];
//			}
//		}
//		else{
//			angle[0] = e[0];
//			angle[1] = e[1];
//			if(IsUseCompass && MagValid){
//				angle[2] = _mCompass->getAngle()[2];
//			}
//			else if(IsUseEncoderYaw){
//				angle[2] = _mEncoderYaw->getYaw();
//			}
//			else{
//				angle[2] = e[2];
//			}
//		}
//
//		Matrix3f A;
//		A.setIdentity();
//		Matrix3f H;
//		H.setIdentity();
//		static int delayCount = 0;
//		if(delayCount++ >= 10 && valid && AccValid){
//			delayCount = 0;
//			if(_QuaternionKalman->Filtering(A, e, H, angle)){
//				angle = _QuaternionKalman->getCorrectedData();
//				for(int i = 0; i < 3; i++){
//					if(angle[i] != angle[i]){
//						Valid = false;
//						return false;
//					}
//				}
//				if(IsUseEncoderYaw && fabs(fabs(_mEncoderYaw->getYaw()) - MathTools::PI) < 0.01f){
//					angle[2] = _mEncoderYaw->getYaw();
//				}
//				_Euler = angle;
//			}
//		}
//		else{
//			_Euler = e;
//		}
//		_Quaternion = EulerToQuaternion(_Euler);
//		_Quaternion.normalize();
////		mean[0]->Update(_Euler[0]);
////		mean[1]->Update(_Euler[1]);
////		mean[2]->Update(_Euler[2]);
//		mean[0] += _Euler[0];
//		mean[1] += _Euler[1];
//		mean[2] += _Euler[2];
//		mean[0] /= 2;
//		mean[1] /= 2;
//		mean[2] /= 2;
//		return true;
//	}
//	else{
//		Valid = false;
//		return false;
//	}
//}

Matrix4f Quaternion::calcStateTransMatrix(Vector3f w, float t){
	Matrix4f A;
	A << 0, -w[0], -w[1], -w[2],
		w[0], 0, w[2], -w[1],
		w[1], -w[2], 0, w[0],
		w[2], w[1], -w[0], 0;
	A *= 0.5f;
	Matrix4f T;
	T.setIdentity();
	T += A * t;
	return T;
}

Eigen::Matrix<float, 3, 4> Quaternion::calcQuatToEulerMeasMatrix(Vector4f q){
	Eigen::Matrix<float, 3, 4> C;
	Matrix3f R = QuaternionToMatrix(q);
	Eigen::Matrix<float, 3, 4> n;
	n << 2*R(2,2)*q[1],
		 2*(R(2,2)*q[0]+2*R(1,2)*q[1]),
		 2*(R(2,2)*q[3]+2*R(1,2)*q[2]),
		 2*R(2,2)*q[2],
		 2*q[2],
		 -2*q[3],
		 2*q[0],
		 -2*q[1],
		 2*R(0,0)*q[3],
		 2*R(0,0)*q[2],
		 2*(R(0,0)*q[0]+2*R(0,1)*q[2]),
		 2*(R(0,0)*q[0]+2*R(0,1)*q[3]);
	Eigen::Matrix<float, 3, 1> d;
	d << R(2,2)*R(2,2)+R(1,2)*R(1,2),
		 sqrtf(1-R(0,2)*R(0,2)),
		 R(0,0)*R(0,0)+R(0,1)*R(0,1);
	C << -n(0,0)/d(0,0),-n(0,1)/d(0,0),-n(0,2)/d(0,0),-n(0,3)/d(0,0),
		 n(1,0)/d(1,0),n(1,1)/d(1,0),n(1,2)/d(1,0),n(1,3)/d(1,0),
		 -n(2,0)/d(2,0),-n(2,1)/d(2,0),-n(2,2)/d(2,0),-n(2,3)/d(2,0);
	return C;
}

Vector3f Quaternion::QuaternionToEuler(Vector4f q){
	Vector3f euler;
	euler[0] = atan2f(2 * (q[0] * q[1] + q[2] * q[3]), 1 - 2 * (q[1] * q[1] + q[2] * q[2]));
	float a = 2 * (q[0] * q[1] + q[2] * q[3]), b = 1 - 2 * (q[1] * q[1] + q[2] * q[2]);
	float r32 = 2 * (q[0] * q[1] + q[2] * q[3]);
	float r33 = 1 - 2 * (q[1] * q[1] + q[2] * q[2]);
	euler[1] = atan2f(-2 * (q[1] * q[3] - q[0] * q[2]), sqrtf(r32 * r32 + r33 * r33));
	euler[2] = atan2f(2 * (q[0] * q[3] + q[1] * q[2]), 1 - 2 * (q[2] * q[2] + q[3] * q[3]));

//	euler[0] = atan2f(2 * (q[0] * q[1] - q[2] * q[3]), 1 - 2 * (q[1] * q[1] + q[3] * q[3]));
//	euler[1] = asinf(2 * (q[1] * q[2] - q[0] * q[3]));
//	euler[2] = atan2f(2 * (q[0] * q[2] - q[1] * q[3]), 1 - 2 * (q[2] * q[2] + q[3] * q[3]));
//	if(q[1]*q[2]+q[0]*q[3] == 0.5){
//		euler[2] = 2*atan2f(q[1],q[0]);
//		euler[0] = 0;
//	}
//	if(q[1]*q[2]+q[0]*q[3] == -0.5){
//		euler[2] = -2*atan2f(q[1],q[0]);
//		euler[0] = 0;
//	}

	return euler;
}

Vector4f Quaternion::EulerToQuaternion(Vector3f euler){

	float CosHalfRoll = cosf(euler[0] / 2.0f), SinHalfRoll = sinf(euler[0] / 2.0f);
	float CosHalfPitch = cosf(euler[1] / 2.0f), SinHalfPitch = sinf(euler[1] / 2.0f);
	float CosHalfYaw = cosf(euler[2] / 2.0f), SinHalfYaw = sinf(euler[2] / 2.0f);
	Vector4f q;
	q[0] = CosHalfRoll * CosHalfPitch * CosHalfYaw + SinHalfRoll * SinHalfPitch * SinHalfYaw;
	q[1] = SinHalfRoll * CosHalfPitch * CosHalfYaw - CosHalfRoll * SinHalfPitch * SinHalfYaw;
	q[2] = CosHalfRoll * SinHalfPitch * CosHalfYaw + SinHalfRoll * CosHalfPitch * SinHalfYaw;
	q[3] = CosHalfRoll * CosHalfPitch * SinHalfYaw - SinHalfRoll * SinHalfPitch * CosHalfYaw;
	return q;
}

Vector4f Quaternion::FixedAnglesToQuaternion(Vector3f fixedAngles){
	float CosHalfRoll = cosf(fixedAngles[0] / 2.0f), SinHalfRoll = sinf(fixedAngles[0] / 2.0f);
	float CosHalfPitch = cosf(fixedAngles[1] / 2.0f), SinHalfPitch = sinf(fixedAngles[1] / 2.0f);
	float CosHalfYaw = cosf(fixedAngles[2] / 2.0f), SinHalfYaw = sinf(fixedAngles[2] / 2.0f);
	Vector4f q;
	q[0] = CosHalfRoll * CosHalfPitch * CosHalfYaw - SinHalfRoll * SinHalfPitch * SinHalfYaw;
	q[1] = CosHalfRoll * SinHalfPitch * SinHalfYaw + SinHalfRoll * CosHalfPitch * CosHalfYaw;
	q[2] = CosHalfRoll * SinHalfPitch * CosHalfYaw - SinHalfRoll * CosHalfPitch * SinHalfYaw;
	q[3] = CosHalfRoll * CosHalfPitch * SinHalfYaw + SinHalfRoll * SinHalfPitch * CosHalfYaw;
	return q;
}

Matrix3f Quaternion::EulerToMatrix(Vector3f euler){
	Matrix3f R;
	float CosRoll = cosf(euler[0]), SinRoll = sinf(euler[0]);
	float CosPitch = cosf(euler[1]), SinPitch = sinf(euler[1]);
	float CosYaw = cosf(euler[2]), SinYaw = sinf(euler[2]);
	R(0,0) = CosYaw * CosPitch;
	R(0,1) = -SinYaw * CosRoll + CosYaw * SinPitch * SinRoll;
	R(0,2) = SinYaw * SinRoll + CosYaw * SinPitch * CosRoll;
	R(1,0) = SinYaw * CosPitch;
	R(1,1) = CosYaw * CosRoll + SinYaw * SinPitch * SinRoll;
	R(1,2) = -CosYaw * SinRoll + SinYaw * SinPitch * CosRoll;
	R(2,0) = -SinPitch;
	R(2,1) = CosPitch * SinRoll;
	R(2,2) = CosPitch * CosRoll;
	return R;
}

Vector3f Quaternion::MatrixToFixedAngles(Matrix3f R){
	Vector3f fixedAngles;
	fixedAngles[0] = atan2f(-R(1,2), R(2,2));
	fixedAngles[1] = atan2f(R(0,2), sqrtf(R(1,2)*R(1,2) + R(2,2)*R(2,2)));
	fixedAngles[2] = atan2f(-R(0,1), R(0,0));
	return fixedAngles;
}

Matrix3f Quaternion::QuaternionToMatrix(Vector4f q){
	Matrix3f M;
	M << 1-2*(q[2]*q[2]+q[3]*q[3]),
		 2*(q[1]*q[2]-q[0]*q[3]),
		 2*(q[1]*q[3]+q[0]*q[2]),
		 2*(q[1]*q[2]+q[0]*q[3]),
		 1-2*(q[1]*q[1]+q[3]*q[3]),
		 2*(q[2]*q[3]-q[0]*q[1]),
		 2*(q[1]*q[3]-q[0]*q[2]),
		 2*(q[2]*q[3]+q[0]*q[1]),
		 1-2*(q[1]*q[1]+q[2]*q[2]);
	return M;
}

Vector3f Quaternion::getEuler(){
	return _Euler;
}

void Quaternion::setEuler(Vector3f angle){
	_QuaternionKalman->Clear(angle);
	_Quaternion = EulerToQuaternion(angle);
	float f[4] = {_Quaternion[0], _Quaternion[1], _Quaternion[2], _Quaternion[3]};
	setQuaternion(f);
	PrevT.setZero();
}

void Quaternion::Reset(){
	Vector3f angle = _mAcceleration->getFilteredAngle();
	if(IsUseCompass){
		angle[2] = _mCompass->getFilteredAngle()[2];
	}
	else{
		angle[2] = 0;
	}
	_QuaternionKalman->Clear(angle);
	_Quaternion = EulerToQuaternion(angle);
	float f[4] = {_Quaternion[0], _Quaternion[1], _Quaternion[2], _Quaternion[3]};
	setQuaternion(f);
	PrevT.setZero();
}