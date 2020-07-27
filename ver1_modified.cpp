/**
 \author Maciej Trzebinski
 \version 1.0
 \date 21/03/2020
*/
#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <math.h>
#include <unistd.h>

#include <TFile.h>
#include <TRandom.h>
#include <TSystem.h>
#include <TH1F.h>
#include <TTree.h>
#include <TROOT.h>
#include <TSystemDirectory.h>
#include <TList.h>
#include <algorithm>
#include <map>
#include <string.h>
#include "distributions_difference.h"
#include "shift.h"
#include "magnet.h"
using std::cout;
using std::endl;
using std::vector;
using std::sin;
using std::cos;
using std::sinh;
using std::cosh;
using std::sqrt;
using std::string;
using std::ifstream;
using std::istringstream;



double ApplyOperation(double lvalue, double rvalue, const std::string& op) {
  if (op == "+") {
    return lvalue + rvalue;
  } else if (op == "-") {
    return lvalue - rvalue;
  } else if (op == "*") {
    return lvalue * rvalue;
  } 
  return 0;
}

bool operator < (Magnet lhs, Magnet rhs) {
  return lhs.GetName() < rhs.GetName();
}

bool operator == (Magnet lhs, Magnet rhs) {
  return lhs.GetName() == rhs.GetName();
}

// 6 quadrupoles, 2 dipoles, 5 horizotal kickers and 5 vertical kickers

struct MagnetIdIterators {
  void IncreaseItByOne(const std::string& it_name) {
    it_name_to_it.at(it_name) += 1;
  }

  int GetIt(const std::string& it_name) {
    return it_name_to_it.at(it_name);
  }

  std::map<std::string, int> it_name_to_it = {{"RBEND_it", 0},
                                              {"QUADRUPOLE_it", 0}, 
                                              {"VKICKER_it", 0}, 
                                              {"HKICKER_it", 0}};
};

class FileName {
public:
  FileName(const std::string& init_filename, 
           bool is_shifted, 
           bool is_strength_changed) 
    : init_filename(init_filename),
      output_filename(""),
      is_shifted(is_shifted),
      is_strength_changed(is_strength_changed)
  {
  }

  void ProcessFileName() {
    output_filename += "root_PPSS_2020/";
    output_filename.push_back(init_filename[26]);

    if (is_shifted) {
      output_filename += "_shifted_";
    }

    if (is_strength_changed) {
      output_filename += "_changed_strength_";
    }

    output_filename += "pythia8_13TeV_protons_100k_transported_205m" + 
                                   init_filename.substr(init_filename.find("_beta")) + 
                                   ".root";
  }

  std::string GetOutputFileName() const {
    return output_filename;
  }

private:
  std::string init_filename;
  std::string output_filename;
  bool is_shifted = false;
  bool is_strength_changed = false;
};

class ProtonTransport {
  public:
    ProtonTransport(); //!< constructor 
    ~ProtonTransport(); //!< destructor 
    void PrepareBeamline(bool, bool); 
    void simple_tracking(double);
    void simple_pythia_tracking(double);
    void SetBeamEnergy(double);
    double GetBeamEnergy();
    void SetBeampipeSeparation(double);
    double GetBeampipeSeparation();
    void SetShift(const Magnet&, const Shift&);
    void DoShift(const Magnet&, const string&);
    void SetStrengthRatio(const Magnet&, double);
    void ApplyStrengthRatio(const Magnet&, double&);
    void SetProcessedFileName(const std::string&);
    std::string GetROOTOutputFileName() const;
    void WriteChangesInCsv(const std::string&, DistributionsDifference*, int);
    void WriteLostProtonsInCsv(const std::string&) const;
    void SetPositions();
    std::vector<Magnet> GetMagnets() const;
    void SetMagnets(const std::vector<Magnet>&); 
    bool isLost(double, double, double, double, double, double);
    bool is_current_lost=false;
  private:
    double IP1Pos;
    double x, y, z, px, py, pz, sx, sy;
    std::map<Magnet, Shift> magnet_to_shift;
    MagnetIdIterators iterators;
    std::string processed_filename;
    std::string optics_root_file_name;
    std::map<Magnet, double> magnet_to_ratio;
    std::vector<Magnet> magnets;
    std::vector<std::vector<double>> lost_protons;
    //obj type -diplole quadrupole etc
    //shift obj id 1st 2dn dipole etc ; shift value - self explanatory ;shift axis_axis- x y z strength - condition applied in magnet methods
    // values or vectors- depending if we'll shift 2 things at once - if not values will work
    // numb_of_obj_uses - how many times an magnet/dipole etc has been used for specifiv proton - needs to be cleared NEEDS TO BE A VECTOR WITH ENTRY CORESPONDING OBJ TYPE 
    double beam_energy;
    bool BeampipesAreSeparated;
    double BeampipeSeparation;
    void Marker(bool);
    void simple_drift(double, bool);
    void simple_rectangular_dipole(double, double, double, double, double, double);
    void simple_horizontal_kicker(double, double, double, double, double, double);
    void simple_vertical_kicker(double, double, double, double, double, double);
    void simple_quadrupole(double, double, double, double, double, double, bool);
    vector <vector <string> > element;
    bool DoApertureCut;
};

