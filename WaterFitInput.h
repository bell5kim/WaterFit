#include <QString>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cmath>
using namespace std;

class WaterFitInput {
 
public:  
 
   // string of input filename
   char ifile[256];
   // string to check file version
   char version_string[4];
   // nominal energy string (no energy by default)
   // char energy_string[3];
   // name of the list file
   char list_string[50];
  
   char econFile[256];
  
   int FS1;
   int FS2;
  
   float E;                    // photon energy
   float ZI;                   // iso-center position
   float Z0;                   // target (primary source) position
   float ZS;                   // filter (scatter source) position
   float ZC;                   // Collimator Position
   float ZM;                   // modifier (MLC) position
   int MX;                     // collimate x twice: TRUE or FALSE
   int MY;                     // collimate y twice: TRUE or FALSE
   float ZX;                   // x collimator position
   float ZY;                   // y collimator position
   float FFRad;                    //  Flattening Filter Radius
   float eEnergy;              // Electron Mean Energy
   float nu;                   // nu Value
   int   ID;                   // Model ID

   // normalization position and field size
   float XN;
   float YN;
   float ZN;
   float WXN;
   float WYN;

   // number of measured air profiles (number of entries in list file)
   int NC;
 
   // Non AFIT Stuffs but useful for UI
   float DEV;
   float CHI;

	   // parameters to fit
   float p0;  int ip0;  // probabilty for primary photons
   float s0;  int is0;  // primary width
   float h0;  int ih0;  // horn correction parameter 0
   float h1;  int ih1;  // horn correction parameter 1
   float h2;  int ih2;  // horn correction parameter 2
   float h3;  int ih3;  // horn correction parameter 3
   float h4;  int ih4;  // horn correction parameter 4
   float ss;  int iss;  // scatter width

   // parameters to fit
   float norm;  int inorm;  // normalization factor
   float lval;  int ilval;  // parameter l
   float bval;  int ibval;  // parameter b
   float aval;  int iaval;  // parameter a
   float zval;  int izval;  // parameter z
   float Emin;  int iEmin;  // minimum energy
   float Emax;  int iEmax;  // maximum energy
   float pcon;  int ipcon;  // probability for electron contam.
   
   float gy_mu_dmax;
   float Emean;
   float Eprob;
   float chi;
   float avgdev;
   float normValue;
   QString writeFile;
   QString version;
   QString particle;

   bool version_ok;
   bool energy_ok;
   bool p0_ok;
   bool s0_ok;
   bool h0_ok;
   bool h1_ok;
   bool h2_ok;
   bool h3_ok;
   bool h4_ok;
   bool ss_ok;
   bool Z0_ok;
   bool ZS_ok;
   bool ZM_ok;
   bool ZX_ok;
   bool ZY_ok;
   bool ZI_ok;
   bool XN_ok;
   bool YN_ok;
   bool ZN_ok;
   bool WXN_ok;
   bool WYN_ok;
   bool list_ok; 
   
   bool norm_ok;
   bool lval_ok;
   bool bval_ok;
   bool aval_ok;
   bool zval_ok;
   bool Emin_ok;
   bool Emax_ok;
   bool pcon_ok;
  
   void initWaterFitInput();
   void readWaterFitInput(const char *fname);
   void printWaterFitInput();
   void writeWaterFitInput(const char *fname);
   void err_exit(char *location, char *reason, int exit_code);
};


