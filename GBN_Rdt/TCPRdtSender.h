#ifndef TCP_RDT_SENDER_H
#define TCP_RDT_SENDER_H
#include "RdtSender.h"
#define N 4							//���ڳ���
#define K 8							//��Ÿ�����2^3
class TCPRdtSender :public RdtSender
{
private:
	int base = 0;					//�����
	int nextseqnum = 0;				//��һ�����
	Packet packetWaitingAck[K];		//�ѷ��ͻ�δȷ�ϵ����ݰ�
	bool IsFull = false;
	int count = 0;					//������

public:

	bool getWaitingState();
	bool send(Message &message);						//����Ӧ�ò�������Message����NetworkServiceSimulator����,������ͷ��ɹ��ؽ�Message���͵�����㣬����true;�����Ϊ���ͷ����ڵȴ���ȷȷ��״̬���ܾ�����Message���򷵻�false
	void receive(Packet &ackPkt);						//����ȷ��Ack������NetworkServiceSimulator����	
	void timeoutHandler(int seqNum);					//Timeout handler������NetworkServiceSimulator����

public:
	TCPRdtSender();
	virtual ~TCPRdtSender();
};

#endif
