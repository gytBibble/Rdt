#ifndef GBN_RDT_RECEIVER_H
#define GBN_RDT_RECEIVER_H
#include "RdtReceiver.h"
#define K 8
class GBNRdtReceiver :public RdtReceiver
{
private:
	int expectSequenceNumberRcvd = 0;	// �ڴ��յ�����һ���������
	Packet lastAckPkt;					//�ϴη��͵�ȷ�ϱ���

public:
	GBNRdtReceiver();
	virtual ~GBNRdtReceiver();

public:

	void receive(Packet &packet);		//���ձ��ģ�����NetworkService����
};

#endif
