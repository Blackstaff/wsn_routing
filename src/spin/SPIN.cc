
//
// This file is part of wsn_routing
//
// Copyright (C) 2017 Mateusz Czarnecki <mateusz.czarnecki92@gmail.com>
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'LICENSE'
// for details on this and other legal matters.
//

#include "inet/linklayer/common/SimpleLinkLayerControlInfo.h"
#include "inet/networklayer/common/SimpleNetworkProtocolControlInfo.h"
#include "inet/networklayer/contract/IL3AddressType.h"

#include "SPIN.h"

using namespace omnetpp;
using namespace inet;

enum SPINSelfMessageKind {
    REQ_TIMER = 1,
    REQ_CHECK = 2
};

Define_Module(SPIN);

SPIN::~SPIN()
{

    for (std::map<MsgMetadata, SPINDatagram*>::iterator itr = queuedMessages.begin();
            itr != queuedMessages.end(); itr++) {
        cancelAndDelete(itr->second);
    }

    for (std::map<MsgMetadata, SPINDatagram*>::iterator itr =
            queuedRequests.begin(); itr != queuedRequests.end(); itr++) {
        cancelAndDelete(itr->second);
    }
    for (auto & elem : checkTimers) {
        cancelAndDelete(elem);
    }
    queuedMessages.clear();
    queuedRequests.clear();
    knownMessages.clear();
    requestedMessages.clear();
}

void SPIN::initialize(int stage)
{
    NetworkProtocolBase::initialize(stage);
    if (stage == INITSTAGE_LOCAL) {
        //initialize seqence number to 0
        seqNum = 0;
        nbDataPacketsReceived = 0;
        nbDataPacketsSent = 0;
        nbDataPacketsForwarded = 0;
        nbHops = 0;
        headerLength = par("headerLength");
    } else if (stage == INITSTAGE_NETWORK_LAYER_3) {
        myNetwAddr = interfaceTable->getInterface(0)->getMacAddress();
    }
}

void SPIN::finish()
{
    recordScalar("nbDataPacketsReceived", nbDataPacketsReceived);
    recordScalar("nbDataPacketsSent", nbDataPacketsSent);
    recordScalar("nbDataPacketsForwarded", nbDataPacketsForwarded);
    if (nbDataPacketsReceived > 0) {
        recordScalar("meanNbHops", (double)nbHops / (double)nbDataPacketsReceived);
    }
    else {
        recordScalar("meanNbHops", 0);
    }
}

bool SPIN::handleNodeStart(IDoneCallback *doneCallback)
{
    return true;
}

bool SPIN::handleNodeShutdown(IDoneCallback *doneCallback)
{
    cancelTimers();
    return true;
}

void SPIN::handleNodeCrash()
{
    cancelTimers();
}

void SPIN::cancelTimers()
{
    for (std::map<MsgMetadata, SPINDatagram*>::iterator itr = queuedMessages.begin();
            itr != queuedMessages.end(); itr++) {
        cancelAndDelete(itr->second);
    }

    for (std::map<MsgMetadata, SPINDatagram*>::iterator itr =
            queuedRequests.begin(); itr != queuedRequests.end(); itr++) {
        cancelAndDelete(itr->second);
    }
    for (auto & elem : checkTimers) {
        cancelAndDelete(elem);
    }
    queuedMessages.clear();
    queuedRequests.clear();
    checkTimers.clear();
}

void SPIN::handleUpperPacket(cPacket *m)
{
    SPINDatagram *msg = encapsMsg(m, DATA);
    msg->setSeqNum(seqNum);
    seqNum++;

    advertiseData(msg);
    nbDataPacketsSent++;
}

