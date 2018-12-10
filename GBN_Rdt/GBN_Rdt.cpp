// GBN_Rdt.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "Global.h"
#include "RdtSender.h"
#include "RdtReceiver.h"
#include "GBNRdtSender.h"
#include "GBNRdtReceiver.h"


int main(int argc, char** argv[])
{
	RdtSender *ps = new GBNRdtSender();
	RdtReceiver * pr = new GBNRdtReceiver();
	pns->init();
	pns->setRtdSender(ps);
	pns->setRtdReceiver(pr);
	//pns->setInputFile("C:\\Users\\crackryan\\Desktop\\input.txt");
	//pns->setOutputFile("C:\\Users\\crackryan\\Desktop\\output.txt");
	pns->setInputFile("C:\\Users\\17361\\Documents\\Tencent Files\\1736103647\\FileRecv\\rdt-win-student\\input.txt");
	pns->setOutputFile("C:\\Users\\17361\\Documents\\Tencent Files\\1736103647\\FileRecv\\rdt-win-student\\output.txt");
	//pns->setInputFile("input.txt"); 
	//pns->setOutputFile("output.txt");
	pns->start();

	delete ps;
	delete pr;
	delete pUtils;									//ָ��Ψһ�Ĺ�����ʵ����ֻ��main��������ǰdelete
	delete pns;										//ָ��Ψһ��ģ�����绷����ʵ����ֻ��main��������ǰdelete

	return 0;
}


