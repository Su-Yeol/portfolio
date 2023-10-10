#include "PCommunicator.h"
#include "PControlModule.h"

/* Struct */
GPSStruct GPS;

/* Loop Flag & Signal */
bool MainFlag = true;
bool SocketFlag = true;

// 30 ~ 130 byte low
/* ------------------------------- Main ------------------------------- */
int main(int argc, const char *argv[])
{
    (void)(argc); // 메인함수에 전달되는 정보의 갯수
    (void)(argv); // 메인함수에 전달되는 실질적인 정보로, 문자열의 배열

    thread KeyThread, GPSThread;

    struct timeval startTime, endTime;
    uint16_t TimeGap;

    /* GPS Path Save File */
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    char TimeBuffer[50], format[5] = ".txt";
    char GPSPath[100];

    // Path name
    sprintf(TimeBuffer, "%02d.%02d.%02d-%02d:%02d:%02d", tm.tm_year % 100, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
    sprintf(GPSPath, "%s%s.txt", GPSRecordPath.c_str(), TimeBuffer);

    FILE *GPSFile;
    if (GPSRecord)
    {
        GPSFile = fopen(GPSPath, "w");
        if (GPSFile == NULL)
        {
            printf("<Error Opening File>\n");
            return 1;
        }
    }

    /* thread of Communication */
    KeyThread = thread(Key);
    GPSThread = thread(GPSParser);

    gettimeofday(&startTime, NULL);
    while (GPS.Time == 0)
    {
        gettimeofday(&endTime, NULL);
        TimeGap = (endTime.tv_sec - startTime.tv_sec) * 1000 + ((endTime.tv_usec - startTime.tv_usec) / 1000); // [ms]
        if (TimeGap > 500)
        {
            std::cout << "[JudgmentModule] ------------------ GPSData updating ...... GPSTime : " << (int)GPS.Time << endl;
            gettimeofday(&startTime, NULL);
        }
    }
    std::cout << "[JudgmentModule] ------------------ GPSData update success! " << endl;

    std::cout << "[JudgmentModule] ------------------ JudgmentModule START! ------------------" << endl;
    gettimeofday(&startTime, NULL);
    while (MainFlag)
    {
        try
        {
            gettimeofday(&endTime, NULL);
            TimeGap = (endTime.tv_sec - startTime.tv_sec) * 1000 + ((endTime.tv_usec - startTime.tv_usec) / 1000); // [ms]
            if (TimeGap >= MainCycle)
            {
                if (GPSRecord)
                {
                    printf("GPS.Latitude %.7f | GPS.Longitude %.7f\n", GPS.Latitude, GPS.Longitude);
                    fprintf(GPSFile, "%.7f/%.7f\n", GPS.Latitude, GPS.Longitude);
                }
                gettimeofday(&startTime, NULL);
            }
        }

        catch (out_of_range &e)
        {
            std::cout << "<MAIN> Out_of_range Error" << '\n';
        }
        catch (length_error &e)
        {
            std::cout << "<MAIN> Length Error" << '\n';
        }
        catch (exception &e)
        {
            std::cout << "<MAIN> EXCEPTION " << '\n';
            std::cout << e.what() << '\n';
        }
    }

    GPSThread.join();
    if (GPSRecord)
        fclose(GPSFile);

    std::cout << "[JudgmentModule] ------------------ JudgmentModule END! ------------------" << endl;
    KeyThread.join();

    return 0;
}
