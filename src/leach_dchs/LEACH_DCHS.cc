//
// This file is part of wsn_routing
//
// Copyright (C) 2017 Mateusz Czarnecki <mateusz.czarnecki92@gmail.com>
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'LICENSE'
// for details on this and other legal matters.
//

#include "LEACH_DCHS.h"
#include "inet/common/ModuleAccess.h"

Define_Module(LEACH_DCHS);

void LEACH_DCHS::initialize(int stage)
{
    LEACH::initialize(stage);
    if (stage == INITSTAGE_LOCAL) {
        cModule *energyStorageModule = getModuleFromPar<cModule>(par("energyStorageModule"), this);
        energyStorage = check_and_cast<power::IEnergyStorage *>(energyStorageModule);
        notCHRounds = 0;
    } else if (INITSTAGE_LAST) {
        maxEnergy = energyStorage->getNominalCapacity().get();
    }
}

void LEACH_DCHS::selectCH()
{
    if (roundNumber >= (1 / percentage)) {
        roundNumber = 0;
        isCt = false;
        isCH = false;
    }

    double randomNumber = uniform(0, 1);
    if (isCH) {
        isCH = false;
        isCt = true;
    }
    double currentEnergy = energyStorage->getResidualCapacity().get();
    if (isCt) {
        probability = 0;
    } else {
        probability = percentage / (1 - percentage * (roundNumber % (int)(1/percentage)))
                * (currentEnergy / maxEnergy + (notCHRounds / (1/percentage)) * (1 - (currentEnergy / maxEnergy)));
    }
    if (randomNumber < probability) {
        isCH = true;
        notCHRounds = 0;
    } else if (!isCt) {
        ++notCHRounds;
    }
}
