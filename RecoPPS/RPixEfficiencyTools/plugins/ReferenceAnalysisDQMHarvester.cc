// -*- C++ -*-
//
// Package:    RecoPPS/RPixEfficiencyTools
// Class:      ReferenceAnalysisDQMHarvester
//
/**\class ReferenceAnalysisDQMHarvester ReferenceAnalysisDQMHarvester.cc
 RecoPPS/RPixEfficiencyTools/plugins/ReferenceAnalysisDQMHarvester.cc

 Description: [one line class summary]

 Implementation:
                 [Notes on implementation]
*/
//
// Original Author:  Andrea Bellora
//         Created:  Wed, 22 Aug 2017 09:55:05 GMT
//
//

#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "DQMServices/Core/interface/DQMEDHarvester.h"
#include "DQMServices/Core/interface/DQMStore.h"

#include "CondFormats/RunInfo/interface/LHCInfo.h"
#include "CondFormats/DataRecord/interface/LHCInfoRcd.h"

#include "DataFormats/Common/interface/DetSetVector.h"

#include "DataFormats/CTPPSDetId/interface/CTPPSDetId.h"
#include "DataFormats/CTPPSReco/interface/CTPPSPixelLocalTrack.h"

#include "Geometry/VeryForwardGeometryBuilder/interface/CTPPSGeometry.h"
#include "Geometry/Records/interface/VeryForwardRealGeometryRecord.h"

#include <string>
#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <exception>
#include <fstream>
#include <memory>
#include <set>


class ReferenceAnalysisDQMHarvester: public DQMEDHarvester{
public:
  explicit ReferenceAnalysisDQMHarvester(const edm::ParameterSet &);
  ~ReferenceAnalysisDQMHarvester();
  void dqmEndJob(DQMStore::IBooker &, DQMStore::IGetter &) override;
  void beginRun(edm::Run const &run, edm::EventSetup const & eventSetup) override {
  };

private:
  edm::ESGetToken<CTPPSGeometry, VeryForwardRealGeometryRecord> geomEsToken_;
  std::vector<CTPPSPixelDetId> detids_;

};

ReferenceAnalysisDQMHarvester::ReferenceAnalysisDQMHarvester(const edm::ParameterSet &iConfig): 
geomEsToken_(esConsumes<edm::Transition::BeginRun>()) {
  

}


void ReferenceAnalysisDQMHarvester::beginRun(edm::Run const& run, edm::EventSetup const& eventSetup)
{
  if(detids_.size()==0){ //first run
      const auto& geom = eventSetup.getData(geomEsToken_);
      for (auto it = geom.beginSensor(); it != geom.endSensor(); ++it) {
        if (!CTPPSPixelDetId::check(it->first))
          continue;
        const CTPPSPixelDetId detid(it->first);
        
        detids_.emplace_back(detid);
    }
  }
}
ReferenceAnalysisDQMHarvester::~ReferenceAnalysisDQMHarvester() {
  
}

void ReferenceAnalysisDQMHarvester::dqmEndJob(DQMStore::IBooker & ibooker, DQMStore::IGetter & igetter){
  
  for (auto &rpId : detids_) {
    uint32_t arm = rpId.arm();
    uint32_t station = rpId.station();
    uint32_t rp = rpId.rp();
    uint32_t plane 
    std::string rpDirName = Form("Arm%i_st%i_rp3", arm, station);
    ibooker->cd(rpDirName.data());
    MonitorElement* h2RefinedTrackEfficiency = igettet.get("h2RefinedTrackEfficiency_arm%i_st%i_rp%i");
    h2RefinedTrackEfficiency_->Divide(h2RefinedTrackEfficiency_[rpId],
                                            h2TrackHitDistribution_[rpId]);
    h2RefinedTrackEfficiency_->SetMaximum(1.);
    if (station == 0) {
      h2RefinedTrackEfficiency_rotated->Divide(
          h2RefinedTrackEfficiency_rotated,
          h2TrackHitDistribution_rotated);
      h2RefinedTrackEfficiency_rotated->SetMaximum(1.);
    }
  }
  outputFile_->Close();
  delete outputFile_;


}

// define this as a plug-in
DEFINE_FWK_MODULE(ReferenceAnalysisDQMHarvester);