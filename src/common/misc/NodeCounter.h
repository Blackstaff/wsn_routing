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

using namespace omnetpp;
using namespace inet;

class NodeCounter : public cSimpleModule, public cListener
{
  public:
    static simsignal_t aliveNodesChangedSignal;
    static simsignal_t sinkDownSignal;

  protected:
    int aliveNodes;

  public:
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, cObject *status, cObject *details) override;

  protected:
    virtual int numInitStages() const override { return NUM_INIT_STAGES; }
    virtual void initialize(int stage) override;
    virtual void handleMessage(cMessage *msg) override { throw cRuntimeError("This module doesn't handle messages"); }
};

#endif
