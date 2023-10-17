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
    char line[1024], c;            // why? line 1024
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
        // printf("%d번째 Global GPS 읽어오기 : %lf/%lf\n", i, Global.Latitude[i], Global.Longitude[i]);
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
        // printf("[ImportFile] Position.Latitude : %.2lf || Position.Longitude : %.2lf\n", Position.Latitude, Position.Longitude);
    }

    /* 현재 차량 위치를 설정하고 WayPoint들 사이의 거리를 계산하여 최단 거리를 찾는 과정 */
    for (uint32_t i = 0; i < WayPointNum; i++)
    {
        SetTargetVertex(i, &TargetVertex); // WayPoint의 위,경도 정보를 TargetVertex에 설정
        // printf("[ImportFile 1] TargetVertex.Latitude : %.2lf || TargetVertex.Longitude : %.2lf\n", TargetVertex.Latitude, TargetVertex.Longitude);

        dist = CalCulateDistance(&Position, &TargetVertex); // 현재 위치(차랑)와 TargetVertex 사이의 거리를 계산하여 dist에 저장
        // printf("[ImportFile 1] i : %d || dist : %.2lf\n", i, dist);

        // ************************
        if (dist < MinimumDistance)
        {
            // 현재 차량 위치와 경로상의 최단거리 및 인덱스 값 저장
            MinimumDistance = dist;
            MinimumDistanceIdx = i;
            // printf("[ImportFile] MinimumDistanceIdx : %d || MinimumDistance : %.1lf\n", MinimumDistanceIdx, MinimumDistance);
        }
    }

    if (MinimumDistanceIdx == WayPointNum)
    {
        MinimumDistanceIdx--;
    }

    LastVertex = MinimumDistanceIdx;
    StartVertex = MinimumDistanceIdx;
    EndVertex = MinimumDistanceIdx + 1;
    // printf("[ImportFile] 초기 LastVertex: %d, StartVertex: %d, EndVertex: %d\n", LastVertex, StartVertex, EndVertex);

    /* 현재 차량 위치와 Minimum 다음 WayPoint 간의 거리를 계산한다. 전방 50m 보다 작은 거리면 EndVertex(WayPoint index)를 증가시킨다. */
    for (uint32_t i = EndVertex; i < (WayPointNum - 1); i++)
    {
        // Global.~[i]: WayPoint 위도, 경도
        TargetVertex.Longitude = Global.Longitude[i];
        TargetVertex.Latitude = Global.Latitude[i];

        dist = CalCulateDistance(&Position, &TargetVertex);
        // printf("[ImportFile 2] %d번째 Global 위도: %.1lf, Global 경도: %.1lf, 거리: %.1f\n", i, Global.Longitude[i], Global.Latitude[i], dist);

        if (dist < FrontLength) // 증가X
        {
            // 전방 50m 안에서 Vertex 1개씩 증가
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
    WayPointNum = 0;
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

    if (MinimumDistance > 7)
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
        // printf("[GenerateLocalPath 1] %d번째 || TargetVertex 위도: %.4lf || TargetVertex 경도: %.4lf || 거리: %.4f\n", i, TargetVertex.Longitude, TargetVertex.Latitude, CurrentDistance);

        if (CurrentDistance < MinimumDistance)
        {
            MinimumDistance = CurrentDistance;
            MinimumDistanceIdx = i;
            // printf("[GenerateLocalPath] MinimumDistance : %.4lf || MinimumDistanceIdx : %d\n", MinimumDistance, MinimumDistanceIdx);
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
            // printf("[GenerateLocalPath 2] %d번째 || TargetVertex 위도: %.4lf || TargetVertex 경도: %.4lf || 거리: %.4f\n", j, TargetVertex.Longitude, TargetVertex.Latitude, CurrentDistance);

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
    // printf("[GenerateLocalPath] LastVertex : %d || StartVertex : %d || EndVertex : %d\n", LastVertex, StartVertex, EndVertex);

    FrontDistance = 0; // 앞으로 이동할 거리
    for (uint32_t k = StartVertex; k < EndVertex; k++)
    {
        FrontDistance += VertexDistance[k];
    }

    /* FrontPathIdx is changed by Vehicle velocity
        ㄴ Local coordinate */
    PathDencity = 0.2 + (Vehicle.Velocity * 3.6) * 0.01;
    FrontPathIdx = (uint32_t)(FrontDistance / PathDencity);
    // printf("Vehicle.Velocity : %f || PathDencity : %f || FrontDistance : %f || FrontPathIdx : %d\n", Vehicle.Velocity, PathDencity, FrontDistance, FrontPathIdx);

    /* Update Current Vehicle Location information */
    Global.Heading = (Position.Azimuth * (-1) + 90) * (M_PI / 180.); // 현재 방향(0~360도)
    Global.GapLatitude = Position.Latitude * Lat2meter;              // 위도 위치 정보를 미터 단위로 변환
    Global.GapLongitude = Position.Longitude * Lon2meter;            // 경도 위치 정보를 미터 단위로 변환
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
            // printf("[GenerateLocalPath] Local.X[%d], Local.Y[%d] : %.1lf, %.1lf\n", n, n, Local.X[n], Local.Y[n]);
        }
        Local.Length = FrontPathIdx;
    }
}

