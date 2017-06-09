//
// This file is part of wsn_routing
//
// Copyright (C) 2017 Mateusz Czarnecki <mateusz.czarnecki92@gmail.com>
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'LICENSE'
// for details on this and other legal matters.
//

#include "ALEACH.h"

#include "inet/common/ModuleAccess.h"

Define_Module(ALEACH);

void ALEACH::initialize(int stage)
{
    LEACH::initialize(stage);
    if (stage == INITSTAGE_LOCAL) {
        cModule *energyStorageModule = getModuleFromPar<cModule>(par("energyStorageModule"), this);
        energyStorage = check_and_cast<power::IEnergyStorage *>(energyStorageModule);
        numSensors = par("numSensors");
        expectedCHNum = ceil(numSensors * percentage);
    } else if (INITSTAGE_LAST) {
        maxEnergy = energyStorage->getNominalCapacity().get() * numSensors;
    }
}

void ALEACH::selectCH()
{
    if (roundNumber >= (numSensors / expectedCHNum)) {
        roundNumber = 0;
        isCt = false;
        isCH = false;
    }

    double randomNumber = uniform(0, 1);
    if (isCH) {
        isCH = false;
        isCt = true;
    }
    double generalProb = (double)expectedCHNum / (double)(numSensors - expectedCHNum * (roundNumber % (numSensors / expectedCHNum) ));
    double currentEnergy = energyStorage->getResidualCapacity().get();
    double currentStateProb = (currentEnergy / maxEnergy) * ((double) expectedCHNum / numSensors);
    if (isCt) {
        probability = 0;
    } else {
        probability = generalProb + currentStateProb;
    }
    if (randomNumber < probability) {
        isCH = true;
    }
}
