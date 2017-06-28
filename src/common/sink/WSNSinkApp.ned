//
// This file is part of wsn_routing
//
// Copyright (C) 2017 Mateusz Czarnecki <mateusz.czarnecki92@gmail.com>
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'LICENSE'
// for details on this and other legal matters.
//

package routing.common.sink;

import routing.common.IWSNApp;

//
// TODO auto-generated module
//
simple WSNSinkApp like IWSNApp
{
    parameters:
        @display("i=block/sink");
    	int protocol;
    	string interfaceTableModule;
    	bool broadcastSinkAddress = default(true);
        @signal[rcvdPk](type=cPacket);
        @statistic[rcvdPk](title="packets received"; source=rcvdPk; record=count,"sum(packetBytes)","vector(count)"; interpolationmode=none);
        @statistic[endToEndDelay](title="end-to-end delay"; source="messageAge(rcvdPk)"; unit=s; record=histogram,vector; interpolationmode=none);
    gates:
        input appIn;
        output appOut @loose;
}
