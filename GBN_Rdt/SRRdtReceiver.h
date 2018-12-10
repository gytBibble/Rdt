#ifndef SR_RDT_RECEIVER_H
#define SR_RDT_RECEIVER_H
#include "RdtReceiver.h"
#define N 4							//���ڳ���
#define K 8							//��Ÿ�����2^3
class SRRdtReceiver :public RdtReceiver
{
private:
	int base = 0;					//�����
	Packet packetWaitingAck[K];		//���մ���
	bool IsReceive[K];				//��Ƿ����Ƿ���ǰ�����չ�
	Packet lastAckPkt;				

public:
	SRRdtReceiver();
	virtual ~SRRdtReceiver();

public:

	void receive(Packet &packet);		//���ձ��ģ�����NetworkService����
};

#endif
