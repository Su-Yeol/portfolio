#include "JPathManager.h"

GPSStruct Position; // 현재 차량 위치

// --------------------------------------------------------------------------------------------------- //
// --------------------------------------- Public Function ------------------------------------------- //
// --------------------------------------------------------------------------------------------------- //
/* config.ini에서 올려놓은 경로를 이용할 때 사용 */
void PathConverter::ImportFile(const char *file)
{
    /* Update LastVertex, StartVertex, EndVertex, VertexDistance */
    // Read ODD file
    FILE *ref = fopen(file, "ro"); // file을 읽기모드(ro)로 열기
    char line[1024], c;
    GPSStruct FirstVertex, SecondVertex, TargetVertex;
    double MinimumDistance = 500.0, dist = 0.0;

    /* memset(void* ptr, int value, size_t num) = memory + setting
        메모리의 내용을 원하는 크기만큼 특정값으로 세팅하는 함수
        void* ptr: 세팅하고자 하는 메모리의 시작 주소
        value: 메모리에 세팅하고자 하는 값
        size_t num: 바이트 단위의 메모리 크기 */
    memset(&Global.Latitude, 0, PathSize);
    memset(&Global.Longitude, 0, PathSize);
    memset(&Local.X, 0, PathSize);
    memset(&Local.Y, 0, PathSize);
    memset(&VertexDistance, 0, PathSize);
    WayPointNum = 0;
    EndVertex = 0;
    StartVertex = 0;
    LastVertex = 0;
    MinimumDistanceIdx = 0;
    FrontPathIdx = 0;

    /* WayPoint 정보를 읽어와 그 개수를 계산하는 부분
       각 줄마다 파일에서 한 줄 씩 읽어오고 해당 줄의 길이를 확인하여 WayPoint ++ */
    fseek(ref, 0, SEEK_SET); // ref 파일의 처음으로 이동시킨다. 파일을 처음부터 읽기 위해 시작위치로 이동

    while (1)
    {
        fgets(line, 1024, ref); // ref 파일에서 한 줄을 읽어 line에 저장(최대 1024)
        c = strlen(line);       // 읽어온 줄의 길이를 저장
        if (c == 0)
        {
            break;
        }
        WayPointNum++;
        line[0] = '\0';
    }
    fseek(ref, 0, SEEK_SET);

    /* 파일에서 WayPoint의 위도와 경도 정보를 읽어와서 fscanf를 이용하여 Global.Latitude, Longtitude에 저장 */
    for (int i = 0; i < (WayPointNum - 1); i++)
    {
        fscanf(ref, "%lf/%lf", &Global.Latitude[i], &Global.Longitude[i]);
    }

    /* WayPoint 사이의 거리를 계산하고 해당 값을 VertexDistance에 저장 */
    if (WayPointNum >= 1)
    {
        for (int i = 0; i < (WayPointNum - 2); i++) // WayPiontNum-2: 마지막은 다음이 없어서 계산할 필요X
        {
            FirstVertex.Longitude = Global.Longitude[i];
            SecondVertex.Longitude = Global.Longitude[i + 1];
            FirstVertex.Latitude = Global.Latitude[i];
            SecondVertex.Latitude = Global.Latitude[i + 1];
            VertexDistance[i] = CalCulateDistance(&FirstVertex, &SecondVertex);
        }
        std::cout << "[PathManager] ------------ File Waypoint size : " << WayPointNum << " --------------- " << endl;
    }
    else
        std::cout << "[PathManager] ------------ File Waypoints are not existed ---------------" << endl;

    // 초기 차량 위치 GPS 위도 경도 업데이트
    while (Position.Latitude == 0 || Position.Longitude == 0)
    {
        UpdatePosition(&Position);
    }

    /* 현재 차량 위치를 설정하고 WayPoint들 사이의 거리를 계산하여 최단 거리를 찾는 과정 */
    for (uint32_t i = 0; i < WayPointNum; i++)
    {
        SetTargetVertex(i, &TargetVertex);                  // WayPoint의 위,경도 정보를 TargetVertex에 설정
        dist = CalCulateDistance(&Position, &TargetVertex); // 현재 위치(차랑)와 TargetVertex 사이의 거리를 계산하여 dist에 저장

        if (dist < MinimumDistance)
        {
            // 현재 차량 위치와 경로상의 최단거리 및 인덱스 값 저장
            MinimumDistance = dist;
            MinimumDistanceIdx = i;
        }
    }

    if (MinimumDistanceIdx == WayPointNum)
    {
        MinimumDistanceIdx--;
    }

    LastVertex = MinimumDistanceIdx;
    StartVertex = MinimumDistanceIdx;
    EndVertex = MinimumDistanceIdx + 1;

    /* 현재 차량 위치와 Minimum 다음 WayPoint 간의 거리를 계산한다. 전방 50m 보다 작은 거리면 EndVertex(WayPoint index)를 증가시킨다. */
    for (uint32_t i = EndVertex; i < (WayPointNum - 1); i++)
    {
        // Global.~[i]: WayPoint 위도, 경도
        TargetVertex.Longitude = Global.Longitude[i];
        TargetVertex.Latitude = Global.Latitude[i];
        dist = CalCulateDistance(&Position, &TargetVertex);

        if (dist < FrontLength) // 증가X
        {
            EndVertex++;
        }
        else
            break;
    }
    // printf("[ImportFile] 최종 EndVertex: %d\n", EndVertex);
}

