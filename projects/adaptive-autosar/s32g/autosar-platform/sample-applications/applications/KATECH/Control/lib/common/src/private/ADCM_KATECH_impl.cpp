#include "ADCM_KATECH.h"
#include "common/include/CanSpecProvider.h"

namespace {
const ICanSpecProvider& kCanSpec = GetCanSpecProvider();

double GetF64(const char* key)
{
    return kCanSpec.GetF64(key);
}

std::uint32_t GetU32(const char* key)
{
    return kCanSpec.GetU32(key);
}

void ValidateRequiredCanSpecKeys()
{
    const char* required_keys[] = {
        "calib.control.deg_per_rad",
        "calib.control.gap.upper_bound",
        "calib.control.gravity",
        "calib.control.handle.saturation",
        "calib.control.handle_filter.divisor",
        "calib.control.handle_filter.w1",
        "calib.control.handle_filter.w2",
        "calib.control.handle_filter.w3",
        "calib.control.handle_filter.w4",
        "calib.control.lookahead_gain",
        "calib.control.low_speed.threshold",
        "calib.control.radius_gain.base_high",
        "calib.control.radius_gain.base_low",
        "calib.control.radius_gain.gap_scale",
        "calib.control.radius_gain.speed_scale",
        "calib.control.ref_speed.min",
        "calib.control.ref_speed.scale",
        "calib.control.scc.accel_lower",
        "calib.control.scc.accel_neutral",
        "calib.control.scc.accel_offset",
        "calib.control.scc.accel_scale",
        "calib.control.scc.decel_gain",
        "calib.control.scc.delta_divisor",
        "calib.control.scc.distance_limit",
        "calib.control.scc.lambda",
        "calib.control.scc.safety.base",
        "calib.control.scc.safety.margin",
        "calib.control.scc.stop_accel",
        "calib.control.scc.stop_distance",
        "calib.control.scc.time_to_collision",
        "calib.control.steer_ratio",
        "calib.control.urban_speed.threshold",
        "calib.control.vx.fallback",
        "calib.dynamics.cf",
        "calib.dynamics.cr",
        "calib.dynamics.front_length",
        "calib.dynamics.mass",
        "calib.dynamics.wheelbase",
        "calib.path.density.base",
        "calib.path.density.gain",
        "calib.path.density.velocity_scale",
        "calib.path.lat_to_meter",
        "calib.path.lon_to_meter",
        "calib.vehicle.mps_divisor",
        "control.lati.center_gate.enable",
        "control.lati.center_gate.incheon_min_m",
        "control.lati.center_gate.katech_min_m",
        "control.lati.center_gate.kcity_min_m",
        "control.lati.incline.distance_threshold_m",
        "control.lati.incline.gain_velocity",
        "control.lati.incline.gain_yaw",
        "control.lati.incline.handle_high",
        "control.lati.incline.handle_low",
        "control.lati.incline.low_velocity_mps",
        "control.lati.incline.reference_lat",
        "control.lati.incline.reference_lon",
        "control.lati.incline.velocity_threshold_mps",
        "control.lati.incline.yawrate_threshold",
        "control.lati.left.gain_1",
        "control.lati.left.gain_2",
        "control.lati.left.gain_3",
        "control.lati.left.gain_4",
        "control.lati.left.threshold_1",
        "control.lati.left.threshold_2",
        "control.lati.left.threshold_3",
        "control.lati.left.threshold_4",
        "control.lati.lookahead_min_m",
        "control.lati.right.gain_1",
        "control.lati.right.gain_2",
        "control.lati.right.gain_3",
        "control.lati.right.gain_4_base",
        "control.lati.right.gain_4_turn",
        "control.lati.right.threshold_1",
        "control.lati.right.threshold_2",
        "control.lati.right.threshold_3",
        "control.lati.right.threshold_4",
        "control.lati.s_curve.negative_gain",
        "control.lati.s_curve.positive_gain",
        "control.longi.accel_log_scale",
        "control.longi.distance_input_scale",
        "control.remote.accel_scale",
        "control.remote.brake_scale",
        "control.remote.input_max",
        "fault.default.front_radar",
        "fault.default.hmi",
        "fault.default.lcan",
        "fault.default.mobileye",
        "geo.center.incheon.lat",
        "geo.center.incheon.lon",
        "geo.center.katech.lat",
        "geo.center.katech.lon",
        "geo.center.kcity.lat",
        "geo.center.kcity.lon",
        "path.local.min_distance_init_m",
        "path.local.pose_forward_offset_m",
        "path.relative_yaw.nearest_threshold_m",
        "path.relative_yaw.next_threshold_m",
        "runtime.counter.alive_wrap",
        "runtime.index.local_path_lateral",
        "runtime.log.print_period",
        "state.automode.debounce_count",
        "state.automode.switch_on",
        "state.default.lidar_obj_dist_front_m",
        "state.default.lidar_obj_rel_spd_front_mps",
        "state.default.near_sensor",
        "state.default.scc_target_velocity_mps",
        "state.mdps.auto_mode",
        "state.mdps.fault_min",
        "state.mdps.manual_max",
    };
    for (const char* key : required_keys) {
        (void)kCanSpec.GetString(key);
    }
}

const bool kCanSpecValidated = []() {
    ValidateRequiredCanSpecKeys();
    return true;
}();

const std::uint32_t kLogPrintPeriod = GetU32("runtime.log.print_period");
const std::uint32_t kAliveWrapValue = GetU32("runtime.counter.alive_wrap");
const std::uint32_t kLocalPathLateralIndex = GetU32("runtime.index.local_path_lateral");

const double kControlGravity = GetF64("calib.control.gravity");
const double kControlLookAheadGain = GetF64("calib.control.lookahead_gain");
const double kControlTargetVelocityScale = GetF64("calib.control.target_velocity_scale");
const double kSccDistanceLimit = GetF64("calib.control.scc.distance_limit");
const double kSccSafetyBase = GetF64("calib.control.scc.safety.base");
const double kSccSafetyMargin = GetF64("calib.control.scc.safety.margin");
const double kSccStopDistance = GetF64("calib.control.scc.stop_distance");
const double kSccStopAcceleration = GetF64("calib.control.scc.stop_accel");
const double kSccLambda = GetF64("calib.control.scc.lambda");
const double kSccTimeToCollision = GetF64("calib.control.scc.time_to_collision");
const double kSccDeltaDivisor = GetF64("calib.control.scc.delta_divisor");
const double kSccAccelOffset = GetF64("calib.control.scc.accel_offset");
const double kSccAccelScale = GetF64("calib.control.scc.accel_scale");
const double kSccAccelNeutral = GetF64("calib.control.scc.accel_neutral");
const double kSccDecelGain = GetF64("calib.control.scc.decel_gain");
const double kSccAccelLowerBound = GetF64("calib.control.scc.accel_lower");
const double kRefSpeedScale = GetF64("calib.control.ref_speed.scale");
const double kRefSpeedMin = GetF64("calib.control.ref_speed.min");
const double kUrbanSpeedThreshold = GetF64("calib.control.urban_speed.threshold");
const double kLowSpeedThreshold = GetF64("calib.control.low_speed.threshold");
const double kVxFallback = GetF64("calib.control.vx.fallback");
const double kGapUpperBound = GetF64("calib.control.gap.upper_bound");
const double kRadiusGainBaseLow = GetF64("calib.control.radius_gain.base_low");
const double kRadiusGainGapBase = GetF64("calib.control.radius_gain.gap_base");
const double kRadiusGainGapScale = GetF64("calib.control.radius_gain.gap_scale");
const double kSteerRatio = GetF64("calib.control.steer_ratio");
const double kDegPerRad = GetF64("calib.control.deg_per_rad");
const double kRadiusGainBaseHigh = GetF64("calib.control.radius_gain.base_high");
const double kRadiusGainSpeedScale = GetF64("calib.control.radius_gain.speed_scale");
const double kHandleGapScale = GetF64("calib.control.handle_gap.scale");
const double kHandleSaturation = GetF64("calib.control.handle.saturation");
const double kHandleFilterW1 = GetF64("calib.control.handle_filter.w1");
const double kHandleFilterW2 = GetF64("calib.control.handle_filter.w2");
const double kHandleFilterW3 = GetF64("calib.control.handle_filter.w3");
const double kHandleFilterW4 = GetF64("calib.control.handle_filter.w4");
const double kHandleFilterDivisor = GetF64("calib.control.handle_filter.divisor");
}  // namespace

