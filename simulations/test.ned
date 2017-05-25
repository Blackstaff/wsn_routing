//
// This file is part of wsn_routing
//
// Copyright (C) 2017 Mateusz Czarnecki <mateusz.czarnecki92@gmail.com>
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'LICENSE'
// for details on this and other legal matters.
//

package routing.simulations;

import inet.applications.generic.IPvXTrafGen;
import inet.applications.generic.IPvXTrafSink;
import inet.common.lifecycle.LifecycleController;
import inet.common.queue.Sink;
import inet.common.scenario.ScenarioManager;
import inet.examples.adhoc.hostautoconf.Host;
import inet.examples.inet.routerperf.BurstHost;
import inet.linklayer.configurator.L2NetworkConfigurator;
import inet.networklayer.common.InterfaceTable;
import inet.networklayer.generic.GenericRoutingTable;
import inet.node.ethernet.EtherHost;
import inet.node.rtp.RTPHost;
import inet.physicallayer.ieee802154.packetlevel.Ieee802154NarrowbandScalarRadioMedium;
import routing.common.sensor.WSNSensorApp;
import routing.common.sensor.WSNSensorNode;
import routing.common.sink.WSNSinkNode;
import inet.networklayer.probabilistic.ProbabilisticBroadcast;


network Test
{
    @display("bgb=583,510");
    submodules:
        lifecycleController: LifecycleController {
            @display("p=54,101");
        }
        scenarioManager: ScenarioManager {
            @display("p=54,160");
        }
        wsnSensorNode: WSNSensorNode {
            @display("p=235,86");
        }
        wsnSinkNode: WSNSinkNode {
            @display("p=235,176");
        }
        radioMedium: Ieee802154NarrowbandScalarRadioMedium {
            @display("p=53,51");
        }
        wsnSensorNode1: WSNSensorNode {
            @display("p=139,258");
        }
        wsnSensorNode2: WSNSensorNode {
            @display("p=515,458");
        }
        wsnSensorNode3: WSNSensorNode {
            @display("p=280,226");
        }
        wsnSensorNode4: WSNSensorNode {
            @display("p=367,291");
        }
        wsnSensorNode5: WSNSensorNode {
            @display("p=433,345");
        }
        wsnSensorNode6: WSNSensorNode {
            @display("p=482,398");
        }
        wsnSensorNode7: WSNSensorNode {
            @display("p=330,259");
        }
    connections:
}
