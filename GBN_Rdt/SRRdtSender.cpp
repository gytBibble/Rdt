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
	//窗口满，返回true
	return IsFull;
}




bool SRRdtSender::send(Message &message) {
	if (this->getWaitingState()) { //发送方窗口已满
		return false;
	}
	this->IsReceive[nextseqnum] = false;			//还未被确认
	this->packetWaitingAck[nextseqnum].acknum = -1; //忽略该字段
	this->packetWaitingAck[nextseqnum].seqnum = this->nextseqnum;
	this->packetWaitingAck[nextseqnum].checksum = 0;
	memcpy(this->packetWaitingAck[nextseqnum].payload, message.data, sizeof(message.data));
	this->packetWaitingAck[nextseqnum].checksum = pUtils->calculateCheckSum(this->packetWaitingAck[nextseqnum]);
	pns->startTimer(SENDER, Configuration::TIME_OUT, nextseqnum);//启动发送方定时器
	pUtils->printPacket("发送方发送报文", this->packetWaitingAck[nextseqnum]);
	pns->sendToNetworkLayer(RECEIVER, this->packetWaitingAck[nextseqnum]);//调用模拟网络环境的sendToNetworkLayer，通过网络层发送到对方

	nextseqnum = (nextseqnum + 1) % K;
	if ((nextseqnum == (base + N) % K)) IsFull = true;//发送窗口满
	return true;
}

void SRRdtSender::receive(Packet &ackPkt) {
		//检查校验和是否正确
		int checkSum = pUtils->calculateCheckSum(ackPkt);

		//如果校验和正确，并且确认序号=发送方已发送并等待确认的数据包序号
		if (checkSum == ackPkt.checksum)
		{
			pUtils->printPacket("发送方正确收到确认", ackPkt);
			if (this->IsReceive[ackPkt.acknum] == false)
			{
				this->IsReceive[ackPkt.acknum] = true;	//确认
				pns->stopTimer(SENDER, ackPkt.acknum);	//关闭定时器
				while (base != nextseqnum && this->IsReceive[base] == true)
				{
					//base向后移动
					base = (base + 1) % K;
				}
			}
			IsFull = false;
		}
		else
		{
			pUtils->printPacket("发送方没有正确收到确认", ackPkt);
		}
}

void SRRdtSender::timeoutHandler(int seqNum) {
	//唯一一个定时器,无需考虑seqNum
	pns->stopTimer(SENDER, seqNum);										//首先关闭定时器
	pUtils->printPacket("发送方定时器时间到，重发所有已发送但未确认的报文", this->packetWaitingAck[seqNum]);
	pns->startTimer(SENDER, Configuration::TIME_OUT, seqNum);			//重新启动发送方定时器
	pns->sendToNetworkLayer(RECEIVER, this->packetWaitingAck[seqNum]);	//重新发送数据包
}
