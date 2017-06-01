//
// This file is part of wsn_routing
//
// Copyright (C) 2017 Mateusz Czarnecki <mateusz.czarnecki92@gmail.com>
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'LICENSE'
// for details on this and other legal matters.
//

#include "WSNSensorApp.h"
#include "../PacketType.h"
#include "../sink/SinkInfo_m.h"

#include "inet/common/ModuleAccess.h"
#include "inet/common/lifecycle/NodeOperations.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/networklayer/common/IPSocket.h"
#include "inet/networklayer/contract/IL3AddressType.h"
#include "inet/networklayer/contract/INetworkProtocolControlInfo.h"

using namespace omnetpp;
using namespace inet;

Define_Module(WSNSensorApp);

//simsignal_t WSNSensorApp::rcvdPkSignal = registerSignal("rcvdPk");
//simsignal_t WSNSensorApp::sentPkSignal = registerSignal("sentPk");

WSNSensorApp::WSNSensorApp()
{
}

WSNSensorApp::~WSNSensorApp()
{
}

void WSNSensorApp::initialize(int stage)
{
    cSimpleModule::initialize(stage);

    // because of IPvXAddressResolver, we need to wait until interfaces are registered,
    // address auto-assignment takes place etc.
    if (stage == INITSTAGE_LOCAL) {
        protocol = par("protocol");
        numPackets = par("numPackets");
        startTime = par("startTime");
        stopTime = par("stopTime");
        if (stopTime >= SIMTIME_ZERO && stopTime < startTime)
            throw cRuntimeError("Invalid startTime/stopTime parameters");

        packetLengthPar = &par("packetLength");
        sendIntervalPar = &par("sendInterval");
        broadcastSinkAddress = par("broadcastSinkAddress");

        numSent = 0;
        numReceived = 0;
        WATCH(numSent);
        WATCH(numReceived);
    }
    else if (stage == INITSTAGE_APPLICATION_LAYER) {
        IPSocket ipSocket(gate("appOut"));
        ipSocket.registerProtocol(protocol);

        timer = new cMessage("sendTimer");
        nodeStatus = dynamic_cast<NodeStatus *>(findContainingNode(this)->getSubmodule("status"));
        isOperational = (!nodeStatus) || nodeStatus->getState() == NodeStatus::UP;
        setOperational(isOperational);
    }
    else if (stage == INITSTAGE_LAST) {
        if (!broadcastSinkAddress) {
            par("destAddresses") = MACAddress::BROADCAST_ADDRESS.str();
        }

        if (isNodeUp())
            startApp();
    }
}

void WSNSensorApp::processPacket(cPacket *msg) {
    if (msg->getKind() == P_SinkInfo) {
        processSinkInfo(check_and_cast<SinkInfo *>(msg));
    }
    IPvXTrafGen::processPacket(msg);
}

void WSNSensorApp::processSinkInfo(SinkInfo *msg) {
    std::string address = msg->getAddress().str();
    const char *destAddresses = par("destAddresses").stringValue();
    if (*destAddresses == 0) {
        par("destAddresses") = address;
    }
    else {
        std::stringstream ss;
        ss << destAddresses << " " << address;
        par("destAddresses") = ss.str();
    }
}

void WSNSensorApp::sendPacket()
{
    char msgName[32];
    sprintf(msgName, "%s-appData-%d", getParentModule()->getName(), numSent);

    cPacket *payload = new cPacket(msgName);
    payload->setByteLength(packetLengthPar->longValue());

    L3Address destAddr = chooseDestAddr();

    IL3AddressType *addressType = destAddr.getAddressType();
    INetworkProtocolControlInfo *controlInfo = addressType->createNetworkProtocolControlInfo();
    //controlInfo->setSourceAddress();
    controlInfo->setDestinationAddress(destAddr);
    controlInfo->setTransportProtocol(protocol);
    payload->setControlInfo(check_and_cast<cObject *>(controlInfo));

    EV_INFO << "Sending packet: ";
    printPacket(payload);
    emit(sentPkSignal, payload);
    send(payload, "appOut");
    numSent++;
}

