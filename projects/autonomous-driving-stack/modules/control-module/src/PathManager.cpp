#if !defined(USE_PRIVATE_IMPL)
#include "../../common/include/modules/control-module/PathManager.h"

const double FrontLength = 0.0;
const double OffsetLatitude = 0.0;
const double OffsetLongitude = 0.0;
const double Lat2meter = 1.0;
const double Lon2meter = 1.0;

/**
 * @brief Short description of `PathConvert::ImportFile`.
 *
 * Detailed explanation of what `PathConvert::ImportFile` does,
 * including major steps and responsibilities.
 *
 * @param file Input parameter.
 * @return void
 */
void PathConvert::ImportFile(const char *file)
{
    (void)file;
    WayPointNum = 0;
    PathErrorFlag = true;
}

/**
 * @brief Short description of `PathConvert::InitializePath`.
 *
 * Detailed explanation of what `PathConvert::InitializePath` does,
 * including major steps and responsibilities.
 *
 * @param None.
 * @return void
 */
void PathConvert::InitializePath()
{
    WayPointNum = 0;
    PathErrorFlag = true;
}

/**
 * @brief Short description of `PathConvert::GenerateLocalPath`.
 *
 * Detailed explanation of what `PathConvert::GenerateLocalPath` does,
 * including major steps and responsibilities.
 *
 * @param None.
 * @return void
 */
void PathConvert::GenerateLocalPath()
{
    Local.Length = 0;
    Global.LocalizationGap = 0.0;
}

/**
 * @brief Short description of `PathConvert::UpdatePosition`.
 *
 * Detailed explanation of what `PathConvert::UpdatePosition` does,
 * including major steps and responsibilities.
 *
 * @param pos Input parameter.
 * @return void
 */
void PathConvert::UpdatePosition(GPSVariable *pos)
{
    (void)pos;
}

/**
 * @brief Short description of `PathConvert::CalCulateDistance`.
 *
 * Detailed explanation of what `PathConvert::CalCulateDistance` does,
 * including major steps and responsibilities.
 *
 * @param pos1 Input parameter.
 * @param pos2 Input parameter.
 * @return Output value from the function.
 */
double PathConvert::CalCulateDistance(GPSVariable *pos1, GPSVariable *pos2)
{
    (void)pos1;
    (void)pos2;
    return 0.0;
}

/**
 * @brief Short description of `PathConvert::SetTargetVertex`.
 *
 * Detailed explanation of what `PathConvert::SetTargetVertex` does,
 * including major steps and responsibilities.
 *
 * @param idx Input parameter.
 * @param TargetPos Input parameter.
 * @return void
 */
void PathConvert::SetTargetVertex(uint32_t idx, GPSVariable *TargetPos)
{
    (void)idx;
    (void)TargetPos;
}
#endif
