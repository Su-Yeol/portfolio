#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <chrono>
#include <thread>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <DCommunicator.h>
using namespace std;
// UDP
const string S32GIp = "192.168.10.98";
const int S32GPort = 4912;
// CAN interface
CANClass C_CANFrame;
CANClass MDPSFrame;
CANClass E_CANFrame;
CANClass DRVFrame;
u_int16_t CtoMDPS[5] = {0x35, 0x4A, 0xA0, 0x125, 0x12A};
u_int16_t MDPStoC[1] = {0xEA};
u_int16_t EtoDRV[23] = {0x35, 0x4A, 0x60, 0x6F, 0xA0, 0xEA, 0x125, 0x175, 0x1AA, 0x1B0, 0x1B5, 0x1BA,
                        0x2E0, 0x1CF, 0x3C1, 0x411, 0x413, 0x416, 0x422, 0x4D8, 0x4DE, 0x4F2, 0x4FE};
u_int16_t DRVtoE[4] = {0x12A, 0x1E0, 0x160, 0x1EA};
// CRC
uint8_t AlvCnt = 0;
uint8_t AlvCnt2 = 0;
uint8_t AlvCnt3 = 0;
uint8_t MsgA0_DataID_H = 0xF8;
uint8_t MsgA0_DataID_L = 0xA0;
uint8_t Msg165_DataID_H = 0xF9;
uint8_t Msg165_DataID_L = 0x65;
uint8_t Msg1A0_DataID_H = 0xF9;
uint8_t Msg1A0_DataID_L = 0xA0;
uint16_t crc16table[256] = {
    0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50A5, 0x60C6, 0x70E7, // 1
    0x8108, 0x9129, 0xA14A, 0xB16B, 0xC18C, 0xD1AD, 0xE1CE, 0xF1EF, // 2
    0x1231, 0x0210, 0x3273, 0x2252, 0x52B5, 0x4294, 0x72F7, 0x62D6, // 3
    0x9339, 0x8318, 0xB37B, 0xA35A, 0xD3BD, 0xC39C, 0xF3FF, 0xE3DE, // 4
    0x2462, 0x3443, 0x0420, 0x1401, 0x64E6, 0x74C7, 0x44A4, 0x5485, // 5
    0xA56A, 0xB54B, 0x8528, 0x9509, 0xE5EE, 0xF5CF, 0xC5AC, 0xD58D, // 6
    0x3653, 0x2672, 0x1611, 0x0630, 0x76D7, 0x66F6, 0x5695, 0x46B4, // 7
    0xB75B, 0xA77A, 0x9719, 0x8738, 0xF7DF, 0xE7FE, 0xD79D, 0xC7BC, // 8
    0x48C4, 0x58E5, 0x6886, 0x78A7, 0x0840, 0x1861, 0x2802, 0x3823, // 9
    0xC9CC, 0xD9ED, 0xE98E, 0xF9AF, 0x8948, 0x9969, 0xA90A, 0xB92B, // 10
    0x5AF5, 0x4AD4, 0x7AB7, 0x6A96, 0x1A71, 0x0A50, 0x3A33, 0x2A12, // 11
    0xDBFD, 0xCBDC, 0xFBBF, 0xEB9E, 0x9B79, 0x8B58, 0xBB3B, 0xAB1A, // 12
    0x6CA6, 0x7C87, 0x4CE4, 0x5CC5, 0x2C22, 0x3C03, 0x0C60, 0x1C41, // 13
    0xEDAE, 0xFD8F, 0xCDEC, 0xDDCD, 0xAD2A, 0xBD0B, 0x8D68, 0x9D49, // 14
    0x7E97, 0x6EB6, 0x5ED5, 0x4EF4, 0x3E13, 0x2E32, 0x1E51, 0x0E70, // 15
    0xFF9F, 0xEFBE, 0xDFDD, 0xCFFC, 0xBF1B, 0xAF3A, 0x9F59, 0x8F78, // 16
    0x9188, 0x81A9, 0xB1CA, 0xA1EB, 0xD10C, 0xC12D, 0xF14E, 0xE16F, // 17
    0x1080, 0x00A1, 0x30C2, 0x20E3, 0x5004, 0x4025, 0x7046, 0x6067, // 18
    0x83B9, 0x9398, 0xA3FB, 0xB3DA, 0xC33D, 0xD31C, 0xE37F, 0xF35E, // 19
    0x02B1, 0x1290, 0x22F3, 0x32D2, 0x4235, 0x5214, 0x6277, 0x7256, // 20
    0xB5EA, 0xA5CB, 0x95A8, 0x8589, 0xF56E, 0xE54F, 0xD52C, 0xC50D, // 21
    0x34E2, 0x24C3, 0x14A0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405, // 22
    0xA7DB, 0xB7FA, 0x8799, 0x97B8, 0xE75F, 0xF77E, 0xC71D, 0xD73C, // 23
    0x26D3, 0x36F2, 0x0691, 0x16B0, 0x6657, 0x7676, 0x4615, 0x5634, // 24
    0xD94C, 0xC96D, 0xF90E, 0xE92F, 0x99C8, 0x89E9, 0xB98A, 0xA9AB, // 25
    0x5844, 0x4865, 0x7806, 0x6827, 0x18C0, 0x08E1, 0x3882, 0x28A3, // 26
    0xCB7D, 0xDB5C, 0xEB3F, 0xFB1E, 0x8BF9, 0x9BD8, 0xABBB, 0xBB9A, // 27
    0x4A75, 0x5A54, 0x6A37, 0x7A16, 0x0AF1, 0x1AD0, 0x2AB3, 0x3A92, // 28
    0xFD2E, 0xED0F, 0xDD6C, 0xCD4D, 0xBDAA, 0xAD8B, 0x9DE8, 0x8DC9, // 29
    0x7C26, 0x6C07, 0x5C64, 0x4C45, 0x3CA2, 0x2C83, 0x1CE0, 0x0CC1, // 30
    0xEF1F, 0xFF3E, 0xCF5D, 0xDF7C, 0xAF9B, 0xBFBA, 0x8FD9, 0x9FF8, // 31
    0x6E17, 0x7E36, 0x4E55, 0x5E74, 0x2E93, 0x3EB2, 0x0ED1, 0x1EF0  // 32
};
// SCC OnOff
uint8_t SCCbtn;
// int SCC_ObjDstVal = 0x7FE;
// int SCC_ObjRelSpdVal = 0xFFE;
// int SCC_ObjLatPosVal = 0xC8;
int SCC_SysFlrSta = 0;
int SCC_MainOnOffSta = 0;
int SCC_OpSta = 0;
int SCC_TakeoverReq = 0;
int SCC_InfoDis = 0;
int SCC_DrvAlrtDis = 0;
int SCC_ObjDstLvlVal = 0;
int SCC_HeadwayDstSetVal = 0;
int SCC_NSCCAutoSetSpdSta = 0;
int SCC_TrgtSpdSetVal = 0; // CLU speed
int SCC_NSCCOpsta = 0;
int SCC_NSCCOnOffsta = 2;
int SCC_ObjSta = 0;
int SCC_SnstvtyModRetVal = 0;
int SCC_NSCCAutoSetSpdUpdtsta = 0;
int SCC_NSCCInfoPUDis = 0;
int SCC_AccelReqVal = 1023; // Ax: 1023(Refer Val) = 가감속X
int SCC_AccelReqRawVal = 1023;
int SCC_JrkUpplimVal = 0;
int SCC_JrkLwrLimVal = 0;
int SCC_AccelLimBandUppVal = 0;
int SCC_AccelLimBandLwrVal = 0;
int SCC_VehStpReq = 0;
int SCC_DclModReq = 0;
int SCC_TrgtDstVal = 0;
// SCC btn
int SWRC_CrsMainSwSta = 0;
// MDPS, wheel Angle
uint8_t MDPSmodeCnt = 0;
uint8_t MDPSmode = 0;
uint8_t PAmode = 0;
int PA_StrAnglReq = 0; // 0x165
int PrePA_StrAnglReq = 0;
int CurrentAnglVal = 0; // 0xEA
int ReferAnglVal = 0;   // UDP - Wheel angle
int AnglValDiffer;
// Brake, PRND, Lamp(0x3C1: btn, 0x413: light)
uint8_t PRNDmode = 0;
double ESC_CylPrsrVal = 0;