void PathConverter::InitializePath()
{
    GPSStruct FirstVertex, SecondVertex, TargetVertex;
    double MinimumDistance = 500.0, dist = 0.0;
    double distance;

    // Communicator memset Global
    memset(&Local.X, 0, PathSize);
    memset(&Local.Y, 0, PathSize);
    memset(&VertexDistance, 0, PathSize);
    WayPointNum = 128; // Buffersize/8
    EndVertex = 0;
    StartVertex = 0;
    LastVertex = 0;
    MinimumDistanceIdx = 0;
    FrontPathIdx = 0;

    for (uint32_t i = 0; i < WayPointNum - 1; i++)
    {
        FirstVertex.Longitude = Global.Longitude[i];
        SecondVertex.Longitude = Global.Longitude[i + 1];
        FirstVertex.Latitude = Global.Latitude[i];
        SecondVertex.Latitude = Global.Latitude[i + 1];
        distance = CalCulateDistance(&FirstVertex, &SecondVertex);
        if (distance < 10)
        {
            VertexDistance[i] = distance;
        }
        else
            break;
    }

    //
    for (uint32_t i = 0; i < WayPointNum - 1; i++)
    {
        Ibeo.FinalVertexDistance += VertexDistance[i];
    }
    //
    UpdatePosition(&Position);

    for (uint32_t i = 0; i < WayPointNum; i++)
    {
        SetTargetVertex(i, &TargetVertex);
        dist = CalCulateDistance(&Position, &TargetVertex);

        if (dist < MinimumDistance)
        {
            MinimumDistance = dist;
            MinimumDistanceIdx = i;
        }
    }

    if (MinimumDistance > 7) // 경로와 현재위치가 떨어져 있을경우
    {
        PathErrorFlag = true;
        std::cout << "[PathManager/InitializePath] ------------------  GPS not Matched with Received Path. Distance : " << (int)MinimumDistance << endl;
        return;
    }
    else
        PathErrorFlag = false;

    // -------------------------------------------//
    if (MinimumDistanceIdx == WayPointNum)
        MinimumDistanceIdx--;

    LastVertex = MinimumDistanceIdx;
    StartVertex = MinimumDistanceIdx;
    EndVertex = MinimumDistanceIdx + 1;
    // -------------------------------------------//

    for (uint32_t i = EndVertex; i < (WayPointNum - 1); i++)
    {
        TargetVertex.Longitude = Global.Longitude[i];
        TargetVertex.Latitude = Global.Latitude[i];

        dist = CalCulateDistance(&Position, &TargetVertex);

        if (dist < FrontLength)
            EndVertex++;
        else
            break;
    }
}

