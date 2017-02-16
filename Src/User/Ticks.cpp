/*
 * Ticks.cpp
 *
 *  Created on: 2014¦~8¤ë3¤é
 *      Author: YunKei
 */

#include <Ticks.h>

using namespace Time;

uint32_t Ticks::maxTicks = 6000000;

void SysTick_Handler(void){
	HAL_IncTick();
	mTicks->TicksIncrement();
	if(mTicks->getTicks() >= Ticks::maxTicks){
		mTicks->setTicks(0);
	}

	if(mTicks->getTicks() % 100000 == 0){
		mTicks->Sec++;
	}

	if(mTicks->getTicks() % 500000 == 0 && mTask != 0 && mTask->IsPrintTaskNum){
		mTask->PrintTasksInfo();
	}

	if(mTask != 0 && mTask->IsPrintTaskNum){
		if(mTask->mTaskObj[mTask->currentTaskNum]->hangCount++ > mTask->mTaskObj[mTask->currentTaskNum]->TaskPeriod){
			static int delayCount = 0;
			if(delayCount++ > 10000){
				delayCount = 0;
				mTask->PrintTasksInfo(mTask->currentTaskNum, 1);
			}
		}
	}
}

uint32_t Ticks::getTimeout(){
	if(getTicks() >= timeoutStartTimestamp){
		return getTicks() - timeoutStartTimestamp;
	}
	else{
		return maxTicks - timeoutStartTimestamp + getTicks();
	}
}

void Ticks::StartWatchDog(){
	hiwdg.Init.Prescaler = IWDG_PRESCALER_256;
	hiwdg.Init.Reload = 250;
	hiwdg.Instance = IWDG;
	__HAL_IWDG_RELOAD_COUNTER(&hiwdg);
	HAL_IWDG_Init(&hiwdg);
	__HAL_IWDG_START(&hiwdg);
	OnWatchDog = true;
}

void Ticks::RefreshWatchDog(){
	hiwdg.Init.Prescaler = IWDG_PRESCALER_256;
	hiwdg.Init.Reload = 250;
	hiwdg.Instance = IWDG;
	HAL_IWDG_Refresh(&hiwdg);
}

Ticks::Ticks(bool onWatchDog) : Sec(0), ticks(0), timeoutCount(0), timeoutStartTimestamp(0), OnWatchDog(onWatchDog){
	HAL_SYSTICK_Config(SystemCoreClock/100000U);
	if(onWatchDog){
		StartWatchDog();
	}
}

bool Ticks::TicksComp(uint32_t period, uint32_t ticksImg){

	return TicksComp(period, 0, ticksImg);
}

bool Ticks::TicksComp(uint32_t period, uint32_t offset, uint32_t ticksImg){

	if(ticksImg % period == offset){
		return true;
	}
	else{
		return false;
	}
}

void Ticks::TicksIncrement(){
	ticks++;
}

void Ticks::setTicks(uint32_t setValue){
	ticks = setValue;
}

uint32_t Ticks::getTicks(){
	return ticks;
}

bool Ticks::Timeout(){
	return (((int)timeoutCount - (int)getTimeout()) < 0) ? true : false;
}

void Ticks::setTimeout(float value){
	timeoutCount = value * 100000;
	timeoutStartTimestamp = getTicks();
}

void Ticks::PrintTime(){
	int hr = Sec / 3600;
	int min = (Sec - hr * 3600) / 60;
	int sec = (Sec - hr * 3600 - min * 60);
	printf("Time Executed:%d:%d:%d\r\n", hr, min, sec);
}

void Ticks::PrintTicks(){
	printf("Ticks:%ld\r\n", ticks);
}
