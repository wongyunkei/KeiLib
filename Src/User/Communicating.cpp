/*
 * Communicating.cpp
 *
 *  Created on: 2014¦~11¤ë11¤é
 *      Author: YunKei
 */

#include <Communicating.h>

using namespace Communication;
using namespace Math;

Com::Com(Interface interface, uint32_t addr, int index) : _interface(interface), Index(index){
	switch(interface){
		case __UART:
			_UART = (UART*)addr;
			break;
		case __SPI:
//			_Spi = (Spi*)addr;
			break;
		case __RF:
//			_nRF24L01 = (nRF24L01*)addr;
			break;
	}
}

Communicating::Communicating(Com* com) : _com(com), Index(com->Index), Length(0), Cmd(0), Data(0),isToken(false), BufferCount(0), txBufferCount(0){
	for(int i = 0; i < 256; i++){
		mFunc[i] = 0;
	}
}
void Communicating::setExecuteTask(int cmd, pFunc func){
	mFunc[cmd] = func;
}

int Communicating::getTxBufferCount(){
	return txBufferCount;
}

//void Communicating::ReceivePoll(){
//	switch(_com->_interface){
//		case Com::__UART:
//			Length = _com->_UART->AvailableLength;
//			_com->_UART->Read(Buffer + BufferCount, Length);
//			break;
//		case Com::__SPI:
////			Length = _com->_Spi->AvailableLength;
////			_com->_Spi->Read(Buffer + BufferCount, Length);
//			break;
//		case Com::__RF:
////			Length = _com->_nRF24L01->AvailableLength;
////			_com->_nRF24L01->Read(Buffer + BufferCount, Length);
//			break;
//	}
//	BufferCount += Length;
//
//	if(BufferCount > 0){
//		int cutLenght = 0;
//		int startPos = -1;
//		for(int i = 0; i < BufferCount; i++){
//			if(Buffer[i] == '$' && Buffer[i + 4] == '#'){
//				int tokenPos = i;
//				if(startPos == -1){
//					startPos = tokenPos;
//				}
//				char ch[3] = {Buffer[i + 1], Buffer[i + 2], Buffer[i + 3]};
//				cutLenght += 5;
//				int d[3];
//				d[0] = (int)ch[0] - 14;
//				for(int l = 1; l < 3; l++){
//					if(ch[l] < 0){
//						d[l] = (int)ch[l] + 242;
//					}
//					else{
//						d[l] = (int)ch[l] - 14;
//					}
//				}
//				int halfInt = (((int)d[1]) << 8) | ((int)d[2]);
//				Data = MathTools::HalfIntToFloat(halfInt);
//				Cmd = d[0];
//				if(Data == Data){
//					Execute(Cmd, Data);
//				}
//			}
//		}
//		if(startPos > -1){
//			for(int k = 0; k < BufferCount - startPos - cutLenght; k++){
//				Buffer[k] = Buffer[k + startPos + cutLenght];
//			}
//			BufferCount -= startPos + cutLenght;
//		}
//	}
//}