void CCANtoMDPS()
{
    double WheelSpeedFL = 0;
    double WheelSpeedFR = 0;
    double WheelSpeedRL = 0;
    double WheelSpeedRR = 0;
    std::cout << "[CCANtoMDPS] ------------------ C-CAN to MDPS Gateway START! ......" << endl;
    while (true)
    {
        try
        {
            C_CANFrame.ReceiveCANFD();
            for (uint8_t i = 0; i < 5; i++)
            {
                if (C_CANFrame.FrameFd.can_id == CtoMDPS[i])
                {
                    /* MDPS 3 -> 5: WheelSpeed와 관련 있음.
                                    특정값 이상(5kph 이상 X) 올라가면 MDPS 3에서 고정 */
                    if (C_CANFrame.FrameFd.can_id == 0xA0)
                    {
                        int i_A0 = 0;
                        int MsgA0_index = 0;
                        int MsgA0_CRC = 0xFFFF;
                        if (AlvCnt3 > 255)
                        {
                            AlvCnt3 = 0;
                        }
                        C_CANFrame.FrameFd.data[2] = AlvCnt3;
                        C_CANFrame.FrameFd.data[8] = 0x20;
                        C_CANFrame.FrameFd.data[9] = 0x00;
                        C_CANFrame.FrameFd.data[10] = 0x20;
                        C_CANFrame.FrameFd.data[11] = 0x00;
                        C_CANFrame.FrameFd.data[12] = 0x20;
                        C_CANFrame.FrameFd.data[13] = 0x00;
                        C_CANFrame.FrameFd.data[14] = 0x20;
                        C_CANFrame.FrameFd.data[15] = 0x00;
                        for (i_A0 = 2; i_A0 < 24; i_A0++)
                        {
                            MsgA0_index = (uint8_t)((MsgA0_CRC >> 8) ^ C_CANFrame.FrameFd.data[i_A0]) & 0x00FF;
                            MsgA0_CRC = (MsgA0_CRC << 8) ^ crc16table[MsgA0_index];
                        }
                        MsgA0_CRC = (MsgA0_CRC << 8) ^ crc16table[(uint8_t)((MsgA0_CRC >> 8) ^ MsgA0_DataID_L) & 0x00FF];
                        MsgA0_CRC = (MsgA0_CRC << 8) ^ crc16table[(uint8_t)((MsgA0_CRC >> 8) ^ MsgA0_DataID_H) & 0x00FF];
                        C_CANFrame.FrameFd.data[0] = MsgA0_CRC & 0x00FF;
                        C_CANFrame.FrameFd.data[1] = (MsgA0_CRC & 0xFF00) >> 8;
                        AlvCnt3++;
                        // WheelSpeedFL = (((C_CANFrame.FrameFd.data[9] & 0x3F) << 8) + C_CANFrame.FrameFd.data[8]) * 0.03125;
                        // WheelSpeedFR = (((C_CANFrame.FrameFd.data[11] & 0x3F) << 8) + C_CANFrame.FrameFd.data[10]) * 0.03125;
                        // WheelSpeedRL = (((C_CANFrame.FrameFd.data[13] & 0x3F) << 8) + C_CANFrame.FrameFd.data[12]) * 0.03125;
                        // WheelSpeedRR = (((C_CANFrame.FrameFd.data[15] & 0x3F) << 8) + C_CANFrame.FrameFd.data[14]) * 0.03125;
                    }
                    MDPSFrame.FrameFd = C_CANFrame.FrameFd;
                    MDPSFrame.SendCANFD();
                }
            }
        }
        catch (std::out_of_range &e)
        {
            std::cout << "<CCANtoMDPS> Out_of_range Error" << '\n';
        }
        catch (std::length_error &e)
        {
            std::cout << "<CCANtoMDPS> Length Error" << '\n';
        }
        catch (std::exception &e)
        {
            std::cout << "<CCANtoMDPS> EXCEPTION " << '\n';
            std::cout << e.what() << '\n';
        }
    }
    std::cout << "[CCANtoMDPS] ------------------ C-CAN to MDPS Gateway END! ......" << endl;
}