/** \class ProtonTransport
\brief All tools needed for proton transport through LHC structures.

Class constructor sets the following default values: \n 
beam_energy of 6500 \n 
BeampipesAreSeparated = false \n 
BeampipeSeparation of 97.e-3 \n 
DoApertureCut = true
*/
ProtonTransport::ProtonTransport() :
  beam_energy(6500.),
  BeampipesAreSeparated(false),
  BeampipeSeparation(97.e-3),
  DoApertureCut(true)
{
}


ProtonTransport::~ProtonTransport()
{
}


/**
\brief Set beam energy in GeV.

Must be accordingly to settings used in optics file.
\param[in] E value of beam energy in GeV.
*/
void ProtonTransport::SetBeamEnergy(double E){
  beam_energy = E;
}

/**
\brief Return beam energy in GeV.

\return beam_energy
*/
double ProtonTransport::GetBeamEnergy(){
  return beam_energy;
}

/**
\brief Set beam separation in m.

In vicinity of collision point protons are in one, common beampipe.
Splitting into two beampipes at TAN element, just before second dipole.
The nominal separation is 97.e-3 m i.e. 97 mm.

\param[in] d vaule of beam separation in m
*/
void ProtonTransport::SetBeampipeSeparation(double d){
  BeampipeSeparation = d;
}

/**
\brief Get beam separation in m.

\return BeampipeSeparation
*/
double ProtonTransport::GetBeampipeSeparation(){
  return BeampipeSeparation;
}

/**
\brief Beam element - marker.

Marker is a "dummy" beam element used to mark a certain place between elements (in drift).
*/
void ProtonTransport::Marker(bool verbose=false){
  if (!verbose) return;
  cout << "MARKER\t";
  cout << "z [m]: " << z;
  cout << "\tx [mm]: " << x*1.e3; 
  cout << "\ty [mm]: " << y*1.e3;
  cout << "\tpx [GeV]: " << px;
  cout << "\tpy [GeV]: " << py;
  cout << "\tpz [GeV]: " << pz;
  cout << "\tsx: " << sx;
  cout << "\tsy: " << sy << endl;
	//cout << z << "\t" << x << "\t" << y << "\t" << sx*pz << "\t" << sy*pz << endl;
}  

void ProtonTransport::simple_drift(double L, bool verbose=false){
  x+=L*sx;
  y+=L*sy;
  z+=L;
  if (!verbose) return;
  cout << "DRIFT\t";
  cout << "z [m]: " << z;
  cout << "\tx [mm]: " << x*1.e3; 
  cout << "\ty [mm]: " << y*1.e3;
  cout << "\tpx [GeV]: " << px;
  cout << "\tpy [GeV]: " << py;
  cout << "\tpz [GeV]: " << pz;
  cout << "\tsx: " << sx;
  cout << "\tsy: " << sy << endl;
}

void ProtonTransport::simple_rectangular_dipole(double L, double K0L, double rect_x, double rect_y, double el_x, double el_y){
  iterators.IncreaseItByOne("RBEND_it");
  //std::cout << "DIP" << iterators.GetIt("RBEND_it") << std::endl;
  ApplyStrengthRatio(Dipole(iterators.GetIt("RBEND_it"), 0), K0L);

  if (fabs(K0L) < 1.e-15)
  {
    simple_drift(L);
    return;
  }
  DoShift(Dipole(iterators.GetIt("RBEND_it"), 0), "-");
  double x0 = x;
  double y0 = y;
  double z0 = z;
  double sx0 = sx;
  z0 += L;
  x0 += L*sx + L*0.5*K0L*beam_energy/pz; // length * initial slope + length * half of angle (from geometry) * correction due to energy loss
  y0 += L*sy;
  sx0 += K0L*beam_energy/pz;
  //sy does not change
  if (!isLost(x0, y0, rect_x, rect_y, el_x, el_y)) {
    x = x0;
    y = y0;
    z = z0;
    sx = sx0;
  } else {
    lost_protons.push_back(std::vector<double>{px, py, pz});
  }

  DoShift(Dipole(iterators.GetIt("RBEND_it"), 0), "+");
}

