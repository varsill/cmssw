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

ReferenceAnalysisDQMHarvester::~ReferenceAnalysisDQMHarvester() {
  
}

void ReferenceAnalysisDQMHarvester::dqmEndJob(DQMStore::IBooker & ibooker, DQMStore::IGetter & igetter){

}

// define this as a plug-in
DEFINE_FWK_MODULE(ReferenceAnalysisDQMHarvester);