void MDPStoCCAN()
{
    CANClass SPASFrame;
    static uint8_t Chkcnt; // 20ms Count
    static uint8_t MDPS7Cnt;
    std::cout << "[MDPStoCCAN] ------------------ MDPS to C-CAN Gateway START! ......" << endl;
    while (true)
    {
        try
        {
            MDPSFrame.ReceiveCANFD();
            for (uint8_t i = 0; i < 1; i++)
            {
                if (MDPSFrame.FrameFd.can_id == MDPStoC[i])
                {
                    C_CANFrame.FrameFd = MDPSFrame.FrameFd;
                    C_CANFrame.SendCANFD();
                }
            }
            if (MDPSFrame.FrameFd.can_id == 0xEA)
            {
                MDPSmode = MDPSFrame.FrameFd.data[5] & 0x0F;
                CurrentAnglVal = (((MDPSFrame.FrameFd.data[13] & 0xFF) << 8) + ((MDPSFrame.FrameFd.data[12]) & 0xFF)) * 0.1;
                if (CurrentAnglVal > 6000)
                {
                    CurrentAnglVal = CurrentAnglVal - 6553; // 1(L) 0(C) -6553(R)
                }
                Chkcnt++;
                if (Chkcnt == 2) // 20ms
                {
                    int i_165 = 0;
                    int Msg165_index = 0;
                    int Msg165_CRC = 0xFFFF;
                    if (AlvCnt2 > 255)
                    {
                        AlvCnt2 = 0;
                    }
                    SPASFrame.FrameFd.can_id = 0x165;
                    SPASFrame.FrameFd.len = 24;
                    SPASFrame.FrameFd.flags = 0;
                    SPASFrame.FrameFd.__res0 = 0;
                    SPASFrame.FrameFd.__res1 = 0;
                    SPASFrame.FrameFd.data[2] = AlvCnt2;
                    SPASFrame.FrameFd.data[3] = 0x48;
                    SPASFrame.FrameFd.data[4] = 0x00;
                    SPASFrame.FrameFd.data[5] = 0x00;
                    SPASFrame.FrameFd.data[6] = 0x00;
                    SPASFrame.FrameFd.data[7] = 0x00;
                    SPASFrame.FrameFd.data[8] = 0x00;
                    SPASFrame.FrameFd.data[9] = 0x00;
                    SPASFrame.FrameFd.data[10] = 0x00;
                    SPASFrame.FrameFd.data[14] = 0x02;
                    SPASFrame.FrameFd.data[15] = 0x00;
                    SPASFrame.FrameFd.data[16] = 0x00;
                    SPASFrame.FrameFd.data[17] = 0x00;
                    SPASFrame.FrameFd.data[18] = 0x00;
                    SPASFrame.FrameFd.data[19] = 0x00;
                    SPASFrame.FrameFd.data[20] = 0x00;
                    SPASFrame.FrameFd.data[21] = 0x00;
                    SPASFrame.FrameFd.data[22] = 0x00;
                    SPASFrame.FrameFd.data[23] = 0x00;
                    if (SCCbtn == 0)
                    {
                        SCC_OpSta = 0;
                        SCC_MainOnOffSta = 0;
                        if (MDPSmode == 0)
                        {
                            SPASFrame.FrameFd.data[11] = 0x04;   // PA_sta 1
                            PA_StrAnglReq = CurrentAnglVal * 10; // factor 0.1 → * 10
                            SPASFrame.FrameFd.data[12] = ((PA_StrAnglReq) & 0xFF);
                            SPASFrame.FrameFd.data[13] = ((PA_StrAnglReq >> 8) & 0xFF);
                        }
                        else if (MDPSmode == 1)
                        {
                            SPASFrame.FrameFd.data[11] = 0x08; // PA_sta 2
                            PA_StrAnglReq = CurrentAnglVal * 10;
                            SPASFrame.FrameFd.data[12] = ((PA_StrAnglReq) & 0xFF);
                            SPASFrame.FrameFd.data[13] = ((PA_StrAnglReq >> 8) & 0xFF);
                        }
                        else if (MDPSmode == 2)
                        {
                            SPASFrame.FrameFd.data[11] = 0x0C; // PA_sta 3
                            PA_StrAnglReq = CurrentAnglVal * 10;
                            SPASFrame.FrameFd.data[12] = ((PA_StrAnglReq) & 0xFF);
                            SPASFrame.FrameFd.data[13] = ((PA_StrAnglReq >> 8) & 0xFF);
                        }
                        else if (MDPSmode == 3)
                        {
                            SPASFrame.FrameFd.data[11] = 0x0C; // PA_sta 3
                            PA_StrAnglReq = CurrentAnglVal * 10;
                            SPASFrame.FrameFd.data[12] = ((PA_StrAnglReq) & 0xFF);
                            SPASFrame.FrameFd.data[13] = ((PA_StrAnglReq >> 8) & 0xFF);
                        }
                        else if (MDPSmode == 5) // off
                        {
                            SPASFrame.FrameFd.data[11] = 0x0C; // PA_sta 3
                            MDPSmodeCnt = 0;
                        }
                        else if (MDPSmode == 6)
                        {
                            // MDPS 6(제어가능 범위 초과): 10ms - 480dgr/s(4.8deg) 9.6deg 이상 차이나면 빠짐
                            SPASFrame.FrameFd.data[11] = 0x08; // PA_sta 2 → MDPS 3
                        }
                        else // MDPS 7
                        {
                            SPASFrame.FrameFd.data[11] = 0x18; // MDPS 7 → 6 → 3: MDPS 유지
                            MDPS7Cnt++;
                            if (MDPS7Cnt == 1)
                            {
                                SPASFrame.FrameFd.data[11] = 0x0C;
                                MDPS7Cnt = 0;
                            }
                        }
                    }
                    else if (SCCbtn == 1 && PRNDmode == 5)
                    {
                        if (MDPSmode == 3)
                        {
                            SPASFrame.FrameFd.data[11] = 0x10; // PA_sta 4
                            PA_StrAnglReq = CurrentAnglVal * 10;
                            SPASFrame.FrameFd.data[12] = ((PA_StrAnglReq) & 0xFF);
                            SPASFrame.FrameFd.data[13] = ((PA_StrAnglReq >> 8) & 0xFF);
                        }
                        else if (MDPSmode == 4)
                        {
                            SPASFrame.FrameFd.data[11] = 0x14; // PA_sta 5
                            PA_StrAnglReq = CurrentAnglVal * 10;
                            PrePA_StrAnglReq = PA_StrAnglReq; // 지면에 따라 StrAngl 값(L, R)이 변화하는 성향이 존재
                            SPASFrame.FrameFd.data[12] = ((PA_StrAnglReq) & 0xFF);
                            SPASFrame.FrameFd.data[13] = ((PA_StrAnglReq >> 8) & 0xFF);
                        }
                        else if (MDPSmode == 5)
                        {
                            SPASFrame.FrameFd.data[11] = 0x14; // PA_sta 5
                            PA_StrAnglReq = PrePA_StrAnglReq;  // 300ms 유지 → UDP data recv
                            SPASFrame.FrameFd.data[12] = ((PA_StrAnglReq) & 0xFF);
                            SPASFrame.FrameFd.data[13] = ((PA_StrAnglReq >> 8) & 0xFF);
                        }
                        else if (MDPSmode == 6)
                        {
                            SPASFrame.FrameFd.data[11] = 0x08;
                        }
                        else
                        {
                            SPASFrame.FrameFd.data[11] = 0x18;
                            MDPS7Cnt++;
                            if (MDPS7Cnt == 1)
                            {
                                SPASFrame.FrameFd.data[11] = 0x0C;
                                MDPS7Cnt = 0;
                            }
                        }
                    }
                    PAmode = (SPASFrame.FrameFd.data[11] >> 2);
                    for (i_165 = 2; i_165 < 24; i_165++)
                    {
                        Msg165_index = (uint8_t)((Msg165_CRC >> 8) ^ SPASFrame.FrameFd.data[i_165]) & 0x00FF;
                        Msg165_CRC = (Msg165_CRC << 8) ^ crc16table[Msg165_index];
                    }
                    Msg165_CRC = (Msg165_CRC << 8) ^ crc16table[(uint8_t)((Msg165_CRC >> 8) ^ Msg165_DataID_L) & 0x00FF];
                    Msg165_CRC = (Msg165_CRC << 8) ^ crc16table[(uint8_t)((Msg165_CRC >> 8) ^ Msg165_DataID_H) & 0x00FF];
                    SPASFrame.FrameFd.data[0] = Msg165_CRC & 0x00FF;
                    SPASFrame.FrameFd.data[1] = (Msg165_CRC & 0xFF00) >> 8;
                    MDPSFrame.FrameFd = SPASFrame.FrameFd;
                    MDPSFrame.SendCANFD();
                    AlvCnt2++;
                    Chkcnt = 0;
                }
            }
        }
        catch (std::out_of_range &e)
        {
            std::cout << "<MDPStoCCAN> Out_of_range Error" << '\n';
        }
        catch (std::length_error &e)
        {
            std::cout << "<MDPStoCCAN> Length Error" << '\n';
        }
        catch (std::exception &e)
        {
            std::cout << "<MDPStoCCAN> EXCEPTION " << '\n';
            std::cout << e.what() << '\n';
        }
    }
    std::cout << "[MDPStoCCAN] ------------------ MDPS to C-CAN Gateway END! ......" << endl;
}