void Communicating::ReceivePoll(){
	switch(_com->_interface){
		case Com::__UART:
			Length = _com->_UART->AvailableLength;
			_com->_UART->Read(Buffer + BufferCount, Length);
			break;
		case Com::__SPI:
//			Length = _com->_Spi->AvailableLength;
//			_com->_Spi->Read(Buffer + BufferCount, Length);
			break;
		case Com::__RF:
//			Length = _com->_nRF24L01->AvailableLength;
//			_com->_nRF24L01->Read(Buffer + BufferCount, Length);
			break;
	}
	BufferCount += Length;

	if(BufferCount > 0){
		int cutLenght = 0;
		int startPos = -1;
		for(int i = 0; i < BufferCount; i++){
			if(Buffer[i] == '$' && Buffer[i + 8] == '#'){
				int tokenPos = i;
				if(startPos == -1){
					startPos = tokenPos;
				}
				char ch[7] = {Buffer[i + 1], Buffer[i + 2], Buffer[i + 3], Buffer[i + 4], Buffer[i + 5], Buffer[i + 6], Buffer[i + 7]};
				cutLenght += 9;
				int d[7];
				for(int l = 0; l < 7; l++){
					if(ch[l] < 0){
						d[l] = (int)ch[l] + 256 - 14;
					}
					else{
						d[l] = (int)ch[l] - 14;
					}
				}
				int16_t checksum = (int16_t)(((int16_t)d[5] << 8) | ((int16_t)d[6] << 0));
				int16_t sum = (int16_t)(d[0] + d[1] + d[2] + d[3] + d[4]);
				if(checksum == sum){
					union{
						int32_t d;
						float f;
					} x;
					x.d = (d[1] << 24) | (d[2] << 16) | (d[3] << 8) | (d[4] << 0);
					Data = x.f;
					Cmd = d[0];
					if(!MathTools::isNaN(Data)){
						Execute(Cmd, Data);
					}
				}
			}
		}
		if(startPos > -1){
			for(int k = 0; k < BufferCount - startPos - cutLenght; k++){
				Buffer[k] = Buffer[k + startPos + cutLenght];
			}
			BufferCount -= startPos + cutLenght;
		}
	}
}

void Communicating::SendPoll(){
	char D[txBufferCount + 1];
	for(int i = 0; i < txBufferCount; i++){
		D[i] = txBuffer[i];
	}
	D[txBufferCount] = '\0';
	switch(_com->_interface){
		case Com::__UART:
			if((!_com->_UART->Conf->_UseDMA || !_com->_UART->isDmaBusy) && txBufferCount >= 9){
				if(_com->_UART->Print("%s\n", D)){
					txBufferCount = 0;
				}
			}
			break;
		case Com::__SPI:
			if(txBufferCount >= 9){
//				_com->_Spi->Print(Index, "%s\n", D);
				txBufferCount = 0;
			}
			break;
		case Com::__RF:
			if(txBufferCount >= 9){
//				_com->_nRF24L01->Print("%s\n", D);
				txBufferCount = 0;
			}
			break;
	}
}

void Communicating::Execute(int cmd, float data){
	if(mFunc[cmd] != 0){
		Bundle bundle;
		bundle.Cmd = cmd;
		bundle.Cmd = Data;
		mFunc[cmd](&bundle);
		Acknowledgement();
	}
}

void Communicating::Acknowledgement(){
	mLed2->Toggle();
}

//void Communicating::Send(int cmd, float data){
//	char bytes[5];
//	int halfInt = MathTools::FloatToHalfInt(data);
//	bytes[0] = 0x24;
//	bytes[1] = (char)(cmd + 14);
//	bytes[2] = (char)(((halfInt & 0xff00) >> 8) + 14);
//	bytes[3] = (char)((halfInt & 0x00ff) + 14);
//	bytes[4] = 0x23;
//	for(int i = 0; i < 5; i++){
//		txBuffer[txBufferCount++] = bytes[i];
//	}
//}

void Communicating::Send(int cmd, float data){
	char bytes[9];
	union{
		uint32_t d;
		float f;
	} x;
	x.f = data;
	bytes[0] = 0x24;
	bytes[1] = (char)cmd;
	bytes[2] = (char)((x.d & 0xff000000) >> 24);
	bytes[3] = (char)((x.d & 0x00ff0000) >> 16);
	bytes[4] = (char)((x.d & 0x0000ff00) >> 8);
	bytes[5] = (char)((x.d & 0x000000ff) >> 0);
	uint16_t checksum = bytes[1] + bytes[2] + bytes[3] + bytes[4] + bytes[5];
	bytes[6] = (char)((checksum & 0xff00) >> 8);
	bytes[7] = (char)((checksum & 0x00ff) >> 0);
	bytes[8] = 0x23;
	for(int i = 1; i < 8; i++){
		bytes[i] += 14;
	}
	for(int i = 0; i < 9; i++){
		txBuffer[txBufferCount++] = bytes[i];
	}
}
