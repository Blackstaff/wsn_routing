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

#ifndef __ROUTING_LEACH_H_
#define __ROUTING_LEACH_H_

#include <queue>
#include <vector>
#include <math.h>
#include <omnetpp.h>
#include <algorithm>

#include "inet/networklayer/contract/INetworkProtocol.h"
#include "inet/networklayer/base/NetworkProtocolBase.h"
#include "inet/networklayer/common/L3Address.h"
#include "inet/physicallayer/base/packetlevel/FlatRadioBase.h"

#include "LEACHPacket_m.h"

#define TIMER_MIN_SIZE 8
#define TIMER_MAX_SIZE 256

#include "castalia_files/CastaliaMessages.h"
#include "castalia_files/TimerServiceMessage_m.h"

using namespace inet;
using namespace physicallayer;
using namespace std;

enum LeachRoutingTimers {
        START_ROUND = 1,
        SEND_ADV = 2,
        JOIN_CH = 3,
        MAKE_TDMA = 4,
        START_SLOT = 5,
        END_SLOT = 6,
    };

struct CHInfo
{
    L3Address src;
    double rssi;
};

/**
 * TODO - Generated class
 */
class LEACH : public NetworkProtocolBase, public INetworkProtocol
{
  protected:

    /** @brief Network layer sequence number*/
    unsigned long seqNum = 0;

    /** @brief cached variable of my network address */
    L3Address myNetwAddr;

    L3Address sinkAddr;

    int headerLength = 0;

    int advPacketSize;
    int tdmaPacketSize;
    int aggrPacketSize;
    int joinPacketSize;

    double maxPower;
    double sensibility;
    J aggrConsumption;

    double slotLength;
    int clusterLength;
    double percentage;
    double probability;
    double roundLength;
    int roundNumber;
    int dataSN;

    int netBufferSize;

    bool isCH;
    bool isSink;
    bool isCt;
    bool endFormClus;

    FlatRadioBase *radio;

    queue<cPacket*> TXBuffer;

    vector<LEACHPacket*> bufferAggregate;
    vector<double> powers;
    map<double, double> powerConsumptions;
    queue<cPacket *> tempTXBuffer;
    vector<L3Address> clusterMembers;
    list<CHInfo> CHcandidates;

    long nbDataPacketsReceived = 0;
    long nbDataPacketsSent = 0;
    long nbDataPacketsForwarded = 0;

    static simsignal_t transmitterPowerChanged;

  public:
    LEACH() {}
    virtual ~LEACH();
    virtual void finish() override;

  protected:
    /** @brief Initialization of omnetpp.ini parameters*/
    virtual int numInitStages() const override {
        return NUM_INIT_STAGES;
    };

    virtual void initialize(int) override;

    virtual bool handleNodeStart(IDoneCallback *doneCallback) override;
    virtual bool handleNodeShutdown(IDoneCallback *doneCallback) override;
    virtual void handleNodeCrash() override;
    virtual void cancelTimers();

    virtual void handleSelfMessage(cMessage *msg) override;

    /** @brief Handle messages from upper layer */
    virtual void handleUpperPacket(cPacket *) override;

    /** @brief Handle messages from lower layer */
    virtual void handleLowerPacket(cPacket *) override;

    /** @brief Checks whether a message was already broadcasted*/
    //bool notBroadcasted(FloodDatagram *);

    virtual void unpackAndSendUp(LEACHPacket *msg);

    cMessage *decapsMsg(LEACHPacket *);

    LEACHPacket *encapsMsg(cPacket *, int msgType);

    /**
     * @brief Attaches a "control info" (NetwToMac) structure (object) to the message pMsg.
     *
     * This is most useful when passing packets between protocol layers
     * of a protocol stack, the control info will contain the destination MAC address.
     *
     * The "control info" object will be deleted when the message is deleted.
     * Only one "control info" structure can be attached (the second
     * setL3ToL2ControlInfo() call throws an error).
     *
     * @param pMsg      The message where the "control info" shall be attached.
     * @param pDestAddr The MAC address of the message receiver.
     */
    virtual cObject *setDownControlInfo(cMessage * const pMsg,
            const MACAddress& pDestAddr);

    virtual void processStartRound();
    virtual void selectCH();

    int bufferPacket(cPacket *packet);
    void processBufferedPacket();
    void sendAggregate();
    void setPowerLevel(double);
    void setStateSleep();
    void setStateRx();
    void levelTxPower(double);
    virtual void drawPower(J power);
    virtual void parseTransmitterPowers();

  /* TimerService */
  private:
    double timerDrift;
  protected:
    std::vector<TimerServiceMessage*> timerMessages;
    double sigmaCPUClockDrift;
    double cpuClockDrift;

    simtime_t getClock();
    void setTimerDrift(double new_drift);
    void setTimer(int index, simtime_t time);
    simtime_t getTimer(int index);
    void cancelTimer(int index);
    void handleTimerMessage(cMessage *);
    void timerFiredCallback(int index);
};

bool cmpRssi(CHInfo a, CHInfo b);

#endif
