void HighResolutionGoldFoilHeatmap(){
  TFile *f = new TFile("TLoaderHeatmap.root","RECREATE");
  // TH1F *h1 = new TH1F("h1","x distribution",100,-4,4);
  TTree *T = new TTree("ntuple","data from ascii file");
  Long64_t nlines = T->ReadFile("./data/PixelDeltacount300_for_all.csv","pix_x:pix_y:Deltacount");
  // Long64_t nlines = T->ReadFile("Deltacount300_for_all.csv","sector:sec_x:sec_y:partno");

  printf(" found %lld points\n",nlines);

  gStyle->SetOptStat(0);
  gStyle->SetOptTitle(0);
  gStyle->SetTitleSize(0.05,"XY");
  gStyle->SetTitleOffset(.8, "XY");
  gStyle->SetTitleX(0.5);
  gStyle->SetTitleAlign(23);

  //int xProjectionHeight = 500;
  //int yProjectionWidth = 500;
  int profileWidth = 1375;
  int profileHeight = 1645;
  //int canvasWidth = profileWidth + yProjectionWidth;
  //int canvasHeight = profileHeight + xProjectionHeight;
  //double profileWidthFrac = (double) profileWidth / (double) canvasWidth;
  //double profileHeightFrac = (double) profileHeight / (double) canvasHeight;
  

  TCanvas * c1 = new TCanvas("c1", "c1", profileWidth,profileHeight);
  c1->SetRightMargin(0.125);
  c1->SetLeftMargin(0.10);
  c1->SetBottomMargin(0.10);
  //  c1->SetBottomMargin(0.10);
  //TPad *pcenter = new TPad("pcenter", "pcenter", 0.0, 0.0, profileWidthFrac, profileHeightFrac);
  //pcenter->Draw();
  //TPad *pright = new TPad("pright", "pright", profileWidthFrac, 0.0, 1.0, profileHeightFrac);
  //pright->Draw();
  //TPad *ptop = new TPad("ptop", "ptop", 0.0, profileHeightFrac, profileWidthFrac, 1.0);
  //ptop->Draw();

  TLegend* legend2 = new TLegend(0.16,0.91,0.54,0.97,"Bits Shifted at or Beyond 300mV","NDC");
  legend2->SetTextSize(.05);
  legend2->SetFillColor(0);
  legend2->SetBorderSize(0);

  
  //pcenter->cd();
  TH2D * hhD = new TH2D("hhD", "Heatmap of BeamProfile", 1280, 0, 1279, 872, 0, 871); 

  ntuple->Draw("pix_y:pix_x>>hhD","Deltacount","colz");

  int maxCount = 150;
  hhD->SetMaximum(maxCount);
  hhD->GetXaxis()->SetTitle("x [cm]");
  hhD->GetXaxis()->SetLimits(0.0,4.4);
  hhD->GetYaxis()->SetTitle("y [cm]");
  hhD->GetYaxis()->SetLimits(0.0,5.2);
  hhD->GetXaxis()->CenterTitle();
  hhD->GetYaxis()->CenterTitle();
  legend2->Draw("same");
  /*
  pright->cd();
  TH1D * projh2Y = hhD->ProjectionY();
  projh2Y->Draw("hbar");
  projh2Y->GetXaxis()->SetTitle();

  ptop->cd();
  TH1D * projh2X = hhD->ProjectionX();
  projh2X->Draw("bar");
  projh2X->GetXaxis()->SetTitle();
  */
  //TLatex *t = new TLatex();
  //t->SetTextFont(42);
  //t->SetTextSize(0.05);
  //t->DrawLatex(profileWidthFrac,profileHeightFrac,"Bits Shifted at or Beyond 300 mV");

  c1->SaveAs("./plots/BeamHeatmap.pdf");
  
}
