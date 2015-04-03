#include "WaterFitInput.h"

// Initiation of WaterFit Parameters
void WaterFitInput::initWaterFitInput(){
   // cout << "initWaterFitInput() Starts---" << endl;
   E = 6.0;
   FS1 = 0; // if 0, no field
   FS2 = 0; // if 0, no field
   // parameters to fit
   norm = 1800.00;  inorm = 1;  // normalization factor
   lval =    4.50;  ilval = 1;  // parameter l
   bval =    0.45;  ibval = 1;  // parameter b
   aval =    9.00;  iaval = 1;  // parameter a
   Emin =    0.25;  iEmin = 0;  // minimum energy
   Emax =    6.00;  iEmax = 1;  // maximum energy
   pcon =    0.0045;  ipcon = 1;  // probability for electron contam.

   version_ok = false;
   energy_ok  = false;
   norm_ok      = false;
   lval_ok      = false;
   bval_ok      = false;
   aval_ok      = false;
   Emin_ok      = false;
   Emax_ok      = false;
   pcon_ok      = false;
}

// error handler
void WaterFitInput::err_exit(char *location, char *reason, int exit_code)
{
   cerr << endl;
   cerr << "RUN-TIME ERROR in " << location << endl;
   cerr << "Reason: " << reason << "!" << endl;
   cerr << "Exiting to system..." << endl;
   cerr << endl;
   exit(exit_code);
}

// Read Input File
void WaterFitInput::readWaterFitInput(const char *fname) {
   // cout << "readWaterFitInput Starts---" << endl;
   // string of input filename
   // ifile[0] = 0;
   // string to check file version
   //version_string[0] = 0;
   // nominal energy string (no energy by default)
   //energy_string[0] = 0;
   // name of the list file
   //list_string[0] = 0;
   // cout <<  endl;

   // read input file
   // cout << "Opening file: " << argv[1] << endl;
   
   ifstream inp_file;
   inp_file.open(fname,ios::in);
   if (inp_file.bad()) err_exit("getAitFitInput","cannot open input file",8);

   while (!inp_file.eof()) {
      char line[81] = "";  // lines to read from file
      inp_file.getline(line,sizeof(line));
      istringstream line_stream(line);
      char keyword[20] = "";
      line_stream >> keyword;
      if (!strcmp(keyword,"version:")) { line_stream >> version_string;
                                                        version_ok = true; }
      if (!strcmp(keyword,"energy:"))  { line_stream >> E;
                                                        energy_ok  = true; }
      if (!strcmp(keyword,"norm:"))   { line_stream >> norm >> inorm; norm_ok = true; }
      if (!strcmp(keyword,"lval:"))   { line_stream >> lval >> ilval; lval_ok = true; }
      if (!strcmp(keyword,"bval:"))   { line_stream >> bval >> ibval; bval_ok = true; }
      if (!strcmp(keyword,"aval:"))   { line_stream >> aval >> iaval; aval_ok = true; }
      if (!strcmp(keyword,"Emin:"))   { line_stream >> Emin >> iEmin; Emin_ok = true; }
      if (!strcmp(keyword,"Emax:"))   { line_stream >> Emax >> iEmax; Emax_ok = true; }
      if (!strcmp(keyword,"pcon:"))   { line_stream >> pcon >> ipcon; pcon_ok = true; }
#ifndef XVMC		
      if (!strcmp(keyword,"dir:")) {
			if (FS1 == 0) line_stream >> FS1;
			if (FS1 > 0 && FS2 == 0) line_stream >> FS2;
			if (FS2 == FS1) FS2 = 0;
      }
      if (!strcmp(keyword,"econ:"))   { line_stream >> econFile; }
#endif		
   }
   inp_file.close();

   if (!version_ok) err_exit("readWaterFitInput","no version input",8);
   if (!energy_ok)  err_exit("readWaterFitInput","no energy input",8);
   if (!norm_ok)      err_exit("readWaterFitInput","no norm input",8);
   if (!lval_ok)      err_exit("readWaterFitInput","no lval input",8);
   if (!bval_ok)      err_exit("readWaterFitInput","no bval input",8);
   if (!aval_ok){     aval = 9.0; aval_ok = true;
	//	      err_exit("readWaterFitInput","A-Value is set to 9.0",8);
	}
   if (!Emin_ok)      err_exit("readWaterFitInput","no Emin input",8);
   if (!Emax_ok)      err_exit("readWaterFitInput","no Emax input",8);
   if (!pcon_ok)      err_exit("readWaterFitInput","no pcon input",8);
	// printWaterFitInput();
}

// Print Input File
void WaterFitInput::printWaterFitInput() {
 // cout << "printWaterFitInput() Starts ---" << endl;
// if (version_string == "") 
#ifdef XVMC
 	cout << "version:   1.0" << endl;
#else	
 	cout << "version:   4.0" << endl;
#endif	
// else
//	cout << "version:   " << version_string << endl;
 cout << "energy:   " << E  << endl;
 cout << "norm:   " << norm << "   " << inorm << endl;
 cout << "lval:   " << lval << "   " << ilval << endl;
 cout << "bval:   " << bval << "   " << ibval << endl;
 cout << "aval:   " << aval << "   " << iaval << endl;
 cout << "Emin:   " << Emin << "   " << iEmin << endl;
 cout << "Emax:   " << Emax << "   " << iEmax << endl;
 cout << "pcon:   " << pcon << "   " << ipcon << endl;
#ifndef XVMC 
 if (FS1 > 0) cout << "dir:   " << FS1 << "x" << FS1 << endl;
 if (FS2 > 0) cout << "dir:   " << FS2 << "x" << FS2 << endl;
// if (econFile == "")
	 cout << "econ:   EL_CON_BEAM1_PROFILE1.txt" << endl;
// else
//	 cout << "econ:   " << econFile << endl;
#endif	 
}

// Write Input File
void WaterFitInput::writeWaterFitInput(const char *fname) {
	
 // cout << "writeWaterFitInput() Starts ---" << endl;
 ofstream out_file(fname,ios::out);
 if (out_file.bad()){
    err_exit("writeWaterFitInput","cannot open file",8);
 } 
 
 // out_file << "version:   " << version_string << endl;
#ifdef XVMC
 out_file << "version:   1.0" << endl;
#else  
 out_file << "version:   4.0" << endl;
#endif 
 out_file << "energy:   " << E  << endl;
 out_file << "norm:   " << norm << "   " << inorm << endl;
 out_file << "lval:   " << lval << "   " << ilval << endl;
 out_file << "bval:   " << bval << "   " << ibval << endl;
 out_file << "aval:   " << aval << "   " << iaval << endl;
 out_file << "Emin:   " << Emin << "   " << iEmin << endl;
 out_file << "Emax:   " << Emax << "   " << iEmax << endl;
 out_file << "pcon:   " << pcon << "   " << ipcon << endl;
#ifndef XVMC 
 if (FS1 > 0) out_file << "dir:   " << FS1 << "x" << FS1 << endl;
 if (FS2 > 0) out_file << "dir:   " << FS2 << "x" << FS2 << endl;
 // out_file << "econ:   " << econFile << endl;
 out_file << "econ:   EL_CON_BEAM1_PROFILE1.txt" << endl;
#endif 
 
 out_file.close();
}
