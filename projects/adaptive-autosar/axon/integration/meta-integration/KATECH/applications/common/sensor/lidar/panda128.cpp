
#define _USE_MATH_DEFINES

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include <logger.h>

#include "lidar/panda128.h"
#include "lidar/panda128_config.h"


#include "error.h"
#include <vector>
#include <chrono>

#include <cmath>

namespace Lidar
{

double dgree2radian(double degree);
void changeByteOrder16(unsigned short * value);
void changeByteOrder32(unsigned int * value);
void changeByteOrder64(unsigned long long * value);
void changeByteOrder(std::shared_ptr<Lidar::Panda128::xPacket> packet);

constexpr double const_pi()
{
	return std::atan(1) * 4;
}

double dgree2radian(double degree)
{
	return degree * M_PI / 180;
}






unsigned short Panda128::Calculate_CRC_RPF(const unsigned char * buffer, int size, unsigned char AC)
{
	unsigned short tmp;
	unsigned short crc = 0xffff;
	tmp = static_cast<unsigned short>(crc >> 8) ^ static_cast<unsigned short>(AC);
	crc = static_cast<unsigned short>(crc << 8) ^ static_cast<unsigned short>(CRC_CCITT_TABLE[tmp]);
	for(int i = 0; i < size; i++)
	{
		tmp = static_cast<unsigned short>(crc >> 8) ^ buffer[i];
		crc = static_cast<unsigned short>(crc << 8) ^ CRC_CCITT_TABLE[tmp];
	}
	tmp = static_cast<unsigned short>(crc >> 8) ^ static_cast<unsigned char>(0x40);
	crc = static_cast<unsigned short>(crc << 8) ^ CRC_CCITT_TABLE[tmp];
	tmp = static_cast<unsigned short>(crc >> 8) ^ static_cast<unsigned char>(0xF8);
	crc = static_cast<unsigned short>(crc << 8) ^ CRC_CCITT_TABLE[tmp];
	return crc;
}

void changeByteOrder16(unsigned short * value)
{
	unsigned short temp;
	temp = *value;
	*value = ((temp >> 8) & 0x00FF);
	*value |= ((temp << 8) & 0xFF00);
}

void changeByteOrder32(unsigned int * value)
{
	unsigned int temp32;
	temp32 = *value;
	*value = ((temp32 >> 24) & 0x000000FF);
	*value |= ((temp32 >> 8) & 0x0000FF00);
	*value |= ((temp32 << 8) & 0x00FF0000);
	*value |= ((temp32 << 24) & 0xFF000000);
}

void changeByteOrder64(unsigned long long * value)
{
	unsigned long long temp;
	temp = *value;
	*value = ((temp >> 56) & 0x00000000000000FF);
	*value |= ((temp >> 40) & 0x000000000000FF00);
	*value |= ((temp >> 24) & 0x0000000000FF0000);
	*value |= ((temp >> 8) & 0x00000000FF000000);
	*value |= ((temp << 8) & 0x000000FF00000000);
	*value |= ((temp << 24) & 0x0000FF0000000000);
	*value |= ((temp << 40) & 0x00FF000000000000);
	*value |= ((temp << 56) & 0xFF00000000000000);
}

void changeByteOrder(std::shared_ptr<Lidar::Panda128::xPacket> packet)
{
	// changeByteOrder32(&packet->Header.PSEQ);
	// changeByteOrder64(&packet->Header.TREF);
	//changeByteOrder32(&packet->Tail.UDPSequence);
#if 0
	for(int i = 0; i < 160; i++)
	{
		unsigned char buffer[2];
		memcpy(buffer, &packet->FiringReturn[i], 2);
		packet->FiringReturn[i].HDIR = (buffer[0] & 0x80) ? 1 : 0;
		packet->FiringReturn[i].VDIR = (buffer[0] & 0x40) ? 1 : 0;
		packet->FiringReturn[i].VDFL
		    = static_cast<unsigned short>((buffer[0] & 0x3F) << 8) + static_cast<unsigned short>(buffer[1]);
		changeByteOrder16(&packet->FiringReturn[i].AZM);
		changeByteOrder16(&packet->FiringReturn[i].DIST);
	}
#endif
	// changeByteOrder16(&rx_packet->Footer.CRC);
}

Panda128::Panda128(xCallback callback, unsigned short port, int timeOut_)
{
	index = mCount++;
	if(mPcds.empty())
	{
		for(int i = 0; i < 3; ++i)
		{
			std::queue<std::shared_ptr<xPCD>> tmp;
			mPcds.push_back(tmp);
		}
	}
	katech::Log::Info() << "init start";
	// Init data
	mPort = port;
	mLastReceived_Index = -1;
	mPcdInput = std::make_shared<std::vector<xBlock>>();
	mTimeOut = timeOut_;
	// set callback
	mCallback = callback;
	struct sockaddr_in serv_adr;
	// udp port open
	mSocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(mSocket < 0)
	{
		katech::Log::Error() << "UDP socket creation error(" << errno << ")";
	}
	// receive udp
	serv_adr.sin_family = AF_INET;
	serv_adr.sin_port = htons(mPort);
	katech::Log::Info() << "listen port = " << mPort;

    if (inet_pton(AF_INET, "192.168.0.200", &serv_adr.sin_addr) <= 0) {
        katech::Log::Error() << "inet_pton error";
    }

	if(bind(mSocket, reinterpret_cast<struct sockaddr *>(&serv_adr), sizeof(serv_adr)) < 0)
	{
		katech::Log::Error() << "bind() error (" << errno << ")";
	}
	mTimeoutLock = std::make_unique<std::mutex>();
	mCv = std::make_unique<std::condition_variable>();
	// thread start
	mThreadRunning = true;
	mUdpReceiver = std::make_unique<std::thread>(&Panda128::Thread_UdpReceiver, this);
	mPacketProcessorRunning = true;
	mPacketProcessor = std::make_unique<std::thread>(&Panda128::ThreadPacketProcessor, this);
	katech::Log::Info() << "init complete";
}

void Panda128::printPacket(std::shared_ptr<xPacket> packet)
{
#if 0
	INFO("Packet size packet = %d / header = %d / body = %d / block = %d / fc = %d / tail = %d",
	     sizeof(xPacket),
	     sizeof(xHeader),
	     sizeof(xBody),
	     sizeof(xBlock),
	     sizeof(xFunctinalsafety),
	     sizeof(xTail)
	    );
	INFO("Header -> SOP = %02X%02X / Version = %d.%d / DS = %d / FS = %d / IMU = %d / Seq = %d",
	     packet->Header.SOP[0],
	     packet->Header.SOP[1],
	     packet->Header.Version[0],
	     packet->Header.Version[1],
	     packet->Header.Flag_DigitalSignature,
	     packet->Header.Flag_FunctinalSafety,
	     packet->Header.Flag_IMU,
	     packet->Header.Flag_UDP_Sequence
	    );
#endif
#if 1
	INFO("Time = %02d%02d%02d - %02d:%02d:%02d.%06d / Azimuth = %d, %d",
	     packet->Tail.DateTime.Year,
	     packet->Tail.DateTime.Month,
	     packet->Tail.DateTime.Day,
	     packet->Tail.DateTime.Hour,
	     packet->Tail.DateTime.Min,
	     packet->Tail.DateTime.Sec,
	     packet->Tail.DateTime.us,
	     packet->Body.Block[0].Azimuth,
	     packet->Body.Block[1].Azimuth
	    );
#endif
}

std::shared_ptr<xPCD> Panda128::getPCDData(std::shared_ptr<std::vector<xBlock>> pcd_input)
{
	int firing_time_offset_index_0, firing_time_offset_index_1;
	int block_Number = 0;
	std::shared_ptr<xPCD> result = std::make_shared<xPCD>();
	// init offset
	result->reserve(pcd_input->size());
	//INFO("State ( Oper= %d / Azi = %d,%d / rpm = %d , DisUnit = %d)",mOperationalState, mAzimuthState_0,mAzimuthState_1, mMotorSpeed, mDisUnit);
	firing_time_offset_index_0 = firing_time_offset_index_1 =(mOperationalState*4);
	firing_time_offset_index_0 += (mAzimuthState_0 *2);
	firing_time_offset_index_1 += (mAzimuthState_1 *2);
	for(xBlock block : (*pcd_input))
	{
		double azi = block.Azimuth;
		azi /= 100;
		for(int i = 0; i < 128 ; i++)
		{
			double virtical_angle, horizental_angle;
			double length;
			xChannel_XX data = block.Channel[i];
			length = (double)data.Distance * (double)mDisUnit / 1000;	// mDisUnit = 4mm
			// get angle
			virtical_angle = panda128_config_angle_Offset[i][0];
			horizental_angle = azi + panda128_config_angle_Offset[i][1];
//			if(block_Number == 0)
//			{
//				horizental_angle+= (panda128_config_firingtime_offset[i][firing_time_offset_index_0	+ ((length < 2.85)?1:0)] * mMotorSpeed / 1000000);
//			}
//			else
//			{
//				horizental_angle+= (panda128_config_firingtime_offset[i][firing_time_offset_index_1  + ((length < 2.85)?1:0)] * mMotorSpeed / 1000000);
//			}
//			horizental_angle = 90.0;
//			INFO("Channel[%3d] = %lf / %lf / %5d (%3d)", i, horizental_angle, virtical_angle,length, data.Reflectivity);
			horizental_angle = (horizental_angle * M_PI)/180;	//to radian
			virtical_angle = (virtical_angle * M_PI)/180;		// to radian
			xPoint3D temp;
			temp.x = sin(horizental_angle) * length;
			temp.y = cos(horizental_angle) * length;
			temp.z = sin(virtical_angle) * length;
			temp.intensity = data.Reflectivity;
			if (temp.x > -1 && temp.x < 1 && temp.y > -2.5  && temp.y < 2.0)
			{
				continue;
			}
//                        if (temp.x < 0 && temp.x > -1.5 && temp.y < 0.5 && temp.y > -0.5 ){
//                            continue;
//                        }
			result->push_back(temp);
		}
		block_Number++;
		block_Number %= 2;
	}
	INFO("in = %ld / out = %ld", pcd_input->size(), result->size());
	return result;
}

void Panda128::mergePacket(std::shared_ptr<xPacket> packet)
{
	//INFO("last index = %5d / current index = %5d", mLastReceived_Index, packet->Body.Block[0].Azimuth);
	if(mLastReceived_Index > packet->Body.Block[0].Azimuth)
	{
		//INFO("trigerred!!!!!");
		std::shared_ptr<xPCD> pcd = getPCDData(mPcdInput);
		mPcdInput->clear();
		mPcds[index].push(pcd);
		mCallbackCv.notify_all();
	}
	mLastReceived_Index = packet->Body.Block[0].Azimuth;
	mPcdInput->push_back(packet->Body.Block[0]);
	mPcdInput->push_back(packet->Body.Block[1]);
	mAzimuthState_0 = packet->Tail.AzimuthState.Block1;
	mAzimuthState_1 = packet->Tail.AzimuthState.Block2;
	mOperationalState = packet->Tail.OperationalState;
	mMotorSpeed = packet->Tail.MotorSpeed;
	mDisUnit = packet->Header.DisUnit;
}

void Panda128::Thread_UdpReceiver()
{
	if(!mWrapperRunning)
	{
		mWrapperRunning = true;
		mCallbackWrapper = std::make_unique<std::thread>(&Panda128::ThreadCallbackWrapper, this);
	}
	// int receivedLength;
	struct sockaddr server_addr;
	uint64_t lastReceived = 0;
	uint32_t lastReceived_us = 0;
	socklen_t server_addr_size;
	std::shared_ptr<xPacket> rx_packet;
	katech::Log::Info() << index << "Panda128 Rx start";
	while(mThreadRunning)
	{
		uint64_t temp;
		rx_packet = std::make_shared<xPacket>();
		recvfrom(mSocket, rx_packet.get(), sizeof(xPacket), 0, &server_addr, &server_addr_size);
		//INFO("recved!!!");
		//printPacket(rx_packet);
		temp = rx_packet->Tail.DateTime.Year;
		//INFO("time = %016lX", temp);
		temp *= 12;
		temp += rx_packet->Tail.DateTime.Month ;
		//		INFO("time = %016lX", temp);
		temp *= 31;
		temp += rx_packet->Tail.DateTime.Day;
		//		INFO("time = %016lX", temp);
		temp *= 24;
		temp += rx_packet->Tail.DateTime.Hour;
		//		INFO("time = %016lX", temp);
		temp *= 60;
		temp += rx_packet->Tail.DateTime.Min;
		//		INFO("time = %016lX", temp);
		temp *= 60;
		temp += rx_packet->Tail.DateTime.Sec;
		//INFO("time = %016lX", temp);
		temp *= 1000000;
		//INFO("time = %016X", temp);
		temp += rx_packet->Tail.DateTime.us;
		//		INFO("time = %016lX", temp);
		if((lastReceived < temp) )
		{
			mPacket.push(rx_packet);
			// Notify after receiving
			mCv->notify_all();
			lastReceived = temp;
		}
	}
	katech::Log::Info() << "Panda128 Rx stop";
}

void Panda128::ThreadPacketProcessor()
{
	while(mPacketProcessorRunning)
	{
		std::unique_lock<std::mutex> lk(*mTimeoutLock);
		// Received in time
		if(mCv->wait_until(lk, std::chrono::system_clock::now() + std::chrono::milliseconds(mTimeOut))
		        == std::cv_status::no_timeout)
		{
//                    INFO("size of mpacket %d", mPacket.size());
			while(mPacket.size() != 0)
			{
				std::shared_ptr<xPacket> rx_packet = mPacket.front();
				mPacket.pop();
#if 0
				unsigned short calCrc;
				changeByteOrder16(&rx_packet->Footer.CRC);
				calCrc = Panda128::Calculate_CRC_RPF(reinterpret_cast<unsigned char *>(rx_packet.get()),
				                                     sizeof(xPacket) - sizeof(xFooter),
				                                     rx_packet->Footer.AC);
#endif
				//changeByteOrder(rx_packet);
				//printPacket(rx_packet);
				mergePacket(rx_packet);
			}
		}
		// Timeout
		else
		{
			katech::Log::Error() << "Lidar" << index << "packet timeout!";
		}
	}
}

void Panda128::ThreadCallbackWrapper()
{
	while(mWrapperRunning)
	{
		std::unique_lock<std::mutex> uLock(mCallbackLock);
		// Wait until every queue has at least 1
		mCallbackCv.wait(uLock, [&]
		{
			bool atLeastOneEmpty = false;

			for(int i = 0; i < mCount; ++i)
			{
				if(mPcds[i].empty())
				{
					atLeastOneEmpty = true;
					break;
				}
			}

			return !atLeastOneEmpty;
		});
		//katech::Log::Info() << "Received all";
		std::vector<std::shared_ptr<xPCD>> pcds;
		INFO("size of mPcds %d", mPcds.size());
		INFO("mCount : %d", mCount);
		for(int i = 0; i < mCount; ++i)
		{
			pcds.push_back(mPcds[i].back());
			mPcds[i].pop();
		}
		INFO("size of pcds after %d", pcds.size());
		INFO("size of mPcds after %d", mPcds.size());
		mCallback(pcds);
	}
}

Panda128::~Panda128()
{
	mThreadRunning = false;
	mUdpReceiver->join();
	mPacketProcessorRunning = false;
	mPacketProcessor->join();
	// close udp port
	close(mSocket);
	mWrapperRunning = false;
	mCallbackWrapper->join();
}

}  // namespace Lidar

