#if !defined(USE_PRIVATE_IMPL)
#include "../../common/include/modules/decision-module/DPathManager.h"

/**
 * @brief Short description of `PathConverter::ImportFile`.
 *
 * Detailed explanation of what `PathConverter::ImportFile` does,
 * including major steps and responsibilities.
 *
 * @param file Input parameter.
 * @return void
 */
void PathConverter::ImportFile(const char *file)
{
    (void)file;
    WayPointNum = 0;
    FrontVertexDistance = 0.0;
    PreFrontVertexDistance = 0.0;
    IbeoFrontVertexDistance = 0.0;
    IbeoPreFrontVertexDistance = 0.0;
    PathErrorFlag = true;
}

/**
 * @brief Short description of `PathConverter::InitializePath`.
 *
 * Detailed explanation of what `PathConverter::InitializePath` does,
 * including major steps and responsibilities.
 *
 * @param None.
 * @return void
 */
void PathConverter::InitializePath()
{
    WayPointNum = 0;
    PathErrorFlag = true;
}

/**
 * @brief Short description of `PathConverter::GenerateLocalPath`.
 *
 * Detailed explanation of what `PathConverter::GenerateLocalPath` does,
 * including major steps and responsibilities.
 *
 * @param None.
 * @return void
 */
void PathConverter::GenerateLocalPath()
{
    Local.Length = 0;
    Global.LocalizationGap = 0.0;
}

/**
 * @brief Short description of `PathConverter::PedestrianDistance`.
 *
 * Detailed explanation of what `PathConverter::PedestrianDistance` does,
 * including major steps and responsibilities.
 *
 * @param None.
 * @return void
 */
void PathConverter::PedestrianDistance()
{
    Radar.MinPedDist = 0.0;
}

/**
 * @brief Short description of `PathConverter::IbeoPedestrianDistance`.
 *
 * Detailed explanation of what `PathConverter::IbeoPedestrianDistance` does,
 * including major steps and responsibilities.
 *
 * @param None.
 * @return void
 */
void PathConverter::IbeoPedestrianDistance()
{
    Ibeo.MinPedDist = 0.0;
}

/**
 * @brief Short description of `PathConverter::PathHeadingAngle`.
 *
 * Detailed explanation of what `PathConverter::PathHeadingAngle` does,
 * including major steps and responsibilities.
 *
 * @param None.
 * @return void
 */
void PathConverter::PathHeadingAngle()
{
    Global.PathAngle = 0.0;
}

/**
 * @brief Short description of `PathConverter::UpdatePosition`.
 *
 * Detailed explanation of what `PathConverter::UpdatePosition` does,
 * including major steps and responsibilities.
 *
 * @param pos Input parameter.
 * @return void
 */
void PathConverter::UpdatePosition(GPSStruct *pos)
{
    (void)pos;
}

/**
 * @brief Short description of `PathConverter::CalCulateDistance`.
 *
 * Detailed explanation of what `PathConverter::CalCulateDistance` does,
 * including major steps and responsibilities.
 *
 * @param pos1 Input parameter.
 * @param pos2 Input parameter.
 * @return Output value from the function.
 */
double PathConverter::CalCulateDistance(GPSStruct *pos1, GPSStruct *pos2)
{
    (void)pos1;
    (void)pos2;
    return 0.0;
}

/**
 * @brief Short description of `PathConverter::SetTargetVertex`.
 *
 * Detailed explanation of what `PathConverter::SetTargetVertex` does,
 * including major steps and responsibilities.
 *
 * @param idx Input parameter.
 * @param TargetPos Input parameter.
 * @return void
 */
void PathConverter::SetTargetVertex(uint32_t idx, GPSStruct *TargetPos)
{
    (void)idx;
    (void)TargetPos;
}
#endif
