#include "PathManager.h"

GPSStruct Position; // 현재 차량 위치

// --------------------------------------------------------------------------------------------------- //
// --------------------------------------- Public Function ------------------------------------------- //
// --------------------------------------------------------------------------------------------------- //
/* config.ini에서 올려놓은 경로를 이용할 때 사용 */
void PathConverter::ImportFile(const char *file)
{
    /* ODD 경로 탐색 */
    FILE *ref = fopen(file, "ro"); // file을 읽기모드(ro)로 열기
    char line[1024], c;            // why? line 1024

    GPSStruct FirstVertex, SecondVertex, TargetVertex;
    double MinimumDistance = 500.0, dist = 0.0;

    /* memset(void* ptr, int value, size_t num) = memory + setting
        메모리의 내용을 원하는 크기만큼 특정값으로 세팅하는 함수
        void* ptr: 세팅하고자 하는 메모리의 시작 주소
        value: 메모리에 세팅하고자 하는 값
        size_t num: 바이트 단위의 메모리 크기 */
    memset(&Global.Latitude, 0, PathSize); // why? pathsize 8192 -> 8bit 1024
    memset(&Global.Longitude, 0, PathSize);
    memset(&Local.X, 0, PathSize);
    memset(&Local.Y, 0, PathSize);
    memset(&VertexDistance, 0, PathSize);
    WayPointNum = 0;
    EndVertex = 0;   // 마지막 지점
    StartVertex = 0; // 시작 지점
    LastVertex = 0;  // 이전의 EndVertex
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
        cout << "[PathManager]------------File Waypoint size : " << WayPointNum << "---------------" << endl;
    }
    else
        cout << "[PathManager]------------File Waypoints are not existed---------------" << endl;

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

    // -------------------------------------------//
    if (MinimumDistanceIdx == WayPointNum)
        MinimumDistanceIdx--;
    LastVertex = MinimumDistanceIdx;
    StartVertex = MinimumDistanceIdx;
    EndVertex = MinimumDistanceIdx + 1;
    // -------------------------------------------//

    /* 현재 차량 위치와 Minimum 다음 WayPoint 간의 거리를 계산한다. 전방 50m 보다 작은 거리면 EndVertex(WayPoint index)를 증가시킨다. */
    for (uint32_t i = EndVertex; i < (WayPointNum - 1); i++)
    {
        // Global.~[i]: WayPoint 위도, 경도
        TargetVertex.Longitude = Global.Longitude[i];
        TargetVertex.Latitude = Global.Latitude[i];
        dist = CalCulateDistance(&Position, &TargetVertex);

        if (dist < FrontLength) // 전방 50m 안에서 Vertex 1개씩 증가
            EndVertex++;
        else
            break;
    }
}

