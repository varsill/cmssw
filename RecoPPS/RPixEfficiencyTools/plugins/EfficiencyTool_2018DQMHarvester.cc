// -*- C++ -*-
//
// Package:    RecoPPS/RPixEfficiencyTools
// Class:      EfficiencyTool_2018DQMHarvester
//
/**\class EfficiencyTool_2018DQMHarvester EfficiencyTool_2018DQMHarvester.cc
 RecoPPS/RPixEfficiencyTools/plugins/EfficiencyTool_2018DQMHarvester.cc

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


class EfficiencyTool_2018DQMHarvester: public DQMEDHarvester{
public:
  explicit EfficiencyTool_2018DQMHarvester(const edm::ParameterSet &);
  ~EfficiencyTool_2018DQMHarvester();
  void dqmEndJob(DQMStore::IBooker &, DQMStore::IGetter &) override;
  void beginRun(edm::Run const &run, edm::EventSetup const & eventSetup) override {
    const auto& geom = eventSetup.getData(geomEsToken_);
    for (auto it = geom.beginSensor(); it != geom.endSensor(); ++it) {
      if (!CTPPSPixelDetId::check(it->first))
        continue;
      const CTPPSPixelDetId detid(it->first);
      
      detids_.emplace_back(detid);
    }
  };

private:
  static const std::vector<uint32_t> romanPotIds;
  static const std::vector<uint32_t> armIds;
  static const std::vector<uint32_t> stationIds;
  static const std::vector<uint32_t> planeIds;
  edm::ESGetToken<CTPPSGeometry, VeryForwardRealGeometryRecord> geomEsToken_;
  std::vector<CTPPSPixelDetId> detids_;

};

  const std::vector<uint32_t> EfficiencyTool_2018DQMHarvester::romanPotIds = {3};
  const std::vector<uint32_t> EfficiencyTool_2018DQMHarvester::armIds = {0, 1};
  const std::vector<uint32_t> EfficiencyTool_2018DQMHarvester::stationIds = {0, 2};
  const std::vector<uint32_t> EfficiencyTool_2018DQMHarvester::planeIds = {0, 1, 2, 3, 4, 5};

EfficiencyTool_2018DQMHarvester::EfficiencyTool_2018DQMHarvester(const edm::ParameterSet &iConfig): 
geomEsToken_(esConsumes<edm::Transition::BeginRun>()) {
  

}

EfficiencyTool_2018DQMHarvester::~EfficiencyTool_2018DQMHarvester() {
  
}

void EfficiencyTool_2018DQMHarvester::dqmEndJob(DQMStore::IBooker & ibooker, DQMStore::IGetter & igetter){
  igetter.cd();
  ibooker.cd();
  for (const auto& detId : detids_) {
    uint32_t armId = detId.arm();
    uint32_t rpId = detId.rp();
    uint32_t stationId = detId.station();
    uint32_t planeId = detId.plane();
    std::string romanPotBinShiftFolderName = Form("Arm%i/st%i/rp%i", armId, stationId, rpId);
    igetter.setCurrentFolder(romanPotBinShiftFolderName);
    ibooker.setCurrentFolder(romanPotBinShiftFolderName);
    std::string numMonitorName_ = Form("h2AuxEfficiencyMap_arm%i_st%i_rp%i_pl%i", armId, stationId, rpId, planeId);
    std::string denMonitorName_ = Form("h2EfficiencyNormalizationMap_arm%i_st%i_rp%i_pl%i", armId, stationId, rpId, planeId);
    std::string resultName_ = Form("h2EfficiencyMap_arm%i_st%i_rp%i_pl%i", armId, stationId, rpId, planeId);

    MonitorElement *numerator = igetter.get(romanPotBinShiftFolderName+"/"+numMonitorName_);
    MonitorElement *denominator = igetter.get(romanPotBinShiftFolderName+"/"+denMonitorName_);
    MonitorElement *result = igetter.get(romanPotBinShiftFolderName+"/"+resultName_);

    result->divide(numerator, denominator, 1., 1., "B");

    if(stationId==0)
    {
      numMonitorName_ = Form("h2AuxEfficiencyMap_rotated_arm%i_st%i_rp%i_pl%i", armId, stationId, rpId, planeId);
      denMonitorName_ = Form("h2EfficiencyNormalizationMap_rotated_arm%i_st%i_rp%i_pl%i", armId, stationId, rpId, planeId);
      resultName_ = Form("h2EfficiencyMap_rotated_arm%i_st%i_rp%i_pl%i", armId, stationId, rpId, planeId);

      numerator = igetter.get(romanPotBinShiftFolderName+"/"+numMonitorName_);
      denominator = igetter.get(romanPotBinShiftFolderName+"/"+denMonitorName_);
      result = igetter.get(romanPotBinShiftFolderName+"/"+resultName_);

      result->divide(numerator, denominator, 1., 1., "B");
    }
  }
}

// define this as a plug-in
DEFINE_FWK_MODULE(EfficiencyTool_2018DQMHarvester);