void ProtonTransport::simple_horizontal_kicker(double L, double HKICK, double rect_x, double rect_y, double el_x, double el_y){
  iterators.IncreaseItByOne("HKICKER_it");
  //std::cout << "HKICKER" << iterators.GetIt("HKICKER_it") << std::endl;
  ApplyStrengthRatio(HorizontalKicker(iterators.GetIt("HKICKER_it"), 0), HKICK);


  if (fabs(HKICK) < 1.e-15)
  {
    simple_drift(L);
    return;
  }
  DoShift(HorizontalKicker(iterators.GetIt("HKICKER_it"), 0), "-");
  double x0 = x;
  double y0 = y;
  double z0 = z;
  double sx0 = sx;
  z0 += L;
  x0 += L*sx + L*0.5*HKICK*beam_energy/pz; // length * initial slope + length * half of angle (from geometry) * correction due to energy loss
  y0 += L*sy;
  sx0 += HKICK*beam_energy/pz;
  if (!isLost(x0, y0, rect_x, rect_y, el_x, el_y)) {
    x = x0;
    y = y0;
    z = z0;
    sx = sx0;
  } else {
    lost_protons.push_back(std::vector<double>{px, py, pz});
  }

  DoShift(HorizontalKicker(iterators.GetIt("HKICKER_it"), 0), "+");
}

void ProtonTransport::simple_vertical_kicker(double L, double VKICK, double rect_x, double rect_y, double el_x, double el_y){
  iterators.IncreaseItByOne("VKICKER_it");
  //std::cout << "VKICKER" << iterators.GetIt("VKICKER_it") << std::endl;
  ApplyStrengthRatio(VerticalKicker(iterators.GetIt("VKICKER_it"), 0), VKICK);

  if (fabs(VKICK) < 1.e-15)
  {
    simple_drift(L);
    return;
  }
  DoShift(VerticalKicker(iterators.GetIt("VKICKER_it"), 0), "-");
  double x0 = x;
  double y0 = y;
  double z0 = z;
  double sy0 = sy;
  z0 += L;
  x0 += L*sx;
  y0 += L*sy + L*0.5*VKICK*beam_energy/pz; // length * initial slope + length * half of angle (from geometry) * correction due to energy loss
  sy0 += VKICK*beam_energy/pz;
  if (!isLost(x0, y0, rect_x, rect_y, el_x, el_y)) {
    x = x0;
    y = y0;
    z = z0;
    sy = sy0;
  } else {
    lost_protons.push_back(std::vector<double>{px, py, pz});
  }

  DoShift(VerticalKicker(iterators.GetIt("VKICKER_it"), 0), "+"); 
}


void ProtonTransport::SetShift(const Magnet& magnet, const Shift& shift){ //1 quadrupole; id;axis 1x 2y 3z; value
  magnet_to_shift[magnet] = shift;
}

void ProtonTransport::DoShift(const Magnet& m, const string& op) {
  if (magnet_to_shift.find(m) != magnet_to_shift.end()) {
    if (magnet_to_shift.at(m).GetXShift() != 0) x = ApplyOperation(x, magnet_to_shift.at(m).GetXShift(), op);
    if (magnet_to_shift.at(m).GetYShift() != 0) y = ApplyOperation(y, magnet_to_shift.at(m).GetYShift(), op);
    if (magnet_to_shift.at(m).GetZShift() != 0) {
      z = ApplyOperation(z, magnet_to_shift.at(m).GetZShift(), op);
      x = ApplyOperation(x, 
                         ApplyOperation(sx, magnet_to_shift.at(m).GetZShift(), "*"), 
                         "+");
      y = ApplyOperation(y, 
                         ApplyOperation(sy, magnet_to_shift.at(m).GetZShift(), "*"), 
                         "+");
    }
  }
}

void ProtonTransport::SetStrengthRatio(const Magnet& magnet, double ratio) {
  magnet_to_ratio[magnet] = ratio;
}

void ProtonTransport::ApplyStrengthRatio(const Magnet& m, double& strength) {
  if (magnet_to_ratio.find(m) != magnet_to_ratio.end()) {
    strength = strength * magnet_to_ratio.at(m);
  }
}

void ProtonTransport::SetProcessedFileName(const std::string& filename) {
  processed_filename = filename;
}

