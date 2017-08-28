//
// This file is part of wsn_routing
//
// Modified work Copyright (C) 2017 Mateusz Czarnecki <mateusz.czarnecki92@gmail.com>
//
//****************************************************************************
//*  Original work Copyright (c) Federal University of Para, brazil - 2011   *
//*  Developed at the Research Group on Computer Network and Multimedia      *
//*  Communication (GERCOM)                                  *
//*  All rights reserved                                         *
//*                                                                          *
//*  Permission to use, copy, modify, and distribute this protocol and its   *
//*  documentation for any purpose, without fee, and without written         *
//*  agreement is hereby granted, provided that the above copyright notice,  *
//*  and the author appear in all copies of this protocol.                   *
//*                                                                          *
//*  Module:   LEACH Clustering Protocol for Castalia Simulator              *
//*  Version:  0.2                                                           *
//*  Author(s): Adonias Pires <adonias@ufpa.br>                              *
//*             Claudio Silva <claudio.silva@itec.ufpa.br>                   *
//****************************************************************************/
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'LICENSE'
// for details on this and other legal matters.
//

#include "inet/linklayer/common/SimpleLinkLayerControlInfo.h"
#include "inet/networklayer/common/SimpleNetworkProtocolControlInfo.h"
#include "inet/networklayer/contract/IL3AddressType.h"
#include "inet/common/INETMath.h"
#include "inet/common/ModuleAccess.h"

#include "LEACH.h"

using namespace omnetpp;
using namespace inet;

Define_Module(LEACH);

simsignal_t LEACH::transmitterPowerChanged = registerSignal("transmitterPowerChanged");

LEACH::~LEACH()
{
    while (!TXBuffer.empty()) {
        delete TXBuffer.front();
        TXBuffer.pop();
    }
    while (!tempTXBuffer.empty()) {
        delete tempTXBuffer.front();
        tempTXBuffer.pop();
    }
    for (std::vector<LEACHPacket *>::iterator itr = bufferAggregate.begin();
            itr != bufferAggregate.end(); itr++) {
        LEACHPacket* pkt = *itr;
        cancelAndDelete(pkt);
    }
    for (std::vector<TimerServiceMessage*>::iterator itr = timerMessages.begin();
            itr != timerMessages.end(); itr++) {
        TimerServiceMessage* msg = *itr;
        if (msg != NULL) {
            cancelAndDelete(msg);
        }
    }
    bufferAggregate.clear();
    timerMessages.clear();
    powers.clear();
    clusterMembers.clear();
    CHcandidates.clear();
}

void LEACH::initialize(int stage)
{
    NetworkProtocolBase::initialize (stage);
    if (stage == INITSTAGE_LOCAL) {
        //initialize seqence number to 0
        seqNum = 0;
        nbDataPacketsReceived = 0;
        nbDataPacketsSent = 0;
        nbDataPacketsForwarded = 0;
        percentage = par("percentage");
        roundLength = par("roundLength");
        isSink = par("isSink");
        slotLength = 0;
        advPacketSize = par("advPacketSize");
        joinPacketSize = par("joinPacketSize");
        tdmaPacketSize = par("tdmaPacketSize");
        aggrPacketSize = par("aggrPacketSize");
        headerLength = par("headerLength");
        netBufferSize = par("netBufferSize");
        aggrConsumption = J(par("aggrConsumption"));
        consumptionInterval = par("consumptionInterval");
        aggrCoefficient = par("aggrCoefficient");

        CHcandidates.clear();
        clusterMembers.clear();
        roundNumber = 0;
        probability = 0;
        isCH = false;
        endFormClus = false;
        isCt = false;

        parseTransmitterPowers();
        WATCH_VECTOR(powers);
        WATCH_MAP(powerConsumptions);

        sigmaCPUClockDrift = par("sigmaCPUClockDrift");
        //using the "0" rng generator of the ResourceManager module
        cpuClockDrift = normal(0, sigmaCPUClockDrift);
        /* Crop any values beyond +/- 3 sigmas. Some protocols (e.g., MAC) rely on
         * bounded cpuClockDrift. Although the bounds are conservative (usually 3sigmas),
         * if you instantiate thousands of nodes (in multiple runs) we will get a
         * couple of nodes that will be beyond this bound. Limiting/Croping the drift
         * is actually realistic, since usually there is some kind of quality
         * control on quartz crystals or the boards that use them (sensor node)
         */
        if (cpuClockDrift > 3 * sigmaCPUClockDrift)
            cpuClockDrift = 3 * sigmaCPUClockDrift;
        if (cpuClockDrift < -3 * sigmaCPUClockDrift)
            cpuClockDrift = -3 * sigmaCPUClockDrift;
        setTimerDrift(1.0f + cpuClockDrift);

        if(!isSink) setTimer(START_ROUND, 0);
        radio = check_and_cast<FlatRadioBase *>(getModuleFromPar<cModule>(par("radioModule"), this));
        sensibility = math::mW2dBm(radio->getReceiver()->getMinReceptionPower().get() * 1000) + 10;
        WATCH(sensibility);

        energyConsumer = check_and_cast<SimpleEnergyConsumer *>(getModuleFromPar<cModule>(par("energyConsumerModule"), this));
    } else if (stage == INITSTAGE_NETWORK_LAYER_3) {
        myNetwAddr = interfaceTable->getInterface(0)->getMacAddress();
        sinkAddr = *(new L3Address(par("sinkAddr").stringValue()));
    }
}

