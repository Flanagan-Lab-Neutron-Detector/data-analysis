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
//#include "mpi.h" 

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

vector<vector<int>> countVoltageDeltasByPixel(const char* file, int minVoltage, int pix_per_sec_x, int pix_per_sec_y, bool noisy) {
  bool skipHeaders = true;
  std::map<string, std::vector<int>> data = read_csv(file, skipHeaders, noisy);
  
  int sector_width  = 256*16;
  int sector_height = 256;
  
  // Initialize a 2-D array of pixels with all zeroes
  vector<vector<int>> deltaCounts(pix_per_sec_x, vector<int>(pix_per_sec_y, 0));
  for(int i=0; i < data["0"].size(); i++) {
    // Bin pixels in right bins
    int x_phy = (data["1"][i] % sector_width); // x_phy and y_phy in the file come from a rotated system. Rotate back.
    int y_phy = data["0"][i] % sector_height;
    int V_initial = data["2"][i];
    int V_final = data["3"][i];
    int delta = data["4"][i];
    if(delta > minVoltage && 3500 < V_initial && V_initial < 6400 && 3500 < V_final && V_final < 6400) {
      deltaCounts[x_phy / (sector_width / pix_per_sec_x)][y_phy / (sector_height / pix_per_sec_y)]++;
    }
  }

  // Print all counts
  if(noisy) {
    for(int y=0; y < deltaCounts[0].size(); y++) {
      for(int x=0; x < deltaCounts.size(); x++) {
	cout<<deltaCounts[x][y]<<"|";
      }
      cout<<"\n------------------------------------------\n";
    }
  }

  return deltaCounts;
}

// Compute offsets based on part number for heatmap drawing.
std::pair<int,int> offsetsForPart(int n) {
  int xadd, yadd;
  if ((n == 91)||(n == 98)||(n == 100)||(n == 103 )){
    yadd = 128 + 120;
  }
  else if ((n == 73)||(n == 79)||(n == 82)||(n == 85)){
    yadd = 256 + 210;
  }
  else if (( n == 37)||(n == 47)||(n == 57)||(n == 62)){
    yadd = 384 +300;
  } 
  else{
    yadd = 30;
  }
  if (( n == 114)||(n == 98)||(n == 79)||(n == 47)){
    xadd = 16 + 6;
  }
  else if ((n == 115)||(n == 100)||(n == 82)||(n == 57)){
    xadd = 32 + 10;
  }
  else if ((n == 125)||(n == 103)||(n == 85)||(n == 62)){
    xadd = 48 + 14;
  }
  else{
    xadd = 2;
  }
  return { xadd, yadd };
}

void writePixelDeltasAndPositions(const char* sectorPositionMapping, std::string outfile, map<string,vector<int>> pixelDeltaCounts, int partno, int pix_per_sec_x, int pix_per_sec_y, bool noisy){
  // Read sector positions from file
  bool skipHeaders = false;
  std::map<string, std::vector<int>> sectorMap = read_csv(sectorPositionMapping, skipHeaders, noisy);
  for(int i=0; i < sectorMap["sector"].size(); i++) {

    // Offset sec_x and sec_y depending on part number
    std::pair<int,int> offsets = offsetsForPart(partno);
    int xPartOffset = offsets.first;
    int yPartOffset = offsets.second;

    // Gap for the memory bank
    int yGapOffset = sectorMap["sec_y"][i] >= 64 ? 30 : 0;

    // Apply offsets
    sectorMap["sec_x"][i] += xPartOffset;
    sectorMap["sec_y"][i] += yPartOffset + yGapOffset;
  }

  std::vector<int> sectorList = {}; // Want to print sectors, too...
  for(int i=0; i<pixelDeltaCounts["pix_x"].size(); i++) {
    // Now position pixels according to their sector
    int sector = pixelDeltaCounts["sector"][i];
    pixelDeltaCounts["pix_x"][i] += pix_per_sec_x * sectorMap["sec_x"][sector];
    pixelDeltaCounts["pix_y"][i] += pix_per_sec_y * sectorMap["sec_y"][sector];
    sectorList.push_back(sector);
  }
  
  // Write sector info and delta counts to file
  std::vector<std::pair<std::string, std::vector<int>>> vals = {
    {"sector",sectorList},
    {"pix_x",pixelDeltaCounts["pix_x"]},
    {"pix_y",pixelDeltaCounts["pix_y"]},
    {"Deltacount",pixelDeltaCounts["delta_count"]}
  };
  write_csv(outfile, vals);
}