/* TC에서 수시로 Log를 올려줄 경우 사용 - Port 4488(receive), 4444(send) */
void PathConverter::InitializePath()
{
    GPSStruct FirstVertex, SecondVertex, TargetVertex;
    double MinimumDistance = 500.0, dist = 0.0;

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

    for (int i = 0; i < (PathSize / 8) - 1; i++)
    {
        WayPointNum = (PathSize / 8) - 1;
        FirstVertex.Longitude = Global.Longitude[i];
        SecondVertex.Longitude = Global.Longitude[i + 1];
        FirstVertex.Latitude = Global.Latitude[i];
        SecondVertex.Latitude = Global.Latitude[i + 1];
        VertexDistance[i] = CalCulateDistance(&FirstVertex, &SecondVertex); // WayPoint 간의 간격[m]
    }

    UpdatePosition(&Position); // 현재 차량 위치 update

    for (uint32_t i = 0; i < WayPointNum; i++)
    {
        SetTargetVertex(i, &TargetVertex);                  // 각 WayPoint의 위도, 경도
        dist = CalCulateDistance(&Position, &TargetVertex); // 현재 차량 위치와 각 WayPoint의 거리

        if (dist < MinimumDistance) // 현재 차량 위치와 WayPoint의 최소 거리
        {
            MinimumDistance = dist;
            MinimumDistanceIdx = i;
        }
    }

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
    /* GlobalPath -> LocalPath */
    GPSStruct TargetVertex;
    double CurrentDistance, FrontDistance = 0.0;
    double MinimumDistance = 500.0;

    UpdatePosition(&Position);

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
    // 안전장치. endvertex가 waypoint를 넘는경우(50m가 안남았을때)
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

    // 초기화
    if ((StartVertex < MinimumDistanceIdx) && (StartVertex < EndVertex))
    {
        StartVertex = MinimumDistanceIdx;
        LastVertex = MinimumDistanceIdx;
    }

    FrontDistance = 0; // 앞으로 이동할 거리를 계산하기 위한 누적값
    for (uint32_t k = StartVertex; k < EndVertex; k++)
    {
        FrontDistance += VertexDistance[k]; // WayPoint 간의 간격
    }

    // PathDencity = 0.2+속도(km/h)*0.01 (속도 max 60)
    /* 얼마나 많은 WayPoint를 방문해야 하는지 */
    FrontPathIdx = (uint32_t)(FrontDistance / PathDencity);

    /* 현재 방향을 계산하고, 위치 정보를 업데이트 */
    Global.Heading = (Position.Azimuth * (-1) + 90) * (M_PI / 180.); // 현재 방향(0~360도)
    Global.GapLatitude = Position.Latitude * Lat2meter;              // 위도 위치 정보를 미터 단위로 변환
    Global.GapLongitude = Position.Longitude * Lon2meter;            // 경도 위치 정보를 미터 단위로 변환
    Global.GapLatitude += OffsetLatitude * sin(Global.Heading);
    Global.GapLongitude += OffsetLongitude * cos(Global.Heading);
    Global.Heading = Global.Heading * (-1);

    /* 로컬 경로 생성 */
    if (FrontPathIdx)
    {
        SetTargetVertex(StartVertex, &TargetVertex); // StartVertex의 위도, 경도 값
        Global.ValidLatitude[0] = TargetVertex.Latitude * Lat2meter;
        Global.ValidLongitude[0] = TargetVertex.Longitude * Lon2meter;

        // 위도, 경도 -> X, Y[m] 변환
        Local.X[0] = (Global.ValidLongitude[0] - Global.GapLongitude) * cos(Global.Heading) + (Global.ValidLatitude[0] - Global.GapLatitude) * (-1) * sin(Global.Heading);
        Local.Y[0] = (Global.ValidLongitude[0] - Global.GapLongitude) * sin(Global.Heading) + (Global.ValidLatitude[0] - Global.GapLatitude) * cos(Global.Heading);

        // 앞으로 이동할 거리를 기반으로 로컬 경로 생성
        for (uint32_t n = 1; n < FrontPathIdx; n++)
        {
            FrontDistance = 0;
            for (uint32_t m = StartVertex; m < EndVertex; m++)
            {
                FrontDistance += VertexDistance[m];
                if (FrontDistance < PathDencity * n) //
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

double PathConverter::PedestrianDistance()
{
    double CurrentPedestrianDistance = 0.0;
    double MinimumPedestrianDistance = 500.0;
    int CurrentPedestrianIdx = 0; int MininumPedestrianIdx = 0;

    CurrentPedestrianDistance, CurrentPedestrianIdx = CalCulatePedestrianDistance(Local.Length, &Local, &Pedestrian);
    if (CurrentPedestrianDistance < MinimumPedestrianDistance)
    {
        MinimumPedestrianDistance = CurrentPedestrianDistance;
        MininumPedestrianIdx = CurrentPedestrianIdx;
    }

    return MininumPedestrianIdx, Pedestrian.Class[MinimumDistanceIdx], MinimumPedestrianDistance; // index, class, distance
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

/* double PathConverter::CalCulatePedestrianDistance(uint32_t length, LocalPathStruct *pos1, PedestrianStruct *pos2)
{
    // [left camera, class, X, Y, right camera, class, X, Y]
    for (uint32_t p = 0; p < length; p++)
    {
        double Leftdist = sqrt(pow((pos1->X[p] - pos2->X[0]), 2) + pow((pos1->Y[p] - pos2->Y[0]), 2));
        double Rightdist = sqrt(pow((pos1->X[p] - pos2->X[1]), 2) + pow((pos1->Y[p] - pos2->Y[1]), 2));
        return min(Leftdist, Rightdist);
    }
} */

double PathConverter::CalCulatePedestrianDistance(uint32_t length, LocalPathStruct *pos1, PedestrianStruct *pos2)
{
    /* Mobileye */
    for (uint32_t p = 0; p < length; p++) // Vertex
    {
        for (uint32_t r = 0; r < 10; r++) // Mobileye Object count = 10
        {
            pos2->Direction[r] = sqrt(pow((pos1->X[p] - pos2->X[r]), 2) + pow((pos1->Y[p] - pos2->Y[r]), 2));
            return pos2->Direction[r], r;
        }
    }
}

void PathConverter::SetTargetVertex(uint32_t idx, GPSStruct *TargetPos)
{
    TargetPos->Latitude = Global.Latitude[idx];
    TargetPos->Longitude = Global.Longitude[idx];
}