uint8_t MDPSMode = 0;

int sock_L;

struct Dynamics
{
    float m = static_cast<float>(GetF64("calib.dynamics.mass"));
    float WB = static_cast<float>(GetF64("calib.dynamics.wheelbase"));
    float lf = static_cast<float>(GetF64("calib.dynamics.front_length"));
    float lr = WB - lf;
    float wf = lr / (WB * m * kControlGravity);
    float wr = lf / (WB * m * kControlGravity);
    float cf = static_cast<float>(GetF64("calib.dynamics.cf"));
    float cr = static_cast<float>(GetF64("calib.dynamics.cr"));
} IONIC5;

namespace KATECH
{
adcm::Vector3DStruct position;
double targetAx = 0;
bool driveMode = 0;
float Lidar_ObjDistVal_Front = static_cast<float>(GetF64("state.default.lidar_obj_dist_front_m"));
float Lidar_ObjRelSpdVal_Front = static_cast<float>(GetF64("state.default.lidar_obj_rel_spd_front_mps"));
float SCC_TargetVelocity = static_cast<float>(GetF64("state.default.scc_target_velocity_mps"));

// using SCC
float Radar_ObjDistVal_Front = 0;
float Radar_ObjRelSpdVal_Front = 0;

double WheelSpeed_FL = 0;  // [kph]
double WheelSpeed_FR = 0;  // [kph]
double WheelSpeed_RL = 0;  // [kph]
double WheelSpeed_RR = 0;  // [kph]
double ClusterSpeed = 0;  // [kph]
uint8_t GearState = 0;
uint8_t HandleState = 0;
double LateralAcceleration = 0;  // [m/s^2]
double LongitudinalAcceleration = 0;  // [m/s^2]
double Angular_Acceleration = 0;  // Yawrate [rad/s]
uint8_t Turn_Signal = 0;  // OFF:0 , LeftON:1, RightON:2 / command / from ADCM
uint8_t Turn_SignalReport = 0;  // OFF:0 , LeftON:1, RightON:2 / Report
bool AutoDrivingMode = 0;  // OFF:0, ON:1
uint8_t Emergency_Light = 0;  // OFF:0 , ON:1
bool No_Autonomous_Driving = 0;  // Auto:0 , NO_Auto:1
double SteeringWheelAngle = 0;  // [degree]
double LanePosition_Left = 0;  // [m]
double LanePosition_Right = 0;  // [m]
double LaneHeadingAngle_Left = 0;  // [rad]
double LaneHeadingAngle_Right = 0;  // [rad]
double LaneCurvature_Left = 0;  // [1/m]
double LaneCurvature_Right = 0;  // [1/m]
double LaneCurvatureRate_Left = 0;  // [1/m^2]
double LaneCurvatureRate_Right = 0;  // [1/m^2]
double ControlHandle = 0;
double ControlAx = 0;
double ControlHandle_Log = 0;
double ControlAx_Log = 0;

float CornerRadar_x1 = 0;
float CornerRadar_y1 = 0;
float CornerRadar_x2 = 0;
float CornerRadar_y2 = 0;
float CornerRadar_x3 = 0;
float CornerRadar_y3 = 0;
float CornerRadar_x4 = 0;
float CornerRadar_y4 = 0;

float CornerRadar2_x1 = 0;
float CornerRadar2_y1 = 0;
float CornerRadar2_x2 = 0;
float CornerRadar2_y2 = 0;
float CornerRadar2_x3 = 0;
float CornerRadar2_y3 = 0;
float CornerRadar2_x4 = 0;
float CornerRadar2_y4 = 0;

float CornerRadar3_x1 = 0;
float CornerRadar3_y1 = 0;
float CornerRadar3_x2 = 0;
float CornerRadar3_y2 = 0;
float CornerRadar3_x3 = 0;
float CornerRadar3_y3 = 0;
float CornerRadar3_x4 = 0;
float CornerRadar3_y4 = 0;

float CornerRadar4_x1 = 0;
float CornerRadar4_y1 = 0;
float CornerRadar4_x2 = 0;
float CornerRadar4_y2 = 0;
float CornerRadar4_x3 = 0;
float CornerRadar4_y3 = 0;
float CornerRadar4_x4 = 0;
float CornerRadar4_y4 = 0;

uint8_t NearSensor = 0;

float WheelPulse_FL = 0;
float WheelPulse_FR = 0;
float WheelPulse_RL = 0;
float WheelPulse_RR = 0;
float AccelCylPrs = 0;
float brakecylPrs = 0;

uint8_t ADCMFault = 0;
uint16_t vcualivecnt = 0;
uint8_t VCUFault = 0;
uint8_t MobileyeFault = 0;
uint8_t LCANFault = 0;
uint8_t FrontRadarFault = 0;
uint8_t HMIFault = static_cast<uint8_t>(GetU32("fault.default.hmi"));
uint8_t SeatBeltCheck = 0;  // 0이 미착용, 1이 착용
uint8_t LongiError = 0;

uint8_t AutoModeSw;
uint8_t AutoModeSwcnt;

}  // namespace KATECH

FILE* KATECH::log::StateFile;
FILE* KATECH::log::GlobalPathFile;
FILE* KATECH::log::LocalPathFile;
char KATECH::log::StatePath[KATECH::log::kLogPathBufferSize],
    KATECH::log::GlobalPathPath[KATECH::log::kLogPathBufferSize],
    KATECH::log::LocalPathPath[KATECH::log::kLogPathBufferSize];
uint64_t KATECH::log::maincnt;

std::chrono::steady_clock::time_point KATECH::log::current_time;
std::chrono::steady_clock::time_point KATECH::log::old_time;
long long KATECH::log::TimeGap;

double IncheonCenterDistance = 0;
double KatechCenterDistance = 0;
double KCityCenterDistance = 0;
std::vector<KATECH::UtmKStruct>::iterator minItr, StartItr, LastItr, EndItr;
double debugHandle = 0, debugGap = 0;
bool RightTurnFlag = 0, LeftTurnFlag = 0;
double HandleGain;