/* Global -> Local(상대좌표계) */
void PathConverter::GenerateLocalPath()
{
    /* PatheConverter::GenerateLocalPath
        ㄴ 움직이는 차량에 대해 Vertex 값 갱신 및 Global 좌표 → Local 좌표로 변환하는 구간 */
    GPSStruct TargetVertex;
    double CurrentDistance, FrontDistance = 0.0;
    double MinimumDistance = 500.0;
    double PathDencity;

    UpdatePosition(&Position);

    // Update current distance from current vehicle position to Vertex(Last ~ End)
    for (uint32_t i = LastVertex; i < EndVertex; i++) // LastVertex = MinimumDistanceIdx
    {
        SetTargetVertex(i, &TargetVertex);
        CurrentDistance = CalCulateDistance(&Position, &TargetVertex); // 현재 차량 위치와 다음 WayPoint 사이의 현재거리

        if (CurrentDistance < MinimumDistance)
        {
            MinimumDistance = CurrentDistance;
            MinimumDistanceIdx = i;
        }
    }

    Global.LocalizationGap = MinimumDistance; // 현재 차량 위치와 경로 사이의 Gap
    // Update EndVertex when the Vehicle moves forward
    if (EndVertex < (WayPointNum - 1))
    {
        for (uint32_t j = EndVertex; j < (WayPointNum - 1); j++)
        {
            SetTargetVertex(j, &TargetVertex);
            CurrentDistance = CalCulateDistance(&Position, &TargetVertex);

            if (CurrentDistance < FrontLength)
                EndVertex++;
            else
                break;
        }
    }

    // Init StartVertex & LastVertex
    if ((StartVertex < MinimumDistanceIdx) && (StartVertex < EndVertex))
    {
        StartVertex = MinimumDistanceIdx;
        LastVertex = MinimumDistanceIdx;
    }

    FrontDistance = 0; // 앞으로 이동할 거리
    for (uint32_t k = StartVertex; k < EndVertex; k++)
    {
        FrontDistance += VertexDistance[k];
    }

    /* FrontPathIdx is changed by Vehicle velocity
        ㄴ Local coordinate */
    PathDencity = 0.2 + (Vehicle.Velocity * 3.6) * 0.01;
    FrontPathIdx = (uint32_t)(FrontDistance / PathDencity);

    /* Update Current Vehicle Location information */
    Global.Heading = (Position.Azimuth * (-1) + 90) * (M_PI / 180.);
    Global.GapLatitude = Position.Latitude * Lat2meter;
    Global.GapLongitude = Position.Longitude * Lon2meter;
    Global.GapLatitude += OffsetLatitude * sin(Global.Heading);
    Global.GapLongitude += OffsetLongitude * cos(Global.Heading);
    Global.Heading = Global.Heading * (-1);

    /* ---------- FrontPathIdx(< 50m)의 로컬 경로 생성 ---------- */
    if (FrontPathIdx)
    {
        SetTargetVertex(StartVertex, &TargetVertex);
        Global.ValidLatitude[0] = TargetVertex.Latitude * Lat2meter;
        Global.ValidLongitude[0] = TargetVertex.Longitude * Lon2meter;

        Local.X[0] = (Global.ValidLongitude[0] - Global.GapLongitude) * cos(Global.Heading) + (Global.ValidLatitude[0] - Global.GapLatitude) * (-1) * sin(Global.Heading);
        Local.Y[0] = (Global.ValidLongitude[0] - Global.GapLongitude) * sin(Global.Heading) + (Global.ValidLatitude[0] - Global.GapLatitude) * cos(Global.Heading);

        // FrontPathIdx 위치의 이전 값의 Local X, Y 좌표 구하기 - Vehicle Velocity에 영향
        for (uint32_t n = 1; n < FrontPathIdx; n++)
        {
            FrontDistance = 0;
            for (uint32_t m = StartVertex; m < EndVertex; m++)
            {
                FrontDistance += VertexDistance[m];
                if (FrontDistance < PathDencity * n) // 일정한 간격으로 찍기 위해서
                {
                    // n번째 WayPoint의 좌표를 설정하고, 해당 좌표를 로컬 좌표로 변환
                    SetTargetVertex(m, &TargetVertex);
                    Global.ValidLatitude[n] = TargetVertex.Latitude * Lat2meter;
                    Global.ValidLongitude[n] = TargetVertex.Longitude * Lon2meter;

                    Local.X[n] = (Global.ValidLongitude[n] - Global.GapLongitude) * cos(Global.Heading) + (Global.ValidLatitude[n] - Global.GapLatitude) * (-1) * sin(Global.Heading);
                    Local.Y[n] = (Global.ValidLongitude[n] - Global.GapLongitude) * sin(Global.Heading) + (Global.ValidLatitude[n] - Global.GapLatitude) * cos(Global.Heading);
                }
                else
                    break;
            }
        }
        Local.Length = FrontPathIdx;
    }
}
// 11/06 오전 수정
void PathConverter::PedestrianDistance()
{
    // struct timeval startTime, endTime;
    // uint16_t TimeGap;

    FrontVertexDistance = 0.0;
    Radar.MinPedDist = 500; // test
    Radar.WestMinPedDist = 500;
    Radar.EastMinPedDist = -500;
    std::fill_n(Radar.MinPedIdx, 16, 500);
    std::fill_n(Radar.PedDistance, 16, 500);
    std::fill_n(Radar.Latitude, 16, 500);
    std::fill_n(Radar.Longitude, 16, 500);
    int MinIdx = 5000;
    int ObjectClass = 0;
    // Data overshoot check
    static uint8_t RadarCnt;
    static uint8_t RadarFlag;
    static uint8_t PathObjCnt;
    static uint8_t PathObjFlag;
    // 이전 값(전방거리, 경로~보행자 거리) 저장
    static double PreFntVtxDist;
    static double PreRadarPedDist;
    // Haversine Formula
    double toRadian = M_PI / 180.0;
    double toDegree = 180.0 / M_PI;
    double GlobalLat, GlobalLong, RadarLat, RadarLong, deltaLatitude, deltaLongitude, a, c;
    // Azimuth
    double x, y;
    double ObjAzimuth[Radar.ObjectCnt] = {
        500.0,
    };
    double EastPedDist = 500.0;
    double WestPedDist = 500.0;
    // ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ11/06ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ
    // static double preHeading;
    // static int HeadingCnt;
    // static int HeadingFlag;
    // if (Global.Heading == 0)
    // {
    //     HeadingCnt++;
    //     if (HeadingCnt > 3)
    //     {
    //         HeadingCnt = 3 + 1;
    //         Global.Heading = 0;
    //     }
    //     else
    //     {
    //         Global.Heading = preHeading;
    //     }
    // }
    // else
    // {
    //     HeadingCnt = 0;
    //     preHeading = Global.Heading;
    // }
    // ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ

    // gettimeofday(&startTime, NULL);
    for (uint32_t p = 0; p < Radar.ObjectCnt; p++)
    {
        // bool RadarChkFalg = (Radar.X[p] == 0) && (Radar.Y[p] == 0);
        // if (RadarChkFalg == false) // 0, 0 무시
        // {
        if (Radar.Y[p] >= 3.0) // 전방레이더: 뒷바퀴 중심
        {
            // EastPedDist = 500.0;
            // WestPedDist = 500.0;
            // Camera X(횡) Y(종), LiDAR X(종) Y(횡), Radar X(횡) Y(종)
            Radar.Latitude[p] = Position.Latitude + (((Radar.X[p] * cos(Global.Heading)) - (Radar.Y[p] * sin(Global.Heading))) / Lat2meter);
            Radar.Longitude[p] = Position.Longitude + (((Radar.Y[p] * cos(Global.Heading)) + (Radar.X[p] * sin(Global.Heading))) / Lon2meter);
            // printf("%.4lf %.4lf %.7lf %.7lf\n", Radar.X[p], Radar.Y[p], Radar.Latitude[p], Radar.Longitude[p]);
            RadarLat = Radar.Latitude[p] * toRadian;
            RadarLong = Radar.Longitude[p] * toRadian;
            if (Local.Length != 0)
            {
                for (uint32_t r = StartVertex; r < EndVertex - 1; r++)
                {
                    // Haversine Fomula
                    GlobalLat = Global.Latitude[r] * toRadian;
                    GlobalLong = Global.Longitude[r] * toRadian;
                    deltaLatitude = GlobalLat - RadarLat;
                    deltaLongitude = GlobalLong - RadarLong;
                    a = sin(deltaLatitude / 2) * sin(deltaLatitude / 2) + cos(RadarLat) * cos(GlobalLat) * sin(deltaLongitude / 2) * sin(deltaLongitude / 2);
                    c = 2 * atan2(sqrt(a), sqrt(1 - a));
                    Radar.PedDistance[p] = EarthRadius * c;
                    // test
                    if (Radar.PedDistance[p] < Radar.MinPedDist)
                    {
                        Radar.MinPedDist = Radar.PedDistance[p];
                        PreRadarPedDist = Radar.MinPedDist;
                        if (Radar.MinPedDist <= 1.0)
                        {
                            Radar.MinPedIdx[p] = r;
                            PreRadarPedDist = Radar.MinPedDist;
                        }
                    }

                    // // Azimuth
                    // y = sin(deltaLongitude) * cos(RadarLat);
                    // x = cos(GlobalLat) * sin(RadarLat) - sin(GlobalLat) * cos(RadarLat) * cos(deltaLongitude);
                    // ObjAzimuth[p] = atan2(y, x) * toDegree;
                    // // -180 ~ 180 동(-), 서(+)
                    // if (ObjAzimuth[p] > 180)
                    // {
                    //     ObjAzimuth[p] -= 360;
                    // }
                    // else if (ObjAzimuth[p] < -180)
                    // {
                    //     ObjAzimuth[p] += 360;
                    // }
                    // if (ObjAzimuth[p] < 0)
                    // {
                    //     EastPedDist = (-1) * Radar.PedDistance[p];
                    //     if (EastPedDist > Radar.EastMinPedDist)
                    //     {
                    //         Radar.EastMinPedDist = EastPedDist;
                    //         if (Radar.EastMinPedDist >= -1.8)
                    //         {
                    //             Radar.MinPedIdx[p] = r;
                    //         }
                    //     }
                    // }
                    // else if (ObjAzimuth[p] > 0)
                    // {
                    //     WestPedDist = Radar.PedDistance[p];
                    //     if (WestPedDist < Radar.WestMinPedDist)
                    //     {
                    //         Radar.WestMinPedDist = WestPedDist;
                    //         if (Radar.WestMinPedDist <= 1.8)
                    //         {
                    //             Radar.MinPedIdx[p] = r;
                    //         }
                    //     }
                    // }
                    // // 경로~보행자 최소거리
                    // if (fabs(Radar.WestMinPedDist) < fabs(Radar.EastMinPedDist))
                    // {
                    //     Radar.MinPedDist = Radar.WestMinPedDist;
                    //     PreRadarPedDist = Radar.MinPedDist;
                    // }
                    // else if (fabs(Radar.WestMinPedDist) > fabs(Radar.EastMinPedDist))
                    // {
                    //     Radar.MinPedDist = Radar.EastMinPedDist;
                    //     PreRadarPedDist = Radar.MinPedDist;
                    // }
                }
            }
            // }
        }
    }

    // 경로상 장애물의 minimum vertex index 구하기
    for (uint32_t s = 0; s < Radar.ObjectCnt; s++)
    {
        if (Radar.MinPedIdx[s] < MinIdx)
        {
            MinIdx = Radar.MinPedIdx[s];
        }
    }

    // 경로상 장애물 O
    if (MinIdx < EndVertex)
    {
        if (MinIdx != 0)
        {
            if (PathObjCnt > 2)
            {
                RadarCnt = 0;
                RadarFlag = 0;
            }
            FrontVertexDistance = 0.0;
            for (uint32_t t = StartVertex; t < MinIdx - 1; t++)
            {
                FrontVertexDistance += VertexDistance[t];
            }
            PreFntVtxDist = FrontVertexDistance;
        }
        else
        {
            Radar.MinPedDist = PreRadarPedDist;
            FrontVertexDistance = PreFntVtxDist;
        }
    }
    // 경로상 장애물 X, Data overshoot
    else
    {
        // Start & Overshoot
        if (RadarFlag == 0)
        {
            RadarCnt++;
            if (RadarCnt <= 2)
            {
                Radar.MinPedDist = PreRadarPedDist;
                PreFntVtxDist -= (Vehicle.Velocity / 20);
                FrontVertexDistance = PreFntVtxDist;
            }
            else if (RadarCnt > 2)
            {
                FrontVertexDistance = 100;
                PreFntVtxDist = 200;
                RadarFlag = 1;
                RadarCnt = 2 + 1;

                PathObjCnt = 0;
                PathObjFlag = 0;
            }
        }
        // 경로상 장애물 X
        else
        {
            FrontVertexDistance = 100;
            PreFntVtxDist = 200;
            RadarFlag = 1;
            RadarCnt = 2 + 1;

            PathObjCnt = 0;
            PathObjFlag = 0;
        }
    }

    // Prevent 100 -> a
    if (PathObjFlag == 0)
    {
        if (FrontVertexDistance <= 100)
        {
            // if (Radar.MinPedDist <= 1.8 && Radar.MinPedDist >= -1.8)
            if (Radar.MinPedDist <= 1.0)
            {
                PathObjCnt++;
                if (PathObjCnt <= 2)
                {
                    FrontVertexDistance = 100;
                    PreFntVtxDist = FrontVertexDistance;
                }
                else
                {
                    PathObjFlag = 1;
                    PathObjCnt = 2 + 1;
                }
            }
            else
            {
                PathObjCnt = 0;
                FrontVertexDistance = 100;
                PreFntVtxDist = FrontVertexDistance;
            }
        }
    }

    // Prevent Data Overshoot(Init)
    // if (Radar.WestMinPedDist > 10)
    //     Radar.WestMinPedDist = 500;
    // if (Radar.EastMinPedDist < -10)
    //     Radar.EastMinPedDist = -500;
    if (Radar.MinPedDist > 10)
        Radar.MinPedDist = 500;
    if (FrontVertexDistance > 100 || FrontVertexDistance <= 0)
        FrontVertexDistance = 100;
    if (PreFntVtxDist > 200 || PreFntVtxDist == 0)
        PreFntVtxDist = 200;

    // PreFrontVertexDistance = PreFntVtxDist;
    Radar.PathObjDist = FrontVertexDistance;
    Radar.PathObjectFlag = RadarFlag;
    Radar.MinIdx = MinIdx;

    // gettimeofday(&endTime, NULL);
    // TimeGap = (endTime.tv_sec - startTime.tv_sec) * 1000 + ((endTime.tv_usec - startTime.tv_usec) / 1000); // [ms]
    // printf("경로상 %d||Overshoot %d(a->100), %d(100->a)||전방거리 %.4lf||경로~물체 최소거리 %.4lf||%.4lf %.4lf||Index %d||Heading: %d > %.4lf\n",
    //        RadarFlag, RadarCnt, PathObjCnt, FrontVertexDistance, Radar.MinPedDist, Radar.EastMinPedDist, Radar.WestMinPedDist,
    //        MinIdx, HeadingCnt, Global.Heading);
    printf("경로상 %d||Overshoot %d(a->100)||Radar Cnt %d||전방거리 %.4lf||경로~물체 최소거리 %.4lf||Index %d||Heading: %.4lf\n",
           RadarFlag, RadarCnt, Radar.ObjectCnt, FrontVertexDistance, Radar.MinPedDist, MinIdx, Global.Heading);
    // gettimeofday(&startTime, NULL);
}

