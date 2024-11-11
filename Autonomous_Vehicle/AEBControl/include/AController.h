#ifndef AController_H
#define AController_H

#include "AControlModule.h"


#define Gravity 9.80665
#define LookAheadGain 0.5

#define NumBitsPerChar  8U

extern const int TargetSpeed;

extern VehicleStruct Vehicle;
extern GlobalPathStruct Global;
extern LocalPathStruct Local;
extern ControlStruct Control;
extern GPSStruct GPS;


class Longitudinal
{
    public:
        void SCC();

    private:
        double Curvature;
        void GetCurvature();
};

class Lateral
{
    public:
        void PurPursuit();

    private:
        double HeadingError;
        double LookAheadDistance;
        uint32_t LookAheadIdx;
        double Curvature;
};


// Class declaration for model MPC
class ModelPredictiveControl final
{
    // public data and function members
  public:
    // Block signals and states (default storage) for system '<Root>'
    struct DW {
        double LastPcov_PreviousInput[25];// '<S11>/LastPcov'
        double last_x_PreviousInput[5];// '<S11>/last_x'
        double b_Hv[440];
        double Su[200];
        double Cm[110];
        double Bv[110];
        double b_Sx[100];
        double dv[88];
        double Bu[55];
        double CovMat[49];
        double Dv[44];
        double CA[44];
        double b_SuJm[40];
        double WySuJm[40];
        double b_tmp[35];
        double b_B[30];
        double b_A[25];
        double L_tmp[25];
        double L_tmp_m[25];
        double L_tmp_c[25];
        double vseq[22];
        double rseq[20];
        double dv1[20];
        double b_Su1[20];
        double WuI2Jm[20];
        double WduJm[20];
        double Sum[20];
        double a[16];
        double Ai[16];
        double A[16];                  // '<S5>/Adaptive Model'
        double a_k[16];
        double dv2[16];
        double r[12];
        double Product[11];            // '<S5>/Product'
        double L[10];
        double b_C[10];
        double b_utarget[10];
        double Cm_c[10];
        double dv3[10];
        double CA_b[10];
        double CA_p[10];
        double q[9];
        double L_c[9];
        double b_D[9];
        double b_H[9];
        double U[9];
        double RLinv[9];
        double TL[9];
        double Q[9];
        double R[9];
        double b_A_f[9];
        double b[8];
        double rtb_y_j_g[8];
        double Opt[6];
        double Rhs[6];
        double xk[5];
        double xk_g[5];
        double Kinv[4];
        double c_A[4];
        double Am[4];                  // '<S51>/Matrix Concatenate'
        double Bc[4];
        double a__1[4];
        double cTol[4];
        double zopt[3];
        double varargin_1[3];
        double r_m[3];
        double z[3];
        double tau[3];
        double work[3];
        double y_innov[2];
        double rtb_y_gx_n[2];
        double Sum_p[2];
        double F[16];
        double A2[16];
        double V[16];
        double A3[16];
        double A4[16];
        double A4_l[16];
        double last_mv_DSTATE;         // '<S11>/last_mv'
        double y_gx;                   // '<S5>/DataTypeConversion_e1'
        double y_o;                    // '<S5>/DataTypeConversion_e2'
        double y_j;                    // '<S5>/DataTypeConversion_Vx'
        double y_a;                    // '<S3>/DataTypeConversion_umin'
        double y_hh;                   // '<S3>/DataTypeConversion_umax'
        double y_h;                    // '<S5>/DataTypeConversion_Ts'
        double y;                      // '<S51>/DataTypeConversion_rearstiff'
        double y_c;                    // '<S51>/DataTypeConversion_frontstiff'
        double y_m;                    // '<S51>/DataTypeConversion_mass'
        double y_k;                    // '<S51>/DataTypeConversion_rear'
        double y_g;                    // '<S51>/DataTypeConversion_front'
        double y_f;                    // '<S51>/DataTypeConversion_inertia'
        double rtb_Am_tmp;
        double rtb_a4_tmp;
        double normH;
        double s;
        double WuI2Jm_j;
        double b_Jm;
        double rMin;
        double Xnorm0;
        double cMin;
        double cVal;
        double t;
        double cVal_tmp;
        double atmp;
        double beta1;
        double b_A_d;
        double scale;
        double absxk;
        double t_g;
        double normA;
        double b_s;
        double d;
        double s_l;
        double A2_d;
        double temp;
        double c;
        int32_t iC[4];
        int32_t Coef;
        int32_t r2;
        int32_t i;
        int32_t L_tmp_d;
        int32_t i_l;
        int32_t i1;
        int32_t Tries;
        int32_t i_o;
        int32_t CA_tmp;
        int32_t b_Jm_tmp;
        int32_t nA;
        int32_t kDrop;
        int32_t kNext;
        int32_t iSave;
        int32_t exponent;
        int8_t a_b[100];
        int8_t Dvm[44];
        int8_t b_D_n[12];
        int8_t b_b[9];
        int8_t iv[6];
        int8_t UnknownIn[5];
        int8_t c_a[4];
        bool Memory_PreviousInput[4];  // '<S11>/Memory'
        bool bv[4];
    };

