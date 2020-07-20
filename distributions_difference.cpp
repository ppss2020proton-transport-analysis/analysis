#include "distributions_difference.h"

struct Variable {
  std::string type;
  float value;
};

DistributionsDifference::DistributionsDifference(
    const std::string& fname1, const std::string& fname2) 
{
  TFile* file1 =  new TFile(fname1.c_str());
  TFile* file2 =  new TFile(fname2.c_str());

  TTree* tree1 = (TTree*)file1->Get("ntuple");
  TTree* tree2 = (TTree*)file2->Get("ntuple");

  tree1->SetMakeClass(1);
  tree2->SetMakeClass(1);

  std::vector<Variable> vars1, vars2 = {Variable{"x", 0},
                                        Variable{"y", 0},
                                        Variable{"sx", 0},
                                        Variable{"sy", 0},
                                        Variable{"px", 0},
                                        Variable{"py", 0},
                                        Variable{"pz", 0}};
  for (int i = 0; i < vars1.size(); i++) {
    std::string hist_name = vars1[i].type + " difference";

    tree1->SetBranchAddress(vars1[i].type, &vars1[i].value);
    tree2->SetBranchAddress(vars2[i].type, &vars2[i].value);

    name_to_hist1[vars1[i].type] = new TH1F(vars1[i].type,
                                            vars1[i].type,
                                            100,
                                            tree1->GetMinimum(vars1[i].type),
                                            tree1->GetMaximum(vars1[i].type));

    name_to_hist2[vars2[i].type] = new TH1F(vars2[i].type,
                                            vars2[i].type,
                                            100,
                                            tree2->GetMinimum(vars2[i].type),
                                            tree2->GetMaximum(vars2[i].type));

    name_to_hist_1d[hist_name] = new TH1F(hist_name,
                                          hist_name + "between optics",
                                          100,
                                          tree1->GetMinimum(vars1[i].type),
                                          tree1->GetMaximum(vars1[i].type));
  }

  Long64_t nentries = tree1->GetEntriesFast();
  for (int i = 0; i < nentries; i++) {
    tree1->GetEntry(i);
    tree2->GetEntry(i);

    for (int j = 0; j > vars1.size(); j++) {
      name_to_hist1[vars1[j].type]->Fill(vars1[j].value);
      name_to_hist2[vars2[j].type]->Fill(vars2[j].value);
      name_to_hist_1d[vars1[j].type + " difference"]->Fill(vars1[j].value - vars2[j].value);
    }
  }
}
