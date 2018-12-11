#include "stdafx.h"
#include "Global.h"
#include "SRRdtReceiver.h"


SRRdtReceiver::SRRdtReceiver()
{
	lastAckPkt.acknum = -1; //初始状态下，上次发送的确认包的确认序号为-1，使得当第一个接受的数据包出错时该确认报文的确认号为-1
	lastAckPkt.checksum = 0;
	lastAckPkt.seqnum = -1;	//忽略该字段
	for (int i = 0; i < Configuration::PAYLOAD_SIZE; i++) {
		lastAckPkt.payload[i] = '.';
	}
	lastAckPkt.checksum = pUtils->calculateCheckSum(lastAckPkt);
}


SRRdtReceiver::~SRRdtReceiver()
{
}

void SRRdtReceiver::receive(Packet &packet) {
	//检查校验和是否正确
	int checkSum = pUtils->calculateCheckSum(packet);

	//如果校验和正确，同时收到报文的序号等于接收方期待收到的报文序号一致
	if (checkSum == packet.checksum)
	{
		pUtils->printPacket("接收方正确收到发送方的报文", packet);
		if (base + N <= K && (packet.seqnum >= base && packet.seqnum < base + N) || base + N > K && (packet.seqnum >= base || packet.seqnum < (base + N) % K))
		{
			pUtils->printPacket("发送方的报文在窗口内", packet);
			this->IsReceive[packet.seqnum] = true;
			this->packetWaitingAck[packet.seqnum] = packet;
			//取出Message，向上递交给应用层
			Message msg;
			for (; this->IsReceive[base] == true; base = (base + 1) % K)
			{
				memcpy(msg.data, this->packetWaitingAck[base].payload, sizeof(packet.payload));
				pns->delivertoAppLayer(RECEIVER, msg);
				this->IsReceive[base] = false;
			}
		}
		lastAckPkt.acknum = packet.seqnum; //确认序号等于收到的报文序号
		lastAckPkt.checksum = pUtils->calculateCheckSum(lastAckPkt);
		pUtils->printPacket("接收方发送确认报文", lastAckPkt);
		pns->sendToNetworkLayer(SENDER, lastAckPkt);	//调用模拟网络环境的sendToNetworkLayer，通过网络层发送确认报文到对方
	}
	else {
			pUtils->printPacket("接收方没有正确收到发送方的报文,数据校验错误", packet);
	}
}