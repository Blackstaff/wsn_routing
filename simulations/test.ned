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
        sensor0: WSNSensorNode {
            @display("p=235,86");
        }
        sink: WSNSinkNode {
            @display("p=235,176");
        }
        radioMedium: Ieee802154NarrowbandScalarRadioMedium {
            @display("p=53,51");
        }
        sensor1: WSNSensorNode {
            @display("p=139,258");
        }
        sensor2: WSNSensorNode {
            @display("p=515,458");
        }
        sensor3: WSNSensorNode {
            @display("p=280,226");
        }
        sensor4: WSNSensorNode {
            @display("p=367,291");
        }
        sensor5: WSNSensorNode {
            @display("p=433,345");
        }
        sensor6: WSNSensorNode {
            @display("p=482,398");
        }
        sensor7: WSNSensorNode {
            @display("p=330,259");
        }
    connections:
}