void LEACH::finish()
{
    recordScalar("nbDataPacketsReceived", nbDataPacketsReceived);
    recordScalar("nbDataPacketsSent", nbDataPacketsSent);
    recordScalar("nbDataPacketsForwarded", nbDataPacketsForwarded);
}

void LEACH::parseTransmitterPowers()
{
    const char *powersStr = par("powers");
    const char *consumptionsStr = par("powerConsumptions");

    powers = cStringTokenizer(powersStr).asDoubleVector();
    std::vector<double> consumptions = cStringTokenizer(consumptionsStr).asDoubleVector();

    for (uint i = 0; i < powers.size(); i++) {
        powerConsumptions[powers[i]] = consumptions[i];
    }
    sort(powers.begin(), powers.end());
    maxPower = powers.back();
}

bool LEACH::handleNodeStart(IDoneCallback *doneCallback)
{
    if(!isSink) setTimer(START_ROUND, 0);
    return true;
}

bool LEACH::handleNodeShutdown(IDoneCallback *doneCallback)
{
    cancelTimers();
    while (!TXBuffer.empty()) {
        delete TXBuffer.front();
        TXBuffer.pop();
    }
    while (!tempTXBuffer.empty()) {
        delete tempTXBuffer.front();
        tempTXBuffer.pop();
    }
    for (std::vector<LEACHPacket *>::iterator itr = bufferAggregate.begin();
            itr != bufferAggregate.end(); itr++) {
        LEACHPacket* pkt = *itr;
        cancelAndDelete(pkt);
    }
    bufferAggregate.clear();
    return true;
}

void LEACH::handleNodeCrash()
{
    cancelTimers();
}

void LEACH::cancelTimers()
{
    for (std::vector<TimerServiceMessage*>::iterator itr = timerMessages.begin();
            itr != timerMessages.end(); itr++) {
        TimerServiceMessage* msg = *itr;
        if (msg != NULL) {
            cancelAndDelete(msg);
        }
    }
    timerMessages.clear();
}

void LEACH::handleSelfMessage(cMessage *msg)
{
    int msgKind = msg->getKind();

    if (msgKind == TIMER_SERVICE) {
        handleTimerMessage(msg);
    } else {
        NetworkProtocolBase::handleMessage(msg);
    }
}

void LEACH::handleUpperPacket(cPacket *m)
{
    if (!isSink) {
        LEACHPacket *netPacket = encapsMsg(m, LEACH_DATA_PACKET);
//        LEACHPacket *netPacket = new LEACHPacket(m->getName(), m->getKind());
//        netPacket->setByteLength(headerLength);
//        netPacket->setType(LEACH_DATA_PACKET);
//        netPacket->setSrcAddr(myNetwAddr);
//        netPacket->setSeqNum(seqNum++);
//        netPacket->encapsulate(m);
        if (!isCH && endFormClus) {
            CHInfo info = *CHcandidates.begin();
            netPacket->setDestAddr(info.src);
            bufferPacket(netPacket);
        } else if (!isCH && !endFormClus) {
            tempTXBuffer.push(netPacket);
        } else if (isCH) {
            bufferAggregate.push_back(netPacket);
        }
    }
}