/* ---------------------  katech_ipcf function -----------------------------
------------------------ integration ipcf whih beyless autosar ------------
*/
namespace KATECH
{
// int katech_ipcf_fd;
// KATECH_CONTROL_V2A rx_buff[2];

// void katech_ipcf_signal_io_fun(int signum)
// {
//     adcm::Log::Info() << "[SIGIO] Received signal: " << signum;
//     int ret = read(katech_ipcf_fd, &rx_buff[(rx_cnt + 1) % 2], sizeof(KATECH_CONTROL_V2A));
//     adcm::Log::Info() << "[SIGIO] read return: " << ret;
//     if (ret < 0)
//         perror("read error");
//     rx_cnt++;
// }

// void katech_ipcf_signal_int_fun(int signum)
// {
//     adcm::Log::Info() << "signum: 0x" << signum;
//     if (katech_ipcf_fd >= 0) close(katech_ipcf_fd);
//     exit(signum);
// }

// void katech_ipcf_read(KATECH_CONTROL_V2A *data)
// {
//     adcm::Log::Info() << "ReceiveBuffer() rx_cnt=" << rx_cnt;
//     memcpy(data, &rx_buff[rx_cnt%2], sizeof(KATECH_CONTROL_V2A));
// }

// void katech_ipcf_send(KATECH_CONTROL_A2V *data)
// {
//     int write_ret;
// 	write_ret = write(katech_ipcf_fd, data, sizeof(KATECH_CONTROL_A2V));
// }

// int katech_ipcf_init(void)
// {
//     adcm::Log::Info()<<"katech ipcf init start";
//     int Oflags;

//     // signal(SIGIO, katech_ipcf_signal_io_fun);
//     if (signal(SIGIO, katech_ipcf_signal_io_fun) == SIG_ERR) {
//         perror("signal(SIGIO) failed");
//         adcm::Log::Info() << "signal failed";
//     }
//     signal(SIGINT, katech_ipcf_signal_int_fun);

//     katech_ipcf_fd = open("/dev/mem", O_RDWR);
//     if (katech_ipcf_fd < 0)
//     {
//         adcm::Log::Error() << "[SHM] open(/dev/kgateway) failed: " << strerror(errno);
//         return -1;
//     }
//     //printf("open OK, fd = 0x%x\n", katech_ipcf_fd);
//     fcntl(katech_ipcf_fd, F_SETOWN, getpid());
//     Oflags = fcntl(katech_ipcf_fd, F_GETFL);
//     fcntl(katech_ipcf_fd, F_SETFL, Oflags | FASYNC);

//     return 0;
// }

// ========================= Shared Memory =========================
static int shm_fd = -1;  // /dev/mem 파일 디스크립터
static volatile SharedMemory_t* ap_local;
static volatile SharedMemory_t* ap_remote;

char katech_shm_status_flag = 0;

void shm_memcpy_toio(void* dst, const void* src, size_t len)
{
    const uint8_t* s = (const uint8_t*)src;
    volatile uint8_t* d = (volatile uint8_t*)dst;
    while (len--) {
        *d++ = *s++;
    }
}

void shm_memcpy_fromio(void* dst, const void* src, size_t len)
{
    uint8_t* d = (uint8_t*)dst;
    const volatile uint8_t* s = (const volatile uint8_t*)src;
    while (len--) {
        *d++ = *s++;
    }
}

int katech_ipcf_init(void)
{
    adcm::Log::Info() << "[SHM] init start";

    // 1. /dev/mem open
    shm_fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (shm_fd < 0) {
        adcm::Log::Error() << "[SHM] open(/dev/mem) failed: " << strerror(errno);
        return -1;
    }

    size_t page_size = sysconf(_SC_PAGESIZE);
    off_t base_rx = AP_REMOTE_ADDR & ~(page_size - 1);
    off_t base_tx = AP_LOCAL_ADDR & ~(page_size - 1);

    size_t off_rx = AP_REMOTE_ADDR - base_rx;
    size_t off_tx = AP_LOCAL_ADDR - base_tx;

    size_t map_len_rx = SHM_BYTES + off_rx;
    size_t map_len_tx = SHM_BYTES + off_tx;

    void* map_rx = mmap(NULL, map_len_rx, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, base_rx);
    if (map_rx == MAP_FAILED) {
        adcm::Log::Error() << "[SHM] mmap RX failed: " << strerror(errno);
        close(shm_fd);
        return -1;
    }

    void* map_tx = mmap(NULL, map_len_tx, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, base_tx);
    if (map_tx == MAP_FAILED) {
        adcm::Log::Error() << "[SHM] mmap TX failed: " << strerror(errno);
        munmap(map_rx, map_len_rx);
        close(shm_fd);
        return -1;
    }

    ap_remote = (SharedMemory_t*)((uint8_t*)map_rx + off_rx);
    ap_local = (SharedMemory_t*)((uint8_t*)map_tx + off_tx);

    for (int i = 0; i < CHANNEL_COUNT; i++) {
        ap_local->ch[i].head = 0;
        ap_local->ch[i].len = 0;
    }
    __sync_synchronize();

    adcm::Log::Info() << "[SHM] init success";
    return 0;
}

// ===== RX 상태 확인 =====
char katech_shm_status(void)
{
    for (int i = 0; i < CHANNEL_COUNT; i++) {
        if (ap_remote->ch[i].head == 1) {
            katech_shm_status_flag = 1;
            return 1;
        }
    }
    katech_shm_status_flag = 0;
    return 0;
}

void katech_ipcf_read(KATECH_RX_Packet* rx, RxPayloadType type)
{
    if (!rx) {
        adcm::Log::Error() << "[SHM RX] data buffer NULL!";
        return;
    }

    const int channel_id = get_channel_id(type);
    volatile ChannelBuffer_t* ch = &ap_remote->ch[channel_id];

    if (ch->head == 0)
        return;  // 데이터 없음

    if (ch->len == 0 || ch->len > SHM_SLOT_SIZE) {
        adcm::Log::Error() << "[SHM RX] CH" << channel_id 
                           << " invalid len=" << ch->len;
        ch->head = 0;
        return;
    }

    switch (type) {
    case RxPayloadType::V2A: {
        size_t n = ch->len < sizeof(KATECH_CONTROL_V2A) ? ch->len : sizeof(KATECH_CONTROL_V2A);
        shm_memcpy_fromio(&rx->payload.v2a, (const void*)ch->data, n);
        if (rx->payload.v2a.count % kLogPrintPeriod == 0) {
            adcm::Log::Info() << "[SHM RX] CH" << channel_id 
                              << " v2a.count=" << rx->payload.v2a.count
                              << " energy=" << rx->payload.v2a.energy_level;
        }
        break;
    }
    case RxPayloadType::VEHICLE_INFO: {
        size_t n = ch->len < sizeof(KATECH_VEHICLE_INTERFACE_INFO) ? ch->len : sizeof(KATECH_VEHICLE_INTERFACE_INFO);
        shm_memcpy_fromio(&rx->payload.info, (const void*)ch->data, n);
        if (rx->payload.info.count % kLogPrintPeriod == 0) {
            adcm::Log::Info() << "[SHM RX] CH" << channel_id 
                              << " info.count=" << rx->payload.info.count
                              << " GearState=" << rx->payload.info.GearState
                              << " ClusterVelocity=" << rx->payload.info.ClusterVelocity;
        }
        break;
    }
    default:
        adcm::Log::Error() << "[SHM RX] Unknown payload type!";
        return;
    }

    __sync_synchronize();
    ch->head = 0;  // consume
}

void katech_ipcf_send(KATECH_TX_Packet* tx, TxPayloadType type)
{
    if (!tx) {
        adcm::Log::Error() << "[SHM TX] data buffer NULL!";
        return;
    }

    const int channel_id = static_cast<int>(type);
    volatile ChannelBuffer_t* ch = &ap_local->ch[channel_id];

    if (ch->head != 0) {
        if (tx->payload.a2v.count % kLogPrintPeriod == 0) {
            adcm::Log::Warn() << "[SHM TX] CH" << channel_id 
                              << " busy, cannot send! count=" << tx->payload.a2v.count;
        }
        return;
    }

    switch (type) {
    case TxPayloadType::A2V: {
        shm_memcpy_toio((void*)ch->data, (const void*)&tx->payload.a2v, sizeof(KATECH_CONTROL_A2V));
        ch->len = sizeof(KATECH_CONTROL_A2V);
        if (tx->payload.a2v.count % kLogPrintPeriod == 0) {
            adcm::Log::Info() << "[SHM TX] CH" << channel_id 
                              << " count=" << tx->payload.a2v.count
                              << " Accel=" << tx->payload.a2v.Accel
                              << " SteeringAngle=" << tx->payload.a2v.SteeringAngle
                              << " TargetMode=" << tx->payload.a2v.TargetMode
                              << " Turn Signal=" << tx->payload.a2v.TurnIndicatorsCommand;
        }
        break;
    }
    }

    __sync_synchronize();
    ch->head = 1;
}

// // ===== MCU→AP 읽기 =====
// void katech_ipcf_read(KATECH_CONTROL_V2A* data)
// {
//     if (!data) {
//         adcm::Log::Error() << "[SHM] data buffer NULL!";
//         return;
//     }

//     for (int i = 0; i < CHANNEL_COUNT; i++) {
//         volatile ChannelBuffer_t* ch = &ap_remote->ch[i];
//         if (ch->head == 1) {
//             if (ch->len == 0 || ch->len > SHM_SLOT_SIZE) {
//                 adcm::Log::Error() << "[SHM] invalid len=" << ch->len;
//                 ch->head = 0;
//                 continue;
//             }

//             size_t copy_len = (ch->len <= sizeof(KATECH_CONTROL_V2A))
//                                 ? ch->len : sizeof(KATECH_CONTROL_V2A);

//             shm_memcpy_fromio((void*)data, (const void*)ch->data, copy_len);
//             __sync_synchronize();
//             ch->head = 0; // consume

//                 adcm::Log::Info() << "[SHM RX] count=" << data->count
//                                   << " recieve on CH" << i
//                                   << " energy_level=" << data->energy_level
//                                   << " SteeringAngle=" << data->SteeringTireAngle;
//             return;
//         }
//     }
// }

// // ===== AP→MCU 쓰기 =====
// void katech_ipcf_send(KATECH_CONTROL_A2V* data)
// {
//     for (int i = 0; i < CHANNEL_COUNT; i++) {
//         volatile ChannelBuffer_t* ch = &ap_local->ch[i];
//         if (ch->head == 0) {
//             shm_memcpy_toio((void*)ch->data, data, sizeof(KATECH_CONTROL_A2V));

//             ch->len = sizeof(KATECH_CONTROL_A2V);
//             __sync_synchronize();
//             ch->head = 1;

//                 adcm::Log::Info() << "[SHM TX] count=" << data->count << "sent on CH" << i;
//             return;
//         }
//     }
//         adcm::Log::Warn() << "[SHM TX] no free channel buffer! count="
//                           << data->count;
//     }
// }

}  // namespace KATECH

