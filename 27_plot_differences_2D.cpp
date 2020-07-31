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

	//gROOT->ProcessLine( "gErrorIgnoreLevel = 1001;");		// This line prevents text output each time canvas is saved



  TFile* file_optics1 = new TFile(			
    "root_PPSS_2020/1pythia8_13TeV_protons_100k_transported_205m_beta40cm_6500GeV_y-185murad.root");		//This file contains results of transport code without any shifts


  TFile* file_optics2 = new TFile(
    "root_PPSS_2020/1_shifted__changed_strength_pythia8_13TeV_protons_100k_transported_205m_beta40cm_6500GeV_y-185murad.root");

  TTree* tree_optics1 = (TTree*)file_optics1->Get("ntuple");
  TTree* tree_optics2 = (TTree*)file_optics2->Get("ntuple");

  tree_optics1->SetMakeClass(1);
  tree_optics2->SetMakeClass(1);

  Float_t x1, x2, y1, y2, sx1, sx2, sy1, sy2, px, py, pz;

  tree_optics1->SetBranchAddress("x", &x1);
  tree_optics2->SetBranchAddress("x", &x2);

  tree_optics1->SetBranchAddress("y", &y1);
  tree_optics2->SetBranchAddress("y", &y2);

  tree_optics1->SetBranchAddress("sx", &sx1);
  tree_optics2->SetBranchAddress("sx", &sx2);

  tree_optics1->SetBranchAddress("sy", &sy1);
  tree_optics2->SetBranchAddress("sy", &sy2);

  tree_optics1->SetBranchAddress("px", &px);
  tree_optics1->SetBranchAddress("py", &py);
  tree_optics1->SetBranchAddress("pz", &pz);

  Long64_t nentries = tree_optics1->GetEntriesFast();
  Long64_t nentries2 = tree_optics2->GetEntriesFast();
  cout<<"Difference in protons: "<<nentries- nentries2<<'\n';
  //std::cout<< std::endl << "Number of entries: " << nentries << std::endl<< std::endl;


  // Here I calcluate means and StdDev for differences, which are later used to set axis limits for 2D histos, those 4 histograms below are not plotted


  double num_of_Stdev=3.5;


  // 2D histos for the x, y, sx and sy vs px/py/pz difference between optics1_default(-185murad) and optics2_shifted(-185murad)
  // TH2F* x_vs_y_def = new TH2F("x_vs_y_def", "Default;x;y;nOfEvents",
  //  		100,tree_optics1->GetMinimum("x"),tree_optics1->GetMaximum("x") , 100, tree_optics1->GetMinimum("y"), tree_optics1->GetMaximum("y"));

  // TH2F* x_vs_y_chang = new TH2F("x_vs_y_chang", "Reduced;x;y;nOfEvents",
  //     100,tree_optics2->GetMinimum("x"),tree_optics2->GetMaximum("x") , 100, tree_optics2->GetMinimum("y"), tree_optics2->GetMaximum("y"));

  double x_min,x_max,y_min,y_max;
  x_min=min(tree_optics1->GetMinimum("x"),tree_optics2->GetMinimum("x"));
  x_max=max(tree_optics1->GetMaximum("x"),tree_optics2->GetMaximum("x"));
  y_min=min(tree_optics1->GetMinimum("y"),tree_optics2->GetMinimum("y"));
  y_max=max(tree_optics1->GetMaximum("y"),tree_optics2->GetMaximum("y"));


  TH2F* x_vs_y_def = new TH2F("x_vs_y_def", "Default;x [m];y [m];nOfEvents",
       200,x_min,x_max , 200,y_min, y_max);

//   TH2F* x_vs_y_chang = new TH2F("x_vs_y_chang", "Reduced;x [m];y [m];nOfEvents",
//       200,tree_optics1->GetMinimum("x"),tree_optics1->GetMaximum("x") , 200, tree_optics1->GetMinimum("y"), tree_optics1->GetMaximum("y"));
 TH2F* x_vs_y_chang = new TH2F("x_vs_y_chang", "Reduced;x [m];y [m];nOfEvents",
       200,x_min,x_max , 200,y_min, y_max);



  for (Int_t i = 0; i < nentries; i++) {
    tree_optics1->GetEntry(i);


    x_vs_y_def->Fill(x1, y1);


  }
  for (Int_t i = 0; i < nentries2; i++) {

    tree_optics2->GetEntry(i);


    x_vs_y_chang->Fill(x2, y2);

  }
  delete tree_optics1, tree_optics2, file_optics1, file_optics2;


  TCanvas* canvas_diffs_2d = new TCanvas("canvas_diffs_2d", "canvas", 1280, 720);
  std::string filename_2d = "plots_PPSS_2020/x_vs_y.pdf";

  canvas_diffs_2d->SaveAs((filename_2d + "[").c_str());


  gStyle->SetOptStat(0);
  x_vs_y_def->Draw("colz");
  canvas_diffs_2d->SaveAs(filename_2d.c_str());
  canvas_diffs_2d->Clear();

  x_vs_y_chang->Draw("colz");
  canvas_diffs_2d->SaveAs(filename_2d.c_str());
  canvas_diffs_2d->Clear();



  canvas_diffs_2d->SaveAs((filename_2d + "]").c_str());
  delete canvas_diffs_2d, x_vs_y_def,x_vs_y_chang;


	
	




  return 0;
}
