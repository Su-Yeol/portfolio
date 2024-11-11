#pragma once
// #include <WinSock2.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ctime> //C++

#include <iostream>
using std::cout;
using std::endl;
#pragma comment(lib,"ws2_32.lib")
#pragma warning(disable:4996)

#include <thread>
#include <chrono>

#include <string>
#include <stdexcept>

#include <vector>

using namespace std;
using std::cin;
using std::this_thread::sleep_for;

#define BUF_SIZE 1024 // block transfer size  
#define QUEUE_SIZE 10

constexpr int SLEEP_TIME = 100;

//void Tcp_Connect();

#include "PathManager.h"

class TCPSocket
{
public:
	TCPSocket();
	virtual ~TCPSocket();
	
	void Start_Thread();
	bool GetPath(PathVertexVct* pMainPath, double longitude, double latitude);
	static void Tcp_Connect(void* pParam);
	

public:
	string addr;
	int port;

	int PathType;
	bool m_bSendFlag;
	bool m_DataSendConfirm;
	PathVertexVct m_oPathVertexVct;
	PathManger m_oPathManager;
	thread* m_pTCP_Thread;
	mutex m_oMutex;

	int m_tSpd, m_tSpdflg, m_tTurnSig, m_tTCPStatus;
};