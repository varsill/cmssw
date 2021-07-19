// -*- C++ -*-
//
// Package:    RecoPPS/RPixEfficiencyTools
// Class:      EfficiencyTool_2018DQM
//
/**\class EfficiencyTool_2018DQM EfficiencyTool_2018DQM.cc
 RecoPPS/RPixEfficiencyTools/plugins/EfficiencyTool_2018DQM.cc

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

#include <string>
#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <exception>
#include <fstream>
#include <memory>
#include<set>


class EfficiencyTool_2018DQM: public DQMEDAnalyzer {
public:
  explicit EfficiencyTool_2018DQM(const edm::ParameterSet &);
  ~EfficiencyTool_2018DQM();
  static void fillDescriptions(edm::ConfigurationDescriptions &descriptions);

protected:
  virtual void bookHistograms(DQMStore::IBooker &, edm::Run const &, edm::EventSetup const &) override;
  virtual void dqmBeginRun(edm::Run const &, edm::EventSetup const &) override;
private:
};

EfficiencyTool_2018DQM::EfficiencyTool_2018DQM(const edm::ParameterSet &iConfig) {
  //usesResource("TFileService");


}




EfficiencyTool_2018DQM::~EfficiencyTool_2018DQM() {
  
}












// void EfficiencyTool_2018DQM::endJob() {
//   outputFile_ = new TFile(outputFileName_.data(), "RECREATE");
//   std::cout << "Saving output in: " << outputFile_->GetName() << std::endl;
//   h1BunchCrossing_->Write();
//   h1CrossingAngle_->Write();
//   for (const auto &rpId : romanPotIdVector_) {
//     uint32_t arm = rpId.arm();
//     uint32_t rp = rpId.rp();
//     uint32_t station = rpId.station();
//     if (station == 2) {
//       mapXbins = mapXbins_st2;
//       mapXmin = mapXmin_st2;
//       mapXmax = mapXmax_st2;
//       mapYbins = mapYbins_st2;
//       mapYmin = mapYmin_st2;
//       mapYmax = mapYmax_st2;
//       fitXmin = fitXmin_st2;
//       fitXmax = fitXmax_st2;
//     } else {
//       mapXbins = mapXbins_st0;
//       mapXmin = mapXmin_st0;
//       mapXmax = mapXmax_st0;
//       mapYbins = mapYbins_st0;
//       mapYmin = mapYmin_st0;
//       mapYmax = mapYmax_st0;
//       fitXmin = fitXmin_st0;
//       fitXmax = fitXmax_st0;
//     }

//     std::string romanPotFolderName = Form("Arm%i_st%i_rp%i", arm, station, rp);
//     std::string romanPotBinShiftFolderName =
//         Form("Arm%i_st%i_rp%i/BinShift", arm, station, rp);
//     // std::cout << "Creating directory for: " << romanPotFolderName <<
//     // std::endl;

//     outputFile_->mkdir(romanPotFolderName.data());
//     outputFile_->cd(romanPotFolderName.data());
//     h2TrackHitDistribution_[rpId]->Write();
//     if (supplementaryPlots) {
//       outputFile_->mkdir(romanPotBinShiftFolderName.data());
//       outputFile_->cd(romanPotBinShiftFolderName.data());
//       for (auto binShift : binShifts_) {
//         h2TrackHitDistributionBinShift_[binShift][rpId]->Write();
//         if (station == 0) {
//           h2TrackHitDistributionBinShift_rotated[binShift][rpId]->Write();
//         }
//       }
//       outputFile_->cd(romanPotFolderName.data());
//     }

//     h23PointsTrackHitDistribution_[rpId]->Write();
//     // h2X0Correlation_[rpId]->Write();
//     // h2Y0Correlation_[rpId]->Write();
//     if (isCorrelationPlotEnabled) {
//       // g1X0Correlation_[rpId]->Write();
//       // g1Y0Correlation_[rpId]->Write();
//     }
//   }

//   for (const auto &detId : detectorIdVector_) {
//     uint32_t arm = detId.arm();
//     uint32_t rp = detId.rp();
//     uint32_t station = detId.station();
//     uint32_t plane = detId.plane();
//     std::string planeFolderName =
//         Form("Arm%i_st%i_rp%i/Arm%i_st%i_rp%i_pl%i", arm, station, rp, arm,
//              station, rp, plane);
//     // std::cout << "Creating directory for: " << planeFolderName << std::endl;
//     outputFile_->mkdir(planeFolderName.data());
//     // std::cout << "Created directory for: " << planeFolderName << std::endl;
//     outputFile_->cd(planeFolderName.data());

//     h2ModuleHitMap_[detId]->Write();
//     h2EfficiencyMap_[detId]->Divide(h2AuxEfficiencyMap_[detId],
//                                     h2EfficiencyNormalizationMap_[detId], 1.,
//                                     1., "B");
//     h2EfficiencyMap_[detId]->SetMaximum(1.);
//     h2EfficiencyMap_[detId]->Write();

//     if (station == 0) {
//       h2EfficiencyMap_rotated[detId]->Divide(
//           h2AuxEfficiencyMap_rotated[detId],
//           h2EfficiencyNormalizationMap_rotated[detId], 1., 1., "B");
//       h2EfficiencyMap_rotated[detId]->SetMaximum(1.);
//       h2EfficiencyMap_rotated[detId]->Write();
//       if (supplementaryPlots)
//         h2ModuleHitMap_rotated[detId]->Write();
//     }
//   }

//   for (const auto &rpId : romanPotIdVector_) {
//     uint32_t arm = rpId.arm();
//     uint32_t rp = rpId.rp();
//     uint32_t station = rpId.station();
//     if (station == 2) {
//       mapXbins = mapXbins_st2;
//       mapXmin = mapXmin_st2;
//       mapXmax = mapXmax_st2;
//       mapYbins = mapYbins_st2;
//       mapYmin = mapYmin_st2;
//       mapYmax = mapYmax_st2;
//       fitXmin = fitXmin_st2;
//       fitXmax = fitXmax_st2;
//     } else {
//       mapXbins = mapXbins_st0;
//       mapXmin = mapXmin_st0;
//       mapXmax = mapXmax_st0;
//       mapYbins = mapYbins_st0;
//       mapYmin = mapYmin_st0;
//       mapYmax = mapYmax_st0;
//       fitXmin = fitXmin_st0;
//       fitXmax = fitXmax_st0;
//     }

//     std::string romanPotFolderName = Form("Arm%i_st%i_rp%i", arm, station, rp);
//     outputFile_->cd(romanPotFolderName.data());
//     for (int xBin = 1; xBin <= mapXbins; ++xBin) {
//       for (int yBin = 1; yBin <= mapYbins; ++yBin) {
//         std::map<uint32_t, float> planeEfficiency;
//         std::map<uint32_t, float> planeEfficiencyError;
//         std::map<uint32_t, float> planeEfficiency_rotated;
//         std::map<uint32_t, float> planeEfficiencyError_rotated;

//         for (const auto &plane : listOfPlanes_) {
//           CTPPSPixelDetId planeId = CTPPSPixelDetId(rpId);
//           planeId.setPlane(plane);
//           planeEfficiency[plane] =
//               h2EfficiencyMap_[planeId]->GetBinContent(xBin, yBin);
//           planeEfficiencyError[plane] =
//               h2EfficiencyMap_[planeId]->GetBinError(xBin, yBin);
//           if (station == 0) {
//             planeEfficiency_rotated[plane] =
//                 h2EfficiencyMap_rotated[planeId]->GetBinContent(xBin, yBin);
//             planeEfficiencyError_rotated[plane] =
//                 h2EfficiencyMap_rotated[planeId]->GetBinError(xBin, yBin);
//           }

//           // if(plane == 0){
//           // 	planeEfficiency[plane] = 1.;
//           // 	if(station==0) planeEfficiency_rotated[plane] = 1.;
//           // }
//           // if(plane == 1){
//           // 	planeEfficiency[plane] = 1.;
//           // 	if(station==0) planeEfficiency_rotated[plane] = 1.;
//           // }
//           // if(plane == 2){
//           // 	planeEfficiency[plane] = 1.;
//           // 	if(station==0) planeEfficiency_rotated[plane] = 1.;
//           // }
//           // if(plane == 3){
//           // 	planeEfficiency[plane] = 1.;
//           // 	if(station==0) planeEfficiency_rotated[plane] = 1.;
//           // }
//           // if(plane == 4){
//           // 	planeEfficiency[plane] = 1.;
//           // 	if(station==0) planeEfficiency_rotated[plane] = 1.;
//           // }
//           // if(plane == 5){
//           // 	planeEfficiency[plane] = 0.;
//           // 	if(station==0) planeEfficiency_rotated[plane] = 0.;
//           // }
//         }
//         float efficiency = probabilityCalculation(planeEfficiency);
//         float efficiencyError =
//             errorCalculation(planeEfficiency, planeEfficiencyError);
//         h2TrackEfficiencyMap_[rpId]->SetBinContent(xBin, yBin, efficiency);
//         h2TrackEfficiencyMap_[rpId]->SetBinError(xBin, yBin, efficiencyError);
//         h2TrackEfficiencyErrorMap_[rpId]->SetBinContent(xBin, yBin,
//                                                         efficiencyError);
//         if (station == 0) {
//           float efficiency_rotated =
//               probabilityCalculation(planeEfficiency_rotated);
//           float efficiencyError_rotated = errorCalculation(
//               planeEfficiency_rotated, planeEfficiencyError_rotated);
//           h2TrackEfficiencyMap_rotated[rpId]->SetBinContent(xBin, yBin,
//                                                             efficiency_rotated);
//           h2TrackEfficiencyMap_rotated[rpId]->SetBinError(
//               xBin, yBin, efficiencyError_rotated);
//           h2TrackEfficiencyErrorMap_rotated[rpId]->SetBinContent(
//               xBin, yBin, efficiencyError_rotated);
//         }
//       }
//     }
//     h2TrackEfficiencyMap_[rpId]->Write();
//     h2TrackEfficiencyErrorMap_[rpId]->Write();
//     if (supplementaryPlots) {
//       outputFile_->mkdir((romanPotFolderName + "/ResolutionHistograms").data());
//       // std::cout << (romanPotFolderName+"/ResolutionHistograms").data() <<
//       // std::endl;
//       outputFile_->cd((romanPotFolderName + "/ResolutionHistograms").data());
//       for (int nPlanes = 3; nPlanes <= 6; nPlanes++) {
//         for (int numberOfCls = 0; numberOfCls <= nPlanes; numberOfCls++) {
//           h1X0Sigma[rpId][std::pair(nPlanes, numberOfCls)]->Write();
//           h1Y0Sigma[rpId][std::pair(nPlanes, numberOfCls)]->Write();
//           h1TxSigma[rpId][std::pair(nPlanes, numberOfCls)]->Write();
//           h1TySigma[rpId][std::pair(nPlanes, numberOfCls)]->Write();
//         }
//       }

//       outputFile_->cd(romanPotFolderName.data());

//       h1NumberOfTracks_[rpId]->Write();
//       h2AvgPlanesUsed_[rpId]->Divide(h2TrackHitDistribution_[rpId]);
//       h2AvgPlanesUsed_[rpId]->Write();
//       h1PlanesUsed_[rpId]->Write();
//       h1ChiSquaredOverNDF_[rpId]->Write();
//       h1ConsecutivePlanes_[rpId]->Write();
//     }
//     if (station == 0) {
//       h2TrackEfficiencyMap_rotated[rpId]->Write();
//       h2TrackEfficiencyErrorMap_rotated[rpId]->Write();
//       if (supplementaryPlots) {
//         h2TrackHitDistribution_rotated[rpId]->Write();
//         h2AvgPlanesUsed_rotated[rpId]->Divide(
//             h2TrackHitDistribution_rotated[rpId]);
//         h2AvgPlanesUsed_rotated[rpId]->Write();
//       }
//     }
//     if (supplementaryPlots && station != 0) {
//       TObjArray slices;
//       TF1 *fGaus = new TF1("fGaus", "gaus", mapYmin, mapYmax);
//       h2TrackHitDistribution_[rpId]->FitSlicesY(fGaus, 1, mapXbins, 0, "QNG3",
//                                                 &slices);
//       delete fGaus;
//       ((TH1D *)slices[0])->Write();
//       ((TH1D *)slices[1])->Write();
//       ((TH1D *)slices[2])->Write();
//       ((TH1D *)slices[3])->Write();
//     }
//     if (supplementaryPlots && station == 0) {
//       TObjArray slices;
//       TF1 *fGaus = new TF1("fGaus", "gaus", mapYmin, mapYmax);
//       h2TrackHitDistribution_rotated[rpId]->FitSlicesY(fGaus, 1, mapXbins, 0,
//                                                        "QNG3", &slices);
//       delete fGaus;
//       ((TH1D *)slices[0])->Write();
//       ((TH1D *)slices[1])->Write();
//       ((TH1D *)slices[2])->Write();
//       ((TH1D *)slices[3])->Write();
//     }
//   }

//   if (isCorrelationPlotEnabled)
//     std::cout << "ATTENTION: Remember to insert the fitting parameters in the "
//                  "python configuration"
//               << std::endl;
//   outputFile_->Close();
// }


// define this as a plug-in
DEFINE_FWK_MODULE(EfficiencyTool_2018DQM);