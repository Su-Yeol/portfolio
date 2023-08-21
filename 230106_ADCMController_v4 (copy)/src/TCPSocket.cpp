#include "TCPSocket.h"

int cnt = 0;
int aaaa = 0;

TCPSocket::TCPSocket(void)
{
	addr = "";
	port = 0;
	m_bSendFlag = false;
	m_DataSendConfirm = false;
	m_pTCP_Thread = NULL;
	PathType = 0;
	m_tSpd = 0;
	m_tSpdflg = 0;
	m_tTurnSig = 0;
	m_tTCPStatus = 0;
}

TCPSocket::~TCPSocket(void)
{

	if (m_pTCP_Thread)
	{
		m_pTCP_Thread->join();
		delete m_pTCP_Thread;
	}
}

void TCPSocket::Start_Thread()
{
	m_pTCP_Thread = new thread(Tcp_Connect, this);

	return;
}

bool TCPSocket::GetPath(PathVertexVct* pMainPath, double longitude, double latitude)
{
	return m_oPathManager.GetPath(pMainPath, longitude, latitude);
}

void TCPSocket::Tcp_Connect(void* pParam)
{
	TCPSocket* pTCPSocket = (TCPSocket*)pParam;

	int s = socket(AF_INET, SOCK_STREAM, 0); //Create socket
	if (s == -1)
	{
		cout << "TCP socket() error : " << endl;
		return; //Couldn't create the socket
	}

	struct sockaddr_in target;
    memset(&target, 0, sizeof(target));
	target.sin_family = AF_INET; // address family Internet
	target.sin_port = htons(pTCPSocket->port); //Port to connect on
	target.sin_addr.s_addr = inet_addr(pTCPSocket->addr.c_str()); //Target IP

	timeval tv;
	tv.tv_sec  = 3;
	tv.tv_usec = 0;
	setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv, sizeof(timeval));

	int tcpcnt = 0;
	while(connect(s, (struct sockaddr*)&target, sizeof(target)) == -1){
		cout << "TCP connect() error : " << tcpcnt << endl;
		sleep(1);
		tcpcnt++;
	}

	cout << "-----------------------------" << endl;
	cout << "TCP connect() ON : " << endl;
	cout << "-----------------------------" << endl;
	
	std::string sRand;

	//Header
	int ID, normal, PathType, DataSendConfirm, FileSize, StartIndex, EndIndex;
	double MapMatchPositionX, MapMatchPositionY, CarPositionX, CarPositionY;
	int tSpd, tSpdflg, tTurnSig;

	const int TCPBUF_SIZE = 60;
	unsigned char buf[TCPBUF_SIZE];

	while (true)
	{
		try
		{
			// cout << "tcp" << endl;

			// int s = socket(AF_INET, SOCK_STREAM, 0); //Create socket
			// if (s != -1)
			// {
			// 	struct sockaddr_in target;
			// 	memset(&target, 0, sizeof(target));
			// 	target.sin_family = AF_INET; // address family Internet
			// 	target.sin_port = htons(pTCPSocket->port); //Port to connect on
			// 	target.sin_addr.s_addr = inet_addr(pTCPSocket->addr.c_str()); //Target IP

			// 	int tcpcnt = 0;
			// 	while(connect(s, (struct sockaddr*)&target, sizeof(target)) == -1){
			// 		cout << "TCP connect() error : " << tcpcnt << endl;
			// 		sleep(1);
			// 		tcpcnt++;
			// 	}

			// 	cout << "-----------------------------" << endl;
			// 	cout << "TCP connect() ON : " << endl;
			// 	cout << "-----------------------------" << endl;
			// }

			// while(pTCPSocket->m_DataSendConfirm);
			////////////////////////////// Header
			int HeaderSize = TCPBUF_SIZE;
			int	HeaderSendSize = HeaderSize;
			int HeaderTotalSize = 0;
			
			unsigned char* Header = new unsigned char[TCPBUF_SIZE];
			int nHeaderIndex = 0;
			while (1)
			{
				// cout << "tcp1" << endl;
				int Recv_Header = recv(s, (char*)buf, HeaderSendSize, 0);
				// cout << "tcp1-1" << Recv_Header << endl;
				if(Recv_Header == -1){
					pTCPSocket->m_tTCPStatus = 0;
					close(s);

					s = socket(AF_INET, SOCK_STREAM, 0); //Create socket
					if (s == -1)
					{
						cout << "TCP socket() error : " << endl;
						return; //Couldn't create the socket
					}

					memset(&target, 0, sizeof(target));
					target.sin_family = AF_INET; // address family Internet
					target.sin_port = htons(pTCPSocket->port); //Port to connect on
					target.sin_addr.s_addr = inet_addr(pTCPSocket->addr.c_str()); //Target IP

					timeval tv;
					tv.tv_sec  = 3;
					tv.tv_usec = 0;
					setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv, sizeof(timeval));
					while(connect(s, (struct sockaddr*)&target, sizeof(target)) == -1){
						cout << "TCP connect() error : " << tcpcnt << endl;
						sleep(1);
						tcpcnt++;
					}

					cout << "-----------------------------" << endl;
					cout << "TCP connect() ON : " << endl;
					cout << "-----------------------------" << endl;
				}



				for (int i = 0; i < Recv_Header; i++)
				{
					Header[nHeaderIndex] = buf[i];
					nHeaderIndex++;
				}

				HeaderTotalSize = HeaderTotalSize + Recv_Header;

				if (HeaderTotalSize == TCPBUF_SIZE)
				{
					break;
				}
				else
				{
					HeaderSendSize = HeaderSendSize - Recv_Header;
				}
			}

			pTCPSocket->m_tTCPStatus = 1;

			ID = (int)Header[0];
			normal = (int)Header[1];
			PathType = (int)Header[2];
			DataSendConfirm = (int)Header[3];
			FileSize = (int)((Header[4] << 24) + (Header[5] << 16) + (Header[6] << 8) + Header[7]);
			StartIndex = (int)((Header[8] << 24) + (Header[9] << 16) + (Header[10] << 8) + Header[11]);
			EndIndex = (int)((Header[12] << 24) + (Header[13] << 16) + (Header[14] << 8) + Header[15]);

			MapMatchPositionX = (double)((Header[16] << 24) + (Header[17] << 16) + (Header[18] << 8) + Header[19]) + (((double)((Header[20] << 24) + (Header[21] << 16) + (Header[22] << 8) + Header[23]) * 0.00000001));
			MapMatchPositionY = (double)((Header[24] << 24) + (Header[25] << 16) + (Header[26] << 8) + Header[27]) + (((double)((Header[28] << 24) + (Header[29] << 16) + (Header[30] << 8) + Header[31]) * 0.00000001));

			CarPositionX = (double)((Header[32] << 24) + (Header[33] << 16) + (Header[34] << 8) + Header[35]) + (((double)((Header[36] << 24) + (Header[37] << 16) + (Header[38] << 8) + Header[39]) * 0.00000001));
			CarPositionY = (double)((Header[40] << 24) + (Header[41] << 16) + (Header[42] << 8) + Header[43]) + (((double)((Header[44] << 24) + (Header[45] << 16) + (Header[46] << 8) + Header[47]) * 0.00000001));

			// 221103 
			tSpdflg = (int)((Header[48] << 24) + (Header[49] << 16) + (Header[50] << 8) + Header[51]);
			tSpd    = (int)((Header[52] << 24) + (Header[53] << 16) + (Header[54] << 8) + Header[55]);
			tTurnSig = (int)((Header[56] << 24) + (Header[57] << 16) + (Header[58] << 8) + Header[59]);

			pTCPSocket->m_tSpdflg = tSpdflg;
			pTCPSocket->m_tSpd = tSpd;
			pTCPSocket->m_tTurnSig = tTurnSig;

			delete[] Header;
			
			if (FileSize == 0)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(10));
				//Sleep(10);
				continue;
			}

		
			if (pTCPSocket->m_bSendFlag == false)
			{			
				//pTCPSocket->oPathVertexVct.DeleteAll();
				pTCPSocket->m_oPathVertexVct.clear();
				pTCPSocket->m_bSendFlag = true;
			}

			////////////////////////////// Vertex
			int	BodySendSize = FileSize;
			int BodyTotalSize = 0;

			unsigned char* Vertex = new unsigned char[FileSize];
			unsigned char* VertexByte = new unsigned char[FileSize];

			int nIndex = 0;
			while (1)
			{
				// cout << "tcp2" << endl;
				int Recv_Body = recv(s, (char*)VertexByte, BodySendSize, 0);

				for (int i = 0; i < Recv_Body; i++)
				{
					Vertex[nIndex] = VertexByte[i];
					nIndex++;
				}

				BodyTotalSize = BodyTotalSize + Recv_Body;

				if (BodyTotalSize == FileSize)
				{
					break;
				}
				else
				{
					BodySendSize = BodySendSize - Recv_Body;
				}
			}			
			
			for (int i = 0; i < FileSize / 8; i++)
			{
				int nRelativeX = (int)((Vertex[i*8] << 24) + (Vertex[i*8+1] << 16) + (Vertex[i*8+2] << 8) + Vertex[i*8+3]);
				int nRelativeY = (int)((Vertex[i*8+4] << 24) + (Vertex[i*8+5] << 16) + (Vertex[i*8+6] << 8) + Vertex[i*8+7]);

				float fRelativeX = 0;
				float fRelativeY = 0;

				memcpy(&fRelativeX, &nRelativeX, sizeof(float));
				memcpy(&fRelativeY, &nRelativeY, sizeof(float));

				PathVertex* pPathVertex = new PathVertex();
				pPathVertex->x = MapMatchPositionX + fRelativeX;
				pPathVertex->y = MapMatchPositionY + fRelativeY;

				pTCPSocket->m_oPathVertexVct.push_back(pPathVertex);
			}

			delete[] VertexByte;
			delete[] Vertex;

			// printf("DataSendConfirm: %d %d / FS: %d aaa: %d\n", DataSendConfirm, pTCPSocket->m_oPathVertexVct.size(), FileSize,aaaa);
			// aaaa++;
			if (DataSendConfirm)
			{
				pTCPSocket->m_bSendFlag = false;

				printf("------------------------------------\n");
				printf("ID: %d, normal: %d\n", ID, normal);
				printf("FileSize: %d\n", FileSize);
				printf("PathType: %d\n", PathType);
				printf("S: %d / E: %d\n", StartIndex, EndIndex);
				printf("MapMatchPosition: %f/%f\n", MapMatchPositionX,MapMatchPositionY);
				printf("CarPosition:      %f/%f\n", CarPositionX,CarPositionY);
				printf("------------------------------------\n");

				if (PathType == 1 || PathType == 2)
				{
					pTCPSocket->m_oPathManager.SetPath(&pTCPSocket->m_oPathVertexVct);
					pTCPSocket->m_DataSendConfirm = true;
				}
				else if (PathType == 3)
				{
					pTCPSocket->m_oPathManager.ModificationPath(&pTCPSocket->m_oPathVertexVct, StartIndex, EndIndex);
				}

				DataSendConfirm = 0;
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(10));
			//Sleep(10);

		}
		catch (std::out_of_range& e) {
            std::cout << "<TCPSocket> Out_of_range Error" << '\n';
        }
        catch (std::length_error& e) {
            std::cout << "<TCPSocket> Length Error" << '\n';
        }
        catch(std::exception& e){
            std::cout << "<TCPSocket> EXCEPTION " << '\n';
            std::cout << e.what() << '\n';
        }
		// catch (const std::invalid_argument &ex)
		// {
		// 	std::cerr << "Invalid argument while converting string to number" << endl;
		// 	std::cerr << "Error: " << ex.what() << endl;
		// 	break;
		// }
		// catch (const std::out_of_range &ex)
		// {
		// 	std::cerr << "Invalid argument while converting string to number" << endl;
		// 	std::cerr << "Error: " << ex.what() << endl;
		// 	break;
		// }
	}

	cout << ">>> Close TCP socket <<< " << endl;

	close(s);
	return;
}