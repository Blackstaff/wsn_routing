//
// This file is part of wsn_routing
//
// Copyright (C) 2017 Mateusz Czarnecki <mateusz.czarnecki92@gmail.com>
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'LICENSE'
// for details on this and other legal matters.
//

package routing.aleach;

import inet.networklayer.contract.INetworkLayer;
import routing.leach.LEACHLayer;


module ALEACHLayer extends LEACHLayer like INetworkLayer
{
    networkProtocolType = "ALEACH";
}