// -------------------------------- Private Function ----------------------------------//

void KATECH::func::Initialize(double X,
    double Y,
    double Yaw,
    TrajectoryPointArray Path,
    double Accel,
    bool drvmode,
    float LidarDist,
    float LidarRelSpd,
    float Tspd)
{
    KATECH::position.x = X;
    KATECH::position.y = Y;
    KATECH::position.yaw = Yaw;
    KATECH::targetAx = Accel;
    KATECH::driveMode = drvmode;

    Pos.x = X;
    Pos.y = Y;
    Pos.yaw = Yaw;  // wgs84

    GlobalPath.clear();
    LocalPath.clear();
    for (typename std::vector<adcm::TrajectoryPoint>::iterator itr = Path.begin(); itr != Path.end(); ++itr) {
        UtmKStruct Cache;
        Cache.x = itr->Vector3D.x;
        Cache.y = itr->Vector3D.y;
        Cache.Interval = 0;

        GlobalPath.push_back(Cache);
    }
    // INFO("GlobalPath.size:%d",(int)GlobalPath.size());
    // INFO("[x,y,yaw]=[%.5lf,%.5lf], [x0,y0]=[%.5lf,%.5lf], [xend,yend]=[%.5lf,%.5lf]", Pos.x,Pos.y,
    // GlobalPath.begin()->x, GlobalPath.begin()->y, (GlobalPath.end()-1)->x, (GlobalPath.end()-1)->y);
}

void KATECH::func::SCC()
{
    const double kSccDistanceInputScale = GetF64("control.longi.distance_input_scale");
    const double kSccAccelLogScale = GetF64("control.longi.accel_log_scale");

    const float SccObjectDistance = KATECH::Lidar_ObjDistVal_Front * static_cast<float>(kSccDistanceInputScale);
    float Delta = 0;
    float SafetyDistance = 0;
    float Lambda = static_cast<float>(kSccLambda);
    float TimeToCollision = static_cast<float>(kSccTimeToCollision);
    const double TargetVelocity = KATECH::SCC_TargetVelocity;

    if (SccObjectDistance <= kSccDistanceLimit) {
        SafetyDistance = static_cast<float>(kSccSafetyBase + (TimeToCollision * Velocity) + kSccSafetyMargin);
        Delta = -SccObjectDistance + (SafetyDistance * static_cast<float>(kSccDistanceInputScale));

        if (SccObjectDistance <= kSccStopDistance) {
            KATECH::ControlAx = kSccStopAcceleration;
        } else {
            KATECH::ControlAx = ((-(((-KATECH::Lidar_ObjRelSpdVal_Front) + (Lambda * (Delta / kSccDeltaDivisor)))
                                     / TimeToCollision))
                                    + kSccAccelOffset)
                * kSccAccelScale;

            if ((KATECH::ControlAx > kSccAccelNeutral) && (Velocity >= TargetVelocity)) {
                KATECH::ControlAx = kSccAccelNeutral - ((Velocity - TargetVelocity) * kSccDecelGain);
            }
        }
    } else {
        KATECH::ControlAx = (KATECH::targetAx * kSccAccelScale) + kSccAccelNeutral;
    }

    if (KATECH::ControlAx < kSccAccelLowerBound) {
        KATECH::ControlAx = kSccAccelLowerBound;
    }

    KATECH::ControlAx_Log = (KATECH::ControlAx - kSccAccelNeutral) * kSccAccelLogScale;
    // SCC_ObjDistVal, KATECH::Lidar_ObjRelSpdVal_Front, KATECH::Radar_ObjDistVal_Front,
    // KATECH::Radar_ObjRelSpdVal_Front);
}

