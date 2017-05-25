//
// This file is part of wsn_routing
//
// Copyright (C) 2017 Mateusz Czarnecki <mateusz.czarnecki92@gmail.com>
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'LICENSE'
// for details on this and other legal matters.
//
cplusplus {{
#include "inet/networklayer/common/L3Address.h"
}}

class SPIN;
struct SPIN::MsgMetadata;
class noncobject inet::L3Address;

message CheckMessage
{
    inet::L3Address nodeAddress;
    unsigned long seqNum;
    inet::L3Address advertiser;
    int retryNum = 0;
}