map<string,vector<int>> pixelDeltasForPart(int n, int minVoltageDelta, int pix_per_sec_x, int pix_per_sec_y, bool noisy){
  // Construct file name
  string dir = "/work/03069/whf346/lonestar/NISC_data/NETL_second_trip/";
  char cut[20];
  sprintf(cut,"Combined_%04d_",n);
  string base = cut;
  string ext = ".csv";

  // Set up output map
  map<string,vector<int>> pixelDeltas = {{"sector", {}}, {"pix_x", {}}, {"pix_y", {}}, {"delta_count", {}}};     
  // Loop over sectors
  for(int i = 0; i < 2048; i++){    
    // Add sector number to file name
    char sNum[10];
    sprintf(sNum,"%04d",i);
    string f = dir + base + sNum + ext;
    const char* filename = f.c_str();

    // Read from file
    int nSectors = 2048;
    vector<vector<int>> defaultCounts(pix_per_sec_x, vector<int>(pix_per_sec_y, 10));
    vector<vector<int>> highCounts(pix_per_sec_x, vector<int>(pix_per_sec_y, 20));
    vector<vector<int>> dcounts = i == 1 ? highCounts : defaultCounts; //< nSectors ? countVoltageDeltasByPixel(filename, minVoltageDelta, pix_per_sec_x, pix_per_sec_y, noisy) : defaultCounts;
    
    // Some data is upside down. Explanation pending, but the condition should work and is documented in JR's slides
    if (((i / 128) % 2 == 0) != (i % 2 == 0)) {
      reverse(dcounts.begin(),dcounts.end());
    }
    // Write to map
    for(int x=0; x < dcounts.size(); x++) {
      for(int y=0; y < dcounts[x].size(); y++) {
	pixelDeltas["sector"].push_back(i);
	pixelDeltas["pix_x"].push_back(x);
	pixelDeltas["pix_y"].push_back(y);
	pixelDeltas["delta_count"].push_back(dcounts[x][y]);
      }
    }
  }

  return pixelDeltas;
}

int main(){
  //int partNumbers[] = { 37, 47, 57, 62, 73, 79, 82, 85, 91, 98, 100, 103, 110, 114, 115, 125, 109 };
  int minVoltageDelta = 250;
  bool noisy = true;
  int pix_per_sec_x = 16;
  int pix_per_sec_y = 1;
  const char* sectorPositionMapping = "sectorPositionMappingPart718.csv";

  // Run on different parts in parallel.
  //int MPIprocessNumber;
  //MPI_Init(NULL, NULL);
  //MPI_Comm_rank(MPI_COMM_WORLD, &MPIprocessNumber);

  // Run only on precisely the part corresponding to this MPI process.
  int partno = 0; // partNumbers[MPIprocessNumber];

  char outfilename[40];
  sprintf(outfilename,"./data/Sector1highlight.csv", partno);
  string outfile = outfilename;
    
  cout<<"Part # "<<partno<<endl; 
    
  map<string,vector<int>> dcountmap = pixelDeltasForPart(partno, minVoltageDelta, pix_per_sec_x, pix_per_sec_y, noisy);
    
  writePixelDeltasAndPositions(sectorPositionMapping, outfile, dcountmap, partno, pix_per_sec_x, pix_per_sec_y, noisy);
  
  // Kill MPI after all files have been written out.
  //MPI_Finalize();
}
