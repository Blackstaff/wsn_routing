//
// This file is part of wsn_routing
//
// Copyright (C) 2017 Mateusz Czarnecki <mateusz.czarnecki92@gmail.com>
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'LICENSE'
// for details on this and other legal matters.
//

#ifndef __ROUTING_NODECOUNTER_H_
#define __ROUTING_NODECOUNTER_H_

#include <omnetpp.h>

#include "inet/power/contract/IEnergyStorage.h"
#include "inet/common/lifecycle/OperationalBase.h"
#include "inet/common/lifecycle/NodeOperations.h"

using namespace omnetpp;
using namespace inet;
using namespace power;

class NodeMonitor : public OperationalBase
{
  public:
    static simsignal_t storedEnergyChangedSignal;

  protected:
    J storedEnergy;
    IEnergyStorage *energyStorage;
    cMessage *timer;
    double interval;

  public:
    NodeMonitor();
    virtual ~NodeMonitor();

  protected:
    virtual int numInitStages() const override { return NUM_INIT_STAGES; }
    virtual void initialize(int stage) override;
    virtual void handleMessageWhenUp(cMessage *msg) override;
    virtual void updateStoredEnergy();

    virtual bool handleNodeStart(IDoneCallback *doneCallback) override;
    virtual bool handleNodeShutdown(IDoneCallback *doneCallback) override;
    virtual void handleNodeCrash() override;

    virtual bool isInitializeStage(int stage) override { return stage == INITSTAGE_LOCAL; }
    virtual bool isNodeStartStage(int stage) override { return stage == NodeStartOperation::STAGE_LOCAL; }
    virtual bool isNodeShutdownStage(int stage) override { return stage == NodeShutdownOperation::STAGE_LOCAL; }
};

#endif
