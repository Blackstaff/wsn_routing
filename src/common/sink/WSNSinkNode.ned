//
// This file is part of wsn_routing
//
// Copyright (C) 2017 Mateusz Czarnecki <mateusz.czarnecki92@gmail.com>
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'LICENSE'
// for details on this and other legal matters.
//

package routing.common.sink;

import inet.common.queue.Sink;

import routing.common.WSNNode;

//
// TODO auto-generated type
//
module WSNSinkNode extends WSNNode
{
    @display("i=old/bwsink");
    **.isSink = default(true);
    submodules:
        sink: WSNSinkApp {
            @display("p=147,40");
        }
    connections:
        networkLayer.transportOut++ --> sink.appIn;
        networkLayer.transportIn++ <-- sink.appOut;
}