void KATECH::func::GenerateLocalPath()
{
    const double min_distance_init = GetF64("path.local.min_distance_init_m");
    const double path_density_base = GetF64("calib.path.density.base");
    const double kPathDensityVelocityScale = GetF64("calib.path.density.velocity_scale");
    const double path_density_speed_gain = GetF64("calib.path.density.gain");
    const double geo_scale_x = GetF64("calib.path.lon_to_meter");
    const double geo_scale_y = GetF64("calib.path.lat_to_meter");
    const double pose_forward_offset = GetF64("path.local.pose_forward_offset_m");

    double sum = 0, diffsum = 0, average = 0, StandardDeviation = 0;
    double minDistance = min_distance_init, maxInterval = 0, distance = 0, FrontDistance = 0, PathDencity = 0;
    uint32_t VertexNum = 0;
    double CorrectionX, CorrectionY;
    minItr = GlobalPath.end();

    for (std::vector<UtmKStruct>::iterator itr = GlobalPath.begin(); itr != GlobalPath.end() - 1; ++itr) {
        distance = CalculateDistance(itr->x, std::next(itr)->x, itr->y, std::next(itr)->y);
        itr->Interval = distance;
        sum += distance;
        if (distance > maxInterval)
            maxInterval = distance;
    }
    average = sum / GlobalPath.size();
    for (std::vector<UtmKStruct>::iterator itr = GlobalPath.begin(); itr != GlobalPath.end(); ++itr) {
        diffsum += pow((itr->Interval - average), 2);
        distance = CalculateDistance(Pos.x, itr->x, Pos.y, itr->y);
        if (distance < minDistance) {
            minDistance = distance;
            minItr = itr;
        }
    }
    StandardDeviation = std::sqrt(diffsum / GlobalPath.size());

    Gap = minDistance;

    if (minItr == GlobalPath.end() - 1)
        minItr = std::prev(minItr);
    LastItr = minItr;
    StartItr = minItr;
    EndItr = std::next(minItr);

    for (; EndItr < GlobalPath.end() - 1; ++EndItr) {
        distance = CalculateDistance(Pos.x, EndItr->x, Pos.y, EndItr->y);
        if (distance > PathLength)
            break;
    }

    FrontDistance = 0;
    for (std::vector<UtmKStruct>::iterator itr = StartItr; itr < EndItr; ++itr) {
        FrontDistance += itr->Interval;
    }
    PathDencity = path_density_base + (Velocity * kPathDensityVelocityScale) * path_density_speed_gain;
    VertexNum = (uint32_t)(FrontDistance / PathDencity);

    CorrectionX = Pos.x * geo_scale_x + pose_forward_offset * cos(Pos.yaw);
    CorrectionY = Pos.y * geo_scale_y + pose_forward_offset * sin(Pos.yaw);
    Heading = Pos.yaw * (-1);
    // Heading = Pos.yaw;

    if (VertexNum) {
        LocalPath.resize(VertexNum);
        for (uint32_t i = 1; i < VertexNum; i++) {
            FrontDistance = 0;
            for (std::vector<UtmKStruct>::iterator itr = StartItr; itr < EndItr; ++itr) {
                FrontDistance += itr->Interval;
                if (FrontDistance <= PathDencity * i) {
                    LocalPath[i].X = (itr->x * geo_scale_x - CorrectionX) * cos(Heading)
                        + (itr->y * geo_scale_y - CorrectionY) * (-1) * sin(Heading);
                    LocalPath[i].Y = (itr->x * geo_scale_x - CorrectionX) * sin(Heading)
                        + (itr->y * geo_scale_y - CorrectionY) * cos(Heading);
                } else
                    break;
            }
        }
    }
    // adcm::Log::Info() << "[GenerateLocalPath] LocalPathSize:" << (int)LocalPath.size();
}

