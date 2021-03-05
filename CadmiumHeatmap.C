void CadmiumHeatmap(){
  TFile *f = new TFile("TLoaderHeatmap.root","RECREATE");
  // TH1F *h1 = new TH1F("h1","x distribution",100,-4,4);
  TTree *T = new TTree("ntuple","data from ascii file");
  Long64_t nlines = T->ReadFile("PixelDeltacount300_for_109.csv","pix_x:pix_y:Deltacount");
  // Long64_t nlines = T->ReadFile("Deltacount300_for_all.csv","sector:sec_x:sec_y:partno");

  printf(" found %lld points\n",nlines);

  // int n = 110;
  gStyle->SetOptStat(0);
  gStyle->SetOptTitle(0);
  gStyle->SetTitleSize(0.05,"XY");
  gStyle->SetTitleOffset(.8, "XY");
  gStyle->SetTitleX(0.5);
  gStyle->SetTitleAlign(23);   


  TCanvas * c1 = new TCanvas("c1", "c1", 800,800);
  //c1->SetRightMargin(0.125);
  // c1->SetLeftMargin(0.10);

  //pcenter->cd();
  TLegend* legend2 = new TLegend(0.16,0.91,0.54,0.97,"Bits Shifted at or Beyond 300mV","NDC");
  legend2->SetTextSize(.05);
  legend2->SetFillColor(0);
  legend2->SetBorderSize(0);


  TH2D * h2 = new TH2D("hhD", "Heatmap of BeamProfile", 218, 0, 217, 320, 0, 319); 
  ntuple->Draw("pix_x:218-pix_y>>hhD","Deltacount","colz");
 
  //  h2 ->GetXaxis()->SetTitle("x [cm]");
  //  h2->GetXaxis()->SetLimits(0.0,1.1);
  //  h2 ->GetYaxis()->SetTitle("y [cm]");
  //  h2->GetYaxis()->SetLimits(0.0,1.3);
  //  h2->GetXaxis()->CenterTitle();
  //  h2->GetYaxis()->CenterTitle();
 
  legend2->Draw("same");

}
