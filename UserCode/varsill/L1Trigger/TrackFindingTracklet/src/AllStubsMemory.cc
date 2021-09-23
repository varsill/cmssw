#include "L1Trigger/TrackFindingTracklet/interface/AllStubsMemory.h"
#include "L1Trigger/TrackFindingTracklet/interface/Stub.h"
#include "L1Trigger/TrackFindingTracklet/interface/Settings.h"
#include <iomanip>

using namespace std;
using namespace trklet;

AllStubsMemory::AllStubsMemory(string name, Settings const& settings, unsigned int iSector)
    : MemoryBase(name, settings, iSector) {}

void AllStubsMemory::writeStubs(bool first) {
  const string dirS = settings_.memPath() + "Stubs/";
  openFile(first, dirS, "AllStubs_");

  for (unsigned int j = 0; j < stubs_.size(); j++) {
    string stub = stubs_[j]->str();
    out_ << "0x";
    out_ << std::setfill('0') << std::setw(2);
    out_ << hex << j << dec;
    out_ << " " << stub << " " << hexFormat(stub) << endl;
  }
  out_.close();
}