void SPIN::handleLowerPacket(cPacket *m)
{
    SPINDatagram *msg = check_and_cast<SPINDatagram *>(m);
    int protocol = msg->getTransportProtocol();
    int type = msg->getMsgType();
    MsgMetadata metadata = createMsgMetadata(msg);

    switch (type) {
        case ADV:
            if (!knownMessages.count(metadata) && !queuedRequests.count(metadata)
                    && !requestedMessages.count(metadata)) {
                scheduleReq(metadata, msg->getAdvertiser());
                CheckMessage *checkTimer = new CheckMessage("Check Request");
                checkTimer->setKind(REQ_CHECK);
                checkTimer->setNodeAddress(metadata.nodeAddress);
                checkTimer->setSeqNum(metadata.seqNum);
                checkTimer->setAdvertiser(msg->getAdvertiser());
                checkTimers.push_back(checkTimer);
                scheduleAt(simTime() + uniform(30, 40) / 1000, checkTimer);
            }
            delete m;
            break;
        case REQ:
            if (msg->getAdvertiser() == myNetwAddr) {
                if (queuedMessages.count(metadata)) {
                    SPINDatagram *dataMsg = queuedMessages.find(metadata)->second;
                    dataMsg->removeControlInfo();
                    setDownControlInfo(dataMsg, MACAddress::BROADCAST_ADDRESS);
                    queuedMessages.erase(metadata);
                    sendDown(dataMsg);
                }
            } else {
                if (queuedRequests.count(metadata)) {
                    cancelAndDelete(queuedRequests.find(metadata)->second);
                    //delete queuedRequests.find(metadata)->second;
                    queuedRequests.erase(metadata);
                }
            }
            delete m;
            break;
        case DATA:
            if (!knownMessages.count(metadata)) {
                if (interfaceTable->isLocalAddress(msg->getDestAddr())) {
                    knownMessages.insert(metadata);
                    requestedMessages.erase(metadata);
                    sendUp(decapsMsg(msg), protocol);
                    nbDataPacketsReceived++;
                } else if (msg->getDestAddr().isBroadcast()) {
                    knownMessages.insert(metadata);
                    requestedMessages.erase(metadata);
                    sendUp(decapsMsg(msg->dup()), protocol);
                    nbDataPacketsReceived++;
                    advertiseData(msg);
                } else {
                    knownMessages.insert(metadata);
                    advertiseData(msg);
                }
            } else {
                delete m;
            }
            break;
        default:
            delete m;
            break;
    }

}

void SPIN::handleSelfMessage(cMessage *m)
{
    short kind = m->getKind();
    if (kind == REQ_TIMER) {
        SPINDatagram *msg = check_and_cast<SPINDatagram *>(m);
        MsgMetadata metadata = createMsgMetadata(msg->getSrcAddr(), msg->getSeqNum());
        queuedRequests.erase(metadata);
        requestedMessages.insert(metadata);
        sendDown(msg);
    } else if (kind == REQ_CHECK) {
        CheckMessage *msg = check_and_cast<CheckMessage *>(m);
        MsgMetadata metadata = createMsgMetadata(msg->getNodeAddress(), msg->getSeqNum());
        if(!knownMessages.count(metadata) && msg->getRetryNum() < 5) {
            msg->setRetryNum(msg->getRetryNum() + 1);
            scheduleReq(metadata, msg->getAdvertiser());
            scheduleAt(simTime() + uniform(30, 40) / 1000, msg);
        } else {
            checkTimers.remove(msg);
            delete msg;
        }
    } else {
        SPINDatagram *msg = check_and_cast<SPINDatagram *>(m);
        NetworkProtocolBase::handleSelfMessage(msg);
    }
}

cMessage *SPIN::decapsMsg(SPINDatagram *msg)
{
    SimpleNetworkProtocolControlInfo *controlInfo =
            new SimpleNetworkProtocolControlInfo();
    controlInfo->setSourceAddress(msg->getSrcAddr());
    controlInfo->setProtocol(msg->getTransportProtocol());
    cPacket *transportPacket = msg->decapsulate();
    transportPacket->setControlInfo(controlInfo);
    delete msg;
    return transportPacket;
}

