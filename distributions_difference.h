#ifndef distributions_difference_h
#define distributions_difference_h

#include <string>
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

class DistributionsDifference {
public:
  DistributionsDifference(const std::string&, const std::string&);

private:
  std::map<std::string, TH1F*> name_to_hist1, 
                               name_to_hist2, 
                               name_to_hist_1d, 
                               name_to_hist_2d;
};

#endif