void LEACH::handleLowerPacket(cPacket *m)
{
    LEACHPacket *msg = check_and_cast<LEACHPacket *>(m);
    SimpleLinkLayerControlInfo *ctrlInfo = check_and_cast<SimpleLinkLayerControlInfo *>(msg->removeControlInfo());
    L3Address dest;
    double rssi = math::mW2dBm(ctrlInfo->getRssi() * 1000);
    switch (msg->getType()) {

        case LEACH_DATA_PACKET:
            dest = msg->getDestAddr();
            if (isCH && interfaceTable->isLocalAddress(dest)) {
                EV << "Node " << this->getName() << " Aggregate Data Frame \n";
                bufferAggregate.push_back(msg);
            } else if (interfaceTable->isLocalAddress(dest) && isSink) {
                EV << "Node " << this->getName() << " Processing Data Packet \n";
                unpackAndSendUp(msg);
            } else {
                delete msg;
            }
            break;

        case LEACH_ADV_PACKET:
            if (!isCH && !isSink) {
                EV << "Node " << this->getName() << " Received an Advertisement Message of node " << msg->getSrcAddr() << " with RSSI: " << rssi << "dBm\n";
                CHInfo rec;
                rec.src = msg->getSrcAddr();
                rec.rssi = rssi;
                CHcandidates.push_front(rec);
            }
            delete msg;
            break;

        case LEACH_JOIN_PACKET:
            dest = msg->getDestAddr();
            if (isCH && interfaceTable->isLocalAddress(dest)) {
                EV << "Node " << this->getName() << " Received a Join Request. Adding to clusterMembers\n";
                clusterMembers.push_back(msg->getSrcAddr());
            }
            delete msg;
            break;

        case LEACH_TDMA_PACKET:
            if (!isCH && !isSink) {
                clusterLength = msg->getScheduleArraySize();
                slotLength = roundLength*0.9 / clusterLength;
                for (int i = 0; i < clusterLength; i++) {
                    if (msg->getSchedule(i) == myNetwAddr) {
                        setStateSleep();
                        setTimer(START_SLOT, i * slotLength);
                        EV << "Node " << this->getName() << " Received TDMA pkt, I am: "<< i << "th \n";
                        break;
                    }
                }
            }
            delete msg;
            break;
    }
}

