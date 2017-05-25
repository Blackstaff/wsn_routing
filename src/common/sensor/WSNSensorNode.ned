//
// This file is part of wsn_routing
//
// Copyright (C) 2017 Mateusz Czarnecki <mateusz.czarnecki92@gmail.com>
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'LICENSE'
// for details on this and other legal matters.
//

package routing.common.sensor;

import routing.common.WSNNode;

//
// TODO auto-generated type
//
module WSNSensorNode extends WSNNode
{
    submodules:
        app: WSNSensorApp {
            @display("p=147,31");
        }
    connections:
        networkLayer.transportOut++ --> app.appIn;
        networkLayer.transportIn++ <-- app.appOut;
}
