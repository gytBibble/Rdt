#ifndef TCP_RDT_RECEIVER_H
#define TCP_RDT_RECEIVER_H
#include "RdtReceiver.h"
#define K 8
class TCPRdtReceiver :public RdtReceiver
{
private:
	int expectSequenceNumberRcvd = 0;	// �ڴ��յ�����һ���������
	Packet lastAckPkt;					//�ϴη��͵�ȷ�ϱ���

public:
	TCPRdtReceiver();
	virtual ~TCPRdtReceiver();

public:

	void receive(Packet &packet);		//���ձ��ģ�����NetworkService����
};

#endif