void ProtonTransport::simple_quadrupole(double L, double K1L, double rect_x, double rect_y, double el_x, double el_y, bool verbose=false){
  iterators.IncreaseItByOne("QUADRUPOLE_it");
  //std::cout << "Q" << iterators.GetIt("QUADRUPOLE_it") << std::endl;
  ApplyStrengthRatio(Quadrupole(iterators.GetIt("QUADRUPOLE_it"), 0), K1L);

  if (fabs(K1L) < 1.e-15)
  {
    simple_drift(L);
    return;
  }

  DoShift(Quadrupole(iterators.GetIt("QUADRUPOLE_it"), 0), "-"); 

  double x0 = x;
  double y0 = y;
  double z0 = z;
  double sx0 = sx;
  double sy0 = sy;

  z0 += L;
  double qk  = sqrt((fabs(K1L) * beam_energy)/(pz * L));
  double qkl = qk * L;
  double x_tmp = x0;
  double y_tmp = y0;

  if (K1L >= 0.) //horizontal focussing
  {
    fabs(x0)  > 1.e-15 ? x0 =  cos(qkl) * x0       : x0 = 0.;
    fabs(sx0) > 1.e-15 ? x0 += sin(qkl) * sx0 / qk : x0 += 0.;

    fabs(y0)  > 1.e-15 ? y0 = cosh(qkl) * y0        : y0 = 0.;
    fabs(sy0) > 1.e-15 ? y0 += sinh(qkl) * sy0 / qk : y0 += 0.;

    fabs(sx0) > 1.e-15 ? sx0 = cos(qkl) * sx0           : sx0 = 0.;
    fabs(x_tmp)  > 1.e-15 ? sx0 += -qk * sin(qkl) * x_tmp : sx0 += 0.;

    fabs(sy0) > 1.e-15 ? sy0 = cosh(qkl) * sy0          : sy0 = 0.;
    fabs(y_tmp)  > 1.e-15 ? sy0 += qk * sinh(qkl) * y_tmp : sy0 += 0.;
  }
  else //vertical focussing
  {
    fabs(y0)  > 1.e-15 ? y0 =  cos(qkl) * y0       : y0 = 0.;
    fabs(sy0) > 1.e-15 ? y0 += sin(qkl) * sy / qk : y0 += 0.;

    fabs(x0)  > 1.e-15 ? x0 = cosh(qkl) * x0        : x0 = 0.;
    fabs(sx0) > 1.e-15 ? x0 += sinh(qkl) * sx0 / qk : x0 += 0.;

    fabs(sy0) > 1.e-15 ? sy0 = cos(qkl) * sy0           : sy0 = 0.;
    fabs(y_tmp)  > 1.e-15 ? sy0 += -qk * sin(qkl) * y_tmp : sy0 += 0.;

    fabs(sx0) > 1.e-15 ? sx0 = cosh(qkl) * sx0          : sx0 = 0.;
    fabs(x_tmp)  > 1.e-15 ? sx0 += qk * sinh(qkl) * x_tmp : sx0 += 0.;
  }
  if (!isLost(x0, y0, rect_x, rect_y, el_x, el_y)) {
    x = x0;
    y = y0;
    z = z0;
    sx = sx0;
    sy = sy0;
  } else {
    lost_protons.push_back(std::vector<double>{px, py, pz});
  }
  

  DoShift(Quadrupole(iterators.GetIt("QUADRUPOLE_it"), 0), "+"); 


// std::cout<<numb_of_obj_uses<<'\n';
  
  if (!verbose) return;
  cout << "QUADRUPOLE\t";
  cout << "z [m]: " << z;
  cout << "\tx [mm]: " << x*1.e3; 
  cout << "\ty [mm]: " << y*1.e3;
  cout << "\tpx [GeV]: " << px;
  cout << "\tpy [GeV]: " << py;
  cout << "\tpz [GeV]: " << pz;
  cout << "\tsx: " << sx;
  cout << "\tsy: " << sy << endl;
}