void KATECH::func::PurePursuit()
{
    const double kLookAheadMin = GetF64("control.lati.lookahead_min_m");
    const double kInclineRefLon = GetF64("control.lati.incline.reference_lon");
    const double kInclineRefLat = GetF64("control.lati.incline.reference_lat");
    const double kInclineVelocityThreshold = GetF64("control.lati.incline.velocity_threshold_mps");
    const double kInclineDistanceThreshold = GetF64("control.lati.incline.distance_threshold_m");

    const double kRightThreshold1 = GetF64("control.lati.right.threshold_1");
    const double kRightThreshold2 = GetF64("control.lati.right.threshold_2");
    const double kRightThreshold3 = GetF64("control.lati.right.threshold_3");
    const double kRightThreshold4 = GetF64("control.lati.right.threshold_4");
    const double kRightGain1 = GetF64("control.lati.right.gain_1");
    const double kRightGain2 = GetF64("control.lati.right.gain_2");
    const double kRightGain3 = GetF64("control.lati.right.gain_3");
    const double kRightGain4Turn = GetF64("control.lati.right.gain_4_turn");
    const double kRightGain4Base = GetF64("control.lati.right.gain_4_base");

    const double kLeftThreshold1 = GetF64("control.lati.left.threshold_1");
    const double kLeftThreshold2 = GetF64("control.lati.left.threshold_2");
    const double kLeftThreshold3 = GetF64("control.lati.left.threshold_3");
    const double kLeftThreshold4 = GetF64("control.lati.left.threshold_4");
    const double kLeftGain1 = GetF64("control.lati.left.gain_1");
    const double kLeftGain2 = GetF64("control.lati.left.gain_2");
    const double kLeftGain3 = GetF64("control.lati.left.gain_3");
    const double kLeftGain4 = GetF64("control.lati.left.gain_4");

    const double kInclineYawRateThreshold = GetF64("control.lati.incline.yawrate_threshold");
    const double kInclineLowVelocity = GetF64("control.lati.incline.low_velocity_mps");
    const double kInclineHandleLow = GetF64("control.lati.incline.handle_low");
    const double kInclineHandleHigh = GetF64("control.lati.incline.handle_high");
    const double kInclineGainYaw = GetF64("control.lati.incline.gain_yaw");
    const double kInclineGainVelocity = GetF64("control.lati.incline.gain_velocity");

    const double kSCurveNegativeGain = GetF64("control.lati.s_curve.negative_gain");
    const double kSCurvePositiveGain = GetF64("control.lati.s_curve.positive_gain");

    const bool kCenterGateEnabled = GetU32("control.lati.center_gate.enable") != 0U;
    const double kCenterIncheonLon = GetF64("geo.center.incheon.lon");
    const double kCenterIncheonLat = GetF64("geo.center.incheon.lat");
    const double kCenterKatechLon = GetF64("geo.center.katech.lon");
    const double kCenterKatechLat = GetF64("geo.center.katech.lat");
    const double kCenterKcityLon = GetF64("geo.center.kcity.lon");
    const double kCenterKcityLat = GetF64("geo.center.kcity.lat");
    const double kCenterIncheonMin = GetF64("control.lati.center_gate.incheon_min_m");
    const double kCenterKatechMin = GetF64("control.lati.center_gate.katech_min_m");
    const double kCenterKcityMin = GetF64("control.lati.center_gate.kcity_min_m");

    double CurrentDistance;
    double RefSpeed;
    double LookAheadDistance;
    double RadiusGain;
    double YawRate = KATECH::Angular_Acceleration;
    std::vector<LocalStruct>::iterator LookaheadItr;
    double K, R;  // Radius of path. VehicleCoordinate [0 -> Lookaheadidx]
    static double Handle = 0, Handle_1 = 0, Handle_2 = 0, Handle_3 = 0, Handle_4 = 0;  // PurePursuit
    double Vx = Velocity;

    double Gap = this->Gap;
    double InclineDistance = 0, RightCurveDistance = 0;

    IncheonCenterDistance = CalculateDistance(Pos.x, kCenterIncheonLon, Pos.y, kCenterIncheonLat);
    KatechCenterDistance = CalculateDistance(Pos.x, kCenterKatechLon, Pos.y, kCenterKatechLat);
    KCityCenterDistance = CalculateDistance(Pos.x, kCenterKcityLon, Pos.y, kCenterKcityLat);
    if (kCenterGateEnabled && (IncheonCenterDistance >= kCenterIncheonMin) && (KatechCenterDistance >= kCenterKatechMin)
        && (KCityCenterDistance >= kCenterKcityMin)) {
        KATECH::ControlHandle = 0;
        return;
    }

    RefSpeed = Velocity * kRefSpeedScale;
    if (RefSpeed < kRefSpeedMin)
        RefSpeed = kRefSpeedMin;
    LookAheadDistance = RefSpeed * kControlLookAheadGain;

    if (LocalPath.size()) {
        LookaheadLength = 0;
        for (std::vector<LocalStruct>::iterator itr = LocalPath.begin() + 1; itr != LocalPath.end(); ++itr) {
            CurrentDistance = sqrt(pow(itr->X, 2) + pow(itr->Y, 2));
            if (CurrentDistance < LookAheadDistance) {
                LookaheadItr = itr;
                LookaheadLength = CurrentDistance;
            }
        }

        if (LookaheadLength < kLookAheadMin) {
            // ControlHandle = Handle_1;
            ControlHandle = 0;
            adcm::Log::Info() << "[LatiControl] Fail -- LookaheadLength :" << LookaheadLength;
            return;
        }

        HeadingError = atan2(LookaheadItr->Y, LookaheadItr->X);

        // 경사로인지 체크
        InclineDistance = CalculateDistance(Pos.x, kInclineRefLon, Pos.y, kInclineRefLat);
        if (Velocity < kInclineVelocityThreshold) {
            if (InclineDistance > kInclineDistanceThreshold) {
                Handle = KATECH::ControlHandle;
            }
        } else if ((Velocity * kRefSpeedScale) <= kLowSpeedThreshold)
        {
            if (Velocity < kVxFallback)
                Vx = kVxFallback;
            if (Gap > kGapUpperBound)
                Gap = kGapUpperBound;

            RadiusGain = kRadiusGainBaseLow + (kRadiusGainGapBase - Gap) * kRadiusGainGapScale;
            R = (RadiusGain * RadiusGain + (sin(HeadingError) * sin(HeadingError))) / (2 * sin(HeadingError));

            Handle = kSteerRatio * (IONIC5.WB / R) * kDegPerRad;
            debugHandle = Handle;
            if (Handle < 0) {
                if (Handle < kRightThreshold1) {
                    HandleGain = kRightGain1;
                    RightTurnFlag = 1;
                } else if (Handle < kRightThreshold2) {
                    HandleGain = kRightGain2;
                    RightTurnFlag = 1;
                } else if (Handle < kRightThreshold3) {
                    if (RightTurnFlag == 1)
                        HandleGain = kRightGain3;
                    else
                        HandleGain = 0;
                } else if (Handle < kRightThreshold4) {
                    HandleGain = kRightGain4Base;
                    if (RightTurnFlag == 1)
                        HandleGain = kRightGain4Turn;
                    else
                        HandleGain = kRightGain4Base;
                } else {
                    HandleGain = 0;
                    RightTurnFlag = 0;
                }

                if (InclineDistance < kInclineDistanceThreshold) {
                    if ((abs(YawRate) > kInclineYawRateThreshold) && (Handle < kInclineHandleHigh)
                        && (Handle > kInclineHandleLow))
                        HandleGain = kInclineGainYaw;
                    if ((Velocity < kInclineLowVelocity) && (Handle < kInclineHandleHigh) && (Handle > kInclineHandleLow))
                        HandleGain = kInclineGainVelocity;
                }
                if (Handle > (HandleGain * (Gap / (Vx)) * kHandleGapScale))
                    HandleGain = 0;
            } else {
                if (Handle > kLeftThreshold1) {
                    HandleGain = kLeftGain1;
                    LeftTurnFlag = 1;
                } else if (Handle > kLeftThreshold2) {
                    HandleGain = kLeftGain2;
                    LeftTurnFlag = 1;
                } else if (Handle > kLeftThreshold3) {
                    if (LeftTurnFlag == 1)
                        HandleGain = kLeftGain3;
                    else
                        HandleGain = 0;
                } else if (Handle > kLeftThreshold4) {
                    if (LeftTurnFlag == 1)
                        HandleGain = kLeftGain3;
                    else
                        HandleGain = kLeftGain4;
                } else {
                    HandleGain = 0;
                    LeftTurnFlag = 0;
                }

                if (Handle < (HandleGain * (Gap / (Vx)) * kHandleGapScale))
                    HandleGain = 0;
            }
            Handle = Handle + HandleGain * (Gap / (Vx)) * kHandleGapScale;
        } else if ((Velocity * kRefSpeedScale) <= kUrbanSpeedThreshold)
        {
            if (Velocity < kVxFallback)
                Vx = kVxFallback;
            if (Gap > kGapUpperBound)
                Gap = kGapUpperBound;

            RadiusGain = kRadiusGainBaseHigh + (RefSpeed * kRadiusGainSpeedScale);
            R = (RadiusGain * RadiusGain + (sin(HeadingError) * sin(HeadingError))) / (2 * sin(HeadingError));

            Handle = kSteerRatio * (IONIC5.WB / R) * kDegPerRad;

            if (Handle < 0)
                HandleGain = kSCurveNegativeGain;
            else
                HandleGain = kSCurvePositiveGain;

            Handle = Handle + HandleGain * (Gap / (Vx)) * kHandleGapScale;
        } else
        {
            R = LookAheadDistance / (2 * sin(HeadingError));
            K = ((Velocity * Velocity) / (R * kControlGravity)) * (IONIC5.wf / IONIC5.cf - IONIC5.wr / IONIC5.cr);
            Handle = kSteerRatio * ((IONIC5.WB / R) * kDegPerRad + K);
        }
    } else {
        adcm::Log::Info() << "[LatiControl] LocalPathSize():0";
        Handle = Handle_1;
    }

    if (Handle > kHandleSaturation)
        Handle = kHandleSaturation;
    else if (Handle < -kHandleSaturation)
        Handle = -kHandleSaturation;
    Handle_4 = Handle_3;
    Handle_3 = Handle_2;
    Handle_2 = Handle_1;
    Handle_1 = Handle;

    KATECH::ControlHandle = ((Handle_1 * kHandleFilterW1 + Handle_2 * kHandleFilterW2 + Handle_3 * kHandleFilterW3 + Handle_4 * kHandleFilterW4)
        / kHandleFilterDivisor);

    if (KATECH::HandleState == 1)
        KATECH::ControlHandle_Log = KATECH::ControlHandle;
    else
        KATECH::ControlHandle_Log = 0;
    adcm::Log::Info() << "[LatiControl] Azimuth:" << Pos.yaw * kDegPerRad << " ControlHandle:" << ControlHandle
                      << "HeadingError:" << HeadingError;
}

void KATECH::func::LongiControl()
{
    SCC();
}

void KATECH::func::LatiControl()
{
    PurePursuit();
}

