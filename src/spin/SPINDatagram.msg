//
// This file is part of wsn_routing
//
// Copyright (C) 2017 Mateusz Czarnecki <mateusz.czarnecki92@gmail.com>
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'LICENSE'
// for details on this and other legal matters.
//
cplusplus {{
#include "inet/networklayer/base/NetworkDatagramBase_m.h"
}}

class inet::NetworkDatagramBase;
class noncobject inet::L3Address;

enum SPINMsgType {
    ADV = 0;
    REQ = 1;
    DATA = 2;
}

packet SPINDatagram extends inet::NetworkDatagramBase
{
    int msgType @enum(SPINMsgType);
    inet::L3Address advertiser;
    unsigned long seqNum = 0; // sequence number
    int           transportProtocol = -1;
}