/**
\brief Extract beam elements from twiss file.

Twiss files may provide 
*/
void ProtonTransport::PrepareBeamline(bool verbose=false, bool is_default=false){

  vector <string> sorted_param; //type, S, L, HKICK, VKICK, K0L, K1L, K2L, K3L, APERTYPE, APER_1, APER_2, APER_3, APER_4, X, Y, PX, PY
  vector <string> unsorted_name;
  vector <string> unsorted_param;

  int n_elements = 18;
  int sorting_order[n_elements];
  for (int a=0; a<n_elements; a++) sorting_order[a] = 0;
  string sorting_order_names[n_elements] = {"KEYWORD", "S", "L", "HKICK", "VKICK", "K0L", "K1L", "K2L", "K3L", "APERTYPE", "APER_1", "APER_2", "APER_3", "APER_4", "X", "Y", "PX", "PY"};

  bool IsIP1 = false;

  ifstream in;
  in.open(processed_filename.c_str());
  if (access(processed_filename.c_str(), F_OK)) {cout << "ERROR! No file named: " << processed_filename << endl; return;}
  
  while (!in.eof())
  {
    string line;
    if (in.peek() == 64) //64 is '@' symbol
    {
      //these lines are comments, not used for now in the code so can be skipped
      in.ignore(5000, '\n');
      continue;
    }
    if (in.peek() == 42) //64 is '*' symbol
    {
      //this line contains names of variables; this can differ between twiss files as order depends on parameters given at MAD-X generation step
      getline(in, line);
      istringstream ss(line);
      for (string keyword; ss >> keyword; ) unsorted_name.push_back(keyword);
    
      for (unsigned int a = 0; a<unsorted_name.size(); a++) 
        for (int b=0; b<n_elements; b++)
          if ((unsorted_name.at(a)).compare(sorting_order_names[b]) == 0) sorting_order[b] = a-1; // -1 because description starts with '*' symbol whereas values does not have it

      for (int a=0; a<n_elements; a++)
      {
        if ((a < 9) && sorting_order[a] == 0) {cout << "ERROR! Key element: " << sorting_order_names[a] << " is missing in Twiss file!" << endl; return;}
        if ((a >= 9) && (a < 14) && sorting_order[a] == 0) {cout << "WARNING! Information about aperture is missing. Will not be considered." << endl;}
        if ((a >= 14) && (a < 18) && sorting_order[a] == 0) {cout << "WARNING! Information about " << sorting_order_names[a] << " is missing. Will not be considered." << endl;}
      }
      
      continue;
    }
    if (sorting_order[0] == 0) {cout << "ERROR! In Twiss file there is no line starting with '*' which defines element type..." << endl; return;}
    
    if (in.peek() == 36) //64 is '@' symbol
    {
      //these lines MAD-X parameter types, can be skipped
      in.ignore(5000, '\n');
      continue;
    }
    
    if (in.peek() == EOF) break;
    
    if (verbose) for (int b=0; b<n_elements; b++) cout << sorting_order[b] << endl;
    
    getline(in, line);
    istringstream ss(line);
    unsorted_param.clear();
    for (string value; ss >> value; )
    {
      if (value.compare("IP1") != 0) IsIP1 = true;
      unsorted_param.push_back(value);
    }
    if (!IsIP1) continue; //some twiss files start before the IP... also position should be counted from IP1
    
    sorted_param.clear();
    for (int a=0; a<n_elements; a++) sorted_param.push_back(unsorted_param.at(sorting_order[a]));
    element.push_back(sorted_param);
    
  }

  if (is_default) SetPositions();
}

void ProtonTransport::SetPositions() {
  if (element.empty()) {
    std::cout << "Use PrepareBeamline() first!" << std::endl;
    return;
  }
  for (const auto& el : element) {
    if (stod(el[1]) > 205.) break;
    if (el[0] == "\"QUADRUPOLE\"") {
      iterators.IncreaseItByOne("QUADRUPOLE_it");
      magnets.push_back(Quadrupole(iterators.GetIt("QUADRUPOLE_it"), stod(el[1])));
    } else if (el[0] == "\"RBEND\"") {
      iterators.IncreaseItByOne("RBEND_it");
      magnets.push_back(Dipole(iterators.GetIt("RBEND_it"), stod(el[1])));
    } else if (el[0] == "\"VKICKER\"") {
      iterators.IncreaseItByOne("VKICKER_it");
      magnets.push_back(VerticalKicker(iterators.GetIt("VKICKER_it"), stod(el[1])));
    } else if (el[0] == "\"HKICKER\"") {
      iterators.IncreaseItByOne("HKICKER_it");
      magnets.push_back(HorizontalKicker(iterators.GetIt("HKICKER_it"), stod(el[1])));
    }
  }
}

std::vector<Magnet> ProtonTransport::GetMagnets() const {
  return magnets;
}

void ProtonTransport::SetMagnets(const std::vector<Magnet>& magnets_) {
  magnets  = magnets_;
}

bool ProtonTransport::isLost(double x0, double y0, double rect_x, double rect_y, double el_x, double el_y) {
  if ((x0*x0/(el_x*el_x) + y0*y0/(el_y*el_y) > 1) || ((fabs(x0) > rect_x) || (fabs(y0) > rect_y))) {
    is_current_lost=true;
    return 1;
  } else {

    return 0;
  }
}

