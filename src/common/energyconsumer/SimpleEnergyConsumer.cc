//
// This file is part of wsn_routing
//
// Copyright (C) 2017 Mateusz Czarnecki <mateusz.czarnecki92@gmail.com>
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'LICENSE'
// for details on this and other legal matters.
//

#include "inet/common/ModuleAccess.h"

#include "SimpleEnergyConsumer.h"

Define_Module(SimpleEnergyConsumer);

SimpleEnergyConsumer::SimpleEnergyConsumer() :
    energyConsumerId(-1),
    energySource(nullptr),
    powerConsumption(W(0)),
    timer(nullptr)
{
}

SimpleEnergyConsumer::~SimpleEnergyConsumer()
{
    cancelAndDelete(timer);
}

void SimpleEnergyConsumer::initialize(int stage)
{
    EV << "Initializing SimpleEnergyConsumer, stage = " << stage << endl;
    if (stage == INITSTAGE_LOCAL) {
        timer = new cMessage("timer");
        const char *energySourceModule = par("energySourceModule");
        energySource = dynamic_cast<IEnergySource *>(getModuleByPath(energySourceModule));
        if (!energySource)
            throw cRuntimeError("Energy source module '%s' not found", energySourceModule);
        energyConsumerId = energySource->addEnergyConsumer(this);
        WATCH(powerConsumption);
    } else if (stage == INITSTAGE_LAST) {
        updatePowerConsumption();
    }
}

void SimpleEnergyConsumer::handleMessage(cMessage *message)
{
    if (message == timer) {
        powerConsumption = W(0);
        updatePowerConsumption();
    }
    else
        throw cRuntimeError("Unknown message");
}

void SimpleEnergyConsumer::updatePowerConsumption()
{
    energySource->setPowerConsumption(energyConsumerId, powerConsumption);
    emit(IEnergySource::powerConsumptionChangedSignal, powerConsumption.get());
}

void SimpleEnergyConsumer::setPowerConsumption(W power)
{
    Enter_Method_Silent();
    powerConsumption = power;
    updatePowerConsumption();
    scheduleAt(simTime() + par("consumptionInterval"), timer);
}