void PathConverter::IbeoPedestrianDistance()
{
    // IbeoFrontVertexDistance = 0.0;
    // Ibeo.WestMinPedDist = 500.0;
    // Ibeo.EastMinPedDist = -500.0;
    // std::fill_n(Ibeo.MinPedIdx, 30, 500);
    // std::fill_n(Ibeo.Distance, 30, 500);
    // std::fill_n(Ibeo.Latitude, 30, 500);
    // std::fill_n(Ibeo.Longitude, 30, 500);
    // int MinIdx = 3000;
    // int ObjectClass = 0;
    // // Data overshoot check
    // static uint8_t IbeoCnt;
    // static uint8_t IbeoFlag;
    // static uint8_t PathObjCnt;
    // static uint8_t PathObjFlag;
    // // 이전 값(전방거리, 경로~보행자 거리) 저장
    // static double PreFntVtxDist;
    // static double PreIbeoPedDist;
    // // Haversine Formula
    // const double EarthRadius = 6371000;
    // double toRadian = M_PI / 180.0;
    // double toDegree = 180.0 / M_PI;
    // double GlobalLat, GlobalLong, IbeoLat, IbeoLong, deltaLatitude, deltaLongitude, a, c;
    // // Azimuth
    // double x, y;
    // double ObjAzimuth[Ibeo.ObjectCnt] = {
    //     500.0,
    // };
    // double EastPedDist = 500.0;
    // double WestPedDist = 500.0;
    // for (uint32_t p = 0; p < Ibeo.ObjectCnt; p++)
    // {
    //     EastPedDist = 500.0;
    //     WestPedDist = 500.0;
    //     // Ibeo 위도 경도 -> local(회전 방정식)
    //     Ibeo.Latitude[p] = Position.Latitude + ((Ibeo.Object[p * 3 + 3] * cos(Global.Heading) - Ibeo.Object[p * 3 + 2] * sin(Global.Heading)) / Lat2meter);
    //     Ibeo.Longitude[p] = Position.Longitude + ((Ibeo.Object[p * 3 + 2] * cos(Global.Heading) + Ibeo.Object[p * 3 + 3] * sin(Global.Heading)) / Lon2meter);
    //     IbeoLat = Ibeo.Latitude[p] * toRadian;
    //     IbeoLong = Ibeo.Longitude[p] * toRadian;
    //     if (Local.Length != 0)
    //     {
    //         for (uint32_t r = StartVertex; r < EndVertex - 1; r++)
    //         {
    //             // Haversine Fomula
    //             GlobalLat = Global.Latitude[r] * toRadian;
    //             GlobalLong = Global.Longitude[r] * toRadian;
    //             deltaLatitude = GlobalLat - IbeoLat;
    //             deltaLongitude = GlobalLong - IbeoLong;
    //             a = sin(deltaLatitude / 2) * sin(deltaLatitude / 2) + cos(IbeoLat) * cos(GlobalLat) * sin(deltaLongitude / 2) * sin(deltaLongitude / 2);
    //             c = 2 * atan2(sqrt(a), sqrt(1 - a));
    //             Ibeo.Distance[p] = EarthRadius * c;
    //             // Azimuth
    //             y = sin(deltaLongitude) * cos(IbeoLat);
    //             x = cos(GlobalLat) * sin(IbeoLat) - sin(GlobalLat) * cos(IbeoLat) * cos(deltaLongitude);
    //             ObjAzimuth[p] = atan2(y, x) * toDegree;
    //             // -180 ~ 180 동(-), 서(+)
    //             if (ObjAzimuth[p] > 180)
    //             {
    //                 ObjAzimuth[p] -= 360;
    //             }
    //             else if (ObjAzimuth[p] < -180)
    //             {
    //                 ObjAzimuth[p] += 360;
    //             }
    //             if (ObjAzimuth[p] < 0)
    //             {
    //                 EastPedDist = (-1) * Ibeo.Distance[p];
    //                 if (EastPedDist > Ibeo.EastMinPedDist)
    //                 {
    //                     Ibeo.EastMinPedDist = EastPedDist;
    //                     if (Ibeo.EastMinPedDist >= -1.2)
    //                     {
    //                         Ibeo.MinPedIdx[p] = r;
    //                         ObjectClass = Ibeo.Object[p * 3 + 1];
    //                     }
    //                 }
    //             }
    //             else if (ObjAzimuth[p] > 0)
    //             {
    //                 WestPedDist = Ibeo.Distance[p];
    //                 if (WestPedDist < Ibeo.WestMinPedDist)
    //                 {
    //                     Ibeo.WestMinPedDist = WestPedDist;
    //                     if (Ibeo.WestMinPedDist <= 1.2)
    //                     {
    //                         Ibeo.MinPedIdx[p] = r;
    //                         ObjectClass = Ibeo.Object[p * 3 + 1];
    //                     }
    //                 }
    //             }
    //             // 경로~보행자 최소거리
    //             if (fabs(Ibeo.WestMinPedDist) < fabs(Ibeo.EastMinPedDist))
    //             {
    //                 Ibeo.MinPedDist = Ibeo.WestMinPedDist;
    //                 PreIbeoPedDist = Ibeo.MinPedDist;
    //             }
    //             else if (fabs(Ibeo.WestMinPedDist) > fabs(Ibeo.EastMinPedDist))
    //             {
    //                 Ibeo.MinPedDist = Ibeo.EastMinPedDist;
    //                 PreIbeoPedDist = Ibeo.MinPedDist;
    //             }
    //         }
    //     }
    // }

    IbeoFrontVertexDistance = 0.0;
    Ibeo.MinPedDist = 500.0;
    std::fill_n(Ibeo.MinPedIdx, 30, 500);
    std::fill_n(Ibeo.Distance, 30, 500);
    std::fill_n(Ibeo.Latitude, 30, 500);
    std::fill_n(Ibeo.Longitude, 30, 500);
    int MinIdx = 5000;
    int ObjectClass = 0;

    static int preMinIdx;
    static double PreFntVtxDist;
    static double PreIbeoPedDist;
    static uint16_t IbeoCnt;
    static int IbeoFlag;
    static uint8_t PathObjCnt;
    static int PathObjFlag;
    // Haversine Formula
    double toRadian = M_PI / 180.0;
    double toDegree = 180.0 / M_PI;
    double GlobalLat, GlobalLong, IbeoLat, IbeoLong, deltaLatitude, deltaLongitude, RelativeX, RelativeY, a, c;
    // ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ11/06ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ
    static double preAzimuth;
    if (Vehicle.Velocity <= 0.1)
    {
        GPS.Azimuth = preAzimuth;
    }
    else
    {
        preAzimuth = GPS.Azimuth;
    }
    static double PathRange; // 11/07
    // ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ
    for (uint32_t p = 0; p < Ibeo.ObjectCnt; p++)
    {
        if ((Ibeo.Box[(p * 3) + 2] < 550) && (Ibeo.Box[(p * 3) + 3] < 550)) // 11/06 추가(종, 횡)
        {
            Ibeo.Latitude[p] = Position.Latitude + ((Ibeo.Object[(p * 3) + 3] * cos(Global.Heading) - Ibeo.Object[(p * 3) + 2] * sin(Global.Heading)) / Lat2meter);
            Ibeo.Longitude[p] = Position.Longitude + ((Ibeo.Object[(p * 3) + 2] * cos(Global.Heading) + Ibeo.Object[(p * 3) + 3] * sin(Global.Heading)) / Lon2meter);
            // Ibeo.Latitude[p] = Position.Latitude + ((Ibeo.Object[(p * 3) + 3] * cos(Position.Azimuth * toRadian) - Ibeo.Object[(p * 3) + 2] * sin(Position.Azimuth * toRadian)) / Lat2meter);
            // Ibeo.Longitude[p] = Position.Longitude + ((Ibeo.Object[(p * 3) + 2] * cos(Position.Azimuth * toRadian) + Ibeo.Object[(p * 3) + 3] * sin(Position.Azimuth * toRadian)) / Lon2meter);
            IbeoLat = Ibeo.Latitude[p] * toRadian;
            IbeoLong = Ibeo.Longitude[p] * toRadian;
            if (Local.Length != 0)
            {
                for (uint32_t r = StartVertex; r < EndVertex - 1; r++)
                {
                    // Haversine Fomula
                    GlobalLat = Global.Latitude[r] * toRadian;
                    GlobalLong = Global.Longitude[r] * toRadian;
                    deltaLatitude = GlobalLat - IbeoLat;
                    deltaLongitude = GlobalLong - IbeoLong;
                    // a = (sin(deltaLatitude / 2) * sin(deltaLatitude / 2)) + (cos(IbeoLat) * cos(GlobalLat) * sin(deltaLongitude / 2) * sin(deltaLongitude / 2));
                    // c = 2 * atan2(sqrt(a), sqrt(1 - a));
                    // Ibeo.Distance[p] = EarthRadius * c; // [m]
                    // ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ11/09ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ
                    // Spherical Law of Cosines
                    Ibeo.Distance[p] = acos((sin(IbeoLat) * sin(GlobalLat)) + (cos(IbeoLat) * cos(GlobalLat) * cos(deltaLongitude))) * EarthRadius;
                    // ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ
                    if (Ibeo.Distance[p] < Ibeo.MinPedDist)
                    {
                        Ibeo.MinPedDist = Ibeo.Distance[p];
                        // ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ11/09ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ
                        // printf("NowEnv: %d, PreEnv: %d, Global.PreDist: %.2lf\n", Global.NowEnv, Global.PreEnv, Global.PreDist);
                        if (((Global.NowEnv == 1) && (Global.PreEnv == 4)) && ((Global.PreDist < 615) && (Global.PreDist > 460)))
                        {
                            // 버스정류장 ~ 교통섬 전
                            // printf("버스정류장 ㅎㅇ\n");
                            PreIbeoPedDist = Ibeo.MinPedDist;
                            if (Ibeo.MinPedDist <= 1.4)
                            {
                                Ibeo.MinPedIdx[p] = r;
                                PreIbeoPedDist = Ibeo.MinPedDist;
                                ObjectClass = Ibeo.Object[(p * 3) + 1];
                            }
                        }
                        else
                        {
                            PathRange = fabs(Ibeo.MinPedDist - (double)(0.005 * Ibeo.Box[(p * 3) + 3]));
                            if (PathRange <= 0.75)
                            {
                                Ibeo.MinPedIdx[p] = r;
                                PreIbeoPedDist = PathRange;
                                ObjectClass = Ibeo.Object[(p * 3) + 1];
                                // printf("index %d||Class %d||X, Y %.2lf %.2lf||Box X, Y %d %d\n", p, Ibeo.Objectclassification,
                                //     Ibeo.Object[(p*3)+2], Ibeo.Object[(p*3)+3], Ibeo.Box[(p*3)+2], Ibeo.Box[(p*3)+3]);
                            }
                        }
                        // ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ

                        // PreIbeoPedDist = Ibeo.MinPedDist;
                        // if (Ibeo.MinPedDist <= 1.3)
                        // {
                        //     Ibeo.MinPedIdx[p] = r;
                        //     PreIbeoPedDist = Ibeo.MinPedDist;
                        //     ObjectClass = Ibeo.Object[(p * 3) + 1];
                        // }
                    }
                }
            }
        }
    }

    // 경로상 장애물의 minimum vertex index 구하기
    for (uint32_t s = 0; s < Ibeo.ObjectCnt; s++)
    {
        if (Ibeo.MinPedIdx[s] < MinIdx)
        {
            MinIdx = Ibeo.MinPedIdx[s];
        }
    }

    // 경로상 장애물 O
    if (MinIdx < EndVertex)
    {
        if (MinIdx != 0)
        {
            if (PathObjCnt > 2)
            {
                IbeoCnt = 0;
                IbeoFlag = 0;
            }

            IbeoFrontVertexDistance = 0.0;
            for (uint32_t t = StartVertex; t < MinIdx - 1; t++)
            {
                IbeoFrontVertexDistance += VertexDistance[t];
            }
            PreFntVtxDist = IbeoFrontVertexDistance;
        }
        else
        {
            // Ibeo.MinPedDist = PreIbeoPedDist;
            PathRange = PreIbeoPedDist; // 11/07
            IbeoFrontVertexDistance = PreFntVtxDist;
        }
    }
    // 경로상 장애물 X, Data overshoot
    else
    {
        // Start & Overshoot
        if (IbeoFlag == 0)
        {
            IbeoCnt++;
            if (IbeoCnt <= 2)
            {
                // Ibeo.MinPedDist = PreIbeoPedDist;
                PathRange = PreIbeoPedDist; // 11/07
                PreFntVtxDist -= (Vehicle.Velocity / 20);
                IbeoFrontVertexDistance = PreFntVtxDist;
            }
            else if (IbeoCnt > 2)
            {
                IbeoFrontVertexDistance = 100;
                PreFntVtxDist = 200;
                IbeoFlag = 1;
                IbeoCnt = 2 + 1;

                PathObjCnt = 0;
                PathObjFlag = 0;
            }
        }
        // 경로상 장애물 X
        else
        {
            IbeoFrontVertexDistance = 100;
            PreFntVtxDist = 200;
            IbeoFlag = 1;
            IbeoCnt = 2 + 1;

            PathObjCnt = 0;
            PathObjFlag = 0;
        }
    }

    // Prevent 100 -> a
    if (PathObjFlag == 0)
    {
        if (IbeoFrontVertexDistance <= 100)
        {
            // if (Ibeo.MinPedDist <= 1.2 && Ibeo.MinPedDist >= -1.2) // ** 서 ~ 동
            if (PathRange <= 0.75) // 11/07
            // if (Ibeo.MinPedDist <= 1.3) // 11/06
            {
                PathObjCnt++;
                if (PathObjCnt <= 2)
                {
                    IbeoFrontVertexDistance = 100;
                    PreFntVtxDist = IbeoFrontVertexDistance;
                }
                else
                {
                    PathObjFlag = 1;
                    PathObjCnt = 2 + 1;
                }
            }
            else
            {
                PathObjCnt = 0;
                IbeoFrontVertexDistance = 100;
                PreFntVtxDist = IbeoFrontVertexDistance;
            }
        }
    }

    // Prevent Data Overshoot(Init)
    // if (Ibeo.WestMinPedDist > 10)
    //     Ibeo.WestMinPedDist = 500;
    // if (Ibeo.EastMinPedDist < -10)
    //     Ibeo.EastMinPedDist = -500;
    // if (Ibeo.MinPedDist > 10)
    //     Ibeo.MinPedDist = 500;
    if (IbeoFrontVertexDistance > 100 || IbeoFrontVertexDistance <= 0)
        IbeoFrontVertexDistance = 100;
    if (PreFntVtxDist > 200 || PreFntVtxDist == 0)
        PreFntVtxDist = 200;

    // IbeoPreFrontVertexDistance = PreFntVtxDist;
    Ibeo.PathObjDist = IbeoFrontVertexDistance;
    Ibeo.PathObjectFlag = IbeoFlag;
    Ibeo.MinIdx = MinIdx;

    // gettimeofday(&endTime, NULL);
    // TimeGap = (endTime.tv_sec - startTime.tv_sec) * 1000 + ((endTime.tv_usec - startTime.tv_usec) / 1000); // [ms]
    // printf("경로상 %d||Overshoot %d(a->100), %d(100->a)||전방거리 %.2lf||최소거리 %.2lf||Index %d||Azimuth %.2lf||VtDist %.2lf\n",
    //        IbeoFlag, IbeoCnt, PathObjCnt, IbeoFrontVertexDistance, Ibeo.MinPedDist, MinIdx, GPS.Azimuth, Ibeo.FinalVertexDistance);
    printf("경로상 %d||Overshoot %d(a->100), %d(100->a)||전방거리 %.2lf||최소거리 %.2lf %.2lf||Index %d||Azimuth %.2lf||VtDist %.2lf\n",
           IbeoFlag, IbeoCnt, PathObjCnt, IbeoFrontVertexDistance, Ibeo.MinPedDist, PathRange, MinIdx, GPS.Azimuth, Ibeo.FinalVertexDistance);
    // gettimeofday(&startTime, NULL);
}

// --------------------------------------------------------------------------------------------------- //
// --------------------------------------- Private Function ------------------------------------------ //
// --------------------------------------------------------------------------------------------------- //
void PathConverter::UpdatePosition(GPSStruct *pos)
{
    pos->Longitude = GPS.Longitude;
    pos->Latitude = GPS.Latitude;
    pos->Azimuth = GPS.Azimuth;
}

double PathConverter::CalCulateDistance(GPSStruct *pos1, GPSStruct *pos2)
{
    double dist = sqrt(pow(((pos1->Longitude - pos2->Longitude) * Lon2meter), 2) + pow(((pos1->Latitude - pos2->Latitude) * Lat2meter), 2));
    return dist;
}

void PathConverter::SetTargetVertex(uint32_t idx, GPSStruct *TargetPos)
{
    TargetPos->Latitude = Global.Latitude[idx];
    TargetPos->Longitude = Global.Longitude[idx];
}