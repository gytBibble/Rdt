#include "stdafx.h"
#include "Global.h"
#include "SRRdtReceiver.h"


SRRdtReceiver::SRRdtReceiver()
{
	lastAckPkt.acknum = -1; //��ʼ״̬�£��ϴη��͵�ȷ�ϰ���ȷ�����Ϊ-1��ʹ�õ���һ�����ܵ����ݰ�����ʱ��ȷ�ϱ��ĵ�ȷ�Ϻ�Ϊ-1
	lastAckPkt.checksum = 0;
	lastAckPkt.seqnum = -1;	//���Ը��ֶ�
	for (int i = 0; i < Configuration::PAYLOAD_SIZE; i++) {
		lastAckPkt.payload[i] = '.';
	}
	lastAckPkt.checksum = pUtils->calculateCheckSum(lastAckPkt);
}


SRRdtReceiver::~SRRdtReceiver()
{
}

void SRRdtReceiver::receive(Packet &packet) {
	//���У����Ƿ���ȷ
	int checkSum = pUtils->calculateCheckSum(packet);

	//���У�����ȷ��ͬʱ�յ����ĵ���ŵ��ڽ��շ��ڴ��յ��ı������һ��
	if (checkSum == packet.checksum)
	{
		pUtils->printPacket("���շ���ȷ�յ����ͷ��ı���", packet);
		if (base + N <= K && (packet.seqnum >= base && packet.seqnum < base + N) || base + N > K && (packet.seqnum >= base || packet.seqnum < (base + N) % K))
		{
			pUtils->printPacket("���ͷ��ı����ڴ�����", packet);
			this->IsReceive[packet.seqnum] = true;
			this->packetWaitingAck[packet.seqnum] = packet;
			//ȡ��Message�����ϵݽ���Ӧ�ò�
			Message msg;
			for (; this->IsReceive[base] == true; base = (base + 1) % K)
			{
				memcpy(msg.data, this->packetWaitingAck[base].payload, sizeof(packet.payload));
				pns->delivertoAppLayer(RECEIVER, msg);
				this->IsReceive[base] = false;
			}
		}
		lastAckPkt.acknum = packet.seqnum; //ȷ����ŵ����յ��ı������
		lastAckPkt.checksum = pUtils->calculateCheckSum(lastAckPkt);
		pUtils->printPacket("���շ�����ȷ�ϱ���", lastAckPkt);
		pns->sendToNetworkLayer(SENDER, lastAckPkt);	//����ģ�����绷����sendToNetworkLayer��ͨ������㷢��ȷ�ϱ��ĵ��Է�
	}
	else {
			pUtils->printPacket("���շ�û����ȷ�յ����ͷ��ı���,����У�����", packet);
	}
}