void ECANtoDRV()
{
    static uint8_t SwCnt; // SCC btn "1" Count
    std::cout << "[ECANtoDRV] ------------------ E-Can to DRV Gateway START! ......" << endl;
    while (true)
    {
        try
        {
            E_CANFrame.ReceiveCANFD();
            for (uint8_t i = 0; i < 23; i++)
            {
                if (E_CANFrame.FrameFd.can_id == EtoDRV[i])
                {
                    DRVFrame.FrameFd = E_CANFrame.FrameFd;
                    DRVFrame.SendCANFD();
                }
            }
            if (E_CANFrame.FrameFd.can_id == 0x35) // PRND: P 0, D 5, N 6, R 7
            {
                PRNDmode = E_CANFrame.FrameFd.data[24] & 0x07;
            }
            if (E_CANFrame.FrameFd.can_id == 0x60) // Brake
            {
                /* 각 bit 형 변환 필수 */
                ESC_CylPrsrVal = (((uint16_t)(E_CANFrame.FrameFd.data[17] & 0x0F) << 8) + (uint16_t)(E_CANFrame.FrameFd.data[16] & 0xFF)) * 0.1;
                if (ESC_CylPrsrVal > 3) // Brake SCC off
                {
                    SCCbtn = 0;
                }
            }
            if (E_CANFrame.FrameFd.can_id == 0x1CF) // SCC button
            {
                SWRC_CrsMainSwSta = (E_CANFrame.FrameFd.data[2] >> 3) & 0x03;
                if (SWRC_CrsMainSwSta == 1)
                {
                    if (SCCbtn == 0)
                    {
                        SwCnt++;
                        if (SwCnt == 6)
                        {
                            SCCbtn = 1;
                            SwCnt = 0;
                        }
                    }
                    else if (SCCbtn == 1)
                    {
                        SwCnt++;
                        if (SwCnt == 6)
                        {
                            SCCbtn = 0;
                            SwCnt = 0;
                        }
                    }
                }
            }
        }
        catch (std::out_of_range &e)
        {
            std::cout << "<ECANtoDRV> Out_of_range Error" << '\n';
        }
        catch (std::length_error &e)
        {
            std::cout << "<ECANtoDRV> Length Error" << '\n';
        }
        catch (std::exception &e)
        {
            std::cout << "<ECANtoDRV> EXCEPTION " << '\n';
            std::cout << e.what() << '\n';
        }
    }
    std::cout << "[ECANtoDRV] ------------------ E-Can to DRV Gateway END! ......" << endl;
}

