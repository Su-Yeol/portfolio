#if !defined(USE_PRIVATE_IMPL)
#include "../../common/include/modules/control-module/Controller.h"

const double kControlGravity = 9.81;
const double kControlLookAheadGain = 1.0;
const double kControlTargetVelocityScale = 1.0;

/**
 * @brief Short description of `ControlLogic::PurePursuit`.
 *
 * Detailed explanation of what `ControlLogic::PurePursuit` does,
 * including major steps and responsibilities.
 *
 * @param None.
 * @return void
 */
void ControlLogic::PurePursuit()
{
    Control.Handle = 0.0;
}

/**
 * @brief Short description of `ControlLogic::SCC`.
 *
 * Detailed explanation of what `ControlLogic::SCC` does,
 * including major steps and responsibilities.
 *
 * @param None.
 * @return void
 */
void ControlLogic::SCC()
{
    Control.Acceleration = 0.0;
}

/**
 * @brief Short description of `ControlLogic::GetLateraldeviation`.
 *
 * Detailed explanation of what `ControlLogic::GetLateraldeviation` does,
 * including major steps and responsibilities.
 *
 * @param None.
 * @return void
 */
void ControlLogic::GetLateraldeviation()
{
    Control.LateralDeviation = 0.0;
}

/**
 * @brief Short description of `ControlLogic::GetRelativeYawAngle`.
 *
 * Detailed explanation of what `ControlLogic::GetRelativeYawAngle` does,
 * including major steps and responsibilities.
 *
 * @param None.
 * @return void
 */
void ControlLogic::GetRelativeYawAngle()
{
    Control.RelativeHeadingAngle = 0.0;
}

/**
 * @brief Short description of `ControlLogic::GetCurvature`.
 *
 * Detailed explanation of what `ControlLogic::GetCurvature` does,
 * including major steps and responsibilities.
 *
 * @param None.
 * @return void
 */
void ControlLogic::GetCurvature()
{
    Control.Curvature = 0.0;
}
#endif
