//
// This file is part of wsn_routing
//
// Copyright (C) 2017 Mateusz Czarnecki <mateusz.czarnecki92@gmail.com>
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'LICENSE'
// for details on this and other legal matters.
//

#include "WSNSinkApp.h"
#include "SinkInfo_m.h"
#include "../PacketType.h"

#include "inet/common/ModuleAccess.h"
#include "inet/common/lifecycle/NodeOperations.h"
#include "inet/networklayer/common/IPSocket.h"
#include "inet/networklayer/contract/INetworkProtocolControlInfo.h"
#include "inet/networklayer/contract/IL3AddressType.h"

Define_Module(WSNSinkApp);

void WSNSinkApp::initialize(int stage)
{
    cSimpleModule::initialize(stage);

    if (stage == INITSTAGE_LOCAL) {
        interfaceTable = getModuleFromPar<IInterfaceTable>(par("interfaceTableModule"), this);
        broadcastSinkAddress = par("broadcastSinkAddress");
        numReceived = 0;
        WATCH(numReceived);
    }
    else if (stage == INITSTAGE_NETWORK_LAYER_3) {
        myAddr = interfaceTable->getInterface(0)->getMacAddress();
        /*
         * More generic alternative
            myAddr = interfaceTable->getInterface(0)->getNetworkAddress();
        */
    }
    else if (stage == INITSTAGE_APPLICATION_LAYER) {
        protocol = par("protocol");
        IPSocket ipSocket(gate("appOut"));
        ipSocket.registerProtocol(protocol);

        NodeStatus *nodeStatus = dynamic_cast<NodeStatus *>(findContainingNode(this)->getSubmodule("status"));
        isOperational = (!nodeStatus) || nodeStatus->getState() == NodeStatus::UP;
        if (broadcastSinkAddress) {
            sendSinkInfo();
        }
    }
}

void WSNSinkApp::sendPacket(cPacket *packet, L3Address destAddr)
{
    IL3AddressType *addressType = destAddr.getAddressType();
    INetworkProtocolControlInfo *controlInfo = addressType->createNetworkProtocolControlInfo();
    //controlInfo->setSourceAddress();
    controlInfo->setDestinationAddress(destAddr);
    controlInfo->setTransportProtocol(protocol);
    packet->setControlInfo(check_and_cast<cObject *>(controlInfo));

    EV_INFO << "Sending packet: ";
    printPacket(packet);
    //emit(sentPkSignal, payload);
    send(packet, "appOut");
    //numSent++;
}

void WSNSinkApp::sendSinkInfo() {
    char msgName[32];
    sprintf(msgName, "Sink address broadcast");

    SinkInfo *msg = new SinkInfo(msgName);
    msg->setByteLength(MAC_ADDRESS_SIZE);
    msg->setAddress(myAddr);
    msg->setKind(P_SinkInfo);

    L3Address destAddr = MACAddress::BROADCAST_ADDRESS;
    sendPacket(msg, destAddr);
}
