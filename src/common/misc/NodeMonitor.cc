//
// This file is part of wsn_routing
//
// Copyright (C) 2017 Mateusz Czarnecki <mateusz.czarnecki92@gmail.com>
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'LICENSE'
// for details on this and other legal matters.
//

#include "inet/common/ModuleAccess.h"
#include "NodeMonitor.h"

Define_Module(NodeMonitor);

simsignal_t NodeMonitor::storedEnergyChangedSignal = registerSignal("storedEnergyChanged");

NodeMonitor::NodeMonitor() :
    storedEnergy(J(0)),
    energyStorage(nullptr),
    timer(nullptr),
    interval(0.0)
{
}

NodeMonitor::~NodeMonitor()
{
    cancelAndDelete(timer);
}

void NodeMonitor::initialize(int stage)
{
    cSimpleModule::initialize(stage);
    EV << "Initializing NodeMonitor, stage = " << stage << endl;
    if (stage == INITSTAGE_LOCAL) {
        energyStorage = check_and_cast<IEnergyStorage *>(getModuleFromPar<cModule>(par("energyStorageModule"), this));
        timer = new cMessage("Emit signal timer");
        interval = par("timerInterval");
        updateStoredEnergy();
        scheduleAt(simTime() + interval, timer);
        WATCH(storedEnergy);
        setOperational(true);
    }
}

void NodeMonitor::handleMessageWhenUp(cMessage *msg)
{
    if (msg == timer) {
        updateStoredEnergy();
        scheduleAt(simTime() + interval, timer);
    } else {
        throw cRuntimeError("Unknown self message");
    }
}

void NodeMonitor::updateStoredEnergy()
{
    storedEnergy = energyStorage->getResidualCapacity();
    emit(storedEnergyChangedSignal, storedEnergy.get());
}

bool NodeMonitor::handleNodeStart(IDoneCallback *doneCallback)
{
    updateStoredEnergy();
    scheduleAt(simTime() + interval, timer);
    return true;
}

bool NodeMonitor::handleNodeShutdown(IDoneCallback *doneCallback)
{
    updateStoredEnergy();
    cancelEvent(timer);
    return true;
}

void NodeMonitor::handleNodeCrash()
{
    updateStoredEnergy();
    cancelEvent(timer);
}
