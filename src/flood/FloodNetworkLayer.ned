//
// This file is part of wsn_routing
//
// Copyright (C) 2017 Mateusz Czarnecki <mateusz.czarnecki92@gmail.com>
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'LICENSE'
// for details on this and other legal matters.
//

package routing.flood;

import inet.networklayer.contract.INetworkLayer;
import inet.networklayer.common.SimpleNetworkLayer;


module FloodNetworkLayer extends SimpleNetworkLayer like INetworkLayer
{
    networkProtocolType = "Flood";
    @display("bgb=177,180");
    submodules:
}
