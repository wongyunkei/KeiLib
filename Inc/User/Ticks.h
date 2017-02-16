/*
 * Ticks.h
 *
 *  Created on: 2014¦~8¤ë3¤é
 *      Author: YunKei
 */

#ifndef TICKS_H_
#define TICKS_H_

#include <Program.h>

namespace Time{

	class Ticks{

		public:
			Ticks(bool onWatchDog);
			void TicksIncrement();
			bool TicksComp(uint32_t period, uint32_t ticksImg);
			bool TicksComp(uint32_t period, uint32_t offset, uint32_t ticksImg);
			void setTicks(uint32_t setValue);
			uint32_t getTicks();
			void setTimeout(float value);
			bool Timeout();
			uint32_t getTimeout();
			void StartWatchDog();
			void RefreshWatchDog();
			void PrintTime();
			void PrintTicks();
			static uint32_t maxTicks;
			bool OnWatchDog;
			uint32_t Sec;

		private:
			IWDG_HandleTypeDef hiwdg;
			uint32_t ticks;
			uint32_t timeoutCount;
			uint32_t timeoutStartTimestamp;
	};
};

#endif /* TICKS_H_ */