void KATECH::func::SendBuffer(uint8_t modeCmd, float ControlAx, float ControlSteer)
{
    static uint8_t AliveCnt = 0;

    // KATECH_CONTROL_A2V Txdata;
    // Txdata.count = AliveCnt;
    // Txdata.TargetMode = modeCmd;
    // Txdata.Accel = ControlAx;  // m/ss
    // Txdata.SteeringAngle = ControlSteer;  // deg
    // Txdata.TurnIndicatorsCommand = KATECH::Turn_Signal;
    // Txdata.ADCM_fault = KATECH::ADCMFault;
    // Txdata.HMI_fault = KATECH::HMIFault;

    KATECH_TX_Packet TxPacket;
    TxPacket.payload.a2v.count = AliveCnt;
    TxPacket.payload.a2v.TargetMode = modeCmd;
    TxPacket.payload.a2v.Accel = ControlAx;
    TxPacket.payload.a2v.SteeringAngle = ControlSteer;
    TxPacket.payload.a2v.TurnIndicatorsCommand = KATECH::Turn_Signal;
    TxPacket.payload.a2v.ADCM_fault = KATECH::ADCMFault;
    TxPacket.payload.a2v.HMI_fault = KATECH::HMIFault;

    katech_ipcf_send(&TxPacket, TxPayloadType::A2V);

    adcm::Log::Info() << "[KatechDebug]:modeCmd" << TxPacket.payload.a2v.TargetMode
                      << "ControlAx:" << TxPacket.payload.a2v.Accel
                      << "ControlSteer:" << TxPacket.payload.a2v.SteeringAngle;

    if (AliveCnt >= kAliveWrapValue) {
        AliveCnt = 0;
    } else {
        AliveCnt++;
    }
}

void KATECH::func::SendBuffer_Remote(uint8_t RemoteMod,
    double RxAcc,
    double RxBreak,
    double RxSteer,
    uint8_t RxTurn_signal)
{
    const double input_max = GetF64("control.remote.input_max");
    const double brake_scale = GetF64("control.remote.brake_scale");
    const double accel_scale = GetF64("control.remote.accel_scale");

    static uint8_t AliveCnt = 0;
    float RemoteAcc = 0;
    if (RxBreak > 0) {
        if (RxBreak < 0)
            RxBreak = 0;
        if (RxBreak > input_max)
            RxBreak = input_max;
        RemoteAcc = static_cast<float>((RxBreak / input_max) * brake_scale);
    } else if (RxAcc > 0) {
        if (RxAcc < 0)
            RxAcc = 0;
        if (RxAcc > input_max)
            RxAcc = input_max;
        RemoteAcc = static_cast<float>((RxAcc / input_max) * accel_scale);
    } else
        RemoteAcc = 0;
    
    // KATECH_CONTROL_A2V Txdata;
    // Txdata.count = AliveCnt;
    // Txdata.TargetMode = RemoteMod;
    // Txdata.Accel = RemoteAcc;  // m/s
    // Txdata.SteeringAngle = RxSteer;  // deg
    // Txdata.TurnIndicatorsCommand = RxTurn_signal;
    // Txdata.ADCM_fault = KATECH::ADCMFault;
    // Txdata.HMI_fault = KATECH::HMIFault;

    
    KATECH_TX_Packet TxPacket;
    TxPacket.payload.a2v.count = AliveCnt;
    TxPacket.payload.a2v.TargetMode = RemoteMod;
    TxPacket.payload.a2v.Accel = RemoteAcc;  // m/s
    TxPacket.payload.a2v.SteeringAngle = RxSteer;  // deg
    TxPacket.payload.a2v.TurnIndicatorsCommand = RxTurn_signal;
    TxPacket.payload.a2v.ADCM_fault = KATECH::ADCMFault;
    TxPacket.payload.a2v.HMI_fault = KATECH::HMIFault;

    katech_ipcf_send(&TxPacket, TxPayloadType::A2V);
    // adcm::Log::Info() << "[RemoteA2V] cnt:" << TxPacket.payload.a2v.count 
    //                   << "Accel:" << TxPacket.payload.a2v.Accel
    //                   << "Steer:" << TxPacket.payload.a2v.SteeringAngle;

    if (AliveCnt >= kAliveWrapValue) {
        AliveCnt = 0;
    } else {
        AliveCnt++;
    }
}

void KATECH::func::ReceiveBuffer()
{
    const double kVehicleMpsDivisor = GetF64("calib.vehicle.mps_divisor");
    const std::uint32_t kLcanDefault = GetU32("fault.default.lcan");
    const std::uint32_t kFrontRadarDefault = GetU32("fault.default.front_radar");
    const std::uint32_t kMobileyeDefault = GetU32("fault.default.mobileye");
    const std::uint32_t kNearSensorDefault = GetU32("state.default.near_sensor");
    const std::uint32_t kMdpsFaultMin = GetU32("state.mdps.fault_min");
    const std::uint32_t kMdpsManualMax = GetU32("state.mdps.manual_max");
    const std::uint32_t kMdpsAutoMode = GetU32("state.mdps.auto_mode");
    const std::uint32_t kAutomodeSwitchOn = GetU32("state.automode.switch_on");
    const std::uint32_t kAutomodeDebounceCount = GetU32("state.automode.debounce_count");

    adcm::Log::Info() << "KATECH ReceiveBuffer Start";
    uint8_t LeftTurnSwitch, RightTurnSwitch;  //, AutoModeSwcnt = 0;
    // uint8_t AutoModeSw;
    uint8_t button_flag = 0;

    // adcm::Log::Info() << "automodesw"<< AutoModeSw;

    KATECH_RX_Packet RxPacket;
    katech_ipcf_read(&RxPacket, RxPayloadType::V2A);
    // katech_ipcf_read(&RxPacket, RxPayloadType::VEHICLE_INFO); // test

    const auto &Rxdata = RxPacket.payload.v2a;
    
    // KATECH_CONTROL_V2A Rxdata;
    KATECH::vcualivecnt = Rxdata.count;

    KATECH::Angular_Acceleration = Rxdata.Tx_YawRate;
    KATECH::LateralAcceleration = Rxdata.Tx_LateralAccel;
    KATECH::LongitudinalAcceleration = Rxdata.Tx_LongitudinalAccel;
    KATECH::WheelSpeed_FL = Rxdata.Tx_WheelSpeedFL;
    KATECH::WheelSpeed_FR = Rxdata.Tx_WheelSpeedFR;
    KATECH::WheelSpeed_RL = Rxdata.Tx_WheelSpeedRL;
    KATECH::WheelSpeed_RR = Rxdata.Tx_WheelSpeedRR;
    Velocity = (KATECH::WheelSpeed_FL + KATECH::WheelSpeed_FR) / (2. * kVehicleMpsDivisor);  // [m/s]

    KATECH::SteeringWheelAngle = Rxdata.SteeringTireAngle;
    MDPSMode = Rxdata.ControlMode;
    KATECH::GearState = Rxdata.GearReport;
    KATECH::Radar_ObjDistVal_Front = Rxdata.Tx_FrontSCC_ObjDstVal;
    KATECH::Radar_ObjRelSpdVal_Front = Rxdata.Tx_RadarSCC_ObjRelSpdVal;
    KATECH::ClusterSpeed = Rxdata.Tx_ClusterVelocity;
    KATECH::LanePosition_Left = Rxdata.Tx_ThisLanePositionL;
    KATECH::LanePosition_Right = Rxdata.Tx_ThisLanePositionR;
    KATECH::LaneHeadingAngle_Left = Rxdata.Tx_ThisHeadingAngleL;
    KATECH::LaneHeadingAngle_Right = Rxdata.Tx_ThisHeadingAngleR;
    KATECH::LaneCurvature_Left = Rxdata.Tx_ThisCurvatureL;
    KATECH::LaneCurvature_Right = Rxdata.Tx_ThisCurvatureR;
    KATECH::LaneCurvatureRate_Left = Rxdata.Tx_ThisCurvatureRateL;
    KATECH::LaneCurvatureRate_Right = Rxdata.Tx_ThisCurvatureRateR;
    AutoModeSw = Rxdata.Tx_SWRC_CrsMainSwSta;
    KATECH::Turn_SignalReport = Rxdata.TurnIndicatorsReport;  // 0: OFF 1: LEft 2: Right

    KATECH::Emergency_Light = Rxdata.HazardLightsReport;
    KATECH::WheelPulse_FL = Rxdata.Tx_WheelPulseFL;
    KATECH::WheelPulse_FR = Rxdata.Tx_WheelPulseFR;
    KATECH::WheelPulse_RL = Rxdata.Tx_WheelPulseRL;
    KATECH::WheelPulse_RR = Rxdata.Tx_WheelPulseRR;
    KATECH::AccelCylPrs = Rxdata.Tx_AccelAct;  // pressure
    KATECH::brakecylPrs = Rxdata.Tx_brakecylPrsVal;  // pressure
    // adcm::Log::Info() << "brake : " << KATECH::brakecylPrs << "accel : " << KATECH::AccelCylPrs;

    // KATECH::FrontRadarFault = Rxdata.Tx_Front_fault; // 0이 죽은거, 1이 산거
    KATECH::LCANFault = static_cast<uint8_t>(kLcanDefault);
    KATECH::FrontRadarFault = static_cast<uint8_t>(kFrontRadarDefault);
    KATECH::MobileyeFault = static_cast<uint8_t>(kMobileyeDefault);
    KATECH::NearSensor = static_cast<uint8_t>(kNearSensorDefault);
    KATECH::SeatBeltCheck = Rxdata.Tx_SeatBelt_Status;
    KATECH::LongiError = Rxdata.Tx_SCC_EnblReq;

    if (MDPSMode >= kMdpsFaultMin) {
        KATECH::HandleState = 2;
    } else if (MDPSMode <= kMdpsManualMax) {
        KATECH::HandleState = 0;
        KATECH::No_Autonomous_Driving = 1;
    } else if (MDPSMode == kMdpsAutoMode) {
        KATECH::No_Autonomous_Driving = 0;
        KATECH::HandleState = 1;
    }
    // adcm::Log::Info() << "V2A cnt : "<<Rxdata.count << "AutoModeSw : " << AutoModeSw <<
    // AutoModeSwcnt<<"AutoDrivingMode" << AutoDrivingMode;
    if (AutoModeSw == kAutomodeSwitchOn) {
        // adcm::Log::Info() << "ddddd" << AutoModeSw;
        AutoModeSwcnt++;
        // adcm::Log::Info() << "ddddd" << AutoModeSw << "cnt" << AutoModeSwcnt << button_flag;
        if ((AutoModeSwcnt >= kAutomodeDebounceCount) && (button_flag == 0)) {
            if (KATECH::AutoDrivingMode == 0) {
                KATECH::AutoDrivingMode = 1;
                button_flag = 1;
            } else {
                KATECH::AutoDrivingMode = 0;
                button_flag = 1;
            }
        }
    }
    if (AutoModeSw != kAutomodeSwitchOn) {
        button_flag = 0;
        AutoModeSwcnt = 0;
    }
}

