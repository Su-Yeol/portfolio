/* V2 : 일반 및 AEB 모드 + key 입력 */
#include <time.h>
#include <pthread.h>
#include "Communicator.h"

#define TimeCycle 20

using namespace std; // Standard로써 iostream 내부에 입출력에 관한 함수들을 가지고 있는 네임스페이스

/* ------------------------------- AEB Control Flag ------------------------------- */
CANClass VehicleCANFD;

uint8_t AliveCnt = 0; // Alive Count
uint8_t CRCDataHigh = 0;
uint8_t CRCDataLow = 0;

uint8_t AEBStopAliveCnt = 0; // Stop -> Active counting

bool AEBComFlag = true;
bool AEBStopFlag = false;

uint8_t FCA_WrngLvlSta = 0; // 0 -> 2
uint8_t FCA_SysFlrSta = 0;
uint8_t FCA_OnOffEquipSta = 3;
// (0xC0&(FCA_WrngLvlSta<<6))+ (0x38&(FCA_SysFlrSta<<3)) + (0x07&(FCA_OnOffEquipSta))

uint8_t FCA_HydrlcBstAsstlSta = 0;
uint8_t FCA_StbltActvReq = 0;
uint8_t FCA_VehStpReq = 0;
// (0x60&(FCA_HydrlcBstAsstlSta<<5)) + (0x1C&(FCA_StbltActvReq<<2)) + (0x03&(FCA_VehStpReq))

uint8_t FCA_DclReqVal = 0; // 0 -> 4[g]

uint8_t FCA_FullActvReq = 0;
uint8_t FCA_PartialActvReq = 0; // 0 -> 1
uint8_t FCA_PrefillActvReq = 0;
// (0x30&(FCA_FullActvReq<<4)) + (0x0C&(FCA_PartialActvReq<<2)) + (0x03&(FCA_PrefillActvReq))

uint8_t FCA_WrngSndSta = 0; // 0 -> 2
uint8_t FCA_SnstvtyModRetVal = 2;
// (0xE0&(FCA_WrngSndSta<<5)) + (0x07&(FCA_SnstvtyModRetVal))

uint16_t FCA_RelVel = 0xFF; // FF > 117
uint8_t FCA_TimetoCllsn = 0xFE; // FE > 57
// (0xFF&(FCA_RelVel))
// (0xFE&(FCA_TimetoCllsn<<1)) + (0x01&(FCA_RelVel>>8))

uint8_t Nmode_FCAOff_Sta = 0;
uint8_t FCA_Jnctn_OnOffEquipSta = 2;
// (0x18&(FCA_Jnctn_OnOffEquipSta<<3)) + (0x06&(Nmode_FCAOff_Sta<<1)) + (0x01&(FCA_TimetoCllsn<<8))

uint8_t ADAS_DRV_FCA_Plus_Sta = 0;
uint8_t FCA_WrngTrgtDis = 0; // 0 -> 5
// (0x60&(ADAS_DRV_FCA_Plus_Sta<<5)) + (0x1F&(FCA_WrngTrgtDis))

/* ------------------------------- CRC Calculate ------------------------------- */
uint8_t Msg160_DataID_H = 0xF9;
uint8_t Msg160_DataID_L = 0x60;
uint32_t Msg1A0_CRC = 0xFFFF;

int Msg1A0_index = 0; // Msg1A0_index