void DRVtoECAN()
{
    CANClass CMDFrame; // 0x1A0
    std::cout << "[DRVtoECAN] ------------------ DRV to E-CAN Gateway START! ......" << endl;
    while (true)
    {
        try
        {
            DRVFrame.ReceiveCANFD();
            for (uint8_t i = 0; i < 4; i++)
            {
                if (DRVFrame.FrameFd.can_id == DRVtoE[i])
                {
                    E_CANFrame.FrameFd = DRVFrame.FrameFd;
                    E_CANFrame.SendCANFD();
                }
            }
            if (DRVFrame.FrameFd.can_id == 0x1A0) // SCC Control
            {
                // Radar bypass
                CMDFrame.FrameFd.data[3] = DRVFrame.FrameFd.data[3]; // (SCC_ObjDstVal & 0xFF)
                CMDFrame.FrameFd.data[4] = DRVFrame.FrameFd.data[4]; // ((SCC_ObjRelSpdVal << 3) & 0xF8) + ((SCC_ObjDstVal >> 8) & 0x07);
                CMDFrame.FrameFd.data[5] = DRVFrame.FrameFd.data[5]; // ((SCC_ObjLatPosVal << 7) & 0x80) + ((SCC_ObjRelSpdVal >> 5) & 0x7F);
                CMDFrame.FrameFd.data[6] = DRVFrame.FrameFd.data[6]; // ((SCC_ObjLatPosVal >> 1) & 0xFF);
                if (SCCbtn == 0)                                     // SCC btn, Brake off
                {
                    DRVFrame.FrameFd.data[8] = 0x00; // Opsta, MainOnOffSta = 0: 급발진 방지
                    E_CANFrame.FrameFd = DRVFrame.FrameFd;
                    E_CANFrame.SendCANFD();
                }
                else if (SCCbtn == 1)
                {
                    SCC_OpSta = 1;
                    SCC_MainOnOffSta = 1;
                    SCC_JrkUpplimVal = 6;
                    SCC_JrkLwrLimVal = 5;
                    SCC_HeadwayDstSetVal = 2;
                    int i_1A0 = 0;
                    int Msg1A0_index = 0;
                    int Msg1A0_CRC = 0xFFFF;
                    if (AlvCnt > 255)
                    {
                        AlvCnt = 0;
                    }
                    CMDFrame.FrameFd.can_id = 0x1A0;
                    CMDFrame.FrameFd.len = 32;
                    CMDFrame.FrameFd.flags = 0;
                    CMDFrame.FrameFd.__res0 = 0;
                    CMDFrame.FrameFd.__res1 = 0;
                    CMDFrame.FrameFd.data[2] = AlvCnt;
                    // CMDFrame.FrameFd.data[3] = (SCC_ObjDstVal & 0xFF);
                    // CMDFrame.FrameFd.data[4] = ((SCC_ObjRelSpdVal << 3) & 0xF8) + ((SCC_ObjDstVal >> 8) & 0x07);
                    // CMDFrame.FrameFd.data[5] = ((SCC_ObjLatPosVal << 7) & 0x80) + ((SCC_ObjRelSpdVal >> 5) & 0x7F);
                    // CMDFrame.FrameFd.data[6] = ((SCC_ObjLatPosVal >> 1) & 0xFF);
                    CMDFrame.FrameFd.data[7] = 0x00;
                    CMDFrame.FrameFd.data[8] = ((SCC_OpSta << 4) & 0x70) + ((SCC_MainOnOffSta << 2) & 0x0C) + (SCC_SysFlrSta & 0x03);
                    CMDFrame.FrameFd.data[9] = ((SCC_DrvAlrtDis << 5) & 0x60) + ((SCC_InfoDis << 2) & 0x1C) + (SCC_TakeoverReq & 0x03);
                    CMDFrame.FrameFd.data[10] = SCC_ObjDstLvlVal;
                    CMDFrame.FrameFd.data[11] = ((SCC_NSCCAutoSetSpdSta << 6) & 0xC0) + (SCC_HeadwayDstSetVal & 0x0F);
                    CMDFrame.FrameFd.data[12] = SCC_TrgtSpdSetVal;
                    CMDFrame.FrameFd.data[13] = ((SCC_ObjSta << 4) & 70) + ((SCC_NSCCOnOffsta << 2) & 0x0C) + (SCC_NSCCOpsta & 0x03);
                    CMDFrame.FrameFd.data[14] = ((SCC_NSCCInfoPUDis << 5) & 0xE0) + ((SCC_NSCCAutoSetSpdUpdtsta << 3) & 0x18) + (SCC_SnstvtyModRetVal & 0x07);
                    CMDFrame.FrameFd.data[15] = 0x00;
                    CMDFrame.FrameFd.data[16] = (SCC_AccelReqVal & 0xFF);
                    CMDFrame.FrameFd.data[17] = ((SCC_AccelReqRawVal << 4) & 0xF0) + ((SCC_AccelReqVal >> 8) & 0x07);
                    CMDFrame.FrameFd.data[18] = ((SCC_AccelReqRawVal >> 4) & 0x7F);
                    CMDFrame.FrameFd.data[19] = (SCC_JrkUpplimVal & 0x7F);
                    CMDFrame.FrameFd.data[20] = (SCC_JrkLwrLimVal & 0x7F);
                    CMDFrame.FrameFd.data[21] = (SCC_AccelLimBandUppVal & 0x3F);
                    CMDFrame.FrameFd.data[22] = (SCC_AccelLimBandLwrVal & 0x3F);
                    CMDFrame.FrameFd.data[23] = ((SCC_DclModReq << 2) & 0x0C) + (SCC_VehStpReq & 0x03);
                    CMDFrame.FrameFd.data[24] = (SCC_TrgtDstVal & 0xFF);
                    CMDFrame.FrameFd.data[25] = ((SCC_TrgtDstVal >> 8) & 0x07);
                    CMDFrame.FrameFd.data[26] = 0x00;
                    CMDFrame.FrameFd.data[27] = 0x00;
                    CMDFrame.FrameFd.data[28] = 0x00;
                    CMDFrame.FrameFd.data[29] = 0x00;
                    CMDFrame.FrameFd.data[30] = 0x00;
                    CMDFrame.FrameFd.data[31] = 0x00;
                    for (i_1A0 = 2; i_1A0 < 32; i_1A0++)
                    {
                        Msg1A0_index = (uint8_t)((Msg1A0_CRC >> 8) ^ CMDFrame.FrameFd.data[i_1A0]) & 0x00FF;
                        Msg1A0_CRC = (Msg1A0_CRC << 8) ^ crc16table[Msg1A0_index];
                    }
                    Msg1A0_CRC = (Msg1A0_CRC << 8) ^ crc16table[(uint8_t)((Msg1A0_CRC >> 8) ^ Msg1A0_DataID_L) & 0x00FF];
                    Msg1A0_CRC = (Msg1A0_CRC << 8) ^ crc16table[(uint8_t)((Msg1A0_CRC >> 8) ^ Msg1A0_DataID_H) & 0x00FF];
                    CMDFrame.FrameFd.data[0] = Msg1A0_CRC & 0x00FF;
                    CMDFrame.FrameFd.data[1] = (Msg1A0_CRC & 0xFF00) >> 8;
                    E_CANFrame.FrameFd = CMDFrame.FrameFd;
                    E_CANFrame.SendCANFD();
                    AlvCnt++;
                }
            }
        }
        catch (std::out_of_range &e)
        {
            std::cout << "<DRVtoECAN> Out_of_range Error" << '\n';
        }
        catch (std::length_error &e)
        {
            std::cout << "<DRVtoECAN> Length Error" << '\n';
        }
        catch (std::exception &e)
        {
            std::cout << "<DRVtoECAN> EXCEPTION " << '\n';
            std::cout << e.what() << '\n';
        }
    }
    std::cout << "[DRVtoECAN] ------------------ DRV to E-CAN Gateway END! ......" << endl;
}