void ProtonTransport::simple_tracking(double obs_point){

  int m_process_code;
  vector<float> *m_px;
  vector<float> *m_py;
  vector<float> *m_pz;
  vector<float> *m_e;
  
  TBranch        *b_process_code;   //!
  TBranch        *b_px;   //!
  TBranch        *b_py;   //!
  TBranch        *b_pz;   //!
  TBranch        *b_e;   //!

  m_px = 0;
  m_py = 0;
  m_pz = 0;
  m_e = 0;


  TFile *f = new TFile("pythia8_13TeV_protons_100k.root", "READ");
  TTree * ntuple;
  f->GetObject("ntuple",ntuple);

  TH1F * h_sigma;
  f->GetObject("sigma",h_sigma);
  TH1F * h_eff;
  f->GetObject("efficiency",h_eff);

  gROOT->ProcessLine("#include <vector>");

  ntuple->SetMakeClass(1);

  ntuple->SetBranchAddress("process_code", &m_process_code, &b_process_code);
  ntuple->SetBranchAddress("px", &m_px, &b_px);
  ntuple->SetBranchAddress("py", &m_py, &b_py);
  ntuple->SetBranchAddress("pz", &m_pz, &b_pz);
  ntuple->SetBranchAddress("e", &m_e, &b_e);

  int nevents = ntuple->GetEntries();
  
  int n_process_code;
  float n_px, n_py, n_pz, n_e;
  float n_x, n_y, n_sx, n_sy;

  FileName* fn = new FileName(processed_filename, !magnet_to_shift.empty(), !magnet_to_ratio.empty());
  fn->ProcessFileName();
  optics_root_file_name = fn->GetOutputFileName();

  std::cout << "The ROOT output file: " << optics_root_file_name << std::endl; 
  TFile * p = new TFile(optics_root_file_name.c_str(),"recreate");

  TH1F * cs = new TH1F("sigma", "cross-section [mb]", 1, 0., 1.);
  TH1F * efficiency = new TH1F("efficiency", "efficiency", 1, 0., 1.);
  TTree * tree = new TTree("ntuple", "ntuple");
  tree->Branch("process_code", &n_process_code);
//  tree->Branch("n_particles", &n_particles);
  tree->Branch("px", &n_px);
  tree->Branch("py", &n_py);
  tree->Branch("pz", &n_pz);
  tree->Branch("e", &n_e);
  tree->Branch("x", &n_x);
  tree->Branch("y", &n_y);
  tree->Branch("sx", &n_sx);
  tree->Branch("sy", &n_sy);
  
  for (int evt=0; evt<nevents; evt++)
  {
    //std::cout << "\n\nRESET\n\n" << std::endl;
    iterators = {};
    ntuple->GetEntry(evt);

//  for (double E=6500.; E<=1.00001*7000.; E += 100.)
  {
  //  for (double x0 = 2.e-5; x0 >= -1.000001*2.e-5; x0 -= 1.e-5)
    {
    // for (double y0 = -2.e-5; y0 <= 1.000001*2.e-5; y0 += 1.e-5)
      {
      //  for (double px0 = 0.4e0; px0 >=-1.000001*0.4e0; px0 -= 0.2e0)
        {
        //  for (double py0 = -0.4e0; py0 <=1.000001*0.4e0; py0 += 0.2e0)
          {
//            double z0 = 0.;
//double E=7000., x0=0., y0=0., px0=0., py0=0.;            
 //           cout << E << "\t" << x0 << "\t" << y0 << "\t" << z0 << "\t" << px0 << "\t" << py0 << "\t";

  x = 0.;
  y = 0.;
  z = 0.;
  px = m_px->at(0);
  py = m_py->at(0) + 140.e-6*6500.;
  pz = m_pz->at(0);
  sx = px/pz;
  sy = py/pz;

  BeampipesAreSeparated = false;

  bool Observe = false, Observed = false;
  for (unsigned int a=0; a<element.size(); a++)
  {  
    if (fabs(z+stod(element[a].at(2)) - obs_point) < 1.e-3 && !Observe && !Observed) {Observed = true; Observe = true;}
    else Observe = false;
    if ((element[a].at(0)).compare("\"MARKER\"") == 0) ProtonTransport::Marker(Observe); //Marker(true) will return proton x, y, z, px, py, pz at position of marker
    else if ((element[a].at(0)).compare("\"DRIFT\"") == 0) {
      ProtonTransport::simple_drift(stod(element[a].at(2)), Observe); // L
    }
    else if ((element[a].at(0)).compare("\"RBEND\"") == 0) { 
      ProtonTransport::simple_rectangular_dipole(stod(element[a].at(2)), stod(element[a].at(5)), 
                                                 stod(element[a].at(10)), stod(element[a].at(11)), stod(element[a].at(12)), stod(element[a].at(13))); //L, K0L
    }  
    else if ((element[a].at(0)).compare("\"HKICKER\"") == 0) { 
      ProtonTransport::simple_horizontal_kicker(stod(element[a].at(2)), stod(element[a].at(3)),
                                                stod(element[a].at(10)), stod(element[a].at(11)), stod(element[a].at(12)), stod(element[a].at(13))); //L, HKICK
    }
    else if ((element[a].at(0)).compare("\"VKICKER\"") == 0) { 
      ProtonTransport::simple_vertical_kicker(stod(element[a].at(2)), stod(element[a].at(4)),
                                              stod(element[a].at(10)), stod(element[a].at(11)), stod(element[a].at(12)), stod(element[a].at(13))); //L, VKICK
    }
    else if ((element[a].at(0)).compare("\"QUADRUPOLE\"") == 0) { 
      ProtonTransport::simple_quadrupole(stod(element[a].at(2)), stod(element[a].at(6)),
                                         stod(element[a].at(10)), stod(element[a].at(11)), stod(element[a].at(12)), stod(element[a].at(13)), Observe); //L, K1L 
    }
    else if ((element[a].at(0)).compare("\"MULTIPOLE\"") == 0)
    {
      if ( fabs(stod(element[a].at(5))) > 1.e-10 )
      {
        cout << "Warning! MULTIPOLE taken as rectangular dipole! Check in twiss files if this is correct! Position: " << element[a].at(1) << endl;
        ProtonTransport::simple_rectangular_dipole(stod(element[a].at(2)), stod(element[a].at(5)), 
                                                   stod(element[a].at(10)), stod(element[a].at(11)), stod(element[a].at(12)), stod(element[a].at(13))); //L, K0L
      }
      else if ( fabs(stod(element[a].at(3))) > 1.e-10 )
      {
        cout << "Warning! MULTIPOLE taken as horizontal kicker! Check in twiss files if this is correct! Position: " << element[a].at(1) << endl;
        ProtonTransport::simple_horizontal_kicker(stod(element[a].at(2)), stod(element[a].at(3)),
                                                  stod(element[a].at(10)), stod(element[a].at(11)), stod(element[a].at(12)), stod(element[a].at(13))); //L, HKICK
      }
      else if ( fabs(stod(element[a].at(4))) > 1.e-10 )
      {
        cout << "Warning! MULTIPOLE taken as vertical kicker! Check in twiss files if this is correct! Position: " << element[a].at(1) << endl;
        ProtonTransport::simple_vertical_kicker(stod(element[a].at(2)), stod(element[a].at(4)),
                                                stod(element[a].at(10)), stod(element[a].at(11)), stod(element[a].at(12)), stod(element[a].at(13))); //L, VKICK
      }
      else if ( fabs(stod(element[a].at(6))) > 1.e-10 )
      {
        cout << "Warning! MULTIPOLE taken as quadrupole! Check in twiss files if this is correct! Position: " << element[a].at(1) << endl;
        ProtonTransport::simple_quadrupole(stod(element[a].at(2)), stod(element[a].at(6)),
                                           stod(element[a].at(10)), stod(element[a].at(11)), stod(element[a].at(12)), stod(element[a].at(13)), Observe); //L, K1L 
      }
      else
      {
//TODO        cout << "Warning! MULTIPOLE taken as drift! Check in twiss files if this is correct! Position: " << element[a].at(1) << endl;
        ProtonTransport::simple_drift(stod(element[a].at(2)), Observe); // L
      }

    }
/*
   The following elements are taken as a drift (if L!=0) or monitor (if L=0):
      * SOLENOID (TODO)
      * RCOLLIMATOR (TODO)
      * MONITOR (L=0 anyway)
      * PLACEHOLDER
      * INSTRUMENT
*/
    else
    {
      if (stod(element[a].at(2)) > 1.e-10) ProtonTransport::simple_drift(stod(element[a].at(2)));
      else ProtonTransport::Marker(Observe);
    }
    


    if (z > 130. && !BeampipesAreSeparated)
    {
      BeampipesAreSeparated = true;
      x += BeampipeSeparation;
    }
    if (z > obs_point)
    {
      //cout << element[a].at(0) << "\t" << z << "\t" << x  << "\t" << sx << "\t" << sx*(z - obs_point) << endl;
      n_process_code = m_process_code;
      n_px = m_px->at(0);
      n_py = m_py->at(0);
      n_pz = m_pz->at(0);
      n_e = m_e->at(0);
      n_x = x - sx*(z - obs_point);
      n_y = y - sy*(z - obs_point);
      n_sx = sx;
      n_sy = sy;
      
      tree->Fill();
      
      
      break;
    }

}
  
}

}

}

}

}

}

  cs->Fill(0.5, h_sigma->GetBinContent(1));
  cs->Write();
  efficiency->Fill(0.5, h_eff->GetBinContent(1));
  efficiency->Write();
  tree->Write();
//  p->Write();
  p->Close();
  cout<<lost_protons.size()<<'\n';

}

