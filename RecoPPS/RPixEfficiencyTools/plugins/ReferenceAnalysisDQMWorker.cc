// -*- C++ -*-
//
// Package:    RecoPPS/RPixEfficiencyTools
// Class:      ReferenceAnalysisDQMWorker
//
/**\class ReferenceAnalysisDQMWorker ReferenceAnalysisDQMWorker.cc
 RecoPPS/RPixEfficiencyTools/plugins/ReferenceAnalysisDQMWorker.cc

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

#include "DQMServices/Core/interface/DQMEDAnalyzer.h"
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
#include<set>


class ReferenceAnalysisDQMWorker: public DQMEDAnalyzer {

public:
  explicit ReferenceAnalysisDQMWorker(const edm::ParameterSet &);
  ~ReferenceAnalysisDQMWorker();
  static void fillDescriptions(edm::ConfigurationDescriptions &descriptions);

protected:
  virtual void bookHistograms(DQMStore::IBooker &, edm::Run const &, edm::EventSetup const &) override;
  virtual void dqmBeginRun(edm::Run const &, edm::EventSetup const &) override;

private:
  virtual void analyze(const edm::Event &, const edm::EventSetup &) override;
};

ReferenceAnalysisDQMWorker::ReferenceAnalysisDQMWorker(const edm::ParameterSet &iConfig) {
  
}

ReferenceAnalysisDQMWorker::~ReferenceAnalysisDQMWorker() {


}


void ReferenceAnalysisDQMWorker::bookHistograms(DQMStore::IBooker& ibooker, edm::Run const & run, edm::EventSetup const & eventSetup)
{
}


void ReferenceAnalysisDQMWorker::dqmBeginRun(edm::Run const &, edm::EventSetup const &)
{

}
void ReferenceAnalysisDQMWorker::analyze(const edm::Event &iEvent,
                                  const edm::EventSetup &iSetup) {
}


void ReferenceAnalysisDQMWorker::fillDescriptions(
    edm::ConfigurationDescriptions &descriptions) {
}


// define this as a plug-in
DEFINE_FWK_MODULE(ReferenceAnalysisDQMWorker);