void LEACH::timerFiredCallback(int index) {
    switch (index) {

    case START_ROUND: {
        if (isCH) {
            sendAggregate();
            processBufferedPacket();
            //trace() << "Node " << self << " Sent Pkt Aggr"  << "\n";
        }
        processStartRound();
        break;
    }
    case SEND_ADV: {
        LEACHPacket *crtlPkt = new LEACHPacket(
                "ClusterHead Announcement Packet", NETWORK_LAYER_PACKET);
        crtlPkt->setByteLength(advPacketSize);
        crtlPkt->setType(LEACH_ADV_PACKET);
        crtlPkt->setSrcAddr(myNetwAddr);
        crtlPkt->setDestAddr(myNetwAddr.getAddressType()->getBroadcastAddress());
        setDownControlInfo(crtlPkt, MACAddress::BROADCAST_ADDRESS);
        sendDown(crtlPkt);
        //trace() << "Node " << self << " Sent Beacon";
        break;
    }
    case JOIN_CH: {
        if (CHcandidates.size() != 0) {
            CHcandidates.sort(cmpRssi);
            LEACHPacket *crtlPkt = new LEACHPacket("ClusterMember Join Packet");
            crtlPkt->setType(LEACH_JOIN_PACKET);
            crtlPkt->setByteLength(joinPacketSize);
            crtlPkt->setSrcAddr(myNetwAddr);
            CHInfo info = *CHcandidates.begin();
            crtlPkt->setDestAddr(info.src);
            setDownControlInfo(crtlPkt, MACAddress::BROADCAST_ADDRESS);
            sendDown(crtlPkt);
            endFormClus = true;
            //trace() << "Node " << self << " Sent Join Request to " << dst;
        }
        break;
    }
    case MAKE_TDMA: {
        if (clusterMembers.size() != 0) {
            LEACHPacket *crtlPkt = new LEACHPacket("ClusterHead TDMA Packet");
            crtlPkt->setByteLength(tdmaPacketSize);
            crtlPkt->setType(LEACH_TDMA_PACKET);
            crtlPkt->setSrcAddr(myNetwAddr);
            crtlPkt->setDestAddr(myNetwAddr.getAddressType()->getBroadcastAddress());
            clusterLength = clusterMembers.size();
            crtlPkt->setScheduleArraySize(clusterMembers.size());
            for (int i = 0; i < clusterLength; i++)
                crtlPkt->setSchedule(i, clusterMembers[i]);
            setDownControlInfo(crtlPkt, MACAddress::BROADCAST_ADDRESS);
            sendDown(crtlPkt);
            //trace() << "Node " << self << " Sent TDMA pkt";
        }
        break;
    }
    case START_SLOT: {
        if (!isCH) {
            CHInfo info = *CHcandidates.begin();
            double power = maxPower - ((info.rssi) - (sensibility));
            levelTxPower(power);
            //trace() << "Node " << self << " Sent Data Packet"  << "\n";
            processBufferedPacket();
            setTimer(END_SLOT, slotLength);
        }
        break;
    }
    case END_SLOT: {
        //trace() << "Node " << self << " Sleept"  << "\n";
        if (!isSink && !isCH)
            setStateSleep();
        break;
    }
    }
}

void LEACH::processStartRound()
{
    setStateRx();
    setPowerLevel(maxPower);
    endFormClus = false;
    CHcandidates.clear();
    clusterMembers.clear();
    if (getTimer(START_SLOT) != 0) {
        cancelTimer(START_SLOT);
    }

    selectCH();

    double setupTime = roundLength * 0.1;
    double timer = uniform(0, setupTime * 0.1);
    if (isCH) {
        setTimer(SEND_ADV, (timer));
        setTimer(MAKE_TDMA, setupTime*0.9 + timer);
    } else {
        setTimer(JOIN_CH, (setupTime*0.45 + timer));
    }

    roundNumber++;
    setTimer(START_ROUND, roundLength);
}

void LEACH::selectCH()
{
    if (roundNumber >= 1 / percentage) {
        roundNumber = 0;
        isCt = false;
        isCH = false;
    }

    double randomNumber = uniform(0, 1);
    if (isCH) {
        isCH = false;
        isCt = true;
    }
    if (isCt) {
        probability = 0;
    } else {
        if (roundNumber >= (1 / percentage - 1)) {
            probability = 1;
        } else {
            probability =
                    percentage
                    / (1
                            - percentage
                            * (roundNumber
                                    % (int) (1 / percentage)));
        }
    }
    if (randomNumber < probability) {
        isCH = true;
    }
}

void LEACH::sendAggregate() {
    int aggrNum = bufferAggregate.size();
    if (aggrNum != 0) {
        int dataPacketSize = bufferAggregate.back()->getBitLength();
        double bitsLength = aggrNum * dataPacketSize;
        J energyBit = aggrConsumption * bitsLength;
        drawPower(energyBit);
        LEACHPacket *aggrPacket = new LEACHPacket("ClusterHead Aggregated Packet");
        aggrPacket->setByteLength(aggrPacketSize);
        aggrPacket->setType(LEACH_DATA_PACKET);
        aggrPacket->setSrcAddr(myNetwAddr);
        aggrPacket->setDestAddr(sinkAddr);
        aggrPacket->setTransportProtocol(bufferAggregate.back()->getTransportProtocol());

        cPacket *sumPacket = new cPacket("Aggregated packet contents");

        while(!bufferAggregate.empty()) {
            LEACHPacket *packet = bufferAggregate.back();
            int packetLength = packet->getByteLength();
            sumPacket->setByteLength(sumPacket->getByteLength() + packetLength);
            bufferAggregate.pop_back();
            cancelAndDelete(packet);
        }

        sumPacket->setByteLength((sumPacket->getByteLength() / aggrNum) +
          aggrCoefficient * (sumPacket->getByteLength() - (sumPacket->getByteLength() / aggrNum)));

        aggrPacket->encapsulate(sumPacket);
        aggrPacket->setAggrNum(aggrNum);

        bufferPacket(aggrPacket);
        //bufferAggregate.clear();
    }
}