std::string ProtonTransport::GetROOTOutputFileName() const {
  return optics_root_file_name;
}

void ProtonTransport::WriteLostProtonsInCsv(const std::string& filename) const {
  std::ofstream f;
  f.open(filename, std::fstream::app);

  f << "No," << "px," << "py," << "pz\n";

  for (int i = 0; i < lost_protons.size(); i++) {
    f << i << "," << lost_protons[i][0] 
      << "," << lost_protons[i][1] << "," 
      << lost_protons[i][2] << "\n";
  }

  f.close();
}

void ProtonTransport::WriteChangesInCsv(const std::string& filename, DistributionsDifference* diff, int run_id) {
  std::map<std::string, double> var_name_to_rms = diff->GetRMSs("histos_1d_diffs");
  std::map<std::string, double> var_name_to_mean = diff->GetMeans("histos_1d_diffs");

  std::ofstream f;
  f.open(filename, std::fstream::app);
  
  // Check if file's not empty (or we already have columns names)
  std::ifstream f_check;
  f_check.open(filename);
  if (f_check.peek() == std::ifstream::traits_type::eof()) {
    f << "No," << "Magnet," << "Position," << "x_shift[m]," << "y_shift[m]," << "z_shift[m]," << "Strength_ratio";
    for (const auto& [var_name, rms] : var_name_to_rms) {
      f << "," << "RMS(" << var_name << ")," << "Mean(" << var_name << ")"; 
    }
    f << "\n";
  }
  f_check.close();
  
  int local_run_id = 1;
  if (!magnet_to_shift.empty()) {
    for (const auto& [magnet, shift] : magnet_to_shift) {
      f << run_id << "_" << local_run_id << "," 
        << magnet.GetName() << "," 
        << magnet.GetPosition() << "," << shift.GetXShift() << "," 
        << shift.GetYShift() << "," << shift.GetZShift() << ","; 

      if (magnet_to_ratio.find(magnet) != magnet_to_ratio.end()) {
        f << magnet_to_ratio.at(magnet);
      } else {
        f << "1";
      }

      if (local_run_id == 1) {
        for (const auto& [var_name, rms] : var_name_to_rms) {
          f << "," << rms << "," << var_name_to_mean.at(var_name);
        }
      } else {
        f << ",,,,,,,,,,,,,,";
      }
      ++local_run_id;
      f << "\n";
    }
  }
  
  if (!magnet_to_ratio.empty()) {
    for (const auto& [magnet, ratio] : magnet_to_ratio) {
      if (magnet_to_shift.find(magnet) == magnet_to_shift.end()) {
        f << run_id << "_" << local_run_id << "," 
          << magnet.GetName() << "," 
          << magnet.GetPosition() << "," << 0 << "," << 0 << "," << 0 << ","; 

        f << ratio;

        if (local_run_id == 1) {
          for (const auto& [var_name, rms] : var_name_to_rms) {
            f << "," << rms << "," << var_name_to_mean.at(var_name);
          }
        } else {
          f << ",,,,,,,,,,,,,,";
        }
        ++local_run_id;
        f << "\n";
      }
    }
  }
  
  f.close();
}