/* 현재 차량위치와 보행자의 거리 */
void PathConverter::PedestrianDistance()
{
    /* Mobileye, Ibeo 사용 시 배열 index 및 변수들 수정필요 */
    FrontVertexDistance = 0.0;
    Ibeo.MinimumPedestrianDistance = 500.0;
    std::fill_n(Ibeo.MinimumPedestrianIdx, 30, 500);
    std::fill_n(Ibeo.Distance, 30, 500);
    int MinimumIdx = 500;
    int ObjectClass = 0;

    static uint16_t IbeoCnt; // Data overshoot count
    static int IbeoFlag;    // 경로상 장애물 0(O), 1(X)
    uint8_t IbeoCntChk = 10; // Data overshoot count check

    // Mobileye
    /* for (uint32_t p = 0; p < 10; p++) // Mobileye Object count = 10
    {
        // for문이 Local.Length까지 해버리면 마지막 X, Y값이 0, 0으로 찍힌다.
        for (uint32_t r = 0; r < Local.Length - 1; r++)
        {
            //printf("Local.Length - 1: %d\n", Local.Length - 1);
            bool PCheckFlag = (Mobileye.X[p] == 0) && (Mobileye.Y[p] == 0);
            if (PCheckFlag == false)
            {
                // Path 종: X, 횡: Y || Mobileye 종: Y, 횡: X
                Mobileye.Distance[p] = sqrt(pow((Local.X[r] - Mobileye.Y[p]), 2) + pow((Local.Y[r] - Mobileye.X[p]), 2));
                //printf("X: %.1lf/%.1lf  Y: %.1lf/%.1lf\n", Local.X[r], Mobileye.Y[p], Local.Y[r], Mobileye.X[p]);

                if (Mobileye.Distance[p] < Mobileye.MinimumPedestrianDistance)
                {
                    Mobileye.MinimumPedestrianDistance = Mobileye.Distance[p];
                    if(Mobileye.MinimumPedestrianDistance < 1.8)
                    {
                        Mobileye.MinimumPedestrianIdx[p] = r;
                    }
                }
            }
            //printf("Idx: %d  X: %.1lf/%.1lf  Y: %.1lf/%.1lf\n", Mobileye.MinimumPedestrianIdx[p], Local.Y[Mobileye.MinimumPedestrianIdx[p]], Mobileye.X[p],
                   //Local.X[Mobileye.MinimumPedestrianIdx[p]], Mobileye.Y[p]);
        }
    } */

    // Ibeo RADARS
    for (uint32_t p = 0; p < Ibeo.ObjectCnt; p++) // Ibeo Object count = (max)30, Mobileye Object count = (max)10
    {
        // printf("%.4lf, %.4lf\n", Ibeo.Object[p * 3 + 2], Ibeo.Object[p * 3 + 3]);
        if (Local.Length != 0)
        {
            // for문이 Local.Length까지 해버리면 마지막 X, Y값이 0, 0으로 찍힌다.
            for (uint32_t r = 0; r < Local.Length - 1; r++)
            {
                bool PCheckFlag = (Ibeo.Object[p * 3 + 2] == 0) && (Ibeo.Object[p * 3 + 3] == 0);
                if (PCheckFlag == false)
                {
                    // printf("[PedestrianDistance] X: %.4lf, %.4lf / Y: %.4lf, %.4lf\n", Local.X[r], Ibeo.Object[p * 3 + 2], Local.Y[r], Ibeo.Object[p * 3 + 3]);
                    // Path 종: X, 횡: Y || Mobileye 종: Y, 횡: X || Ibeo 종: X, 횡: Y

                    // 자동차 전용도로, 좌측 깜빡이 flag, 차선 변경 가능여부 flag(한쪽이라도 불가 -> 임의의 시간 유지 약 5~10s -> flag)
                    // if (LoadType == 2 && LeftTurnSwitch == 1 && 차선flag == 17)
                    // {
                    //     // 횡 방향으로 경로상(-2m ~ 2m)에 있으면
                    //     if (Ibeo.Object[p * 3 + 3] <= 1.9 && Ibeo.Object[p * 3 + 3] >= -1.9)
                    //     {
                    //         // printf("1\n");
                    //         Ibeo.Distance[p] = Ibeo.Object[p * 3 + 2]; // 0으로 튀는거 확인
                    //
                    //         if (Ibeo.Distance[p] < Ibeo.MinimumPedestrianDistance)
                    //         {
                    //             Ibeo.MinimumPedestrianDistance = Ibeo.Distance[p];
                    //         }
                    //         FrontVertexDistance = Ibeo.MinimumPedestrianDistance;
                    //         printf("Object 번호: %d || Object 전방거리 %.4lf\n", p, FrontVertexDistance);
                    //     }
                    // }

                    // else
                    // {
                    Ibeo.Distance[p] = sqrt(pow((Local.X[r] - Ibeo.Object[p * 3 + 2]), 2) + pow((Local.Y[r] - Ibeo.Object[p * 3 + 3]), 2));
                    // printf("[PedestrianDistance] Vertex와 Object Distance: %.4lf\n", Ibeo.Distance[p]);

                    if (Ibeo.Distance[p] < Ibeo.MinimumPedestrianDistance)
                    {
                        Ibeo.MinimumPedestrianDistance = Ibeo.Distance[p];
                        // printf("[PedestrianDistance] Vertex와 Object 최소 Distance: %.4lf\n", Ibeo.MinimumPedestrianDistance);

                        if (Ibeo.MinimumPedestrianDistance <= 1.8)
                        {
                            Ibeo.MinimumPedestrianIdx[p] = r;
                            ObjectClass = Ibeo.Object[p * 3 + 1];
                            // printf("[PedestrianDistance] X: %.4lf, %.4lf / Y: %.4lf, %.4lf\n", Local.X[r], Ibeo.Object[p * 3 + 2], Local.Y[r], Ibeo.Object[p * 3 + 3]);
                        }
                    }
                    // } 
                }
                // printf("Object 전방거리 %.4lf\n", FrontVertexDistance);
            }
        }
    }

    /* 경로상 장애물의 minimum vertex index 구하기 */
    for (uint32_t s = 0; s < Ibeo.ObjectCnt; s++)
    {
        if (Ibeo.MinimumPedestrianIdx[s] < MinimumIdx)
        {
            MinimumIdx = Ibeo.MinimumPedestrianIdx[s];
        }
    }

    /* Minimum Vertex ~ Pedestrian Front Distance */
    // 경로상 장애물 O
    if (MinimumIdx < Local.Length)
    {
        IbeoCnt = 0;
        IbeoFlag = 0;
        // Local idx 0 : 차량의 위치 ~ 보행자의 위치까지의 거리
        for (uint32_t t = 0; t < MinimumIdx; t++)
        {
            FrontVertexDistance += sqrt(pow((Local.X[t] - Local.X[t + 1]), 2) + pow((Local.Y[t] - Local.Y[t + 1]), 2));
            // printf("%d번 ~ %d번째 Local X %.4lf, %.4lf || Local Y %.4lf, %.4lf || 전방거리 : %.4lf\n", t, t+1, Local.X[t], Local.X[t+1], Local.Y[t], Local.Y[t+1], FrontVertexDistance);
        }
        PreFrontVertexDistance = FrontVertexDistance;
    }
    // 경로상 장애물 X, Overshoot
    else
    {
        // FrontVertexDistance = 100;
        if (IbeoFlag == 0 || IbeoFlag == 1)
            IbeoFlag = IbeoFlag;
        else // start
            IbeoFlag = 0;

        // Start & Overshoot
        if (IbeoFlag == 0)
        {
            if (IbeoCnt >= 0 && IbeoCnt <= IbeoCntChk)
                IbeoCnt = IbeoCnt; // overshoot
            else
                IbeoCnt = 0; // start

            IbeoCnt++;
            if (PreFrontVertexDistance != FrontVertexDistance)
                PreFrontVertexDistance = 200; // start
            else
                PreFrontVertexDistance = PreFrontVertexDistance; // overshoot

            /* Check overshoot data */
            if (IbeoCnt <= IbeoCntChk)
            {
                // 현재 차량 속도에서 50ms마다 움직이는 거리
                PreFrontVertexDistance -= (Vehicle.Velocity / 20);
                // printf("Velocity %.4lf || PreFrontVertexDistance %.4lf\n", Vehicle.Velocity, PreFrontVertexDistance);
                FrontVertexDistance = PreFrontVertexDistance;
            }
            else if (IbeoCnt > IbeoCntChk)
            {
                FrontVertexDistance = 100;
                PreFrontVertexDistance = 200;
                IbeoFlag = 1;
                IbeoCnt = IbeoCntChk + 1;
            }
        }
        // 경로상 장애물 X
        else
        {
            FrontVertexDistance = 100;
            PreFrontVertexDistance = 200;
            IbeoFlag = 1;
            IbeoCnt = IbeoCntChk + 1;
        }
    }
    /* Prevent Value Overshoot(init 0) */
    if (Ibeo.MinimumPedestrianDistance == 0)
        Ibeo.MinimumPedestrianDistance = 500;
    if (MinimumIdx == 0)
        MinimumIdx = 500;

    if (FrontVertexDistance > 100)
        FrontVertexDistance = 100;
    else if (FrontVertexDistance == 0)
        FrontVertexDistance = 100;
    if (PreFrontVertexDistance > 200)
        PreFrontVertexDistance = 200;
    else if (PreFrontVertexDistance == 0)
        PreFrontVertexDistance = 200;

    printf("[PedestrianDistance] Object Class: %d || 경로상 유무 %d || Overshoot %d || Vertex 전방거리 %.4lf || Vertex와 보행자 최소거리 %.4lf || 최소거리 Vertex index %d\n",
           ObjectClass, IbeoFlag, IbeoCnt, FrontVertexDistance, Ibeo.MinimumPedestrianDistance, MinimumIdx);
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
