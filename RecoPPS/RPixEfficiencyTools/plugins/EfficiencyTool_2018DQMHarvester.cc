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

#include <string>
#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <exception>
#include <fstream>
#include <memory>
#include <set>

int binGroupingX = 1;
int binGroupingY = 1;
int mapXbins_st0 = 200 / binGroupingX;
float mapXmin_st0 = 0. * TMath::Cos(18.4 / 180. * TMath::Pi());
float mapXmax_st0 = 30. * TMath::Cos(18.4 / 180. * TMath::Pi());
int mapYbins_st0 = 240 / binGroupingY;
float mapYmin_st0 = -16.;
float mapYmax_st0 = 8.;
float fitXmin_st0 = 45.;
float fitXmax_st0 = 58.;

int mapXbins = mapXbins_st0;
float mapXmin = mapXmin_st0;
float mapXmax = mapXmax_st0;
int mapYbins = mapYbins_st0;
float mapYmin = mapYmin_st0;
float mapYmax = mapYmax_st0;
float fitXmin = fitXmin_st0;
float fitXmax = fitXmax_st0;

class EfficiencyTool_2018DQMHarvester: public DQMEDHarvester{
public:
  explicit EfficiencyTool_2018DQMHarvester(const edm::ParameterSet &);
  ~EfficiencyTool_2018DQMHarvester();
  void dqmEndJob(DQMStore::IBooker &, DQMStore::IGetter &) override;

private:
  static const std::vector<uint32_t> romanPotIds;
  static const std::vector<uint32_t> armIds;
  static const std::vector<uint32_t> stationIds;
  static const std::vector<uint32_t> planeIds;

};

  const std::vector<uint32_t> EfficiencyTool_2018DQMHarvester::romanPotIds = {3};
  const std::vector<uint32_t> EfficiencyTool_2018DQMHarvester::armIds = {0, 1};
  const std::vector<uint32_t> EfficiencyTool_2018DQMHarvester::stationIds = {0, 2};
  const std::vector<uint32_t> EfficiencyTool_2018DQMHarvester::planeIds = {0, 1, 2, 3, 4, 5};

EfficiencyTool_2018DQMHarvester::EfficiencyTool_2018DQMHarvester(const edm::ParameterSet &iConfig) {
  

}

EfficiencyTool_2018DQMHarvester::~EfficiencyTool_2018DQMHarvester() {
  
}

void EfficiencyTool_2018DQMHarvester::dqmEndJob(DQMStore::IBooker & ibooker, DQMStore::IGetter & igetter){
  igetter.cd();
  ibooker.cd();
  MonitorElement* result = ibooker.book2DD("ptRatio", "pt ratio pf matched objects", mapXbins, mapXmin, mapXmax, mapYbins, mapYmin, mapYmax);

  for (int iBinX = 1; iBinX < 50; ++iBinX) {
    for (int iBinY = 1; iBinY < 100; ++iBinY)
    {
        result->setBinContent(iBinX, iBinY, 1.);
    }
  }
  for(auto& armId: armIds)
  {
    for(auto& stationId: stationIds)
    {
      for(auto& rpId: romanPotIds)
      {
        for(auto& planeId: planeIds)
        {
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
    }
  }

}

// define this as a plug-in
DEFINE_FWK_MODULE(EfficiencyTool_2018DQMHarvester);