void KATECH::func::DataLogger(bool operation)
{
    // KATECH::log::current_time = std::chrono::steady_clock::now();
    // KATECH::log::TimeGap = std::chrono::duration_cast<std::chrono::milliseconds>(KATECH::log::current_time -
    // KATECH::log::old_time).count();
    fprintf(KATECH::log::StateFile,
        "%lu/%lld/%.7lf/%.7lf/%.2lf/%d/%d/%d/%.2lf/%.2lf/%.2lf/%d/%.2lf/%d/%d/%d/%d/%d\n",
        KATECH::log::maincnt,
        KATECH::log::TimeGap,
        KATECH::position.y,
        KATECH::position.x,
        KATECH::position.yaw,
        KATECH::driveMode,
        KATECH::HandleState,
        (int)KATECH::ControlHandle,
        Gap,
        LookaheadLength,
        HeadingError,
        (int)KATECH::ControlAx,
        Velocity,
        (int)operation,
        (int)debugHandle,
        (int)HandleGain,
        (int)RightTurnFlag,
        (int)LeftTurnFlag);
    // fprintf(KATECH::log::StateFile, "%lu/%lld/%.2lf/%.2lf/%.2lf/%.2lf/%.2lf/%.2lf/%d/%d\n", KATECH::log::maincnt,
    // KATECH::log::TimeGap, KATECH::SCC_TargetVelocity, Velocity, KATECH::Lidar_ObjDistVal_Front,
    // KATECH::Lidar_ObjRelSpdVal_Front, KATECH::Radar_ObjDistVal_Front, KATECH::Radar_ObjRelSpdVal_Front,
    // (int)KATECH::targetAx,(int)KATECH::ControlAx);
    for (std::vector<UtmKStruct>::iterator itr = GlobalPath.begin(); itr != GlobalPath.end(); ++itr) {
        fprintf(KATECH::log::GlobalPathFile, "%lu/%.7lf/%.7lf\n", KATECH::log::maincnt, itr->y, itr->x);
    }

    for (std::vector<LocalStruct>::iterator itr = LocalPath.begin(); itr != LocalPath.end(); ++itr) {
        fprintf(KATECH::log::LocalPathFile, "%lu/%.2lf/%.2lf\n", KATECH::log::maincnt, itr->Y, itr->X);
    }

    KATECH::log::maincnt++;
    // KATECH::log::old_time = KATECH::log::current_time;
}

double KATECH::func::CalculateDistance(double x1, double x2, double y1, double y2)
{
    const double geo_scale_x = GetF64("calib.path.lon_to_meter");
    const double geo_scale_y = GetF64("calib.path.lat_to_meter");
    double distance = sqrt(pow(((x1 - x2) * geo_scale_x), 2) + pow(((y1 - y2) * geo_scale_y), 2));
    return distance;
}

double KATECH::func::GetLateraldeviation()
{
    double Error;

    Error = Gap;
    if (LocalPath[kLocalPathLateralIndex].Y > 0)
        Error = Error * (-1);

    return Error;
}

double KATECH::func::GetRelativeYawAngle()
{
    const double kNearestThreshold = GetF64("path.relative_yaw.nearest_threshold_m");
    const double kNextThreshold = GetF64("path.relative_yaw.next_threshold_m");
    const double kDegPerRad = GetF64("calib.control.deg_per_rad");

    double Error;
    double PathDirection;
    static double PrePathDirection = 0;
    double distance = 0, NearestDistance = 0;
    std::vector<UtmKStruct>::iterator NearestIdx;
    std::vector<UtmKStruct>::iterator NextIdx;
    for (std::vector<UtmKStruct>::iterator itr = StartItr; itr < EndItr; ++itr) {
        distance = CalculateDistance(itr->x, StartItr->x, itr->y, StartItr->y);
        if (distance < kNearestThreshold) {
            NearestIdx = itr;
            NearestDistance = distance;
        } else if (distance < kNextThreshold)
            NextIdx = itr;
    }

    if (((NearestIdx != StartItr) && (NextIdx != StartItr)) && (NearestDistance < LookaheadLength)) {
        PathDirection = atan2((NextIdx->y - NearestIdx->y), (NextIdx->x - NearestIdx->x)) * (-1) * kDegPerRad;
        // else                      PrePathDirection = PathDirection;

        Error = Pos.yaw * kDegPerRad - PathDirection;
        adcm::Log::Info() << "incline" << Pos.yaw * kDegPerRad << PathDirection << StartItr->yaw * kDegPerRad;
    } else {
        adcm::Log::Info() << "Cannot Calc RHA. Path is too short";
        Error = 0;
    }
    return Error;
}
