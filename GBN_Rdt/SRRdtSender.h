#ifndef SR_RDT_SENDER_H
#define SR_RDT_SENDER_H
#include "RdtSender.h"
#define N 4							//窗口长度
#define K 8							//序号个数，2^3
class SRRdtSender :public RdtSender
{
private:
	int base = 0;					//基序号
	int nextseqnum = 0;				//下一个序号
	Packet packetWaitingAck[K];
	bool IsReceive[K];				//标记分组是否被接收
	bool IsFull = false;

public:
	bool getWaitingState();
	bool send(Message &message);						//发送应用层下来的Message，由NetworkServiceSimulator调用,如果发送方成功地将Message发送到网络层，返回true;如果因为发送方处于等待正确确认状态而拒绝发送Message，则返回false
	void receive(Packet &ackPkt);						//接受确认Ack，将被NetworkServiceSimulator调用	
	void timeoutHandler(int seqNum);					//Timeout handler，将被NetworkServiceSimulator调用

public:
	SRRdtSender();
	virtual ~SRRdtSender();
};

#endif


