/*
 * MathTools.h
 *
 *  Created on: 2014¦~8¤ë24¤é
 *      Author: YunKei
 */

#ifndef MATHTOOLS_H_
#define MATHTOOLS_H_

#include <Program.h>

namespace Math{

	class MathTools{

		public:

			static float PI;
			static float DEGREE_PER_RADIAN;
			static float RADIAN_PER_DEGREE;
			static bool isNaN(float value);
			static float Sign(float);
			static float DegreeToRadian(float);
			static float RadianToDegree(float);
			static float Trim(float lowerBound, float value, float upperBound);
			static float CutOff(float, float, float);
			static float TrimResolution(float value, int precision);
			static float CalcLength(float* x, int length);
			static bool CheckWithInInterval(float value, float mean, float var);
		private:


	};
};

#endif /* MATHTOOLS_H_ */
