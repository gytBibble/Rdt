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
	//窗口满，返回true
	return IsFull;
}




bool TCPRdtSender::send(Message &message) {
	if (this->getWaitingState()) 
	{ 
		//发送方窗口已满
		return false;
	}
	this->packetWaitingAck[nextseqnum].acknum = -1; //忽略该字段
	this->packetWaitingAck[nextseqnum].seqnum = this->nextseqnum;
	this->packetWaitingAck[nextseqnum].checksum = 0;
	memcpy(this->packetWaitingAck[nextseqnum].payload, message.data, sizeof(message.data));
	this->packetWaitingAck[nextseqnum].checksum = pUtils->calculateCheckSum(this->packetWaitingAck[nextseqnum]);
	if (base == nextseqnum)
	{
		//发送是窗口的第一个数据包
		pns->startTimer(SENDER, Configuration::TIME_OUT, nextseqnum);		//启动发送方定时器
	}
	pUtils->printPacket("发送方发送报文", this->packetWaitingAck[nextseqnum]);
	pns->sendToNetworkLayer(RECEIVER, this->packetWaitingAck[nextseqnum]);	//调用模拟网络环境的sendToNetworkLayer，通过网络层发送到对方

	nextseqnum = (nextseqnum + 1) % K;
	if ((nextseqnum == (base + N) % K)) IsFull = true;						//发送窗口满
	return true;
}

void TCPRdtSender::receive(Packet &ackPkt) {
	//检查校验和是否正确
	int checkSum = pUtils->calculateCheckSum(ackPkt);

	//如果校验和正确，并且确认序号=发送方已发送并等待确认的数据包序号
	if (checkSum == ackPkt.checksum)// && ackPkt.acknum >= base && ackPkt.acknum < nextseqnum)
	{
		//冗余Ack检测
		int temp = ackPkt.acknum;
		if (base + N <= K && (temp >= base && temp < base + N) || base + N > K && (temp >= base || temp < (base + N) % K))
		{
			//在窗口内
			count = 0;				//计数器清0
			base = (temp + 1) % K;	//窗口后滑
		}
		else count++;
		if (count == 3)
		{
			//快速重传
			count = 0;
			pns->stopTimer(SENDER, 0);											//关闭定时器
			pns->startTimer(SENDER, Configuration::TIME_OUT, 0);
			printf("发送方检测到冗余Ack三次，进行快速重传!\n");
			pUtils->printPacket("", this->packetWaitingAck[base]);
			pns->sendToNetworkLayer(RECEIVER, this->packetWaitingAck[base]);	//重新发送数据包
		}
		else if (count == 0)
		{
			//和GBN一样
			pUtils->printPacket("发送方正确收到确认", ackPkt);
			pns->stopTimer(SENDER, 0);	//关闭定时器
			if (base != nextseqnum)		//如果窗口非空，继续计时
				pns->startTimer(SENDER, Configuration::TIME_OUT, 0);
		}
		IsFull = false;
	}
	else {
		pUtils->printPacket("发送方没有正确收到确认", ackPkt);
	}
}

void TCPRdtSender::timeoutHandler(int seqNum) {
	//唯一一个定时器,无需考虑seqNum
	//只发送第一个,区别于GBN的重发
	printf("发送方定时器时间到，重发所有已发送但未确认的报文\n");
	pns->stopTimer(SENDER, 0);										//首先关闭定时器
	pns->startTimer(SENDER, Configuration::TIME_OUT, 0);			//重新启动发送方定时器
	pUtils->printPacket("", this->packetWaitingAck[base]);
	pns->sendToNetworkLayer(RECEIVER, this->packetWaitingAck[base]);//重新发送数据包
}
