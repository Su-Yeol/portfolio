#include "DPathManager.h"

GPSStruct Position;
// --------------------------------------- Public Function ------------------------------------------- //
/* config.ini에서 올려놓은 경로를 이용할 때 사용 */
void PathConverter::ImportFile(const char *file)
{
    /* Update LastVertex, StartVertex, EndVertex, VertexDistance */
    FILE *ref = fopen(file, "ro");
    char line[1024], c;
    GPSStruct FirstVertex, SecondVertex, TargetVertex;
    double MinimumDistance = 500.0, dist = 0.0;
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
    fseek(ref, 0, SEEK_SET);
    while (1)
    {
        fgets(line, 1024, ref);
        c = strlen(line);
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
    {
        std::cout << "[PathManager] ------------ File Waypoints are not existed ---------------" << endl;
    }
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
}

void PathConverter::InitializePath()
{
    GPSStruct FirstVertex, SecondVertex, TargetVertex;
    double MinimumDistance = 500.0, dist = 0.0;
    double distance;
    memset(&Local.X, 0, PathSize);
    memset(&Local.Y, 0, PathSize);
    memset(&VertexDistance, 0, PathSize);
    WayPointNum = BufferSize / 64; // BufferSize(1024)/8
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
    // // Vertex interval Check
    // for (uint32_t i = 0; i < WayPointNum - 1; i++)
    // {
    //     Ibeo.FinalVertexDistance += VertexDistance[i];
    // }
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
    {
        PathErrorFlag = false;
    }

    if (MinimumDistanceIdx == WayPointNum)
    {
        MinimumDistanceIdx--;
    }
    LastVertex = MinimumDistanceIdx;
    StartVertex = MinimumDistanceIdx;
    EndVertex = MinimumDistanceIdx + 1;
    for (uint32_t i = EndVertex; i < (WayPointNum - 1); i++)
    {
        TargetVertex.Longitude = Global.Longitude[i];
        TargetVertex.Latitude = Global.Latitude[i];
        dist = CalCulateDistance(&Position, &TargetVertex);
        if (dist < FrontLength)
        {
            EndVertex++;
        }
        else
            break;
    }
}

void PathConverter::GenerateLocalPath()
{
    /* 움직이는 차량에 대해 Vertex 값 갱신 및 Global 좌표 → Local 좌표로 변환하는 구간 */
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
    Global.Heading = (Position.Azimuth * (-1) + 90) * toRadian;
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
    FrontVertexDistance = 0.0;
    Radar.MinPedDist = 500;
    Radar.WestMinPedDist = -500.0;
    Radar.EastMinPedDist = 500.0;
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
    // Haversine Formula || Spherical Law of Cosines
    double GlobalLat, GlobalLong, RadarLat, RadarLong, deltaLatitude, deltaLongitude, RelativeX, RelativeY, a, c, x, y;
    // Object Azimuth
    double ObjAzimuth[Radar.ObjectCnt] = {
        500.0,
    };
    // 이전 값(전방거리, 경로~보행자 거리, Azimuth) 저장
    static double PreFntVtxDist;
    static double PreRadarPedDist;
    static double preAzimuth;
    if (Vehicle.Velocity <= 0.1)
    {
        GPS.Azimuth = preAzimuth;
    }
    else
    {
        preAzimuth = GPS.Azimuth;
    }
    for (uint32_t p = 0; p < Radar.ObjectCnt; p++)
    {
        if (Radar.Y[p] >= 3.0) // 전방레이더: 뒷바퀴 중심
        {
            // Radar.MinPedDist = 500;
            // Radar.WestMinPedDist = -500.0;
            // Radar.EastMinPedDist = 500.0;
            // Camera X(횡) Y(종), LiDAR X(종) Y(횡), Radar X(횡) Y(종)
            Radar.Latitude[p] = Position.Latitude + (((Radar.X[p] * cos(Global.Heading)) - (Radar.Y[p] * sin(Global.Heading))) / Lat2meter);
            Radar.Longitude[p] = Position.Longitude + (((Radar.Y[p] * cos(Global.Heading)) + (Radar.X[p] * sin(Global.Heading))) / Lon2meter);
            RadarLat = Radar.Latitude[p] * toRadian;
            RadarLong = Radar.Longitude[p] * toRadian;
            if (Local.Length != 0)
            {
                for (uint32_t r = StartVertex; r < EndVertex - 1; r++)
                {
                    // Haversine Fomula || Spherical Law of Cosines
                    GlobalLat = Global.Latitude[r] * toRadian;
                    GlobalLong = Global.Longitude[r] * toRadian;
                    deltaLatitude = GlobalLat - RadarLat;
                    deltaLongitude = GlobalLong - RadarLong;
                    // a = sin(deltaLatitude / 2) * sin(deltaLatitude / 2) + cos(RadarLat) * cos(GlobalLat) * sin(deltaLongitude / 2) * sin(deltaLongitude / 2);
                    // c = 2 * atan2(sqrt(a), sqrt(1 - a));
                    // Radar.PedDistance[p] = EarthRadius * c;
                    Radar.PedDistance[p] = acos((sin(RadarLat) * sin(GlobalLat)) + (cos(RadarLat) * cos(GlobalLat) * cos(deltaLongitude))) * EarthRadius;
                    // Azimuth
                    y = sin(deltaLongitude) * cos(RadarLat);
                    x = (cos(GlobalLat) * sin(RadarLat)) - (sin(GlobalLat) * cos(RadarLat) * cos(deltaLongitude));
                    ObjAzimuth[p] = atan2(y, x) * (-1) * toDegree; // -180~180
                    if (Radar.PedDistance[p] < Radar.MinPedDist)
                    {
                        Radar.MinPedDist = Radar.PedDistance[p];
                        PreRadarPedDist = Radar.MinPedDist;
                        // ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ Distance ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ
                        if (Radar.MinPedDist <= 1.8)
                        {
                            Radar.MinPedIdx[p] = r;
                            PreRadarPedDist = Radar.MinPedDist;
                        }
                        // ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ 11.20 Distance + Azimuth(Bearing) ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ
                        // if (ObjAzimuth[p] < 0)
                        // {
                        //     Radar.WestMinPedDist = (-1) * Radar.MinPedDist;
                        //     if (Radar.WestMinPedDist >= -1.8)
                        //     {
                        //         Radar.MinPedIdx[p] = r;
                        //         PreRadarPedDist = Radar.WestMinPedDist;
                        //     }
                        // }
                        // else
                        // {
                        //     Radar.EastMinPedDist = Radar.MinPedDist;
                        //     if (Radar.EastMinPedDist <= 1.8)
                        //     {
                        //         Radar.MinPedIdx[p] = r;
                        //         PreRadarPedDist = Radar.EastMinPedDist;
                        //     }
                        // }
                    }
                }
            }
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
                PreFntVtxDist -= (Vehicle.Velocity / 20); // 50ms
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
            // if (Radar.EastMinPedDist <= 1.8 && Radar.WestMinPedDist >= -1.8)
            if (Radar.MinPedDist <= 1.8)
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
    if (FrontVertexDistance > 100 || FrontVertexDistance <= 0)
    {
        FrontVertexDistance = 100;
    }
    if (PreFntVtxDist > 200 || PreFntVtxDist == 0)
    {
        PreFntVtxDist = 200;
    }

    Radar.PathObjDist = FrontVertexDistance;
    Radar.PathObjectFlag = RadarFlag;
    Radar.MinIdx = MinIdx;
    // printf("경로상 %d||Overshoot %d(a->100), %d(100->a)||전방거리 %.2lf||Obj Azimuth %.4lf||경로~물체 최소거리 %.2lf %.2lf||Index %d||Azimuth %.2lf\n",
    //        RadarFlag, RadarCnt, PathObjCnt, FrontVertexDistance, ObjAzimuth[MinIdx], Radar.WestMinPedDist, Radar.EastMinPedDist, MinIdx, GPS.Azimuth);
}

void PathConverter::IbeoPedestrianDistance()
{
    // IbeoFrontVertexDistance = 0.0;
    // Ibeo.WestMinPedDist = 500.0;
    // Ibeo.EastMinPedDist = -500.0;
    // std::fill_n(Ibeo.MinPedIdx, 30, 500);
    // std::fill_n(Ibeo.PedDistance, 30, 500);
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
    // double GlobalLat, GlobalLong, IbeoLat, IbeoLong, deltaLatitude, deltaLongitude, a, c;
    // double EastPedDist = 500.0;
    // double WestPedDist = 500.0;
    // for (uint32_t p = 0; p < Ibeo.ObjectCnt; p++)
    // {
    //     EastPedDist = 500.0;
    //     WestPedDist = -500.0;
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
    //             Ibeo.PedDistance[p] = EarthRadius * c;
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
    //                 EastPedDist = (-1) * Ibeo.PedDistance[p];
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
    //                 WestPedDist = Ibeo.PedDistance[p];
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
    Ibeo.PathObjIdx = 500;
    // Ibeo.WestMinPedDist = -500.0;
    // Ibeo.EastMinPedDist = 500.0;
    std::fill_n(Ibeo.MinPedIdx, 30, 500);
    std::fill_n(Ibeo.PedDistance, 30, 500);
    std::fill_n(Ibeo.Latitude, 30, 500);
    std::fill_n(Ibeo.Longitude, 30, 500);
    int MinIdx = 5000;
    int ObjectClass = 0;
    // Data overshoot check
    static uint8_t IbeoCnt;
    static uint8_t IbeoFlag;
    static uint8_t PathObjCnt;
    static uint8_t PathObjFlag;
    // Haversine Formula || Spherical Law of Cosines
    double GlobalLat, GlobalLong, IbeoLat, IbeoLong, deltaLatitude, deltaLongitude, RelativeX, RelativeY, a, c, x, y;
    // // Azimuth
    double ObjAzimuth[Ibeo.ObjectCnt] = {
        500.0,
    };
    // 이전 값(전방거리, 경로~보행자 거리, Azimuth) 저장
    static double PreFntVtxDist;
    static double PreIbeoPedDist;
    static double preAzimuth;
    if (Vehicle.Velocity <= 0.1)
    {
        GPS.Azimuth = preAzimuth;
    }
    else
    {
        preAzimuth = GPS.Azimuth;
    }
    static double PathRange; // 참고할 최단거리
    PathHeadingAngle();      // 11.30
    for (uint32_t p = 0; p < Ibeo.ObjectCnt; p++)
    {
        if ((Ibeo.Box[(p * 3) + 2] < 550) && (Ibeo.Box[(p * 3) + 3] < 550)) // 11/06 추가(종, 횡)
        {
            Ibeo.Latitude[p] = Position.Latitude + ((Ibeo.Object[(p * 3) + 3] * cos(Global.Heading) - Ibeo.Object[(p * 3) + 2] * sin(Global.Heading)) / Lat2meter);
            Ibeo.Longitude[p] = Position.Longitude + ((Ibeo.Object[(p * 3) + 2] * cos(Global.Heading) + Ibeo.Object[(p * 3) + 3] * sin(Global.Heading)) / Lon2meter);
            // Ibeo.Latitude[p] = Position.Latitude + ((Ibeo.Object[(p * 3) + 3] * cos(Global.PathAngle) - Ibeo.Object[(p * 3) + 2] * sin(Global.PathAngle)) / Lat2meter);
            // Ibeo.Longitude[p] = Position.Longitude + ((Ibeo.Object[(p * 3) + 2] * cos(Global.PathAngle) + Ibeo.Object[(p * 3) + 3] * sin(Global.PathAngle)) / Lon2meter);
            IbeoLat = Ibeo.Latitude[p] * toRadian;
            IbeoLong = Ibeo.Longitude[p] * toRadian;
            if (Local.Length != 0)
            {
                for (uint32_t r = StartVertex; r < EndVertex - 1; r++)
                {
                    // Haversine Formula || Spherical Law of Cosines
                    GlobalLat = Global.Latitude[r] * toRadian;
                    GlobalLong = Global.Longitude[r] * toRadian;
                    deltaLatitude = GlobalLat - IbeoLat;
                    deltaLongitude = GlobalLong - IbeoLong;
                    // a = (sin(deltaLatitude / 2) * sin(deltaLatitude / 2)) + (cos(IbeoLat) * cos(GlobalLat) * sin(deltaLongitude / 2) * sin(deltaLongitude / 2));
                    // c = 2 * atan2(sqrt(a), sqrt(1 - a));
                    // Ibeo.PedDistance[p] = EarthRadius * c; // [m]
                    Ibeo.PedDistance[p] = acos((sin(IbeoLat) * sin(GlobalLat)) + (cos(IbeoLat) * cos(GlobalLat) * cos(deltaLongitude))) * EarthRadius;

                    if (Ibeo.PedDistance[p] < Ibeo.MinPedDist)
                    {
                        Ibeo.MinPedDist = Ibeo.PedDistance[p];
                        if (((Global.NowEnv == 1) && (Global.PreEnv == 4)) && ((Global.PreDist < 615) && (Global.PreDist > 440)))
                        {
                            PreIbeoPedDist = Ibeo.MinPedDist;
                            PathRange = Ibeo.MinPedDist;
                            if (PathRange <= 1.4)
                            {
                                Ibeo.MinPedIdx[p] = r;
                                Ibeo.PathObjIdx = p;
                                PreIbeoPedDist = PathRange;
                                ObjectClass = Ibeo.Object[(p * 3) + 1];
                            }
                        }
                        // 하이패스 입구~출구
                        else if (((Global.NowEnv == 2) && (Global.PreEnv == 5)) && ((Global.PreDist < 80) && (Global.PreDist > 0)))
                        {
                            PreIbeoPedDist = Ibeo.MinPedDist;
                            PathRange = Ibeo.MinPedDist;
                            if (PathRange <= 0.5)
                            {
                                Ibeo.MinPedIdx[p] = r;
                                Ibeo.PathObjIdx = p;
                                PreIbeoPedDist = PathRange;
                                ObjectClass = Ibeo.Object[(p * 3) + 1];
                            }
                        }
                        // 하이패스 출구~음영구간 입구
                        else if (((Global.NowEnv == 5) && (Global.PreEnv == 5)) && ((Global.PreDist < 440) && (Global.PreDist > 285)))
                        {
                            PreIbeoPedDist = Ibeo.MinPedDist;
                            PathRange = Ibeo.MinPedDist;
                            if (PathRange <= 0.5)
                            {
                                Ibeo.MinPedIdx[p] = r;
                                Ibeo.PathObjIdx = p;
                                PreIbeoPedDist = PathRange;
                                ObjectClass = Ibeo.Object[(p * 3) + 1];
                            }
                        }
                        // 음영구간 입구~출구 직전
                        else if (((Global.NowEnv == 5) && (Global.PreEnv == 5)) && ((Global.PreDist < 286) && (Global.PreDist > 170)))
                        {
                            PreIbeoPedDist = Ibeo.MinPedDist;
                            PathRange = Ibeo.MinPedDist;
                            if (PathRange <= 1.5)
                            {
                                Ibeo.MinPedIdx[p] = r;
                                Ibeo.PathObjIdx = p;
                                PreIbeoPedDist = PathRange;
                                ObjectClass = Ibeo.Object[(p * 3) + 1];
                            }
                        }
                        // 경사로
                        // else if (((Global.NowEnv == 6) && (Global.PreEnv == 6)) && ((Global.PreDist < 85) && (Global.PreDist > 0)))
                        // {
                        //     PreIbeoPedDist = Ibeo.MinPedDist;
                        //     PathRange = Ibeo.MinPedDist;
                        //     if (PathRange <= 1.2)
                        //     {
                        //         Ibeo.MinPedIdx[p] = r;
                        //         Ibeo.PathObjIdx = p;
                        //         PreIbeoPedDist = PathRange;
                        //         ObjectClass = Ibeo.Object[(p * 3) + 1];
                        //     }
                        // }
                        else
                        {
                            PathRange = fabs(Ibeo.MinPedDist - (double)(0.005 * Ibeo.Box[(p * 3) + 3])); // 가드레일: BoxSizeX > 10.0[m]
                            if (PathRange <= 0.8)
                            {
                                Ibeo.MinPedIdx[p] = r;
                                Ibeo.PathObjIdx = p;
                                PreIbeoPedDist = PathRange;
                                ObjectClass = Ibeo.Object[(p * 3) + 1];
                            }
                        }
                        // PreIbeoPedDist = Ibeo.MinPedDist;
                        // if (Ibeo.MinPedDist <= 1.5)
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
            PathRange = PreIbeoPedDist;
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
                PathRange = PreIbeoPedDist;
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
            if (PathRange <= 0.8) // 11/07
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
    if (IbeoFrontVertexDistance > 100 || IbeoFrontVertexDistance <= 0)
    {
        IbeoFrontVertexDistance = 100;
    }
    if (PreFntVtxDist > 200 || PreFntVtxDist == 0)
    {
        PreFntVtxDist = 200;
    }
    if (Ibeo.FaultCheckFlag == 1) // 12.04
    {
        IbeoFrontVertexDistance = 200;
    }
    // IbeoPreFrontVertexDistance = PreFntVtxDist;
    Ibeo.PathObjDist = IbeoFrontVertexDistance;
    Ibeo.PathObjectFlag = IbeoFlag;
    Ibeo.MinIdx = MinIdx;
    // printf("경로상 %d||Overshoot %d(a->100), %d(100->a)||전방거리 %.2lf||최소거리 %.2lf %.2lf||Index %d||Azimuth %.2lf %.2lf\n",
    //        IbeoFlag, IbeoCnt, PathObjCnt, IbeoFrontVertexDistance, Ibeo.MinPedDist, PathRange, MinIdx, Global.Heading, Global.PathAngle);
}
// 11.30
void PathConverter::PathHeadingAngle()
{
    double Error;
    double PathDirection;
    double distance = 0;
    double GPSHeading;
    static double PrePathDirection = 0.0;
    uint32_t NearestIdx = 0;
    uint32_t NextIdx = 0;
    for (uint32_t i = 0; i < Local.Length; i++)
    {
        distance = sqrt(pow((Global.ValidLatitude[i] - Global.ValidLatitude[0]), 2) + pow((Global.ValidLongitude[i] - Global.ValidLongitude[0]), 2));
        if (distance < 3.0) // 뒷바퀴
        {
            NearestIdx = i;
        }
        else if (distance < 5.0) // 앞바퀴
        {
            NextIdx = i;
        }
    }
    if (((NearestIdx != 0) && (NextIdx != 0)) && (NearestIdx < Local.Length))
    {
        PathDirection = atan2((Global.ValidLatitude[NextIdx] - Global.ValidLatitude[NearestIdx]), (Global.ValidLongitude[NextIdx] - Global.ValidLongitude[NearestIdx]));
        PathDirection = ((PathDirection * (-1)) + 90) * toDegree;
        if (PathDirection == 0.0)
            PathDirection = PrePathDirection;
        else
            PrePathDirection = PathDirection;
        // if ((PathDirection > -180) && (PathDirection < 0))
        //     PathDirection = 360 + PathDirection;
        // GPSHeading = Global.Heading * toDegree;
        // Error = std::abs(std::abs(GPSHeading) - std::abs(PathDirection));
        Global.PathAngle = PathDirection * toRadian;
        // Error = (Global.Heading * toDegree) - PathDirection;
    }
    else
    {
        printf("[PathManager/GetRHA]-------- Indexing Error. NearIdx:%d  NextIdx:%d EndVertex:%d\n", NearestIdx, NextIdx, EndVertex);
    }
}
// --------------------------------------- Private Function ------------------------------------------ //
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