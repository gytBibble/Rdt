#include "stdafx.h"
#include "Global.h"
#include "SRRdtSender.h"


SRRdtSender::SRRdtSender()
{
	for (int i = 0; i < K; i++) IsReceive[i] = false;
}


SRRdtSender::~SRRdtSender()
{
}



bool SRRdtSender::getWaitingState() {
	//������������true
	return IsFull;
}




bool SRRdtSender::send(Message &message) {
	if (this->getWaitingState()) { //���ͷ���������
		return false;
	}
	this->IsReceive[nextseqnum] = false;			//��δ��ȷ��
	this->packetWaitingAck[nextseqnum].acknum = -1; //���Ը��ֶ�
	this->packetWaitingAck[nextseqnum].seqnum = this->nextseqnum;
	this->packetWaitingAck[nextseqnum].checksum = 0;
	memcpy(this->packetWaitingAck[nextseqnum].payload, message.data, sizeof(message.data));
	this->packetWaitingAck[nextseqnum].checksum = pUtils->calculateCheckSum(this->packetWaitingAck[nextseqnum]);
	pns->startTimer(SENDER, Configuration::TIME_OUT, nextseqnum);//�������ͷ���ʱ��
	pUtils->printPacket("���ͷ����ͱ���", this->packetWaitingAck[nextseqnum]);
	pns->sendToNetworkLayer(RECEIVER, this->packetWaitingAck[nextseqnum]);//����ģ�����绷����sendToNetworkLayer��ͨ������㷢�͵��Է�

	nextseqnum = (nextseqnum + 1) % K;
	if ((nextseqnum == (base + N) % K)) IsFull = true;//���ʹ�����
	return true;
}

void SRRdtSender::receive(Packet &ackPkt) {
		//���У����Ƿ���ȷ
		int checkSum = pUtils->calculateCheckSum(ackPkt);

		//���У�����ȷ������ȷ�����=���ͷ��ѷ��Ͳ��ȴ�ȷ�ϵ����ݰ����
		if (checkSum == ackPkt.checksum)
		{
			pUtils->printPacket("���ͷ���ȷ�յ�ȷ��", ackPkt);
			if (this->IsReceive[ackPkt.acknum] == false)
			{
				this->IsReceive[ackPkt.acknum] = true;	//ȷ��
				pns->stopTimer(SENDER, ackPkt.acknum);	//�رն�ʱ��
				while (base != nextseqnum && this->IsReceive[base] == true)
				{
					//base����ƶ�
					base = (base + 1) % K;
				}
			}
			IsFull = false;
		}
		else
		{
			pUtils->printPacket("���ͷ�û����ȷ�յ�ȷ��", ackPkt);
		}
}

void SRRdtSender::timeoutHandler(int seqNum) {
	//Ψһһ����ʱ��,���迼��seqNum
	pns->stopTimer(SENDER, seqNum);										//���ȹرն�ʱ��
	pUtils->printPacket("���ͷ���ʱ��ʱ�䵽���ط������ѷ��͵�δȷ�ϵı���", this->packetWaitingAck[seqNum]);
	pns->startTimer(SENDER, Configuration::TIME_OUT, seqNum);			//�����������ͷ���ʱ��
	pns->sendToNetworkLayer(RECEIVER, this->packetWaitingAck[seqNum]);	//���·������ݰ�
}