int LEACH::bufferPacket(cPacket *packet)
{
    if ((int)TXBuffer.size() >= netBufferSize) {
        bubble("Buffer overflow");
        cancelAndDelete(packet);
        return 0;
    } else {
        TXBuffer.push(packet);
        EV << "Packet buffered from application layer, buffer state: " <<
                TXBuffer.size() << "/" << netBufferSize;
        return 1;
    }
}

void LEACH::processBufferedPacket() {
    while (!tempTXBuffer.empty()) {
        cPacket *pkt = tempTXBuffer.front();
        LEACHPacket *netPacket = dynamic_cast<LEACHPacket*>(pkt);
        CHInfo info = *CHcandidates.begin();
        netPacket->setDestAddr(info.src);
        bufferPacket(netPacket);
        tempTXBuffer.pop();
    }

    while (!TXBuffer.empty()) {
        cPacket *pkt = TXBuffer.front();
        setDownControlInfo(pkt, MACAddress::BROADCAST_ADDRESS);
        sendDown(pkt);
        TXBuffer.pop();
    }
}

void LEACH::unpackAndSendUp(LEACHPacket *msg)
{
    int protocol = msg->getTransportProtocol();
    int aggrNum = msg->getAggrNum();
    if (aggrNum > 1) {
        cPacket *transportPacket = check_and_cast<cPacket *>(decapsMsg(msg));
        int byteLength = transportPacket->getByteLength() / aggrNum;
        transportPacket->setByteLength(byteLength);
        for (int i = 0; i < aggrNum; i++) {
            sendUp(transportPacket->dup(), protocol);
        }
        delete transportPacket;
    } else {
        sendUp(decapsMsg(msg), protocol);
    }
}

LEACHPacket *LEACH::encapsMsg(cPacket *msg, int msgType)
{
    LEACHPacket *netPacket = new LEACHPacket(msg->getName(), msg->getKind());
    int byteLength = 0;
    switch (msgType) {
        case LEACH_DATA_PACKET:
            byteLength = headerLength;
            break;
        default:
            break;
    }
    netPacket->setByteLength(byteLength);
    netPacket->setType(msgType);
    netPacket->setSrcAddr(myNetwAddr);
    netPacket->setSeqNum(seqNum++);

    INetworkProtocolControlInfo *cInfo = check_and_cast_nullable<
                INetworkProtocolControlInfo *>(msg->removeControlInfo());

    netPacket->setTransportProtocol(cInfo->getTransportProtocol());

    netPacket->encapsulate(msg);
    EV << " pkt encapsulated\n";
    return netPacket;
}

cMessage *LEACH::decapsMsg(LEACHPacket *msg)
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

void LEACH::setStateRx() {
    EV << "setStateRx\n";
    if (radio->getRadioMode() != IRadio::RADIO_MODE_RECEIVER) {
        radio->setRadioMode(IRadio::RADIO_MODE_RECEIVER);
    }
}

void LEACH::setPowerLevel(double powerLevel) {
    EV << "Set transmitter power level to" << powerLevel << "dBm\n";
    mW power = mW(math::dBm2mW(powerLevel));
    radio->setPower(power);
    emit(transmitterPowerChanged, powerConsumptions[powerLevel]);
    //send(createRadioCommand(SET_TX_OUTPUT, powerLevel), "toMacModule");
}

void LEACH::setStateSleep() {
    EV << "setStateSleep\n";
    radio->setRadioMode(IRadio::RADIO_MODE_SLEEP);
}

