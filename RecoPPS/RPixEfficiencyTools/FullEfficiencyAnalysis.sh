#!/bin/bash
if [ $# -ne 2 ]
then
    echo "Two arguments required. Nothing done."
else
	export CMSSW_BASE=${2}
	export SCRAM_ARCH=slc7_amd64_gcc700
	export X509_USER_PROXY=$CMSSW_BASE/src/RecoCTPPS/RPixEfficiencyTools/x509up_u93252
	cd $CMSSW_BASE/src/RecoCTPPS/RPixEfficiencyTools/
	eval `scramv1 runtime -sh`
	# eval "cmsRun python/EfficiencyAnalysis_cfg.py sourceFileList=InputFiles/Run${1}.dat outputFileName=OutputFiles/Run${1}.root runNumber=${1} bunchSelection=NoSelection"
	# eval "cmsRun python/RefinedEfficiencyAnalysis_cfg.py sourceFileList=InputFiles/Run315512.dat efficiencyFileName=OutputFiles/Run${1}.root runNumber=${1}"
	eval "cmsRun python/EfficiencyVsXi_cfg.py sourceFileList=InputFiles/Run315512.dat efficiencyFileName=OutputFiles/Run${1}_refinedEfficiency.root runNumber=${1}"
	# eval "cmsRun python/InterpotEfficiency_cfg.py sourceFileList=InputFiles/Run${1}.dat outputFileName=OutputFiles/Run${1}_interpotEfficiency.root runNumber=${1}"
fi