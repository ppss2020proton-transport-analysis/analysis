#include<iostream>
#include"TCanvas.h"
#include"TF1.h"
#include"TH1F.h"
#include"TH2F.h"
#include"TFile.h"
#include"TTree.h"
#include "TLegend.h"
#include "TStyle.h"
#include <TROOT.h>
#include <TChain.h>
#include <string>
#include "TSystem.h"
#include "TSystemDirectory.h"

using namespace std;



int main() {

	gROOT->ProcessLine( "gErrorIgnoreLevel = 1001;");		// This line prevents text output each time canvas is saved


  TFile* file_optics1 = new TFile(      
    "root_PPSS_2020/1pythia8_13TeV_protons_100k_transported_205m_beta40cm_6500GeV_y-185murad.root");    //This file contains results of transport code without any shifts


  TFile* file_optics2 = new TFile(
    "root_PPSS_2020/1_shifted__changed_strength_pythia8_13TeV_protons_100k_transported_205m_beta40cm_6500GeV_y-185murad.root");

  TTree* tree_optics1 = (TTree*)file_optics1->Get("ntuple");
  TTree* tree_optics2 = (TTree*)file_optics2->Get("ntuple");

  tree_optics1->SetMakeClass(1);
  tree_optics2->SetMakeClass(1);

  Float_t x1, x2, y1, y2, sx1, sx2, sy1, sy2, px, py, pz,px1,px2,ev1,ev2;
  bool is_lost_1,is_lost_2;

  tree_optics1->SetBranchAddress("ev_id", &ev1);
  tree_optics2->SetBranchAddress("ev_id", &ev2);

  tree_optics1->SetBranchAddress("is_lost", &is_lost_1);
  tree_optics2->SetBranchAddress("is_lost", &is_lost_2);

  tree_optics1->SetBranchAddress("x", &x1);
  tree_optics2->SetBranchAddress("x", &x2);

  tree_optics1->SetBranchAddress("y", &y1);
  tree_optics2->SetBranchAddress("y", &y2);

  tree_optics1->SetBranchAddress("sx", &sx1);
  tree_optics2->SetBranchAddress("sx", &sx2);

  tree_optics1->SetBranchAddress("sy", &sy1);
  tree_optics2->SetBranchAddress("sy", &sy2);

  tree_optics1->SetBranchAddress("px", &px1);
  tree_optics1->SetBranchAddress("py", &py);
  tree_optics1->SetBranchAddress("pz", &pz);

  tree_optics2->SetBranchAddress("px", &px2);
  Long64_t nentries = tree_optics1->GetEntriesFast();
  Long64_t nentries2 = tree_optics2->GetEntriesFast();
  //std::cout<< std::endl << "Number of entries: " << nentries << std::endl<< std::endl;


  // Here I calcluate means and StdDev for differences, which are later used to set axis limits for 2D histos, those 4 histograms below are not plotted

  TH1F* x_diff_binset = new TH1F("x_diff_binset", "", 100, 1.1*(tree_optics1->GetMinimum("x")-tree_optics2->GetMaximum("x")), 1.1*(tree_optics1->GetMaximum("x")-tree_optics2->GetMinimum("x")) );
  TH1F* y_diff_binset = new TH1F("y_diff_binset", "", 100, 1.1*(tree_optics1->GetMinimum("y")-tree_optics2->GetMaximum("y")), 1.1*(tree_optics1->GetMaximum("y")-tree_optics2->GetMinimum("y")) );
  TH1F* sx_diff_binset = new TH1F("sx_diff_binset", "", 100, 1.1*(tree_optics1->GetMinimum("sx")-tree_optics2->GetMaximum("sx")), 1.1*(tree_optics1->GetMaximum("sx")-tree_optics2->GetMinimum("sx")) );
  TH1F* sy_diff_binset = new TH1F("sy_diff_binset", "", 100, 1.1*(tree_optics1->GetMinimum("sy")-tree_optics2->GetMaximum("sy")), 1.1*(tree_optics1->GetMaximum("sy")-tree_optics2->GetMinimum("sy")) );


  for (Int_t i = 0; i < nentries; i++) {
    tree_optics1->GetEntry(i);
    tree_optics2->GetEntry(i);
    if (is_lost_1||is_lost_2){
      continue;
    }
    else
    {
    x_diff_binset->Fill(x1-x2);
    y_diff_binset->Fill(y1-y2);
    sx_diff_binset->Fill(sx1-sx2);
    sy_diff_binset->Fill(sy1-sy2);
    }
  }
 double num_of_Stdev= 7;

 ///brb




  TH1F* x_diff = new TH1F("x_diff", "x_diff [m]", 100, (x_diff_binset->GetMean()-num_of_Stdev*x_diff_binset->GetStdDev()),(x_diff_binset->GetMean()+num_of_Stdev*x_diff_binset->GetStdDev()) );
  TH1F* y_diff = new TH1F("y_diff", "y_diff [m]", 100, (y_diff_binset->GetMean()-num_of_Stdev*y_diff_binset->GetStdDev()),(y_diff_binset->GetMean()+num_of_Stdev*y_diff_binset->GetStdDev()) );
  TH1F* sx_diff = new TH1F("sx_diff", "sx_diff [rad]", 100, (sx_diff_binset->GetMean()-num_of_Stdev*sx_diff_binset->GetStdDev()),(sx_diff_binset->GetMean()+num_of_Stdev*sx_diff_binset->GetStdDev()) );
  TH1F* sy_diff = new TH1F("sy_diff", "sy_diff[rad]", 100, (sy_diff_binset->GetMean()-num_of_Stdev*sy_diff_binset->GetStdDev()),(sy_diff_binset->GetMean()+num_of_Stdev*sy_diff_binset->GetStdDev()) );

  TH1F* x_lost = new TH1F("x_lost", "lost protons", 100, (x_diff_binset->GetMean()-num_of_Stdev*x_diff_binset->GetStdDev()),(x_diff_binset->GetMean()+num_of_Stdev*x_diff_binset->GetStdDev()) );
  TH1F* y_lost = new TH1F("y_lost", "lost protons", 100, (y_diff_binset->GetMean()-num_of_Stdev*y_diff_binset->GetStdDev()),(y_diff_binset->GetMean()+num_of_Stdev*y_diff_binset->GetStdDev()) );
  TH1F* sx_lost = new TH1F("sx_lost", "lost protons", 100, (sx_diff_binset->GetMean()-num_of_Stdev*sx_diff_binset->GetStdDev()),(sx_diff_binset->GetMean()+num_of_Stdev*sx_diff_binset->GetStdDev()) );
  TH1F* sy_lost = new TH1F("sy_lost", "lost protons", 100, (sy_diff_binset->GetMean()-num_of_Stdev*sy_diff_binset->GetStdDev()),(sy_diff_binset->GetMean()+num_of_Stdev*sy_diff_binset->GetStdDev()) );



  for (Int_t i = 0; i < nentries; i++) {
    tree_optics1->GetEntry(i);
    tree_optics2->GetEntry(i);
    if (is_lost_1||is_lost_2){
      if(is_lost_1)x_lost->Fill(x1);
      if(is_lost_2)x_lost->Fill(x2);
      continue;
    }
    else
    {
    x_diff->Fill(x1-x2);
    y_diff->Fill(y1-y2);
    sx_diff->Fill(sx1-sx2);
    sy_diff->Fill(sy1-sy2);
    }
  }









 


  // 2D histos for the x, y, sx and sy vs px/py/pz difference between optics1_default(-185murad) and optics2_shifted(-185murad)
  TH2F* x_diff_vs_y_diff = new TH2F("x_diff_vs_y_diff", "x_diff_vs_y_diff;x_diff [m];y_diff [m];nOfEvents",
   		100,(x_diff_binset->GetMean()-num_of_Stdev*x_diff_binset->GetStdDev()),(x_diff_binset->GetMean()+num_of_Stdev*x_diff_binset->GetStdDev()),
      100,(y_diff_binset->GetMean()-num_of_Stdev*y_diff_binset->GetStdDev()),(y_diff_binset->GetMean()+num_of_Stdev*y_diff_binset->GetStdDev()));

  TH2F* x_diff_vs_sx_diff = new TH2F("x_diff_vs_sx_diff", "x_diff_vs_sx_diff;x_diff [m];sx_diff [rad];nOfEvents",
      100,(x_diff_binset->GetMean()-num_of_Stdev*x_diff_binset->GetStdDev()),(x_diff_binset->GetMean()+num_of_Stdev*x_diff_binset->GetStdDev()),
      100,(sx_diff_binset->GetMean()-num_of_Stdev*sx_diff_binset->GetStdDev()),(sx_diff_binset->GetMean()+num_of_Stdev*sx_diff_binset->GetStdDev()));

  TH2F* y_diff_vs_sy_diff = new TH2F("y_diff_vs_sy_diff", "y_diff_vs_sy_diff;y_diff [m];sy_diff [rad];nOfEvents",
      100,(y_diff_binset->GetMean()-num_of_Stdev*y_diff_binset->GetStdDev()),(y_diff_binset->GetMean()+num_of_Stdev*y_diff_binset->GetStdDev()),
      100,(sy_diff_binset->GetMean()-num_of_Stdev*sy_diff_binset->GetStdDev()),(sy_diff_binset->GetMean()+num_of_Stdev*sy_diff_binset->GetStdDev()));





  // TH2F* y_diff_vs_px = new TH2F("y_diff_vs_px", "y difference between default and shifted opics vs px;y_diff;px;nOfEvents",
  // 		100,(y_diff_binset->GetMean()-num_of_Stdev*y_diff_binset->GetStdDev()),(y_diff_binset->GetMean()+num_of_Stdev*y_diff_binset->GetStdDev()), 100, tree_optics1->GetMinimum("px"), tree_optics1->GetMaximum("px"));
  // TH2F* sx_diff_vs_px = new TH2F("sx_diff_vs_px", "sx difference between default and shifted opics vs px;sx_diff;px;nOfEvents",
  //  		100,(sx_diff_binset->GetMean()-num_of_Stdev*sx_diff_binset->GetStdDev()),(sx_diff_binset->GetMean()+num_of_Stdev*sx_diff_binset->GetStdDev()), 100, tree_optics1->GetMinimum("px"), tree_optics1->GetMaximum("px"));
  // TH2F* sy_diff_vs_px = new TH2F("sy_diff_vs_px", "sy difference between default and shifted opics vs px;sy_diff;px;nOfEvents",
  // 		100,(sy_diff_binset->GetMean()-num_of_Stdev*sy_diff_binset->GetStdDev()),(sy_diff_binset->GetMean()+num_of_Stdev*sy_diff_binset->GetStdDev()),
  //      100, tree_optics1->GetMinimum("px"), tree_optics1->GetMaximum("px"));

  // TH2F* x_diff_vs_py = new TH2F("x_diff_vs_py", "x difference between default and shifted opics vs py;x_diff;py;nOfEvents",
  // 		100,(x_diff_binset->GetMean()-num_of_Stdev*x_diff_binset->GetStdDev()),(x_diff_binset->GetMean()+num_of_Stdev*x_diff_binset->GetStdDev()), 100, tree_optics1->GetMinimum("py"), tree_optics1->GetMaximum("py"));
  // TH2F* y_diff_vs_py = new TH2F("y_diff_vs_py", "y difference between default and shifted opics vs py;y_diff;py;nOfEvents",
 	// 	100,(y_diff_binset->GetMean()-num_of_Stdev*y_diff_binset->GetStdDev()),(y_diff_binset->GetMean()+num_of_Stdev*y_diff_binset->GetStdDev()), 100, tree_optics1->GetMinimum("py"), tree_optics1->GetMaximum("py"));
  // TH2F* sx_diff_vs_py = new TH2F("sx_diff_vs_py", "sx difference between default and shifted opics vs py;sx_diff;py;nOfEvents",
 	// 	100,(sx_diff_binset->GetMean()-num_of_Stdev*sx_diff_binset->GetStdDev()),(sx_diff_binset->GetMean()+num_of_Stdev*sx_diff_binset->GetStdDev()), 100, tree_optics1->GetMinimum("py"), tree_optics1->GetMaximum("py"));
  // TH2F* sy_diff_vs_py = new TH2F("sy_diff_vs_py", "sy difference between default and shifted opics vs py;sy_diff;py;nOfEvents",
		// 100,(sy_diff_binset->GetMean()-num_of_Stdev*sy_diff_binset->GetStdDev()),(sy_diff_binset->GetMean()+num_of_Stdev*sy_diff_binset->GetStdDev()), 100, tree_optics1->GetMinimum("py"), tree_optics1->GetMaximum("py"));

  // TH2F* x_diff_vs_pz = new TH2F("x_diff_vs_pz", "x difference between default and shifted opics vs pz;x_diff;pz;nOfEvents",
  //   	100,(x_diff_binset->GetMean()-4*x_diff_binset->GetStdDev()),(x_diff_binset->GetMean()+4*x_diff_binset->GetStdDev()), 100, tree_optics1->GetMinimum("pz"), tree_optics1->GetMaximum("pz"));
  // TH2F* y_diff_vs_pz = new TH2F("y_diff_vs_pz", "y difference between default and shifted opics vs pz;y_diff;pz;nOfEvents",
  //  		100,(y_diff_binset->GetMean()-num_of_Stdev*y_diff_binset->GetStdDev()),(y_diff_binset->GetMean()+num_of_Stdev*y_diff_binset->GetStdDev()), 100, tree_optics1->GetMinimum("pz"), tree_optics1->GetMaximum("pz"));
  // TH2F* sx_diff_vs_pz = new TH2F("sx_diff_vs_pz", "sx difference between default and shifted opics vs pz;sx_diff;pz;nOfEvents",
  //  		100,(sx_diff_binset->GetMean()-num_of_Stdev*sx_diff_binset->GetStdDev()),(sx_diff_binset->GetMean()+num_of_Stdev*sx_diff_binset->GetStdDev()), 100, tree_optics1->GetMinimum("pz"), tree_optics1->GetMaximum("pz"));
  // TH2F* sy_diff_vs_pz = new TH2F("sy_diff_vs_pz", "sy difference between default and shifted opics vs pz;sy_diff;pz;nOfEvents",
  //  				100,(sy_diff_binset->GetMean()-num_of_Stdev*sy_diff_binset->GetStdDev()),(sy_diff_binset->GetMean()+num_of_Stdev*sy_diff_binset->GetStdDev()), 100, tree_optics1->GetMinimum("pz"), tree_optics1->GetMaximum("pz"));

   for (Int_t i = 0; i < nentries; i++) {
    tree_optics1->GetEntry(i);
    tree_optics2->GetEntry(i);

    if (is_lost_1||is_lost_2){
      continue;
    }
    else
    {
    x_diff_vs_y_diff->Fill(x1-x2, y1-y2);
    x_diff_vs_sx_diff->Fill(x1-x2, sx1-sx2);
    y_diff_vs_sy_diff->Fill(y1-y2, sy1-sy2);

    // y_diff_vs_px->Fill(y1-y2, px);
    // y_diff_vs_py->Fill(y1-y2, py);
    // y_diff_vs_pz->Fill(y1-y2, pz);

    // sx_diff_vs_px->Fill(sx1-sx2, px);
    // sx_diff_vs_py->Fill(sx1-sx2, py);
    // sx_diff_vs_pz->Fill(sx1-sx2, pz);

    // sy_diff_vs_px->Fill(sy1-sy2, px);
    // sy_diff_vs_py->Fill(sy1-sy2, py);
    // sy_diff_vs_pz->Fill(sy1-sy2, pz);
    }

  }
  delete tree_optics1, tree_optics2, file_optics1, file_optics2;


  TCanvas* canvas_diffs_2d = new TCanvas("canvas_diffs_2d", "canvas", 1280, 720);
  std::string filename_2d = "plots_PPSS_2020/test.pdf";

  canvas_diffs_2d->SaveAs((filename_2d + "[").c_str());

  x_diff_vs_y_diff->Draw("colz");
  canvas_diffs_2d->SaveAs(filename_2d.c_str());
  canvas_diffs_2d->Clear();
  cout<<x_diff_vs_y_diff->GetMean(2)<<'\n'; //y axkis
  cout<<x_diff_vs_y_diff->GetMean(1)<<'\n'; //x axkis

  x_diff_vs_sx_diff->Draw("colz");
  canvas_diffs_2d->SaveAs(filename_2d.c_str());
  canvas_diffs_2d->Clear();

  y_diff_vs_sy_diff->Draw("colz");
  canvas_diffs_2d->SaveAs(filename_2d.c_str());
  canvas_diffs_2d->Clear();

  x_diff->Draw();
  canvas_diffs_2d->SaveAs(filename_2d.c_str());
  canvas_diffs_2d->Clear();

  y_diff->Draw();
  canvas_diffs_2d->SaveAs(filename_2d.c_str());
  canvas_diffs_2d->Clear();

  sx_diff->Draw();
  canvas_diffs_2d->SaveAs(filename_2d.c_str());
  canvas_diffs_2d->Clear();

  sy_diff->Draw();
  canvas_diffs_2d->SaveAs(filename_2d.c_str());
  canvas_diffs_2d->Clear();

  //WIP
  // x_lost->Draw();
  // canvas_diffs_2d->SaveAs(filename_2d.c_str());
  // canvas_diffs_2d->Clear();

  // sx_diff_vs_pz->Draw("colz");
  // canvas_diffs_2d->SaveAs(filename_2d.c_str());
  // canvas_diffs_2d->Clear();

  // sy_diff_vs_px->Draw("colz");
  // canvas_diffs_2d->SaveAs(filename_2d.c_str());
  // canvas_diffs_2d->Clear();

  // sy_diff_vs_py->Draw("colz");
  // canvas_diffs_2d->SaveAs(filename_2d.c_str());
  // canvas_diffs_2d->Clear();

  // sy_diff_vs_pz->Draw("colz");
  // canvas_diffs_2d->SaveAs(filename_2d.c_str());
  // canvas_diffs_2d->Clear();

  canvas_diffs_2d->SaveAs((filename_2d + "]").c_str());

  delete canvas_diffs_2d, x_diff_vs_y_diff,x_diff_vs_sx_diff,y_diff_vs_sy_diff;// x_diff_vs_py, x_diff_vs_pz, 
         // y_diff_vs_px, y_diff_vs_py, y_diff_vs_pz, 
         // sx_diff_vs_px, sx_diff_vs_py, sx_diff_vs_pz, 
         // sy_diff_vs_px, sy_diff_vs_py, sy_diff_vs_pz;






  return 0;
}
