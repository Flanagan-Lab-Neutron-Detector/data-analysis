//#include "TSystem.h
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <utility> // std::pair
#include <numeric>
#include <map> 
#include "mpi.h" 

using namespace std;

// Print a list of integers
void print(std::vector <int> const & pn) {
  std::cout << "The vector elements are : ";

  for(int i=0; i < pn.size(); i++){
    std::cout << pn.at(i) << ' ';
  }
  std::cout << '\n';
}

// Make a CSV file with one or more columns of integer values
// Each column of data is represented by the pair <column name, column data>
//   as std::pair<std::string, std::vector<int>>
// The dataset is represented as a vector of these columns
// Note that all columns should be the same size
void write_csv(std::string filename, std::vector<std::pair<std::string, std::vector<int>>> dataset){
  // Create an output filestream object
  std::ofstream myFile(filename);
  cout << "Writing file: " << filename << "\n";

  // Send column names to the stream
  for(int j = 0; j < dataset.size(); j++)
    {
      myFile << dataset.at(j).first;
      if(j != dataset.size() - 1) myFile << ","; // No comma at end of line
    }
  myFile << "\n";

  cout << "wrote headers\n";

  // Send data to the stream
  for(int i = 0; i < dataset.at(0).second.size(); ++i)
    {
      for(int j = 0; j < dataset.size(); ++j)
        {
	  myFile << dataset.at(j).second.at(i);
	  if(j != dataset.size() - 1) myFile << ","; // No comma at end of line
        }
      myFile << "\n";
    }

  // Close the file
  myFile.close();
}


// Read a csv file into a map of strings to vectors
std::map<std::string, std::vector<int>> read_csv(std::string filename, bool skipHeaders, bool noisy){
  // Initialize dataset
  std::map<std::string, std::vector<int>> dataset;

  // Open file. The info is helpful for debugging
  ifstream csv(filename);
  if(noisy){
    if(csv.is_open()){
      cout<<"Opened file successfully:\n";
    }else{
      cout<<"File failed to open:\n";
    }
    cout<<filename<<"\n";
  }

  // Read header. Order of headers matters for now, but we want them in a map eventually... so memorize the header order.
  string line;
  getline(csv, line);
  stringstream ss(line);
  string head;
  vector<string> heads;
  while(getline(ss, head, ',')){
    dataset[head] = {};
    heads.push_back(head);
  }
  if(noisy) {
    std::cout << "read headers {";
    for(auto head : heads) {
      std::cout << head << ",";
    }
    std::cout << "}\n";
  }
  // Read line by line
  while(getline(csv, line)){
    stringstream ss(line);
    string snum;
    int num;
    int i=0;
    while(getline(ss, snum, ',')){
      std::istringstream(snum) >> num;
      if(skipHeaders){
	dataset[to_string(i)].push_back(num); // headers are dummies, just integer indices...
      } else {
	dataset[heads[i]].push_back(num); // headers are appropriate
      }
      i++;
    }
  }
  return dataset;
}

vector<int> countHotNeighborsForSector(string filename, int minVoltageDelta, bool noisy) {
  // Read data
  bool skipHeaders = true;
  std::map<std::string, std::vector<int>> data = read_csv(filename, skipHeaders, noisy);
  // Transform to graph for the sake of short lookup times.
  map<int,map<int,int>> deltas = {};
  for (int i=0; i < data["0"].size(); i++) {
    int x_phy = data["0"][i];
    int y_phy = data["1"][i];
    int V_init = data["2"][i];
    int V_final = data["3"][i];
    int delta = data["4"][i];
    // Only add deltas with a reasonable chance of being useful. We want to perform cuts on V_init + V_final here.
    if (delta > minVoltageDelta && 3500 < V_init && V_init < 6400 && 3500 < V_final && V_final < 6400) {
      if (deltas.count(x_phy)) { // contains x_phy already
	deltas[x_phy][y_phy] = delta;
      } else { // need to add entry for x_phy
	deltas[x_phy] = {{y_phy,delta}};
      }
    }
  }
  
  // Now count adjacent bit flips
  vector<int> adjacentCounts = {0,0,0,0,0,0,0,0};
  // Loop over x
  for (const pair<int,map<int,int>> &x_phy_pair : deltas) {
    int x_phy = x_phy_pair.first;
    //Loop over y
    for (const pair<int,int> &y_phy_pair : x_phy_pair.second) {
      int y_phy = y_phy_pair.first;
      int delta = y_phy_pair.second;
      int xRight = x_phy + 1;
      int yRight = y_phy;
      int deltaRight = deltas.count(xRight) && deltas[xRight].count(yRight) ? deltas[xRight][yRight] : 0;
      int xBelow = x_phy;
      int yBelow = y_phy + 1;
      int deltaBelow = deltas.count(xBelow) && deltas[xBelow].count(yBelow) ? deltas[xBelow][yBelow] : 0;
      int xLeft = x_phy - 1;
      int yLeft = y_phy;
      int deltaLeft = deltas.count(xLeft) && deltas[xLeft].count(yLeft) ? deltas[xLeft][yLeft] : 0;
      int xAbove = x_phy;
      int yAbove = y_phy - 1;
      int deltaAbove = deltas.count(xAbove) && deltas[xAbove].count(yAbove) ? deltas[xAbove][yAbove] : 0;

      
      int adjacentDeltaCount = 0;
      adjacentDeltaCount += deltaRight > 0 ? 1 : 0;
      adjacentDeltaCount += deltaBelow > 0 ? 1 : 0;
      adjacentDeltaCount += deltaLeft > 0 ? 1 : 0;
      adjacentDeltaCount += deltaAbove > 0 ? 1 : 0;
      
      adjacentCounts[adjacentDeltaCount] += 1;
    }
  }
  return adjacentCounts;
}

