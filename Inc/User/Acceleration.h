/*
 * Acceleration.h
 *
 *  Created on: 2014¦~8¤ë24¤é
 *      Author: YunKei
 */

#ifndef ACCELERATION_H_
#define ACCELERATION_H_

#include <Program.h>

namespace Sensors{
	class MPU6050;
};

using namespace Math;
using namespace Sensors;
using Eigen::Vector3f;

namespace Inertia{

	class Acceleration{

		public:
			static float Gravity;
			Acceleration(MPU6050* mMPU6050);
//			Acceleration(Sensors::MPU6500* mMPU6500);
			bool Update();
			Vector3f getAcc();
			Vector3f getAngle();
			Vector3f getFilteredAcc();
			Vector3f getFilteredAngle();
			void setAcc(Vector3f value);
			bool getIsValided();

		private:
			Sensors::MPU6050* _mMPU6050;
//			Sensors::MPU6500* _mMPU6500;
			bool isValided;
			Vector3f Acc;
			Vector3f FilteredAcc;
			MovingWindowFilters* AccMovingWindowFilters[3];
			bool isMPU6500;
	};
};

#endif /* ACCELERATION_H_ */
