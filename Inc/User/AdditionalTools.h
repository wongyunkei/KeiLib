/*
 * AdditionalTools.h
 *
 *  Created on: 2014¦~8¤ë7¤é
 *      Author: YunKei
 */

#ifndef ADDITIONALTOOLS_H_
#define ADDITIONALTOOLS_H_

#include <Program.h>

using Eigen::MatrixXf;
using Eigen::VectorXf;

#define _BV(X)					(1L << X)
#define MSB(X)					((X & 0xff00) >> 8)
#define LSB(X)					(X & 0x00ff)

namespace Utility{
	class AdditionalTools{
		public:
			static void setBuffer(int index, float* buf, int length);
			static void setBuffer(int index, VectorXf buf, int length);
			static void setBuffer(int index, float** buf,int rows, int cols);
			static void setBuffer(int index, MatrixXf buf,int rows, int cols);
			static float* getBuffer(int index);
			static VectorXf getVector(int index, int length);
			static float** getMatrix(int index);
			static MatrixXf getMatrix(int index, int rows, int cols);
			static void printfBuffer(int index, int length);
			static void printMatix(char* label, MatrixXf M,int rows, int cols);
		private:
	};
};

#endif /* ADDITIONALTOOLS_H_ */
