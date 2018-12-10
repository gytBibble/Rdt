#ifndef GBN_RDT_RECEIVER_H
#define GBN_RDT_RECEIVER_H
#include "RdtReceiver.h"
#define K 8
class GBNRdtReceiver :public RdtReceiver
{
private:
	int expectSequenceNumberRcvd = 0;	// 期待收到的下一个报文序号
	Packet lastAckPkt;					//上次发送的确认报文

public:
	GBNRdtReceiver();
	virtual ~GBNRdtReceiver();

public:

	void receive(Packet &packet);		//接收报文，将被NetworkService调用
};

#endif
