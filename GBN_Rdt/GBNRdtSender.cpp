#include "stdafx.h"
#include "Global.h"
#include "GBNRdtSender.h"


GBNRdtSender::GBNRdtSender()
{
}


GBNRdtSender::~GBNRdtSender()
{
}



bool GBNRdtSender::getWaitingState() {
	//������������true
	return IsFull;
}




bool GBNRdtSender::send(Message &message) {
	if (this->getWaitingState()) { //���ͷ���������
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
		pns->startTimer(SENDER, Configuration::TIME_OUT, nextseqnum);//�������ͷ���ʱ��
	}
	pUtils->printPacket("���ͷ����ͱ���", this->packetWaitingAck[nextseqnum]);
	pns->sendToNetworkLayer(RECEIVER, this->packetWaitingAck[nextseqnum]);//����ģ�����绷����sendToNetworkLayer��ͨ������㷢�͵��Է�

	nextseqnum = (nextseqnum + 1) % K;
	if ((nextseqnum == (base + N) % K)) IsFull = true;//���ʹ�����
	return true;
}

void GBNRdtSender::receive(Packet &ackPkt) {
		//���У����Ƿ���ȷ
		int checkSum = pUtils->calculateCheckSum(ackPkt);

		//���У�����ȷ������ȷ�����=���ͷ��ѷ��Ͳ��ȴ�ȷ�ϵ����ݰ����
		if (checkSum == ackPkt.checksum)
		{
			base = (ackPkt.acknum + 1) % K;
			pns->stopTimer(SENDER, 0);	//�رն�ʱ��
			if (base != nextseqnum)		//������ڷǿգ�������ʱ
				pns->startTimer(SENDER, Configuration::TIME_OUT, 0);
			pUtils->printPacket("���ͷ���ȷ�յ�ȷ��", ackPkt);
			IsFull = false;
		}
		else {
			pUtils->printPacket("���ͷ�û����ȷ�յ�ȷ��", ackPkt);
		}
}

void GBNRdtSender::timeoutHandler(int seqNum) {
	//Ψһһ����ʱ��,���迼��seqNum
	printf("���ͷ���ʱ��ʱ�䵽���ط������ѷ��͵�δȷ�ϵı���\n");
	pns->stopTimer(SENDER, 0);											//���ȹرն�ʱ��
	pns->startTimer(SENDER, Configuration::TIME_OUT, 0);				//�����������ͷ���ʱ��
	for (int i = base; i != nextseqnum; i = (i + 1) % K)
	{
		pUtils->printPacket("", this->packetWaitingAck[i]);
		pns->sendToNetworkLayer(RECEIVER, this->packetWaitingAck[i]);	//���·������ݰ�
	}
}
