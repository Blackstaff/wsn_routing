//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
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
        maxEnergy = energyStorage->getNominalCapacity().get();
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
    double generalProb = expectedCHNum / (numSensors - expectedCHNum * (roundNumber % (numSensors / expectedCHNum) ));
    double currentEnergy = energyStorage->getResidualCapacity().get();
    double currentStateProb = (currentEnergy / maxEnergy) * (expectedCHNum / numSensors);
    if (isCt) {
        probability = 0;
    } else {
        probability = generalProb + currentStateProb;
    }
    if (randomNumber < probability) {
        isCH = true;
    }
}
