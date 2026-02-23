/*
 * katech_database.h
 *
 *  Created on: 2024. 5. 9.
 *      Author: jyhan
 */

#ifndef KATECH_DATABASE_H_
#define KATECH_DATABASE_H_

#include <stdint.h>
#include <stdbool.h>
//#include <linux/types.h>

#define KATECH_NAME_DB_IONIQ_RxCAN0_YRS_01_10ms 0
typedef struct _KATECH_DB_IONIQ_RxCAN0_YRS_01_10ms_{
	float LatAccel;
	float LongAccel;
	float YawRate;
}DB_IONIQ_RxCAN0_YRS_01_10ms;

#define KATECH_NAME_DB_IONIQ_RxCAN0_IEB_01_10ms 1
typedef struct _KATECH_DB_IONIQ_RxCAN0_IEB_01_10ms_{
	uint8_t BrakeAct;
	float BrakePedalValue;
}DB_IONIQ_RxCAN0_IEB_01_10ms;

#define KATECH_NAME_DB_IONIQ_RxCAN0_WHL_01_10ms 2
typedef struct _KATECH_DB_IONIQ_RxCAN0_WHL_01_10ms_{
	float WheelSpdFL;
	float WheelSpdFR;
	float WheelSpdRL;
	float WheelSpdRR;
	float WheelPulseFL;
	float WheelPulseFR;
	float WheelPulseRL;
	float WheelPulseRR;

	float AvgWheelSpeed;
}DB_IONIQ_RxCAN0_WHL_01_10ms;

#define KATECH_NAME_DB_IONIQ_RxCAN0_SAS_01_10ms 3
typedef struct _KATECH_DB_IONIQ_RxCAN0_SAS_01_10ms_{
	float StrAngle;
	float StrSpeed;
}DB_IONIQ_RxCAN0_SAS_01_10ms;

#define KATECH_NAME_DB_IONIQ_RxCAN0_Unknown_0x130 4
typedef struct _KATECH_DB_IONIQ_RxCAN0_Unknown_0x130_{
	uint8_t GearPosition;
	uint8_t ADCMGearPosition;
}DB_IONIQ_RxCAN0_Unknown_0x130;

#define KATECH_NAME_DB_IONIQ_RxCAN0_ESC_03_20ms 5
typedef struct _KATECH_DB_IONIQ_RxCAN0_ESC_03_20ms_{
	uint8_t ParkingBrk;
}DB_IONIQ_RxCAN0_ESC_03_20ms;

#define KATECH_NAME_DB_IONIQ_RxCAN0_CLU_01_20ms 6
typedef struct _KATECH_DB_IONIQ_RxCAN0_CLU_01_20ms_{
	uint8_t DisplaySpd;
}DB_IONIQ_RxCAN0_CLU_01_20ms;

#define KATECH_NAME_DB_IONIQ_RxCAN0_CLU_02_100ms 7
typedef struct _KATECH_DB_IONIQ_RxCAN0_CLU_02_100ms_{
	uint8_t FuelLevel;
	float AvgFuelConsum;
	float Odometer;
	uint16_t DTE;
}DB_IONIQ_RxCAN0_CLU_02_100ms;

#define KATECH_NAME_DB_IONIQ_RxCAN0_RR_C_RDR_01_50ms 8
typedef struct _KATECH_DB_IONIQ_RxCAN0_RR_C_RDR_01_50ms_{
	uint8_t RadarFusion;
}DB_IONIQ_RxCAN0_RR_C_RDR_01_50ms;


#define KATECH_NAME_DB_IONIQ_RxCAN0_ICU_02_200ms 9
typedef struct _KATECH_DB_IONIQ_RxCAN0_ICU_02_200ms_{
	uint8_t DrvSeatBelt_Status;
	uint8_t AsstSeatBelt_Status;
}DB_IONIQ_RxCAN0_ICU_02_200ms;

#define KATECH_NAME_DB_IONIQ_RxCAN0_ICU_04_200ms 10
typedef struct _KATECH_DB_IONIQ_RxCAN0_ICU_04_200ms_{
	uint8_t Emergency_Light;
}DB_IONIQ_RxCAN0_ICU_04_200ms;



#define KATECH_NAME_DB_IONIQ_RxCAN1_Unknown_0x35 11
typedef struct _KATECH_DB_IONIQ_RxCAN1_Unknown_0x35_{
	uint8_t AccelAct;
}DB_IONIQ_RxCAN1_Unknown_0x35;

#define KATECH_NAME_DB_IONIQ_RxCAN1_ESC_01_10ms 12
typedef struct _KATECH_DB_IONIQ_RxCAN1_ESC_01_10ms_{
	float brakecylPrsVal;
}DB_IONIQ_RxCAN1_ESC_01_10ms;


#define KATECH_NAME_DB_IONIQ_RxCAN1_WHL_01_10ms 13
typedef struct _KATECH_DB_IONIQ_RxCAN1_WHL_01_10ms_{
	float WHL_SpdFLVal;
	float WHL_SpdFRVal;
	float WHL_SpdRLVal;
	float WHL_SpdRRVal;
	float Avg_WhlSpd;

}DB_IONIQ_RxCAN1_WHL_01_10ms;

