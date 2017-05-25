//
// This file is part of wsn_routing
//
// Copyright (C) 2017 Mateusz Czarnecki <mateusz.czarnecki92@gmail.com>
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'LICENSE'
// for details on this and other legal matters.
//

#ifndef __ROUTING_SPIN_H_
#define __ROUTING_SPIN_H_

#include <omnetpp.h>
#include <map>

#include "inet/networklayer/contract/INetworkProtocol.h"
#include "inet/networklayer/base/NetworkProtocolBase.h"
#include "inet/networklayer/common/L3Address.h"

#include "SPINDatagram_m.h"
#include "CheckMessage_m.h"

using namespace omnetpp;
using namespace inet;

/**
 * TODO - Generated class
 */
class SPIN : public NetworkProtocolBase, public INetworkProtocol
{

  public:
    typedef struct {
        L3Address nodeAddress;
        unsigned long seqNum;
    } MsgMetadata, *pMsgMetadata;

    struct MsgMetadataCompare {
        bool operator()(const MsgMetadata& lhs, const MsgMetadata& rhs) const {
            if (lhs.nodeAddress == rhs.nodeAddress) {
                return lhs.seqNum < rhs.seqNum;
            } else {
                return lhs.nodeAddress < rhs.nodeAddress;
            }
        }
    };

  protected:
    /** @brief Network layer sequence number*/
    unsigned long seqNum = 0;

    /** @brief cached variable of my network address */
    L3Address myNetwAddr;

    /** @brief Length of the header*/
    int headerLength = 0;

    /** @brief Default time-to-live (ttl) used for this module*/
    int defaultTtl = 0;

    std::map<MsgMetadata, SPINDatagram*, MsgMetadataCompare> queuedMessages;
    std::map<MsgMetadata, SPINDatagram*, MsgMetadataCompare> queuedRequests;
    std::set<MsgMetadata, MsgMetadataCompare> knownMessages;
    std::set<MsgMetadata, MsgMetadataCompare> requestedMessages;

    long nbDataPacketsReceived = 0;
    long nbDataPacketsSent = 0;
    long nbDataPacketsForwarded = 0;
    long nbHops = 0;

  public:
    SPIN() {}
    virtual ~SPIN();
    virtual void finish() override;

  protected:
    /** @brief Initialization of omnetpp.ini parameters*/
    virtual int numInitStages() const override {
        return NUM_INIT_STAGES;
    }

    virtual void initialize(int) override;

    virtual void handleSelfMessage(cMessage *msg) override;

    /** @brief Handle messages from upper layer */
    virtual void handleUpperPacket(cPacket *) override;

    /** @brief Handle messages from lower layer */
    virtual void handleLowerPacket(cPacket *) override;

    /** @brief Checks whether a message was already broadcasted*/
    //bool notBroadcasted(FloodDatagram *);

    cMessage *decapsMsg(SPINDatagram *);

    SPINDatagram *encapsMsg(cPacket *, int msgType);

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

    // Set control info for "ADV" and "REQ" messages
    virtual void setDownExtraControlInfo(cPacket *msg, L3Address srcAddress);

    void advertiseData(SPINDatagram *msg);
    void scheduleReq(MsgMetadata metadata, L3Address advertiser);
  private:
    MsgMetadata createMsgMetadata(L3Address nodeAddress, unsigned long seqNum);
    MsgMetadata createMsgMetadata(SPINDatagram *msg);
};

#endif
