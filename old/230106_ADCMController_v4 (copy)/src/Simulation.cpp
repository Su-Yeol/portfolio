#include <iostream>
#include <thread>
#include <sys/time.h>
#include <math.h>
#include <exception> // 예외처리

#include "Simulation.h"
#include "UDPSocket.h"

int simul_cnt = 0;
bool set_pos = 0;

bool Accel = 0, Brake = 0, AUTOMODE = 0, PATHreq = 0, Seatbelt = 0; 
bool TurnSigLeft = 0, TurnSigRight = 0, Hazard_Light = 0;
uint8_t CMD1 = 0, CMD2 = 0, GearState = 0;
uint8_t Velocity = 10, FrontRadar = 30;
uint16_t RPM = 0;
double CurrTime = 0.0;

void SIMULATION(void)
{
    struct timeval t1, t2;
    struct timeval t3, t4;
    gettimeofday(&t1, NULL);
    gettimeofday(&t3, NULL);

    UDPSocket ADCM2TC;
    uint8_t TeleConsTxDataFrame[100] = {0,};
    bool enable_broadcast = false;
    ADCM2TC.setDestination("192.168.100.70", 2027, enable_broadcast);

    while (1)
    {
        try{
            gettimeofday(&t2, NULL);
            double diffTime_ms = (t2.tv_sec - t1.tv_sec) * 1000 + ((t2.tv_usec - t1.tv_usec) / 1000);
            if (diffTime_ms >= SIMUL_CYCLE_MS)
            {
                gettimeofday(&t1, NULL);

                if(!set_pos){
                    global_azimuth = REF_Azi[simul_cnt] * -1 + 90;
                    // global_azimuth = REF_Azi[simul_cnt];
                    global_latitude = REF_Lat[simul_cnt];
                    global_longitude = REF_Lon[simul_cnt];

                    simul_cnt++;
                }

                // KCITY Start Point
                // global_latitude = 37.23920546;
                // global_longitude = 126.77314629;
                // global_azimuth = 24 * -1 + 90;
            }

            gettimeofday(&t4, NULL);
            double tcudp_ms = (t4.tv_sec - t3.tv_sec) * 1000 + ((t4.tv_usec - t3.tv_usec) / 1000);
            if (tcudp_ms >= 100){
                gettimeofday(&t3, NULL);
                CurrTime += 1;


                TeleConsTxDataFrame[0] = 0x00;
                TeleConsTxDataFrame[1] = 0x00;
                TeleConsTxDataFrame[2] = 0x04;
                TeleConsTxDataFrame[3] = 0x0F;

                TeleConsTxDataFrame[4] = (0 & 0xFF);
                TeleConsTxDataFrame[5] = 0xFF;
                TeleConsTxDataFrame[6] = (Accel << 7) | (Brake << 6) | (GearState << 4) | (AUTOMODE << 3) | (PATHreq << 2) | (Seatbelt << 1);
                TeleConsTxDataFrame[7] = 'R';
                TeleConsTxDataFrame[8] = (uint32_t)(CurrTime*100) >> 24 & 0xFF;
                TeleConsTxDataFrame[9] = (uint32_t)(CurrTime*100) >> 16 & 0xFF;
                TeleConsTxDataFrame[10] = (uint32_t)(CurrTime*100) >> 8 & 0xFF;
                TeleConsTxDataFrame[11] = (uint32_t)(CurrTime*100) & 0xFF;
                TeleConsTxDataFrame[12] = (uint32_t)(global_latitude*10000000) >> 24 & 0xFF;
                TeleConsTxDataFrame[13] = (uint32_t)(global_latitude*10000000) >> 16 & 0xFF;
                TeleConsTxDataFrame[14] = (uint32_t)(global_latitude*10000000) >> 8 & 0xFF;
                TeleConsTxDataFrame[15] = (uint32_t)(global_latitude*10000000) & 0xFF;
                TeleConsTxDataFrame[16] = (uint32_t)(global_longitude*10000000) >> 24 & 0xFF;
                TeleConsTxDataFrame[17] = (uint32_t)(global_longitude*10000000) >> 16 & 0xFF;
                TeleConsTxDataFrame[18] = (uint32_t)(global_longitude*10000000) >> 8 & 0xFF;
                TeleConsTxDataFrame[19] = (uint32_t)(global_longitude*10000000) & 0xFF;
                TeleConsTxDataFrame[20] = (uint32_t)(global_azimuth*10000000) >> 24 & 0xFF;
                TeleConsTxDataFrame[21] = (uint32_t)(global_azimuth*10000000) >> 16 & 0xFF;
                TeleConsTxDataFrame[22] = (uint32_t)(global_azimuth*10000000) >> 8 & 0xFF;
                TeleConsTxDataFrame[23] = (uint32_t)(global_azimuth*10000000) & 0xFF;
                TeleConsTxDataFrame[24] = Velocity & 0xFF;

                TeleConsTxDataFrame[25] = RPM & 0xFF;
                TeleConsTxDataFrame[26] = RPM >> 8 & 0xFF;
                TeleConsTxDataFrame[27] = Hazard_Light & 0xFF;

                TeleConsTxDataFrame[28] = 0;
                TeleConsTxDataFrame[29] = 0;
                TeleConsTxDataFrame[30] = 0;
                TeleConsTxDataFrame[31] = 0;
                TeleConsTxDataFrame[32] = 0;
                TeleConsTxDataFrame[33] = 0;
                TeleConsTxDataFrame[34] = 0;
                TeleConsTxDataFrame[35] = 0;

                TeleConsTxDataFrame[36] = FrontRadar >> 24 & 0xFF;
                TeleConsTxDataFrame[37] = FrontRadar >> 16 & 0xFF;
                TeleConsTxDataFrame[38] = FrontRadar >> 8 & 0xFF;
                TeleConsTxDataFrame[39] = FrontRadar & 0xFF;
                TeleConsTxDataFrame[40] = 0;
                TeleConsTxDataFrame[41] = 0;
                TeleConsTxDataFrame[42] = 0;
                TeleConsTxDataFrame[43] = 0;

                TeleConsTxDataFrame[44] = 0;
                TeleConsTxDataFrame[45] = 0;
                TeleConsTxDataFrame[46] = 0;
                TeleConsTxDataFrame[47] = 0;
                TeleConsTxDataFrame[48] = 0;
                TeleConsTxDataFrame[49] = 0;
                TeleConsTxDataFrame[50] = 0;
                TeleConsTxDataFrame[51] = 0;

                TeleConsTxDataFrame[52] = CMD1;
                TeleConsTxDataFrame[53] = CMD2;

                TeleConsTxDataFrame[54] = (TurnSigLeft<<4)+TurnSigRight;
                TeleConsTxDataFrame[55] = 0x11; // (LeftAllowed<<4)+RightAllowed;
                TeleConsTxDataFrame[56] = 0x01; // moveflg (r: 0, g: 1)

                ADCM2TC.send(TeleConsTxDataFrame, 57); // TX_BUF_SIZE = 4096 체크.
            }
        }
        catch (std::out_of_range& e) {
            std::cout << "<SIMULATION> Out_of_range Error" << '\n';
        }
        catch (std::length_error& e) {
            std::cout << "<SIMULATION> Length Error" << '\n';
        }
        catch(std::exception& e){
            std::cout << "<SIMULATION> EXCEPTION " << '\n';
            std::cout << e.what() << '\n';
        }
    }

    return;
}