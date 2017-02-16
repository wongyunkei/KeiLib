/*
 * Task.cpp
 *
 *  Created on: 2014¦~8¤ë6¤é
 *      Author: YunKei
 */

#include <Task.h>

Bundle* Task::mBundle = 0;

TaskObj::TaskObj(float period, float offset, pTask fn, string fnName, bool isPeriodic, int BreakCout){
	mTask = fn;
	TaskPeriod = period;
	TaskOffset = offset;
	if(TaskOffset == TaskPeriod){
		TaskOffset = 0;
	}
	TaskName = fnName;
	IsPeriodic = isPeriodic;
	_BreakCout = BreakCout;
	hangCount = 0;
	executedCount = 0;
}

Task::Task() : OnWatchDog(mTicks->OnWatchDog), TasksNum(0), currentTaskNum(0), IsPrintTaskNum(false), KeepLoopping(true), Count(-1){
	mBundle = new Bundle();
}

void Task::Attach(float period, pTask fn, string fnName, int BreakCout, bool keepLoopping){
	Attach(period, 0, fn, fnName, BreakCout, keepLoopping);
}

void Task::Attach(float period, float offset, pTask fn, string fnName, int BreakCout, bool keepLoopping){
	if(TasksNum >= 1023){
		printf("Cannot attach task any more!\r\n");
		return;
	}
	bool isPeriodic = true;
	if(BreakCout > 0){
		isPeriodic = false;
	}
	mTaskObj[TasksNum] = new TaskObj(period*100000, offset*100000, fn, fnName, isPeriodic, BreakCout);
	TasksNum++;
	KeepLoopping = keepLoopping;
}

void Task::DeAttach(string fnName){
	for(int i = 0; i < TasksNum; i++){
		if(mTaskObj[i]->TaskName.compare(fnName) == 0){
			delete mTaskObj[i];
			for(int j = i; j < TasksNum - 1; j++){
				mTaskObj[j] = mTaskObj[j + 1];
			}
			TasksNum--;
			return;
		}
	}
}


void Task::PrintTasksInfo(){
	PrintTasksInfo(0, TasksNum);
}

void Task::PrintTasksInfo(int start, int length){
	printf("TaskNum: %d Start: %d End: %d\r\n", TasksNum, start, start + length - 1);
	for(int i = 0; i < TasksNum; i++){
		if(i >= start && i <= start + length - 1){
			printf("%d: %s: ExeutedCount:%d Time:%f Duration: %f HangCount:%f Period:%f Offset:%f\r\n", i, mTaskObj[i]->TaskName.c_str(),
					mTaskObj[i]->executedCount,
					mTaskObj[i]->duration[1] / 100000.0,
					mTaskObj[i]->duration[1] - mTaskObj[i]->duration[0],
					mTaskObj[i]->hangCount / 100000.0,
					mTaskObj[i]->TaskPeriod / 100000.0,
					mTaskObj[i]->TaskOffset / 100000.0);
		}
	}
	printf("CurrentTask::%s CurrentTaskNum:%d\r\n", mTaskObj[currentTaskNum]->TaskName.c_str(), currentTaskNum);
}

void Task::PrintDeration(){
	for(int i = 0; i < TasksNum; i++){
		printf("%d %s:t:%d\r\n", mTaskObj[i]->TaskName.c_str(), mTaskObj[i]->duration[1] - mTaskObj[i]->duration[0]);
	}
}

void Task::Run(bool isPrintTaskNum){

	IsPrintTaskNum = isPrintTaskNum;

	uint32_t ticksImg = 0;
	bool isBreak = false;

	do{
		if(mTicks->getTicks() != ticksImg){
			ticksImg = mTicks->getTicks();
			if(OnWatchDog){
				mTicks->RefreshWatchDog();
			}
			for(int i = 0; i < TasksNum; i++){
				if(mTicks->TicksComp(mTaskObj[i]->TaskPeriod, mTaskObj[i]->TaskOffset, ticksImg)){
					mTaskObj[i]->hangCount = 0;
					currentTaskNum = i;
					if(mTaskObj[i]->_BreakCout != 0){
						mBundle->mTaskObj = mTaskObj[i];
						mTaskObj[i]->duration[0] = mTicks->getTicks();
						mTaskObj[i]->mTask(mBundle);
						mTaskObj[i]->executedCount++;
						mTaskObj[i]->duration[1] = mTicks->getTicks();
						if(!(mTaskObj[i]->IsPeriodic)){
							if(mTaskObj[i]->_BreakCout > 0){
								mTaskObj[i]->_BreakCout--;
							}
						}
					}
				}
			}
			for(int i = 0; i < TasksNum; i++){
				if(!mTaskObj[i]->IsPeriodic){
					if(mTaskObj[i]->_BreakCout == 0){
						DeAttach(mTaskObj[i]->TaskName);
						if(!KeepLoopping){
							isBreak = true;
						}
					}
				}
			}
		}
	}while(KeepLoopping || !isBreak);
}
