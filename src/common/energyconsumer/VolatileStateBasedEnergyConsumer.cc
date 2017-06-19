//
// This file is part of wsn_routing
//
// Copyright (C) 2017 Mateusz Czarnecki <mateusz.czarnecki92@gmail.com>
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'LICENSE'
// for details on this and other legal matters.
//

#include "inet/common/ModuleAccess.h"

#include "VolatileStateBasedEnergyConsumer.h"

Define_Module(VolatileStateBasedEnergyConsumer);

VolatileStateBasedEnergyConsumer::VolatileStateBasedEnergyConsumer() :
    baseConsumption(W(NaN))
{
}

void VolatileStateBasedEnergyConsumer::initialize(int stage)
{
    StateBasedEnergyConsumer::initialize(stage);
    EV << "Initializing VolatileStateBasedEnergyConsumer, stage = " << stage << endl;
    if (stage == INITSTAGE_LOCAL) {
        baseConsumption = W(par("baseConsumption"));
        getContainingNode(this)->subscribe("transmitterPowerChanged", this);
        WATCH(transmitterTransmittingPowerConsumption);
    }
}

void VolatileStateBasedEnergyConsumer::receiveSignal(cComponent *source, simsignal_t signalID, double value, cObject *details)
{
    transmitterTransmittingPowerConsumption = baseConsumption + mW(value);
    transmitterTransmittingPreamblePowerConsumption = transmitterTransmittingPowerConsumption;
    transmitterTransmittingHeaderPowerConsumption = transmitterTransmittingPowerConsumption;
    transmitterTransmittingDataPowerConsumption = transmitterTransmittingPowerConsumption;
}
