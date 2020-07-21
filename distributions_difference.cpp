#include "distributions_difference.h"

struct Variable {
  std::string type;
  float value;
};

DistributionsDifference::DistributionsDifference(
    const std::string& fname1, const std::string& fname2) 
{
  VarNameToHist var_name_to_hist1, 
                               var_name_to_hist2, 
                               var_name_to_hist_1d_diffs;
                               
  VarNameToHist2D var_name_to_hist_2d_diffs;                          

  TFile* file1 =  new TFile(fname1.c_str());
  TFile* file2 =  new TFile(fname2.c_str());

  TTree* tree1 = (TTree*)file1->Get("ntuple");
  TTree* tree2 = (TTree*)file2->Get("ntuple");

  tree1->SetMakeClass(1);
  tree2->SetMakeClass(1);

  std::vector<Variable> vars1 = {Variable{"x", 0},
                                 Variable{"sx", 0},
                                 Variable{"y", 0},
                                 Variable{"sy", 0},
                                 Variable{"px", 0},
                                 Variable{"py", 0},
                                 Variable{"pz", 0}};
  std::vector<Variable> vars2 = {Variable{"x", 0},
                                 Variable{"sx", 0},
                                 Variable{"y", 0},
                                 Variable{"sy", 0},
                                 Variable{"px", 0},
                                 Variable{"py", 0},
                                 Variable{"pz", 0}};

  for (int i = 0; i < vars1.size(); i++) {
    std::string hist_name = vars1[i].type + " difference";
    

    tree1->SetBranchAddress(vars1[i].type.c_str(), &vars1[i].value);
    tree2->SetBranchAddress(vars2[i].type.c_str(), &vars2[i].value);

    var_name_to_hist1[vars1[i].type] = new TH1F((vars1[i].type + "1").c_str(),
                                                vars1[i].type.c_str(),
                                                100,
                                                tree1->GetMinimum(vars2[i].type.c_str()),
                                                tree1->GetMaximum(vars2[i].type.c_str()));

    var_name_to_hist2[vars2[i].type] = new TH1F((vars2[i].type + "2").c_str(),
                                            vars2[i].type.c_str(),
                                            100,
                                            tree2->GetMinimum(vars2[i].type.c_str()),
                                            tree2->GetMaximum(vars2[i].type.c_str()));

    var_name_to_hist_1d_diffs[hist_name] = new TH1F(hist_name.c_str(),
                                          (hist_name + " between optics").c_str(),
                                          200,
                                          tree1->GetMinimum(vars1[i].type.c_str()) - tree2->GetMaximum(vars2[i].type.c_str()),
                                          tree1->GetMaximum(vars1[i].type.c_str()) - tree2->GetMinimum(vars2[i].type.c_str()));
    for (int j = 4; j < 7; ++j) //loop over momenta
    {
      std::string hist_name2D = vars1[i].type + " difference vs " + vars1[j].type;

      var_name_to_hist_2d_diffs[hist_name2D] = new TH2F(hist_name2D.c_str(),
          (hist_name2D + " between optics").c_str(),
          100,
          tree1->GetMinimum(vars1[i].type.c_str()) - tree2->GetMaximum(vars2[i].type.c_str()),
          tree1->GetMaximum(vars1[i].type.c_str()) - tree2->GetMinimum(vars2[i].type.c_str()),
          100,
          tree1->GetMinimum(vars1[j].type.c_str()) ,
          tree1->GetMaximum(vars1[j].type.c_str()) );


    }




  }

  //another 2d loop here

  Long64_t nentries = tree1->GetEntriesFast();
  for (int i = 0; i < nentries; i++) {
    tree1->GetEntry(i);
    tree2->GetEntry(i);

    for (int j = 0; j < vars1.size(); j++) {
      var_name_to_hist1.at(vars1[j].type)->Fill(vars1[j].value);
      var_name_to_hist2.at(vars2[j].type)->Fill(vars2[j].value);

      //std::cout<<vars1[j].type + " difference"+"\n";
      var_name_to_hist_1d_diffs.at(vars1[j].type + " difference")->Fill(vars1[j].value - vars2[j].value);

       
      
      for (int k = 4; k < 7; ++k) //loop over momenta
      {
        //std::cout<<(vars1[j].type + " difference vs " + vars1[k].type +"\n");
        var_name_to_hist_2d_diffs.at(vars1[j].type + " difference vs " + vars1[k].type)->Fill(vars1[j].value - vars2[j].value,vars1[k].value);

      }


    }

  }

  set_name_to_histos["histos1"] = var_name_to_hist1;
  set_name_to_histos["histos2"] = var_name_to_hist2;
  set_name_to_histos["histos_1d_diffs"] = var_name_to_hist_1d_diffs;
  set_name_to_histos2D["histos_2d_diffs"] = var_name_to_hist_2d_diffs;


}

std::map<std::string, double> DistributionsDifference::GetRMSs(const std::string& set_name) const {
  std::map<std::string, double> var_name_to_rms;
  VarNameToHist var_name_to_hist = set_name_to_histos.at(set_name);

  for (const auto& [var_name, hist] : var_name_to_hist) {
    var_name_to_rms[var_name] = hist->GetRMS();
  }
  return var_name_to_rms;
}

std::map<std::string, double> DistributionsDifference::GetMeans(const std::string& set_name) const {
  std::map<std::string, double> var_name_to_mean;
  VarNameToHist var_name_to_hist = set_name_to_histos.at(set_name);

  for (const auto& [var_name, hist] : var_name_to_hist) {
    var_name_to_mean[var_name] = hist->GetMean();
  }
  return var_name_to_mean;
}