void LEACH::levelTxPower(double linkBudget) {
    vector<double>::iterator constIterator;
    double powerLevel = maxPower;
    for (constIterator = powers.begin(); constIterator != powers.end();
            constIterator++) {
        if (*constIterator > (linkBudget)) {
            powerLevel = *constIterator;
            break;
        }
    }
    setPowerLevel(powerLevel);
}

void LEACH::drawPower(J energy)
{
    W power = W(energy.get() / consumptionInterval);
    energyConsumer->setPowerConsumption(power);
}

cObject *LEACH::setDownControlInfo(cMessage *const pMsg, const MACAddress& pDestAddr)
{
    SimpleLinkLayerControlInfo *const cCtrlInfo = new SimpleLinkLayerControlInfo();
    cCtrlInfo->setDest(pDestAddr);
    pMsg->setControlInfo(cCtrlInfo);
    return cCtrlInfo;
}

bool cmpRssi(CHInfo a, CHInfo b) {
    return (a.rssi > b.rssi);
}

/********************* TimerService *****************************/
void LEACH::setTimerDrift(double new_drift)
{
    timerDrift = new_drift;
}

void LEACH::cancelTimer(int timerIndex)
{
    if (timerIndex < 0)
        throw cRuntimeError("cancelTimer(): timerIndex=%i negative index is not allowed",timerIndex);
    if (timerIndex >= TIMER_MAX_SIZE)
        throw cRuntimeError("cancelTimer(): timerIndex=%i is too large",timerIndex);
    if (timerIndex >= timerMessages.size())
        return;
    TimerServiceMessage* tmp = timerMessages[timerIndex];
    if (tmp != NULL && tmp->isScheduled())
        cancelAndDelete(tmp);
    timerMessages[timerIndex] = NULL;
}

void LEACH::setTimer(int timerIndex, simtime_t time)
{
    if (timerIndex < 0)
        throw cRuntimeError("setTimer(): timerIndex=%i negative index is not allowed",timerIndex);
    if (timerIndex >= TIMER_MAX_SIZE)
        throw cRuntimeError("setTimer(): timerIndex=%i is too large",timerIndex);
    cancelTimer(timerIndex);
    if (timerIndex >= timerMessages.size()) {
        int newSize = timerMessages.size() + TIMER_MIN_SIZE;
        if (newSize > TIMER_MAX_SIZE)
            newSize = TIMER_MAX_SIZE;
        else if (timerIndex >= newSize)
            newSize = timerIndex + 1;
        timerMessages.resize(newSize,NULL);
    }
    timerMessages[timerIndex] = new TimerServiceMessage("Timer message", TIMER_SERVICE);
    timerMessages[timerIndex]->setTimerIndex(timerIndex);
    scheduleAt(simTime() + timerDrift * time, timerMessages[timerIndex]);
}

void LEACH::handleTimerMessage(cMessage * msg)
{
    int msgKind = (int)msg->getKind();
    if (msgKind == TIMER_SERVICE) {
        TimerServiceMessage *timerMsg = check_and_cast<TimerServiceMessage*>(msg);
        int timerIndex = timerMsg->getTimerIndex();
        if (timerIndex < 0 || timerIndex >= timerMessages.size()) {
            delete timerMsg;
            return;
        }
        if (timerMessages[timerIndex] != NULL) {
            timerMessages[timerIndex] = NULL;
            timerFiredCallback(timerIndex);
        }
        delete timerMsg;
    }
}

simtime_t LEACH::getTimer(int timerIndex)
{
    if (timerIndex < 0)
        throw cRuntimeError("getTimer(): timerIndex=%i negative index is not allowed",timerIndex);
    if (timerIndex >= TIMER_MAX_SIZE)
        throw cRuntimeError("getTimer(): timerIndex=%i is too large",timerIndex);
    if (timerIndex >= timerMessages.size())
        return -1;
    if (timerMessages[timerIndex] == NULL)
        return -1;
    else
        return timerMessages[timerIndex]->getArrivalTime() * timerDrift;
}

simtime_t LEACH::getClock()
{
    return simTime() * timerDrift;
}