void UDPRecv()
{
    UDPClass S32G;
    static uint8_t S32GAlvCnt;
    S32G.SetSocket(S32GIp, S32GPort, 1); // 50ms
    std::cout << "[UDPRecv] ------------------ UDP Receive START! ......" << endl;
    while (true)
    {
        try
        {
            S32G.Receive(14);
            if (S32G.Buffer[0] == 0x04 && S32G.Buffer[1] == 0x09 && S32G.Buffer[2] == 0x01 && S32G.Buffer[3] == 0x02)
            {
                S32GAlvCnt = S32G.Buffer[4];
                SCC_TrgtSpdSetVal = S32G.Buffer[5];                                                                             // CLU Speed
                SCC_AccelReqVal = ((S32G.Buffer[9] << 24) + (S32G.Buffer[8] << 16) + (S32G.Buffer[7] << 8) + (S32G.Buffer[6])); // MDPS 5 Ax
                SCC_AccelReqRawVal = SCC_AccelReqVal;
                ReferAnglVal = ((int)(S32G.Buffer[13] << 24) + (int)(S32G.Buffer[12] << 16) + (int)(S32G.Buffer[11] << 8) + (int)(S32G.Buffer[10]));
                if (MDPSmode == 5 && MDPSmodeCnt <= 6) // MDPS 5일 때, StrAngle 300ms 유지
                {
                    PA_StrAnglReq = PrePA_StrAnglReq;
                    MDPSmodeCnt++; // SCC off → 0
                }
                else if (MDPSmode == 5 && MDPSmodeCnt > 6)
                {
                    AnglValDiffer = CurrentAnglVal - ReferAnglVal;
                    if (abs(AnglValDiffer) >= 200)
                    {
                        if (CurrentAnglVal > ReferAnglVal)
                        {
                            PrePA_StrAnglReq = PrePA_StrAnglReq - 100;
                        }
                        else if (CurrentAnglVal < ReferAnglVal)
                        {
                            PrePA_StrAnglReq = PrePA_StrAnglReq + 100;
                        }
                    }
                    else if (abs(AnglValDiffer) >= 150)
                    {
                        if (CurrentAnglVal > ReferAnglVal)
                        {
                            PrePA_StrAnglReq = PrePA_StrAnglReq - 80;
                        }
                        else if (CurrentAnglVal < ReferAnglVal)
                        {
                            PrePA_StrAnglReq = PrePA_StrAnglReq + 80;
                        }
                    }
                    else if (abs(AnglValDiffer) >= 100)
                    {
                        if (CurrentAnglVal > ReferAnglVal)
                        {
                            PrePA_StrAnglReq = PrePA_StrAnglReq - 65;
                        }
                        else if (CurrentAnglVal < ReferAnglVal)
                        {
                            PrePA_StrAnglReq = PrePA_StrAnglReq + 65;
                        }
                    }
                    else if (abs(AnglValDiffer) >= 50)
                    {
                        if (CurrentAnglVal > ReferAnglVal)
                        {
                            PrePA_StrAnglReq = PrePA_StrAnglReq - 50;
                        }
                        else if (CurrentAnglVal < ReferAnglVal)
                        {
                            PrePA_StrAnglReq = PrePA_StrAnglReq + 50;
                        }
                    }
                    else if (abs(AnglValDiffer) >= 30)
                    {
                        if (CurrentAnglVal > ReferAnglVal)
                        {
                            PrePA_StrAnglReq = PrePA_StrAnglReq - 25;
                        }
                        else if (CurrentAnglVal < ReferAnglVal)
                        {
                            PrePA_StrAnglReq = PrePA_StrAnglReq + 25;
                        }
                    }
                    else if (abs(AnglValDiffer) >= 20)
                    {
                        if (CurrentAnglVal > ReferAnglVal)
                        {
                            PrePA_StrAnglReq = PrePA_StrAnglReq - 15;
                        }
                        else if (CurrentAnglVal < ReferAnglVal)
                        {
                            PrePA_StrAnglReq = PrePA_StrAnglReq + 15;
                        }
                    }
                    else if (abs(AnglValDiffer) >= 10)
                    {
                        if (CurrentAnglVal > ReferAnglVal)
                        {
                            PrePA_StrAnglReq = PrePA_StrAnglReq - 10;
                        }
                        else if (CurrentAnglVal < ReferAnglVal)
                        {
                            PrePA_StrAnglReq = PrePA_StrAnglReq + 10;
                        }
                    }
                    else if (abs(AnglValDiffer) >= 5)
                    {
                        if (CurrentAnglVal > ReferAnglVal)
                        {
                            PrePA_StrAnglReq = PrePA_StrAnglReq - 5;
                        }
                        else if (CurrentAnglVal < ReferAnglVal)
                        {
                            PrePA_StrAnglReq = PrePA_StrAnglReq + 5;
                        }
                    }
                    else if (abs(AnglValDiffer) >= 3)
                    {
                        if (CurrentAnglVal > ReferAnglVal)
                        {
                            PrePA_StrAnglReq = PrePA_StrAnglReq - 2;
                        }
                        else if (CurrentAnglVal < ReferAnglVal)
                        {
                            PrePA_StrAnglReq = PrePA_StrAnglReq + 2;
                        }
                    }
                    else if (abs(AnglValDiffer) >= 1)
                    {
                        if (CurrentAnglVal > ReferAnglVal)
                        {
                            PrePA_StrAnglReq = PrePA_StrAnglReq - 1;
                        }
                        else if (CurrentAnglVal < ReferAnglVal)
                        {
                            PrePA_StrAnglReq = PrePA_StrAnglReq + 1;
                        }
                    }
                    else if (abs(AnglValDiffer) == 0)
                    {
                        PrePA_StrAnglReq = PrePA_StrAnglReq;
                    }
                }
                if (S32GAlvCnt >= 255)
                {
                    S32GAlvCnt = 0;
                }
            }
        }
        catch (std::out_of_range &e)
        {
            std::cout << "<UDPRecv> Out_of_range Error" << '\n';
        }
        catch (std::length_error &e)
        {
            std::cout << "<UDPRecv> Length Error" << '\n';
        }
        catch (std::exception &e)
        {
            std::cout << "<UDPRecv> EXCEPTION " << '\n';
            std::cout << e.what() << '\n';
        }
    }
    S32G.CloseSocket();
    std::cout << "[UDPRecv] ------------------ UDP Receive END! ......" << endl;
}

