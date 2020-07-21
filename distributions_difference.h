#ifndef distributions_difference_h
#define distributions_difference_h

#include <string>
#include <iostream>
#include <vector>
#include <map>
#include "TH1F.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TTree.h"
#include "TLegend.h"
#include "TStyle.h"
#include <TROOT.h>

using VarNameToHist = std::map<std::string, TH1F*>;
using VarNameToHist2D = std::map<std::string, TH2F*>;

class DistributionsDifference {
public:
  DistributionsDifference(const std::string&, const std::string&);

  std::map<std::string, double> GetRMSs(const std::string&) const;

  std::map<std::string, double> GetMeans(const std::string&) const;

  std::map<std::string, double> GetRMSs_2D(const std::string&) const;

  std::map<std::string, double> GetMeans_2D(const std::string&) const;

private:
  std::map<std::string, VarNameToHist> set_name_to_histos;
  std::map<std::string, VarNameToHist2D> set_name_to_histos2D;
};

#endif
