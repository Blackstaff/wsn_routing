//
// This file is part of wsn_routing
//
// Copyright (C) 2017 Mateusz Czarnecki <mateusz.czarnecki92@gmail.com>
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'LICENSE'
// for details on this and other legal matters.
//

#include "inet/common/lifecycle/NodeStatus.h"
#include "NodeCounter.h"

Define_Module(NodeCounter);

simsignal_t NodeCounter::aliveNodesChangedSignal = registerSignal("aliveNodesChanged");
simsignal_t NodeCounter::sinkDownSignal = registerSignal("sinkDown");

void NodeCounter::initialize(int stage)
{
    cSimpleModule::initialize(stage);
    EV << "Initializing NodeCounter, stage = " << stage << endl;
    if (stage == INITSTAGE_LOCAL) {
        aliveNodes = par("numSensors");
        getSimulation()->getSystemModule()->subscribe(NodeStatus::nodeStatusChangedSignal, this);
        WATCH(aliveNodes);
        emit(aliveNodesChangedSignal, aliveNodes);
    }
}

void NodeCounter::receiveSignal(cComponent *source, simsignal_t signalID, cObject *status, cObject *details)
{
    NodeStatus *nodeStatus = check_and_cast<NodeStatus *>(status);
    const char *sourceName = source->getParentModule()->getComponentType()->getName();
    if (strcmp(sourceName, "WSNSinkNode") == 0) {
        if (nodeStatus->getState() == NodeStatus::DOWN) {
            emit(sinkDownSignal, simTime());
        }
    } else {
        if (nodeStatus->getState() == NodeStatus::DOWN) {
            --aliveNodes;
            emit(aliveNodesChangedSignal, aliveNodes);
        } else if (nodeStatus->getState() == NodeStatus::UP) {
            ++aliveNodes;
            emit(aliveNodesChangedSignal, aliveNodes);
        }
    }
}
