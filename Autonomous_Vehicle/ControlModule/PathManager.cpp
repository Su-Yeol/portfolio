#include "ControlModule/include/PathManager.h"

GPSVariable Position;

// --------------------------------------------------------------------------------------------------- //
// --------------------------------------- Public Function ------------------------------------------- //
// --------------------------------------------------------------------------------------------------- //
void PathConvert::ImportFile(const char *file)
{
    FILE *ref = fopen(file, "ro");
    char line[1024], c;

    GPSVariable FirstVertex, SecondVertex, TargetVertex;
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
    
    for (int i=0; i<(WayPointNum-1); i++)
    {
        fscanf(ref, "%lf/%lf", &Global.Latitude[i], &Global.Longitude[i]);
    }

    if (WayPointNum >= 1)
    {
        for (int i=0; i<(WayPointNum-2); i++)
        {
            FirstVertex.Longitude = Global.Longitude[i];
            SecondVertex.Longitude = Global.Longitude[i+1];
            FirstVertex.Latitude = Global.Latitude[i];
            SecondVertex.Latitude = Global.Latitude[i+1];
            VertexDistance[i] = CalCulateDistance(&FirstVertex, &SecondVertex);
        }
        cout<<"[PathManager] ------------------- File Waypoint size : "<<WayPointNum<<" "<<endl;
    }
    else cout<<"[PathManager] ------------------- File Waypoints are not existed "<<endl;

    UpdatePosition(&Position);
    for (uint32_t i=0; i<WayPointNum; i++)
    {
        SetTargetVertex(i, &TargetVertex);
        dist = CalCulateDistance(&Position, &TargetVertex);
        if (dist < MinimumDistance)
        {
            MinimumDistance = dist;
            MinimumDistanceIdx = i;
        }
    }
    if (MinimumDistanceIdx == WayPointNum) MinimumDistanceIdx--;
    LastVertex = MinimumDistanceIdx;
    StartVertex = MinimumDistanceIdx;
    EndVertex = MinimumDistanceIdx + 1;

    for (uint32_t i=EndVertex; i<(WayPointNum-1); i++)
    {
        TargetVertex.Longitude = Global.Longitude[i];
        TargetVertex.Latitude = Global.Latitude[i];
        dist = CalCulateDistance(&Position, &TargetVertex);

        if (dist < FrontLength) EndVertex++;
        else break;
    }
}

void PathConvert::InitializePath()
{
    GPSVariable FirstVertex, SecondVertex, TargetVertex;
    double distance;
    double MinimumDistance = 500.0, dist = 0.0;

    // Communicator memset Global
    memset(&Local.X, 0, PathSize);
    memset(&Local.Y, 0, PathSize);
    memset(&VertexDistance, 0, PathSize);
    WayPointNum = BufferSize/8;
    EndVertex = 0;
    StartVertex = 0;
    LastVertex = 0;
    MinimumDistanceIdx = 0;
    FrontPathIdx = 0;

    for (uint32_t i=0; i<WayPointNum-1; i++)
    {
        FirstVertex.Longitude = Global.Longitude[i];
        SecondVertex.Longitude = Global.Longitude[i+1];
        FirstVertex.Latitude = Global.Latitude[i];
        SecondVertex.Latitude = Global.Latitude[i+1];
        distance = CalCulateDistance(&FirstVertex, &SecondVertex);
        if (distance < 10)
        {
            VertexDistance[i] = distance;
        }
        else break;
    }

    UpdatePosition(&Position);
    for (uint32_t i=0; i<WayPointNum; i++)
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
        cout << "[PathManager/InitializePath] ------------------  GPS not Matched with Received Path. Distance : "<< (int)MinimumDistance << endl;
        return;
    }
    else PathErrorFlag = false;

    if (MinimumDistanceIdx == WayPointNum) MinimumDistanceIdx--;
    LastVertex = MinimumDistanceIdx;
    StartVertex = MinimumDistanceIdx;
    EndVertex = MinimumDistanceIdx + 1;

    for (uint32_t i=EndVertex; i<(WayPointNum-1); i++)
    {
        TargetVertex.Longitude = Global.Longitude[i];
        TargetVertex.Latitude = Global.Latitude[i];
        dist = CalCulateDistance(&Position, &TargetVertex);

        if (dist < FrontLength) EndVertex++;
        else break;
    }
}