#define KATECH_NAME_DB_IONIQ_RxCAN1_ESC_03_20ms 14
typedef struct _KATECH_DB_IONIQ_RxCAN1_ESC_03_20ms_{
	uint8_t PrkBrakeAct;
	uint8_t SCC_EnblReq;

}DB_IONIQ_RxCAN1_ESC_03_20ms;

#define KATECH_NAME_DB_IONIQ_RxCAN1_CLU_01_20ms 15
typedef struct _KATECH_DB_IONIQ_RxCAN1_CLU_01_20ms_{
	uint8_t CurrentSpeed;

}DB_IONIQ_RxCAN1_CLU_01_20ms;

#define KATECH_NAME_DB_IONIQ_RxCAN1_CLU_01_20ms2 16
typedef struct _KATECH_DB_IONIQ_RxCAN1_CLU_01_20ms2_{
	uint8_t LaneQualityL;
	uint8_t LaneQualityR;
	float LanePositionL;
	float LanePositionR;
	float LaneWidthEstimation;
	float CurvatureL;
	float CurvatureRateL;
	float CurvatureR;
	float CurvatureRateR;
	float HeadingAngleL;
	float HeadingAngleR;
	uint8_t CAMStatus;
	int16_t ThisLanePositionL;
	int16_t ThisLanePositionR;
	int16_t ThisHeadingAngleL;
	int16_t ThisHeadingAngleR;
	int16_t ThisCurvatureL;
	int16_t ThisCurvatureR;
	int16_t ThisCurvatureRateL;
	int16_t ThisCurvatureRateR;
}DB_IONIQ_RxCAN1_CLU_01_20ms2;

#define KATECH_NAME_DB_IONIQ_RxCAN1_RR_C_RDR_02_50ms 17
typedef struct _KATECH_DB_IONIQ_RxCAN1_RR_C_RDR_02_50ms_{
	uint8_t LKALHLnWaringState;
	uint8_t LKARHLnWaringState;
}DB_IONIQ_RxCAN1_RR_C_RDR_02_50ms;

#define KATECH_NAME_DB_IONIQ_RxCAN1_SWRC_03_20ms 18
typedef struct _KATECH_DB_IONIQ_RxCAN1_SWRC_03_20ms_{
	uint8_t SWRC_CrsMainSwSta;
	uint8_t SWRC_PlusMinusSwSta;
	uint8_t PlusSW;
	uint8_t MinusSW;
	uint8_t PauseResumeSW;
	uint8_t SCC_OpSta;
	uint8_t SCC_MainOnOffSta;    // 0 : OFF / 1 : ON
	char SCC_StayTune;
	uint8_t SCC_TrgtSpdSetVal;
	uint16_t SCC_AccelReqVal;
	uint16_t SCC_AccelReqRawVal; // offset = -10.23 / factor = 0.01
	uint8_t CurrentSpeed;

}DB_IONIQ_RxCAN1_SWRC_03_20ms;

#define KATECH_NAME_DB_IONIQ_RxCAN1_MFSW_01_200ms 19
typedef struct _KATECH_DB_IONIQ_RxCAN1_MFSW_01_200ms_{
	uint8_t TurnSigLeft;
	uint8_t TurnSigRight;
	uint8_t LeftTurnFlag;
	uint8_t RightTurnFlag;
}DB_IONIQ_RxCAN1_MFSW_01_200ms;

#define KATECH_NAME_DB_IONIQ_RxCAN2_MDPS_01_10ms 20
typedef struct _KATECH_DB_IONIQ_RxCAN2_MDPS_01_10ms_{
	uint8_t MDPS_PaModeSta;
}DB_IONIQ_RxCAN2_MDPS_01_10ms;

#define KATECH_NAME_DB_IONIQ_RxCAN2_Unknown_0x377 21
typedef struct _KATECH_DB_IONIQ_RxCAN2_Unknown_0x377_{
	uint8_t SPASStatus;
}DB_IONIQ_RxCAN2_Unknown_0x377;

#define KATECH_NAME_DB_IONIQ_RxCAN3_ADAS_CMD_20_20ms 22
typedef struct _KATECH_DB_IONIQ_RxCAN3_ADAS_CMD_20_20ms_{
	uint8_t SCC_ObjSta;
	uint8_t SCC_VehStpReq;
	uint8_t RADARStatus;

	float FrontSCC_ObjDstVal;
	float RadarSCC_ObjRelSpdVal;
	float RadarSCC_ObjLatPosVal;
	uint8_t FrontRadar_fault;
}DB_IONIQ_RxCAN3_ADAS_CMD_20_20ms;

#pragma pack(push,4)
typedef struct {
	uint32_t ts_sec;
	uint32_t ts_nsec;
}KATEHC_VEHICLE_INTERFACE_MSG_INFO;
#pragma pack(pop)

