//#include "TSystem.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <utility> // std::pair
#include <numeric>
#include <map> 

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

  // Send column names to the stream
  for(int j = 0; j < dataset.size(); j++)
    {
      myFile << dataset.at(j).first;
      if(j != dataset.size() - 1) myFile << ","; // No comma at end of line
    }
  myFile << "\n";

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
  while(getline(ss,head)){
    dataset[head] = {};
    heads.push_back(head);
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

// Read the voltage deltas from a file. Return a vector of voltage deltas.
vector<int> readVoltageDeltas(const char* file, bool noisy){
  bool skipHeaders = true;
  std::map<string, std::vector<int>> data = read_csv(file, skipHeaders, noisy);
  
  // Return voltage deltas.
  // NOTE: To read from a raw data file in NETL_second_trip, use
  // "0": x_phys | "1": y_phys | "2": v_init | "3": v_final | "4": delta
  return data["4"];
}

// Count the voltage deltas greater than minVoltage
//  ... in the file "file"
int countVoltageDeltas(const char* file, int minVoltage, bool noisy){
  int count = 0;
  vector<int> deltaCounts = readVoltageDeltas(file, noisy);
  for (const int& delta : deltaCounts) {
    count += delta > minVoltage ? 1 : 0;
  }
  return count;
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

void writeDeltasAndPositions(const char* sectorPositionMapping, std::string outfile, vector<int> DeltaCount, int partno, bool noisy){
  // Read sector positions from file
  bool skipHeaders = true;
  std::map<string, std::vector<int>> sectorMap = read_csv(sectorPositionMapping, skipHeaders, noisy);
  vector<int> pn;
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
    pn.push_back(partno);
  }
  // Write sector info and delta counts to file
  std::vector<std::pair<std::string, std::vector<int>>> vals = {
    {"sector",sectorMap["sector"]},
    {"sec_x",sectorMap["sec_x"]},
    {"sec_y",sectorMap["sec_y"]},
    {"Deltacount",DeltaCount},
    {"partno",pn}
  };
  write_csv(outfile, vals);
}

vector<int> countDeltasForPart(int n, int minVoltageDelta, bool noisy){
  // Read from data directory
  string dir = "/work/03069/whf346/lonestar/NISC_data/NETL_second_trip/";
  char cut[20];
  sprintf(cut,"Combined_%04d_",n);
  string base = cut;
  string ext = ".csv";
  vector<int> deltaCountPerSector; 
     
  for(int i = 0; i < 2048; i++){    
    // Generate correct filename
    char sNum[10];
    sprintf(sNum,"%04d",i);
    string f = dir + base + sNum + ext;
    const char* filename = f.c_str();

    int dcount;
    // Only read first n sectors
    int nSectors = 2;
    dcount = i < nSectors ? countVoltageDeltas(filename, minVoltageDelta, noisy) : 0;
    
    // Comforting output. This process can take a while.
    if (noisy){
      cout<<"Deltacount is " << dcount << "\n";
    }
    deltaCountPerSector.push_back(dcount);
  }

  return deltaCountPerSector;
}

int main(){
  int partNumbers[] = { 109 };
  int minVoltageDelta = 250;
  bool noisy = true;
  const char* sectorPositionMapping = "sectorPositionMappingPart718.csv";

  // For each part
  for(const int &partno : partNumbers){

    char outfilename[20];
    sprintf(outfilename,"Deltacount300_for_%d.csv", partno);
    string outfile = outfilename;

    cout<<"Part # "<<partno<<endl; 
    vector<int> DeltaCount = countDeltasForPart(partno, minVoltageDelta, noisy);

    print(DeltaCount);

    writeDeltasAndPositions(sectorPositionMapping, outfile, DeltaCount, partno, noisy);
  }
}
