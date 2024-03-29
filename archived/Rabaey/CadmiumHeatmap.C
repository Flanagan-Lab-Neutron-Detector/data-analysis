void CadmiumHeatmap(){
  TFile *f = new TFile("TLoaderHeatmap.root","RECREATE");
  // TH1F *h1 = new TH1F("h1","x distribution",100,-4,4);
  TTree *T = new TTree("ntuple","data from ascii file");
  Long64_t nlines = T->ReadFile("data/Sector1highlight.csv","sector:pix_x:pix_y:Deltacount");
  // Long64_t nlines = T->ReadFile("Deltacount300_for_all.csv","sector:sec_x:sec_y:partno");

  printf(" found %lld points\n",nlines);

  // int n = 110;
  gStyle->SetOptStat(0);
  gStyle->SetOptTitle(0);
  gStyle->SetTitleSize(0.05,"XY");
  gStyle->SetTitleOffset(.8, "XY");
  gStyle->SetTitleX(0.5);
  gStyle->SetTitleAlign(23);   


  TCanvas * c1 = new TCanvas("c1", "c1", 1375,1645);
  c1->SetRightMargin(0.125);
  c1->SetLeftMargin(0.10);
  c1->SetBottomMargin(0.10);

  //pcenter->cd();
  TLegend* legend2 = new TLegend(0.16,0.91,0.54,0.97,"Sector 1 red","NDC"); // "Bits shifted at or beyond 300 mV"
  legend2->SetTextSize(.05);
  legend2->SetFillColor(0);
  legend2->SetBorderSize(0);


  TH2D * h2 = new TH2D("hhD", "Sector 1 red", 320, 0, 319, 218, 0, 217); // "Beam profile heatmap"
  ntuple->Draw("pix_y:pix_x>>hhD","Deltacount","colz");
 
  h2 ->GetXaxis()->SetTitle("x [cm]");
  h2->GetXaxis()->SetLimits(0.0,1.1);
  h2 ->GetYaxis()->SetTitle("y [cm]");
  h2->GetYaxis()->SetLimits(0.0,1.3);
  h2->GetXaxis()->CenterTitle();
  h2->GetYaxis()->CenterTitle();
 
  legend2->Draw("same");
  c1->SaveAs("./plots/Sector1Highlight.pdf");
}
