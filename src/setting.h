#include <cmath>

static const double PI = M_PI;
static const double m  = 1e-3;

static const double height         = 800*m;
static const double width          = 800*m;

static const double p4_x = 540*m;
static const double p4_y = 0.0;
static const double p5_x = 540*m;
static const double p5_y = 540*m;
static const double p6_x = 0.0;
static const double p6_y = 540*m;
static const double p7_x = 40*m;
static const double p7_y = 0.0;
static const double p8_x = 40*m;
static const double p8_y = 40*m;
static const double p9_x = 0.0;
static const double p9_y = 40*m;

static const int GaussianDiv   = 64;

static const int ironNumber   = 0;
static const int airNumber    = 1;         
static const int coilNumber   = 2;
static const int magnetNumber = 3;


static const double myu_air    = 4.0*PI*1e-7;
static const double myu_iron   = 1000*myu_air;
static const double myu_magnet = 1.05*myu_air;
static const double nyu_air    = 1/myu_air;
static const double nyu_magnet = 1/myu_magnet;
static const double nyu_iron   = 1/myu_iron;

static const double currentAmp  = 20.0;
static const int coilTurn       = 100;
static const double coilS       = 40*m*40*m;
static const double Jo          = currentAmp*coilTurn/coilS;
static const double MagnetT     = 1.25;

static const double NrConv      = 1e-3;
static const double iccg_conv   = 1e-6;
static const double MicroNum    = 1e-5;
static const int    MaxNrLoop   = 20;

static const int totalNodeNumber    = 1070;
static const int totalElementNumber = 2020;

static const int NyuPointNum = 761;

static const char* Msh1File      = "../data/Mesh/magnetic_shield/meshdata.msh1";

static const char* AkimaFile     = "../data/BNu/50A470/akima_coef.csv";
static const char* NyuFile       = "../data/BNu/50A470/nyu_B^2.csv";

static const char* GaussinaFile  = "../data/Gaussian/NGnet_center.csv";
//static const char* GaussinaFile  = "./Gaussian/NGnet_center_notuse.csv";

