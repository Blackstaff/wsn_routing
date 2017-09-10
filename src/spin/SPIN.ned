//
// This file is part of wsn_routing
//
// Copyright (C) 2017 Mateusz Czarnecki <mateusz.czarnecki92@gmail.com>
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'LICENSE'
// for details on this and other legal matters.
//

package routing.spin;

import inet.networklayer.contract.INetworkProtocol;
import inet.networklayer.base.NetworkProtocolBase;


//
// TODO auto-generated module
//
simple SPIN extends NetworkProtocolBase like INetworkProtocol
{
    int headerLength @unit(bit) = default(100b); // length of the network packet header (in bits)
    string energyStorageModule = default("^.^.energyStorage");

    @class(SPIN);
}
