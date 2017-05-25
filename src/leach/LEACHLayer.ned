//
// This file is part of wsn_routing
//
// Copyright (C) 2017 Mateusz Czarnecki <mateusz.czarnecki92@gmail.com>
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'LICENSE'
// for details on this and other legal matters.
//

package routing.leach;

import inet.networklayer.common.SimpleNetworkLayer;
import inet.networklayer.common.EchoProtocol;
import inet.networklayer.contract.INetworkLayer;


module LEACHLayer extends SimpleNetworkLayer like INetworkLayer
{
    @display("bgb=288,210");
    networkProtocolType = "LEACH";
}
