//
// This file is part of wsn_routing
//
// Copyright (C) 2017 Mateusz Czarnecki <mateusz.czarnecki92@gmail.com>
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'LICENSE'
// for details on this and other legal matters.
//

#ifndef __ROUTING_WSNSENSORAPP_H_
#define __ROUTING_WSNSENSORAPP_H_

#include "../sink/SinkInfo_m.h"

#include <omnetpp.h>
#include <vector>

#include "inet/applications/generic/IPvXTrafGen.h"
#include "inet/common/INETDefs.h"

#include "inet/networklayer/common/L3Address.h"
#include "inet/common/lifecycle/ILifecycle.h"
#include "inet/common/lifecycle/NodeStatus.h"

using namespace omnetpp;
using namespace inet;

/**
 * TODO - Generated class
 */
class WSNSensorApp : public IPvXTrafGen
{
  public:
    WSNSensorApp();
    virtual ~WSNSensorApp();
  protected:
    bool broadcastSinkAddress;

    virtual void initialize(int stage) override;
    virtual void processPacket(cPacket *msg) override;
    virtual void sendPacket() override;
    void processSinkInfo(SinkInfo *msg);
};

#endif