SPINDatagram *SPIN::encapsMsg(cPacket *msg, int msgType)
{
    L3Address destNetwAddr;

    EV << "in encaps...\n";

    INetworkProtocolControlInfo *cInfo = check_and_cast_nullable<
            INetworkProtocolControlInfo *>(msg->removeControlInfo());
    SPINDatagram *pkt = new SPINDatagram(msg->getName(),
            msg->getKind());
    pkt->setBitLength(headerLength);

    if (msgType == REQ || msgType == ADV) {
        pkt->setSrcAddr(cInfo->getSourceAddress());
    } else {
        if (cInfo == nullptr) {
            EV
                      << "warning: Application layer did not specifiy a destination L3 address\n"
                      << "\tusing broadcast address instead\n";
            destNetwAddr = destNetwAddr.getAddressType()->getBroadcastAddress();
        } else {
            pkt->setTransportProtocol(cInfo->getTransportProtocol());
            destNetwAddr = cInfo->getDestinationAddress();
            EV << "CInfo removed, netw addr=" << destNetwAddr << endl;
            delete cInfo;
        }

        pkt->setDestAddr(destNetwAddr);
        pkt->setSrcAddr(myNetwAddr);
    }

    pkt->setMsgType(msgType);

    EV << " netw " << myNetwAddr << " sending packet" << endl;

    EV << "sendDown: nHop=L3BROADCAST -> message has to be broadcasted"
              << " -> set destMac=L2BROADCAST" << endl;

    setDownControlInfo(pkt, MACAddress::BROADCAST_ADDRESS);

    //encapsulate the application packet
    pkt->encapsulate(msg);
    EV << " pkt encapsulated\n";
    return pkt;
}

void SPIN::advertiseData(SPINDatagram *msg)
{
    MsgMetadata metadata = createMsgMetadata(msg);
    knownMessages.insert(metadata);
    queuedMessages.insert(std::make_pair(metadata, msg));
    // TODO schedule timer for message expiration
    cPacket *advPayload = new cPacket("ADV");
    advPayload->setByteLength(0);
    setDownExtraControlInfo(advPayload, msg->getSrcAddr());
    SPINDatagram *adv = encapsMsg(advPayload, ADV);
    adv->setSeqNum(msg->getSeqNum());
    adv->setAdvertiser(myNetwAddr);

    sendDown(adv);
}

void SPIN::scheduleReq(MsgMetadata metadata, L3Address advertiser)
{
    cPacket *reqPayload = new cPacket("REQ");
    reqPayload->setByteLength(0);
    setDownExtraControlInfo(reqPayload, metadata.nodeAddress);
    SPINDatagram *req = encapsMsg(reqPayload, REQ);
    req->setKind(REQ_TIMER);
    req->setAdvertiser(advertiser);
    req->setSeqNum(metadata.seqNum);
    queuedRequests.insert(std::make_pair(metadata, req));
    scheduleAt(simTime() + uniform(1, 15) / 1000, req);
}

SPIN::MsgMetadata SPIN::createMsgMetadata(L3Address nodeAddress, unsigned long seqNum)
{
    MsgMetadata metadata;
    metadata.nodeAddress = nodeAddress;
    metadata.seqNum = seqNum;
    return metadata;
}

SPIN::MsgMetadata SPIN::createMsgMetadata(SPINDatagram *msg)
{
    return createMsgMetadata(msg->getSrcAddr(), msg->getSeqNum());
}

cObject *SPIN::setDownControlInfo(cMessage *const pMsg, const MACAddress& pDestAddr)
{
    SimpleLinkLayerControlInfo *const cCtrlInfo = new SimpleLinkLayerControlInfo();
    cCtrlInfo->setDest(pDestAddr);
    pMsg->setControlInfo(cCtrlInfo);
    return cCtrlInfo;
}

void SPIN::setDownExtraControlInfo(cPacket *msg, L3Address srcAddress)
{
    IL3AddressType *addressType = srcAddress.getAddressType();
    INetworkProtocolControlInfo *controlInfo =
            addressType->createNetworkProtocolControlInfo();
    controlInfo->setSourceAddress(srcAddress);
    msg->setControlInfo(check_and_cast<cObject *>(controlInfo));
}
