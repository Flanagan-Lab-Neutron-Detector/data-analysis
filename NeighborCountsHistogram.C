void NeighborCountsHistogram(){
  TFile *f = new TFile("TLoaderHistogram.root", "RECREATE");
  TTree *T = new TTree("ntuple", "data from ascii file");
  Long64_t nlines = T->ReadFile("./data/neighbors_with_high_counts.csv","high_neighbors:count");

  printf(" found %4d lines of data\n", nlines);

  TCanvas * c1 = new TCanvas("c1", "c1", 500, 500);
  c1->SetLogy();
  TH1D * h = new TH1D("h", "Number of adjacent bit flips per bit flip", 8, 0, 7);
  h->SetFillColor(kBlue);
  ntuple->Draw("high_neighbors>>h", "count", "HIST");
}