vector<int> countHotNeighborsForPart(int n, int minVoltageDelta, bool noisy){
  // Construct file name
  string dir = "/work/03069/whf346/lonestar/NISC_data/NETL_second_trip/";
  char cut[20];
  sprintf(cut,"Combined_%04d_",n);
  string base = cut;
  string ext = ".csv";

  vector<int> totalHotNeighborCounts = {0,0,0,0,0,0,0,0};
  // Loop over sectors
  for(int i = 0; i < 2048; i++){    
    // Add sector number to file name
    char sNum[10];
    sprintf(sNum,"%04d",i);
    string f = dir + base + sNum + ext;
    const char* filename = f.c_str();

    // Read from file
    int nSectors = 2048;
    vector<int> defaultCounts = {0,0,0,0,0,0,0,0};
    vector<int> hotNeighborCounts = i < nSectors ? countHotNeighborsForSector(filename, minVoltageDelta, noisy) : defaultCounts;
    for(int i = 0; i < totalHotNeighborCounts.size(); i++) {
      totalHotNeighborCounts[i] += hotNeighborCounts[i];
    }
  }
  if(noisy) {
    cout << "counted hot neighbors for part " << n << ".\n";
  }
  return totalHotNeighborCounts;
}

void writeHighNeighborCounts(vector<int> neighborsWithHighCounts, string outfile, int partno, bool noisy) {
  vector<int> highNeighbors = {};
  vector<int> counts = {};
  for(int i=0; i < neighborsWithHighCounts.size(); i++) {
    highNeighbors.push_back(i);
    counts.push_back(neighborsWithHighCounts[i]);
  }
  write_csv(outfile, {{"high_neighbors", highNeighbors}, {"count", counts}});
}

int main(){
  int root = 0;
  int partNumbers[] = { 37, 47, 57, 62, 73, 79, 82, 85, 91, 98, 100, 103, 110, 114, 115, 125};
  int minVoltageDelta = 250;
  bool noisy = true;

  // Run on different parts in parallel.
  int MPIprocessNumber = 0;
  MPI_Init(NULL, NULL);
  MPI_Comm_rank(MPI_COMM_WORLD, &MPIprocessNumber);

  // Run only on precisely the part corresponding to this MPI process.
  int partno = partNumbers[MPIprocessNumber];

  string outfile = "./data/neighbors_with_high_counts";
    
  cout<<"Part # "<<partno<<endl; 
    
  vector<int> hotNeighborsForPart = countHotNeighborsForPart(partno, minVoltageDelta, noisy);
  cout << "Hot neighbors for part " << partno << ": \n";
  for(const int &h : hotNeighborsForPart) {
    cout << h << " ";
  }
  cout << "\n";
  vector<int> allHotNeighbors = {0,0,0,0,0,0,0,0};
  MPI_Reduce(hotNeighborsForPart.data(), allHotNeighbors.data(), 8, MPI_INT, MPI_SUM, root, MPI_COMM_WORLD);
  if(MPIprocessNumber == root) {
    writeHighNeighborCounts(allHotNeighbors, outfile, partno, noisy);
  }
  
  // Kill MPI after file written
  MPI_Finalize();
}
