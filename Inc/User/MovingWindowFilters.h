/*
 * MovingWindowFilters.h
 *
 *  Created on: 2015¦~3¤ë24¤é
 *      Author: YunKei
 */

#ifndef MOVINGWINDOWFILTERS_H_
#define MOVINGWINDOWFILTERS_H_

namespace Math{

	class MovingWindowFilters{
		public:
			MovingWindowFilters(int windowSize);
			void Update(float value);
			float getAverage();
			void Clear();
			float getMedian();
		private:
			float* buffer;
			int WindowSize;
			int bufferCount;
	};
};

#endif /* MOVINGWINDOWAVERAGEFILTER_H_ */