uint32_t CRCTable[256] = {
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

/* ------------------------------- Key Function ------------------------------- */
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

void Key()
{
    try
    {
        char key;
        for (; !(key == '\n');)
        {
            key = getch(); // 사용자의 키보드 입력을 받아와서 저장

            if (key == '1')
            {
                AEBComFlag = true;
                AEBStopFlag = false;
                //break;
            }
            else if (key == '0')
            {
                AEBComFlag = false;
                AEBStopFlag = true;
                //break;
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

/* ------------------------------- AEB Function ------------------------------- */
void AEBComControl() // Common Situation Control
{
    // cout << "[Communicator]-----------Start-------------" << endl;
    AliveCnt++;

    FCA_WrngLvlSta = 0; FCA_WrngSndSta = 0; FCA_WrngTrgtDis = 0;
    FCA_RelVel = 0xFF; FCA_TimetoCllsn = 0xFE;
    FCA_PartialActvReq = 0; FCA_FullActvReq = 0; FCA_DclReqVal = 0;

    /* Standard Data */
    VehicleCANFD.FrameFd.can_id = 0x160; // ADAS_CMD_10_20ms
    VehicleCANFD.FrameFd.len = 16;
    VehicleCANFD.FrameFd.flags = 0;
    VehicleCANFD.FrameFd.__res0 = 0;
    VehicleCANFD.FrameFd.__res1 = 0;

    VehicleCANFD.FrameFd.data[0] = CRCDataHigh; // CRC High byte
    VehicleCANFD.FrameFd.data[1] = CRCDataLow;  // CRC Low byte
    VehicleCANFD.FrameFd.data[2] = AliveCnt;    // Alive count
    VehicleCANFD.FrameFd.data[3] = (0xC0&(FCA_WrngLvlSta<<6))+ (0x38&(FCA_SysFlrSta<<3)) + (0x07&(FCA_OnOffEquipSta));
    VehicleCANFD.FrameFd.data[4] = (0x60&(FCA_HydrlcBstAsstlSta<<5)) + (0x1C&(FCA_StbltActvReq<<2)) + (0x03&(FCA_VehStpReq));
    VehicleCANFD.FrameFd.data[5] = FCA_DclReqVal;
    VehicleCANFD.FrameFd.data[6] = (0x30&(FCA_FullActvReq<<4)) + (0x0C&(FCA_PartialActvReq<<2)) + (0x03&(FCA_PrefillActvReq));
    VehicleCANFD.FrameFd.data[7] = (0xE0&(FCA_WrngSndSta<<5)) + (0x07&(FCA_SnstvtyModRetVal));
    VehicleCANFD.FrameFd.data[8] = (0xFF&(FCA_RelVel));
    VehicleCANFD.FrameFd.data[9] = (0xFE&(FCA_TimetoCllsn<<1)) + (0x01&(FCA_RelVel>>8));
    VehicleCANFD.FrameFd.data[10] = (0x18&(FCA_Jnctn_OnOffEquipSta<<3)) + (0x06&(Nmode_FCAOff_Sta<<1)) + 0x01&(FCA_TimetoCllsn>>7);
    VehicleCANFD.FrameFd.data[11] = (0x60&(ADAS_DRV_FCA_Plus_Sta<<5))+ (0x1F&(FCA_WrngTrgtDis));
    VehicleCANFD.FrameFd.data[12] = 0x00;
    VehicleCANFD.FrameFd.data[13] = 0x00;
    VehicleCANFD.FrameFd.data[14] = 0x00;
    VehicleCANFD.FrameFd.data[15] = 0x00;

    /* CRC Calculate */
    for (int i_1A0 = 2; i_1A0 < 16; i_1A0++)
    {
        Msg1A0_index = (unsigned char)((Msg1A0_CRC >> 8) ^ VehicleCANFD.FrameFd.data[i_1A0]) & 0x00FF;
        Msg1A0_CRC = (Msg1A0_CRC << 8) ^ CRCTable[Msg1A0_index];
    }
    Msg1A0_CRC = (Msg1A0_CRC << 8) ^ CRCTable[(unsigned char)((Msg1A0_CRC >> 8) ^ Msg160_DataID_L) & 0x00FF];
    Msg1A0_CRC = (Msg1A0_CRC << 8) ^ CRCTable[(unsigned char)((Msg1A0_CRC >> 8) ^ Msg160_DataID_H) & 0x00FF];

    VehicleCANFD.FrameFd.data[0] = Msg1A0_CRC & 0x00FF;
    VehicleCANFD.FrameFd.data[1] = (Msg1A0_CRC & 0xFF00) >> 8;
    Msg1A0_CRC = 0xFFFF;

    // cout << "[Communicator]-----------Send-------------" << endl;
    VehicleCANFD.SendCANFD();
}

void AEBStopControl() // FCA Control
{
    AliveCnt++;
    
    FCA_WrngLvlSta = 2; FCA_WrngSndSta = 2; FCA_WrngTrgtDis = 5;
    FCA_RelVel = 0x117; FCA_TimetoCllsn = 0x57;
    // 0.4g: 1, 0, 40 || 0.6g: 0, 1, 60
    FCA_PartialActvReq = 0; FCA_FullActvReq = 1; FCA_DclReqVal = 60;

    /* Standard Data */
    VehicleCANFD.FrameFd.can_id = 0x160;
    VehicleCANFD.FrameFd.len = 16;
    VehicleCANFD.FrameFd.flags = 0;
    VehicleCANFD.FrameFd.__res0 = 0;
    VehicleCANFD.FrameFd.__res1 = 0;

    VehicleCANFD.FrameFd.data[0] = CRCDataHigh; // CRC High byte
    VehicleCANFD.FrameFd.data[1] = CRCDataLow;  // CRC Low byte
    VehicleCANFD.FrameFd.data[2] = AliveCnt;      // Alive count
    VehicleCANFD.FrameFd.data[3] = (0xC0&(FCA_WrngLvlSta<<6))+ (0x38&(FCA_SysFlrSta<<3)) + (0x07&(FCA_OnOffEquipSta));
    VehicleCANFD.FrameFd.data[4] = (0x60&(FCA_HydrlcBstAsstlSta<<5)) + (0x1C&(FCA_StbltActvReq<<2)) + (0x03&(FCA_VehStpReq));
    VehicleCANFD.FrameFd.data[5] = FCA_DclReqVal;
    VehicleCANFD.FrameFd.data[6] = (0x30&(FCA_FullActvReq<<4)) + (0x0C&(FCA_PartialActvReq<<2)) + (0x03&(FCA_PrefillActvReq));
    VehicleCANFD.FrameFd.data[7] = (0xE0&(FCA_WrngSndSta<<5)) + (0x07&(FCA_SnstvtyModRetVal));
    VehicleCANFD.FrameFd.data[8] = (0xFF&(FCA_RelVel));
    VehicleCANFD.FrameFd.data[9] = (0xFE&(FCA_TimetoCllsn<<1)) + (0x01&(FCA_RelVel>>8));
    VehicleCANFD.FrameFd.data[10] = (0x18&(FCA_Jnctn_OnOffEquipSta<<3)) + (0x06&(Nmode_FCAOff_Sta<<1)) + 0x01&(FCA_TimetoCllsn>>7);
    VehicleCANFD.FrameFd.data[11] = (0x60&(ADAS_DRV_FCA_Plus_Sta<<5))+ (0x1F&(FCA_WrngTrgtDis));
    VehicleCANFD.FrameFd.data[12] = 0x00;
    VehicleCANFD.FrameFd.data[13] = 0x00;
    VehicleCANFD.FrameFd.data[14] = 0x00;
    VehicleCANFD.FrameFd.data[15] = 0x00;

    /* CRC Calculate */
    for (int i_1A0 = 2; i_1A0 < 16; i_1A0++)
    {
        Msg1A0_index = (unsigned char)((Msg1A0_CRC >> 8) ^ VehicleCANFD.FrameFd.data[i_1A0]) & 0x00FF;
        Msg1A0_CRC = (Msg1A0_CRC << 8) ^ CRCTable[Msg1A0_index];
    }
    Msg1A0_CRC = (Msg1A0_CRC << 8) ^ CRCTable[(unsigned char)((Msg1A0_CRC >> 8) ^ Msg160_DataID_L) & 0x00FF];
    Msg1A0_CRC = (Msg1A0_CRC << 8) ^ CRCTable[(unsigned char)((Msg1A0_CRC >> 8) ^ Msg160_DataID_H) & 0x00FF];

    VehicleCANFD.FrameFd.data[0] = Msg1A0_CRC & 0x00FF;
    VehicleCANFD.FrameFd.data[1] = (Msg1A0_CRC & 0xFF00) >> 8;
    Msg1A0_CRC = 0xFFFF;

    // Stop -> Active
    AEBStopAliveCnt++;
    std::cout << "[Communicator]-----------" << (int)AEBStopAliveCnt << "-------------" << endl;

    if (AEBStopAliveCnt >= 100) // 50 = 1s
    {
        AEBComFlag = true;
        AEBStopFlag = false;
        AEBStopAliveCnt = 0;
    }

    VehicleCANFD.SendCANFD();
}

/* ------------------------------- Main ------------------------------- */
int main() // 50ms
{
    thread KeyThread = thread(Key);

    // 20ms time
    struct timeval startTime, endTime;
    gettimeofday(&startTime, NULL);
    // Socket open
    VehicleCANFD.SetSocket("can0", 1);
    std::cout << "[Communicator]-----------Set Socekt-------------" << endl;

    while (true)
    {
        gettimeofday(&endTime, NULL);
        double timeGap = ((endTime.tv_sec - startTime.tv_sec) * 1000) + ((endTime.tv_usec - startTime.tv_usec) / 1000); // 1ms

        if (timeGap >= TimeCycle) // 20ms Alive count
        {
            if (AEBComFlag)
            {
                std::cout << "[Communicator]----------- Common Mode -------------" << endl;
                AEBComControl();
            }

            else if (AEBStopFlag)
            {
                std::cout << "[Communicator]----------- Stop Mode -------------" << endl;
                AEBStopControl();
            }

            gettimeofday(&startTime, NULL); // time init
        }
    }

    KeyThread.join();
}