int main() {
  std::string optics_file_name = "optics_PPSS_2020/alfaTwiss1.txt_beta40cm_6500GeV_y-185murad";
  std::string changes_fn = "HKICKER_variants.csv";

  ProtonTransport* p_default = new ProtonTransport;

  p_default->SetProcessedFileName(optics_file_name);
  p_default->PrepareBeamline(false, true);
  p_default->simple_tracking(205.);

  std::vector<Magnet> magnets = p_default->GetMagnets();

  remove(changes_fn.c_str());

  int run_id = 1;
  TRandom* r = new TRandom();
  for (int i = 0; i < 1; i++) {
    std::cout << "Run: " << run_id 
              << ", Changes fn: " << changes_fn 
              << std::endl;

    ProtonTransport* p = new ProtonTransport;
    p->SetProcessedFileName(optics_file_name);
    p->PrepareBeamline(false);

    for (const auto& magnet : magnets) {
      //if (magnet.GetType() == "QUADRUPOLE") {
        p->SetShift(magnet, Shift(0.00025, 
                                  0.00025, 
                                  0.001));
      //}
      p->SetStrengthRatio(magnet,1.0005);
    }

    p->simple_tracking(205.);

    DistributionsDifference* diff = new DistributionsDifference(p_default->GetROOTOutputFileName(), 
                                                                p->GetROOTOutputFileName());
    p->WriteChangesInCsv(changes_fn, diff, run_id++);

    std::cout << "done\n\n"; 
    delete p;
  }
  delete p_default;

  return 0;
}

