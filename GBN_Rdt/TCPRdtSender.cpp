#include "stdafx.h"
#include "Global.h"
#include "TCPRdtSender.h"


TCPRdtSender::TCPRdtSender()
{
}


TCPRdtSender::~TCPRdtSender()
{
}



bool TCPRdtSender::getWaitingState() {
	//������������true
	return IsFull;
}




bool TCPRdtSender::send(Message &message) {
	if (this->getWaitingState()) 
	{ 
		//���ͷ���������
		return false;
	}
	this->packetWaitingAck[nextseqnum].acknum = -1; //���Ը��ֶ�
	this->packetWaitingAck[nextseqnum].seqnum = this->nextseqnum;
	this->packetWaitingAck[nextseqnum].checksum = 0;
	memcpy(this->packetWaitingAck[nextseqnum].payload, message.data, sizeof(message.data));
	this->packetWaitingAck[nextseqnum].checksum = pUtils->calculateCheckSum(this->packetWaitingAck[nextseqnum]);
	if (base == nextseqnum)
	{
		//�����Ǵ��ڵĵ�һ�����ݰ�
		pns->startTimer(SENDER, Configuration::TIME_OUT, nextseqnum);		//�������ͷ���ʱ��
	}
	pUtils->printPacket("���ͷ����ͱ���", this->packetWaitingAck[nextseqnum]);
	pns->sendToNetworkLayer(RECEIVER, this->packetWaitingAck[nextseqnum]);	//����ģ�����绷����sendToNetworkLayer��ͨ������㷢�͵��Է�

	nextseqnum = (nextseqnum + 1) % K;
	if ((nextseqnum == (base + N) % K)) IsFull = true;						//���ʹ�����
	return true;
}

void TCPRdtSender::receive(Packet &ackPkt) {
	//���У����Ƿ���ȷ
	int checkSum = pUtils->calculateCheckSum(ackPkt);

	//���У�����ȷ������ȷ�����=���ͷ��ѷ��Ͳ��ȴ�ȷ�ϵ����ݰ����
	if (checkSum == ackPkt.checksum)// && ackPkt.acknum >= base && ackPkt.acknum < nextseqnum)
	{
		//����Ack���
		int temp = ackPkt.acknum;
		if (base + N <= K && (temp >= base && temp < base + N) || base + N > K && (temp >= base || temp < (base + N) % K))
		{
			//�ڴ�����
			count = 0;				//��������0
			base = (temp + 1) % K;	//���ں�
		}
		else count++;
		if (count == 3)
		{
			//�����ش�
			count = 0;
			pns->stopTimer(SENDER, 0);											//�رն�ʱ��
			pns->startTimer(SENDER, Configuration::TIME_OUT, 0);
			printf("���ͷ���⵽����Ack���Σ����п����ش�!\n");
			pUtils->printPacket("", this->packetWaitingAck[base]);
			pns->sendToNetworkLayer(RECEIVER, this->packetWaitingAck[base]);	//���·������ݰ�
		}
		else if (count == 0)
		{
			//��GBNһ��
			pUtils->printPacket("���ͷ���ȷ�յ�ȷ��", ackPkt);
			pns->stopTimer(SENDER, 0);	//�رն�ʱ��
			if (base != nextseqnum)		//������ڷǿգ�������ʱ
				pns->startTimer(SENDER, Configuration::TIME_OUT, 0);
		}
		IsFull = false;
	}
	else {
		pUtils->printPacket("���ͷ�û����ȷ�յ�ȷ��", ackPkt);
	}
}

void TCPRdtSender::timeoutHandler(int seqNum) {
	//Ψһһ����ʱ��,���迼��seqNum
	//ֻ���͵�һ��,������GBN���ط�
	printf("���ͷ���ʱ��ʱ�䵽���ط������ѷ��͵�δȷ�ϵı���\n");
	pns->stopTimer(SENDER, 0);										//���ȹرն�ʱ��
	pns->startTimer(SENDER, Configuration::TIME_OUT, 0);			//�����������ͷ���ʱ��
	pUtils->printPacket("", this->packetWaitingAck[base]);
	pns->sendToNetworkLayer(RECEIVER, this->packetWaitingAck[base]);//���·������ݰ�
}