    // Constant parameters (default storage)
    struct ConstP {
        // Pooled Parameter (Mixed Expressions)
        //  Referenced by:
        //    '<S2>/Vehicle dynamics matrix A constant'
        //    '<S2>/Vehicle dynamics matrix C constant'
        //    '<S5>/DX Constant'
        //    '<S5>/X Constant'

        double pooled4[4];

        // Expression: lastPcov
        //  Referenced by: '<S11>/LastPcov'

        double LastPcov_InitialCondition[25];
    };

    // Real-time Model Data Structure
    struct RT_MODEL {
        const char * volatile errorStatus;
    };

    // Copy Constructor
    ModelPredictiveControl(ModelPredictiveControl const&) = delete;

    // Assignment Operator
    ModelPredictiveControl& operator= (ModelPredictiveControl const&) & = delete;

    // Move Constructor
    ModelPredictiveControl(ModelPredictiveControl &&) = delete;

    // Move Assignment Operator
    ModelPredictiveControl& operator= (ModelPredictiveControl &&) = delete;

    // Real-Time Model get method
    ModelPredictiveControl::RT_MODEL * getRTM();

    // model initialize function
    void initialize();

    // model step function
    void GetSteeringAngle();

    // Constructor
    ModelPredictiveControl();

    // Destructor
    ~ModelPredictiveControl();

    // private data and function members
  private:
    // Block states
    DW rtDW;

    // private member function(s) for subsystem '<S3>/DataTypeConversion_umax'
    static void DataTypeConversion_umax(double rtu_u, double *rty_y);

    // private member function(s) for subsystem '<S5>/DataTypeConversion_A'
    static void DataTypeConversion_A(const double rtu_u[4], double rty_y[4]);

    // private member function(s) for subsystem '<Root>'
    void PadeApproximantOfDegree(const double A[16], uint8_t m, double F[16]);
    void expmNoLog2(double A[16], double F[16]);
    int32_t xpotrf(double b_A[9]);
    double minimum(const double x[3]);
    void trisolve(const double b_A[9], double b_B[9]);
    double norm(const double x[3]);
    double maximum(const double x[3]);
    double xnrm2(int32_t n, const double x[9], int32_t ix0);
    void xgemv(int32_t b_m, int32_t n, const double b_A[9], int32_t ia0, const
               double x[9], int32_t ix0, double y[3]);
    void xgerc(int32_t b_m, int32_t n, double alpha1, int32_t ix0, const double
               y[3], double b_A[9], int32_t ia0);
    void KWIKfactor(const double b_Ac[12], const int32_t iC[4], int32_t nA,
                    const double b_Linv[9], double b_D[9], double b_H[9],
                    int32_t n, double RLinv[9], double *Status);
    void DropConstraint(int32_t kDrop, bool iA[4], int32_t *nA, int32_t iC[4]);
    void qpkwik(const double b_Linv[9], const double b_Hinv[9], const double f[3],
                const double b_Ac[12], const double b[4], bool iA[4], int32_t
                maxiter, double FeasTol, double x[3], double lambda[4], int32_t *
                status);
    void mpcblock_optimizer(const double rseq[20], const double vseq[22], double
                            umin, double umax, int32_t switch_in, const double
                            x[5], double old_u, const bool iA[4], const double
                            b_Mlim[4], double b_Mx[20], double b_Mu1[4], double
                            b_Mv[88], const double b_utarget[10], double b_uoff,
                            int32_t b_enable_value, double b_H[9], double b_Ac
                            [12], const double b_Wy[2], const double b_Jm[20],
                            const double b_I1[10], const double b_A[25], const
                            double Bu[55], const double Bv[110], const double
                            b_C[10], const double Dv[44], const int32_t b_Mrows
                            [4], double *u, double useq[11], double *status,
                            bool iAout[4]);

    // Real-Time Model
    RT_MODEL rtM;
};

static void rt_InitInfAndNaN(size_t realSize);
static double rtGetNaN(void);
static float rtGetNaNF(void);
static double rtGetInf(void);
static float rtGetInfF(void);
static double rtGetMinusInf(void);
static float rtGetMinusInfF(void);
double rt_powd_snf(double u0, double u1);
double rt_hypotd_snf(double u0, double u1);
static int32_t div_nde_s32_floor(int32_t numerator, int32_t denominator);
#endif