//
// This file is part of wsn_routing
//
// Copyright (C) 2017 Mateusz Czarnecki <mateusz.czarnecki92@gmail.com>
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'LICENSE'
// for details on this and other legal matters.
//

#ifndef __ROUTING_WSNSINKAPP_H_
#define __ROUTING_WSNSINKAPP_H_

#include <omnetpp.h>

#include "inet/applications/generic/IPvXTrafGen.h"
#include "inet/common/INETDefs.h"
#include "inet/networklayer/contract/IInterfaceTable.h"

using namespace omnetpp;
using namespace inet;

/**
 * TODO - Generated class
 */
class WSNSinkApp : public IPvXTrafSink
{
    protected:
        int protocol;
        bool broadcastSinkAddress;
        IInterfaceTable *interfaceTable;
        L3Address myAddr;
    protected:
        virtual void initialize(int stage) override;
        virtual void sendPacket(cPacket *packet, L3Address destAddr);
        virtual void sendSinkInfo();
};

#endif