void PathConvert::GenerateLocalPath()
{
    GPSVariable TargetVertex;
    double CurrentDistance, FrontDistance = 0.0;
    double MinimumDistance = 500.0;
    double PathDencity;
    
    UpdatePosition(&Position);
    for (uint32_t i=LastVertex; i<EndVertex; i++)
    {
        SetTargetVertex(i, &TargetVertex);
        CurrentDistance = CalCulateDistance(&Position, &TargetVertex);
        if(CurrentDistance < MinimumDistance)
        {
            MinimumDistance = CurrentDistance;
            MinimumDistanceIdx = i;
        }
    }

    Global.LocalizationGap = MinimumDistance;
    if (EndVertex < (WayPointNum-1))
    {
        for (uint32_t j=EndVertex; j<(WayPointNum-1); j++)
        {
            SetTargetVertex(j, &TargetVertex);
            CurrentDistance = CalCulateDistance(&Position, &TargetVertex);
            if (CurrentDistance < FrontLength) EndVertex++;
            else break;
        }
    }
    if((StartVertex < MinimumDistanceIdx) && (StartVertex < EndVertex))
    {
        StartVertex = MinimumDistanceIdx;
        LastVertex = MinimumDistanceIdx;
    }

    FrontDistance = 0;
    for (uint32_t k=StartVertex; k<EndVertex; k++) 
    {
        FrontDistance += VertexDistance[k];
    }
    
    PathDencity = 0.2 + (Vehicle.Velocity*3.6)*0.01;
    FrontPathIdx = (uint32_t)(FrontDistance / PathDencity);
    
    Global.Heading = (Position.Azimuth * (-1) + 90) * (M_PI / 180.);
    Global.GapLatitude = Position.Latitude * Lat2meter;
    Global.GapLongitude = Position.Longitude * Lon2meter;
    Global.GapLatitude += OffsetLatitude * sin(Global.Heading);
    Global.GapLongitude += OffsetLongitude * cos(Global.Heading);
    Global.Heading = Global.Heading * (-1);
    
    if (FrontPathIdx)
    {
        SetTargetVertex(StartVertex, &TargetVertex);
        Global.ValidLatitude[0] = TargetVertex.Latitude * Lat2meter;
        Global.ValidLongitude[0] = TargetVertex.Longitude * Lon2meter;

        Local.X[0] = (Global.ValidLongitude[0] - Global.GapLongitude)*cos(Global.Heading) + (Global.ValidLatitude[0] - Global.GapLatitude)*(-1)*sin(Global.Heading);
        Local.Y[0] = (Global.ValidLongitude[0] - Global.GapLongitude)*sin(Global.Heading) + (Global.ValidLatitude[0] - Global.GapLatitude)*cos(Global.Heading);

        for (uint32_t n=1; n<FrontPathIdx; n++)
        {
            FrontDistance = 0;
            for (uint32_t m=StartVertex; m<EndVertex; m++)
            {
                FrontDistance += VertexDistance[m];
                if (FrontDistance < PathDencity * n)
                {
                    SetTargetVertex(m, &TargetVertex);
                    Global.ValidLatitude[n] = TargetVertex.Latitude * Lat2meter;
                    Global.ValidLongitude[n] = TargetVertex.Longitude * Lon2meter;

                    Local.X[n] = (Global.ValidLongitude[n] - Global.GapLongitude)*cos(Global.Heading) + (Global.ValidLatitude[n] - Global.GapLatitude)*(-1)*sin(Global.Heading);
                    Local.Y[n] = (Global.ValidLongitude[n] - Global.GapLongitude)*sin(Global.Heading) + (Global.ValidLatitude[n] - Global.GapLatitude)*cos(Global.Heading);
                }
                else break;
            }
        }
        Local.Length = FrontPathIdx;
    }
    
}



// --------------------------------------------------------------------------------------------------- //
// --------------------------------------- Private Function ------------------------------------------- //
// --------------------------------------------------------------------------------------------------- //
void PathConvert::UpdatePosition(GPSVariable* pos)
{
    pos->Longitude = GPS.Longitude;
    pos->Latitude = GPS.Latitude;
    pos->Azimuth = GPS.Azimuth;
}

double PathConvert::CalCulateDistance(GPSVariable* pos1, GPSVariable* pos2)
{
    double dist = sqrt(pow(((pos1->Longitude - pos2->Longitude) * Lon2meter), 2) + pow(((pos1->Latitude - pos2->Latitude) * Lat2meter), 2));
    return dist;
}

void PathConvert::SetTargetVertex(uint32_t idx, GPSVariable* TargetPos)
{
    TargetPos->Latitude = Global.Latitude[idx];
    TargetPos->Longitude = Global.Longitude[idx];
}