int main(int argc, const char *argv[])
{
    (void)(argc); // 메인함수에 전달되는 정보의 갯수
    (void)(argv); // 메인함수에 전달되는 실질적인 정보로, 문자열의 배열

    MDPSFrame.SetSocket("can0", 1);
    C_CANFrame.SetSocket("can1", 1);
    DRVFrame.SetSocket("llcecan0", 1);
    E_CANFrame.SetSocket("llcecan1", 1);
    thread UDPThread, MDPStoCThread, CtoMDPSThread, DRVtoEThread, EtoDRVThread;
    UDPThread = thread(UDPRecv);
    CtoMDPSThread = thread(CCANtoMDPS);
    MDPStoCThread = thread(MDPStoCCAN);
    DRVtoEThread = thread(DRVtoECAN);
    EtoDRVThread = thread(ECANtoDRV);
    /* printf 100ms */
    using namespace std::chrono;
    steady_clock::time_point startTime = steady_clock::now();
    while (true)
    {
        auto currentTime = steady_clock::now();
        auto elapsedTime = duration_cast<milliseconds>(currentTime - startTime);
        if (elapsedTime >= milliseconds(100))
        {
            startTime = currentTime;
            elapsedTime = milliseconds(0);
            printf("SCC: %d||OpSta %d||MainSta %d||MDPS: %d||PA: %d||PRND: %d||Brake: %.lf||Ax: %d %d||Handle: %d %d %d\n", SCCbtn,
                   SCC_OpSta, SCC_MainOnOffSta, MDPSmode, PAmode, PRNDmode, ESC_CylPrsrVal, SCC_AccelReqRawVal, SCC_AccelReqVal, CurrentAnglVal,
                   ReferAnglVal, PA_StrAnglReq);
        }
    }
    UDPThread.join();
    CtoMDPSThread.join();
    MDPStoCThread.join();
    EtoDRVThread.join();
    DRVtoEThread.join();
    return 0;
}

