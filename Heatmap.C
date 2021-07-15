

  void Heatmap(){



  TFile *f = new TFile("TLoaderHeatmap.root","RECREATE");
  // TH1F *h1 = new TH1F("h1","x distribution",100,-4,4);
  TTree *T = new TTree("ntuple","data from ascii file");
  Long64_t nlines = T->ReadFile("Deltacount300_for_all.csv","sector:sec_x:sec_y:DeltaCount:partno");
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
  //c1->SetRightMargin(0.125);
  // c1->SetLeftMargin(0.10);

  //pcenter->cd();
  TLegend* legend2 = new TLegend(0.16,0.91,0.54,0.97,"Bits Shifted at or Beyond 300mV","NDC");
  legend2->SetTextSize(.05);
  legend2->SetFillColor(0);
  legend2->SetBorderSize(0);


  TH2D * h2 = new TH2D("hhD", "Heatmap of BeamProfile", 80,0,79, 872, 0, 871);
  ntuple->Draw("sec_y:sec_x>>hhD","DeltaCount","colz");

  h2 ->GetXaxis()->SetTitle("x [cm]");
  h2->GetXaxis()->SetLimits(0.0,4.4);
  h2 ->GetYaxis()->SetTitle("y [cm]");
  h2->GetYaxis()->SetLimits(0.0,5.2);
  h2->GetXaxis()->CenterTitle();
  h2->GetYaxis()->CenterTitle();

  // h2->GetZaxis()->SetTitle("Count of Delta >300mv");
  // h2->GetZaxis()->SetTitleOffset(1.8);

  legend2->Draw("same");
  // h2->GetXaxis()->SetTickLength(1);

  //c1->SaveAs("Heatmapall_forrealpositions.pdf");

  //############################## Create X and Y projections.###########
  TCanvas *xProjectionCanvas = new TCanvas("xProjectionCanvas", "xProjectionCanvas", 600, 600);
  TCanvas *yProjectionCanvas = new TCanvas("yProjectionCanvas", "yProjectionCanvas", 600, 600);
  gStyle->SetOptStat(0);

  TH1D *projh2x = h2->ProjectionX();

  TH1D *projh2y = h2->ProjectionY();

  //need named functions to retrieve new ones from histo.
  TF1 * quad = new TF1("quad", "pol2", 0, 5);
  TF1 * lin1 = new TF1("lin1", "pol1", 0, 5);
  TF1 * lin2 = new TF1("lin2", "pol1", 0, 5);

  xProjectionCanvas->cd();
  projh2x->SetFillColor(kBlue+1);
  projh2x->Draw("bar");
  //fit the peak
  projh2x->Fit("quad", "W", "", 1.9, 3.7);
  TF1 * xPeakFit = projh2x->GetFunction("quad");
  Double_t xmax = xPeakFit->GetMaximum();
  //fit left side
  projh2x->Fit("lin1", "W+", "", 1.5, 2);
  TF1 * xLeftFit = projh2x->GetFunction("lin1");
  Double_t xHalfMaxLeft = xLeftFit->GetX(xmax/2, 1.5, 2);
  //fit right side
  projh2x->Fit("lin2", "W+", "", 3.5, 4);
  TF1 * xRightFit = projh2x->GetFunction("lin2");
  Double_t xHalfMaxRight = xRightFit->GetX(xmax/2, 3.5, 4);
  Double_t xFWHM = xHalfMaxRight - xHalfMaxLeft;
  //Draw fit results...
  TLine * xhalf = new TLine(0, xmax/2, 4.3, xmax/2);
  xhalf->SetLineWidth(2);
  xhalf->SetLineColor(kRed);
  xhalf->Draw();
  TLine * xright = new TLine(xHalfMaxRight, 0, xHalfMaxRight, xmax);
  xright->SetLineWidth(2);
  xright->SetLineColor(kRed);
  xright->Draw();
  TLine * xleft = new TLine(xHalfMaxLeft, 0, xHalfMaxLeft, xmax);
  xleft->SetLineWidth(2);
  xleft->SetLineColor(kRed);
  xleft->Draw();

  printf("xFWHM: %e\n", xFWHM);

  yProjectionCanvas->cd();
  projh2y->SetFillColor(kBlue-2);
  projh2y->Draw("bar");
  //fit the peak
  projh2y->Fit("quad", "W", "", 2, 3.4);
  TF1 * yPeakFit = projh2y->GetFunction("quad");
  Double_t ymax = yPeakFit->GetMaximum();
  //fit left side
  projh2y->Fit("lin1", "W+", "", 1.5, 2);
  TF1 * yLeftFit = projh2y->GetFunction("lin1");
  Double_t yHalfMaxLeft = yLeftFit->GetX(ymax/2, 1.5, 2);
  //fit right side
  projh2y->Fit("lin2", "W+", "", 3.5, 3.8);
  TF1 * yRightFit = projh2y->GetFunction("lin2");
  Double_t yHalfMaxRight = yRightFit->GetX(ymax/2, 3.5, 3.8);
  Double_t yFWHM = yHalfMaxRight - yHalfMaxLeft;
  //Draw fit results...
  TLine * yhalf = new TLine(0, ymax/2, 4.3, ymax/2);
  yhalf->SetLineWidth(2);
  yhalf->SetLineColor(kRed);
  yhalf->Draw();
  TLine * yright = new TLine(yHalfMaxRight, 0, yHalfMaxRight, ymax);
  yright->SetLineWidth(2);
  yright->SetLineColor(kRed);
  yright->Draw();
  TLine * yleft = new TLine(yHalfMaxLeft, 0, yHalfMaxLeft, ymax);
  yleft->SetLineWidth(2);
  yleft->SetLineColor(kRed);
  yleft->Draw();


  printf("yFWHM: %e\n", yFWHM);

  printf("\ndistribution maxima: %e, %e\n", xmax, ymax);

  /*
    TCanvas *c2 = new TCanvas("c2","c2",800,800);

    c2->SetLeftMargin(.15);


    TLegend* legend3 = new TLegend(0.1,0.91,0.24,0.97,"Lego plot of \n DeltaCount vs Sec_X:Sec_Y","NDC");
    legend3->SetTextSize(.05);
    legend3->SetFillColor(0);
    legend3->SetBorderSize(0);
    h2 ->GetXaxis()->SetTitleOffset(1.4);
    h2 ->GetYaxis()->SetTitleOffset(1.4);

    ntuple->Draw("sec_y:sec_x>>hhD","DeltaCount","lego1");

    legend3->Draw("same");

    gPad->SetTheta(30);
    gPad->SetPhi(30);
    gPad->Update();

    c2 ->SaveAs("Lego3plot_forrealpositions.pdf");
  */

}