#pragma pack(push,4)
typedef struct {
	uint16_t count;
	uint8_t CurrMode;
	float GearState;
	float YawRate;
	float LateralAccel;
	float LongitudinalAccel;
	float WheelSpeedFL;
	float WheelSpeedFR;
	float WheelSpeedRL;
	float WheelSpeedRR;
	float WheelPulseFL;
	float WheelPulseFR;
	float WheelPulseRL;
	float WheelPulseRR;
	float HandleAngle;
	float HandleSpd;
	float Distance;
	float RelativeVelocity;
	float ClusterVelocity;
	float AccelState;
	float LeftTurnSwitch;
	float RightTurnSwitch;
	unsigned char MDPSmode;
}KATECH_VEHICLE_INTERFACE_INFO;
#pragma pack(pop)

#pragma pack(push,4)
typedef struct{
	uint16_t count;
	uint8_t TargetMode;
	//uint8_t TargetSpeed;
	float Handle; // 4
	float Ax; //SCC_AccelReqVal m/s^2

	float TargetSteeringAngle;
	float TargetSteeringAngleSpeed;
	float TargetSpeed;
	float Accel;
	float Jerk;

	uint8_t ADCM_fault;//����
	uint8_t HMI_fault;//����

	uint8_t GearCommand;
	uint8_t GateMode;
	bool emergency;
	uint8_t TurnIndicatorsCommand;
	uint8_t HazardLightsCommand;
}KATECH_VEHICLE_INTERFACE_CONTROL;
#pragma pack(pop)


#pragma pack(push,4)
typedef struct{
	uint16_t count;
	uint8_t TargetMode;
	//uint8_t TargetSpeed;

	float SteeringAngle;
	float SteeringAngleSpeed;
	float Speed;
	float Accel;
	float Jerk;
	uint8_t ADCM_fault;//����
	uint8_t HMI_fault;//����


	uint8_t GearCommand;
	uint8_t GateMode;
	bool emergency;
	uint8_t TurnIndicatorsCommand;
	uint8_t HazardLightsCommand;
}KATECH_CONTROL_A2V;
#pragma pack(pop)

#pragma pack(push,4)
typedef struct{
	uint16_t count;
	//uint8_t TargetSpeed;
	uint8_t ControlMode;


	// �߰��׸�
	float Tx_YawRate;
	float Tx_LateralAccel;
	float Tx_LongitudinalAccel;
	float Tx_WheelSpeedFL;
	float Tx_WheelSpeedFR;
	float Tx_ClusterVelocity;

	float Tx_WheelSpeedRL;
	float Tx_WheelSpeedRR;
	float Tx_WheelPulseFL;
	float Tx_WheelPulseFR;
	float Tx_WheelPulseRL;
	float Tx_WheelPulseRR;

	uint8_t Tx_LaneQualityL;
	uint8_t Tx_LaneQualityR;
	float Tx_LanePositionL;
	float Tx_LanePositionR;
	float Tx_LaneWidthEstimation;
	float Tx_CurvatureL;
	float Tx_CurvatureRateL;
	float Tx_CurvatureR;
	float Tx_CurvatureRateR;
	float Tx_HeadingAngleL;
	float Tx_HeadingAngleR;
	int16_t Tx_ThisLanePositionL;
	int16_t Tx_ThisLanePositionR;
	int16_t Tx_ThisHeadingAngleL;
	int16_t Tx_ThisHeadingAngleR;
	int16_t Tx_ThisCurvatureL;
	int16_t Tx_ThisCurvatureR;
	int16_t Tx_ThisCurvatureRateL;
	int16_t Tx_ThisCurvatureRateR;
	uint8_t Tx_PrkBrakeAct;
	uint8_t Tx_AccelAct;
	float Tx_FrontSCC_ObjDstVal;
	float Tx_RadarSCC_ObjRelSpdVal;
	float Tx_RadarSCC_ObjLatPosVal;
	uint8_t HazardLightsReport;
	uint8_t TurnIndicatorsReport;
	float Tx_brakecylPrsVal;
	uint8_t Tx_SeatBelt_Status;
	uint8_t Tx_SWRC_CrsMainSwSta;
	uint8_t Tx_SCC_EnblReq;

	uint8_t Tx_Front_fault;//�۾� �ȵ�

	// �߰��׸�
	float energy_level;
	uint8_t GearReport;

	float SteeringTireAngle;
	float LongitudinalVelocity;
	float LateralVelocity;
	float HeadingRate;
}KATECH_CONTROL_V2A;
#pragma pack(pop)

//#pragma pack(pop)
void katech_database_update(uint32_t name_db);
void *katech_database_read_addr_get(uint32_t name_db);
void *katech_database_write_addr_get(uint32_t name_db);
void katech_get_vehicle_interface_info(KATECH_VEHICLE_INTERFACE_INFO *data);
KATECH_CONTROL_A2V *katech_get_addr_vehicle_interface_control(void);
void katech_set_vehicle_interface_control(KATECH_CONTROL_A2V *data);
void katech_get_vehicle_interface_control(KATECH_CONTROL_A2V *data);
void to_autoware(KATECH_CONTROL_V2A *data);
#endif /* KATECH_DATABASE_H_ */
