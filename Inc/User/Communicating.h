/*
 * Communicating.h
 *
 *  Created on: 2014¦~11¤ë11¤é
 *      Author: YunKei
 */

#ifndef COMMUNICATING_H_
#define COMMUNICATING_H_

#include <Program.h>

using namespace System;

namespace Communication{

	class UART;

	class Com{
		public:
			enum Interface{__UART, __SPI, __RF};
			Com(Interface interface, uint32_t addr, int index = 0);
			Interface _interface;
			UART* _UART;
//			Spi* _Spi;
//			nRF24L01* _nRF24L01;
			int Index;
//				I2C* _I2C;
		private:
	};

	class Communicating{
		public:
			typedef void (*pFunc)(Bundle* bundle);
			enum CMD{
				SYSTEM_RESET,
				WATCHDOG,
				PRINT_MODE,
				HIGH,
				LOW,
				RESET_ALL,
				START,
				STOP,
				POWER,
				ROLL_OFFSET,
				PITCH_OFFSET,
				YAW_OFFSET,
				ROLL_KP,
				ROLL_KI,
				ROLL_KD,
				PITCH_KP,
				PITCH_KI,
				PITCH_KD,
				YAW_KP,
				YAW_KI,
				YAW_KD,
				MAX_LIFT_VALUE,
				MIN_LIFT_VALUE,
				LIFT,
				TARGET_ROLL,
				TARGET_PITCH,
				TARGET_YAW,
				SET_X_TRANSLATION,
				SET_Y_TRANSLATION,
				MOTOR_KP,
				MOTOR_KI,
	            FORWARD,
	            BACKWARD,
	            LEFT,
	            RIGHT,
				PAUSE,
	            CW,
				CCW,
				MOVE,
				MANUAL_MODE,
				RETURN_HOME,
				TEST,
				CLAMPER_STOP_ALL,
				CLAMPER_RESET,
				CLAMPER_START,
				CLAMPER_SET_MOTOR1_TARGET,
				CLAMPER_SET_MOTOR2_TARGET,
				CLAMPER_SET_MOTOR3_TARGET,
				CLAMPER_WATCHDOG,
				SUCCESS,
				CLAMPER_SET_HORIZONTAL,
				SET_SPEED,
				SET_X_POS,
				SET_Y_POS,
				SET_YAW,
				SET_SONIC_CAL_FL,
				SET_SONIC_CAL_FR,
				SET_SONIC_CAL_L,
				SET_SONIC_CAL_R,
				SET_SONIC_CAL_FL_VALUE,
				SET_SONIC_CAL_FR_VALUE,
				SET_SONIC_CAL_L_VALUE,
				SET_SONIC_CAL_R_VALUE,
				SET_X_CAL,
				SET_Y_CAL,
				SET_X_CAL_VALUE,
				SET_Y_CAL_VALUE,
				TRIGGER,
				CLAMPER_STOP_ALL_RUN,
				CLAMPER_RESET_RUN,
				CLAMPER_START_RUN,
				CLAMPER_SET_MOTOR1_TARGET_RUN,
				CLAMPER_SET_MOTOR2_TARGET_RUN,
				CLAMPER_SET_MOTOR3_TARGET_RUN,
				CLAMPER_WATCHDOG_RUN,
				CLAMPER_SET_HORIZONTAL_RUN,
				AUTO_MODE,
				NEXT,
				DEV1FB,
				DEV2FB,
				DEV3FB,
				DEV4FB
			};

			Communicating(Com* com);
			void setExecuteTask(int cmd, pFunc func);
			void ReceivePoll();
			void SendPoll();
			void Execute(int cmd, float data);
			void Send(int cmd, float data);
			int getTxBufferCount();
			void Acknowledgement();
			int Length;
			int txBufferCount;
			int BufferCount;
			Com* _com;

		private:
			bool isToken;
			char Buffer[2048];
			char txBuffer[2048];
			char Bytes[7];
			int Cmd;
			float Data;
			int Index;
			pFunc mFunc[256];
	};
};

#endif /* COMMUNICATING_H_ */