int getch(void)
{
    int ch;
    struct termios buf;
    struct termios save;

    tcgetattr(0, &save);
    buf = save;
    buf.c_lflag &= ~(ICANON | ECHO);
    buf.c_cc[VMIN] = 1;
    buf.c_cc[VTIME] = 0;
    tcsetattr(0, TCSAFLUSH, &buf);
    ch = getchar();
    tcsetattr(0, TCSAFLUSH, &save);
    return ch;
}

void key()
{
    try
    {
        char key;
        for (; !(key == '\n');)
        {
            key = getch();

            if (key == '1')
            {
                // break;
            }
            else if (key == '2')
            {
                // break;
            }
        }
    }
    catch (std::out_of_range &e)
    {
        std::cout << "<Key> Out_of_range Error" << '\n';
    }
    catch (std::length_error &e)
    {
        std::cout << "<Key> Length Error" << '\n';
    }
    catch (std::exception &e)
    {
        std::cout << "<Key> EXCEPTION " << '\n';
        std::cout << e.what() << '\n';
    }
}

// ------------------------------- class Function ------------------------------------------- //
void CANClass::SetSocket(const std::string &ifname, const int canfd)
{
    // PF_CAN: CAN Protocol Family || SOCK_RAW: Socket의 유형, 사용자가 프로토콜에 직접 접근하여 데이터를 처리 || CAN_RAW: CAN Protocol 사용하는데 RAW 소켓을 생성
    if ((sock = socket(PF_CAN, SOCK_RAW, CAN_RAW)) == -1)
        perror("<CAN> socket open error");

    strcpy(ifr.ifr_name, ifname.c_str());
    if (ioctl(sock, SIOCGIFINDEX, &ifr) < 0)
    {
        perror("<CAN> Error with SIOCGIFINDEX ioctl");
        close(sock);
        exit(EXIT_FAILURE);
    }

    memset(&addr, 0, sizeof(addr));
    addr.can_family = AF_CAN; // AF_CAM: CAN Address Family(주소체계)
    addr.can_ifindex = ifr.ifr_ifindex;

    if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("<CAN> Error in socket bind");
        close(sock);
        exit(EXIT_FAILURE);
    }

    if (canfd)
    {
        // SOL_CAN_RAW: CAN Protocol에서 RAW 소켓에 대한 소켓 레벨의 옵션을 설정 || CAN_RAW_FD_FRAMES: CAN FD 프레임 속성
        if (setsockopt(sock, SOL_CAN_RAW, CAN_RAW_FD_FRAMES, &canfd, sizeof(canfd)))
        {
            perror("<CAN> Error enabling CAN FD support");
            close(sock);
            exit(EXIT_FAILURE);
        }
    }
}

void CANClass::ReceiveCANFD()
{
    addrlen = sizeof(addr);
    nbytes = recvfrom(sock, &FrameFd, sizeof(FrameFd), 0, (struct sockaddr *)&addr, &addrlen);
    if (nbytes < 0)
        perror("<CANFD> Read Error");
}

void CANClass::ReceiveCAN()
{
    addrlen = sizeof(addr);
    nbytes = recvfrom(sock, &Frame, sizeof(Frame), 0, (struct sockaddr *)&addr, &addrlen);
    if (nbytes < 0)
        perror("<CAN> Read Error");
}

void CANClass::SendCANFD()
{
    // FramFd : id, length, data[64] --> 20ms마다 : stop하면 다시 원래값으로 변경
    if (write(sock, &FrameFd, sizeof(FrameFd)) != sizeof(struct canfd_frame))
        perror("<CANFD> Send Error");
}

void CANClass::SendCAN()
{
    if (write(sock, &Frame, sizeof(Frame)) != sizeof(struct can_frame))
        perror("<CANFD> Send Error");
}

void CANClass::InitFrame()
{
    Frame.can_id = 0x00;
    Frame.can_dlc = 8;
    for (uint8_t i = 0; i < Frame.can_dlc; i++)
    {
        Frame.data[i] = 0x00;
    }
}

void CANClass::CloseSocket()
{
    close(sock);
}

void UDPClass::SetSocket(const std::string &ip, const int port, const bool BindFlag)
{
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
        perror("<UDP> socket Open Error");

    memset(&Addr, 0x00, sizeof(Addr));
    Addr.sin_family = AF_INET;
    Addr.sin_addr.s_addr = inet_addr(ip.c_str());
    Addr.sin_port = htons(port);

    if (BindFlag)
    {
        if (bind(sock, (struct sockaddr *)&Addr, sizeof(Addr)) < 0)
            perror("<UDP> bind Error");
    }
}

void UDPClass::Receive(const uint16_t buffersize)
{
    addrlen = sizeof(Addr);
    if ((nbytes = recvfrom(sock, Buffer, buffersize, 0, (struct sockaddr *)&Addr, &addrlen)) < 0)
        perror("<UDP> Receive Error");
}

void UDPClass::Send(const uint16_t SendByte)
{
    if (sendto(sock, Buffer, SendByte, 0, (struct sockaddr *)&Addr, sizeof(Addr)) < 0)
        perror("<UDP> Send Error");
}

void UDPClass::CloseSocket()
{
    close(sock);
}