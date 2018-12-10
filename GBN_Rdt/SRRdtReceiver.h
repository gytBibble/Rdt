#ifndef SR_RDT_RECEIVER_H
#define SR_RDT_RECEIVER_H
#include "RdtReceiver.h"
#define N 4							//窗口长度
#define K 8							//序号个数，2^3
class SRRdtReceiver :public RdtReceiver
{
private:
	int base = 0;					//基序号
	Packet packetWaitingAck[K];		//接收窗口
	bool IsReceive[K];				//标记分组是否以前被接收过
	Packet lastAckPkt;				

public:
	SRRdtReceiver();
	virtual ~SRRdtReceiver();

public:

	void receive(Packet &packet);		//接收报文，将被NetworkService调用
};

#endif
