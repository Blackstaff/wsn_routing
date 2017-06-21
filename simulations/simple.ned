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
import inet.environment.common.PhysicalEnvironment;
import inet.examples.adhoc.hostautoconf.Host;
import inet.examples.inet.routerperf.BurstHost;
import inet.linklayer.configurator.L2NetworkConfigurator;
import inet.networklayer.common.InterfaceTable;
import inet.networklayer.generic.GenericRoutingTable;
import inet.node.ethernet.EtherHost;
import inet.node.rtp.RTPHost;
import inet.physicallayer.ieee802154.packetlevel.Ieee802154NarrowbandScalarRadioMedium;
import routing.common.misc.NodeCounter;
import routing.common.sensor.WSNSensorApp;
import routing.common.sensor.WSNSensorNode;
import routing.common.sink.WSNSinkNode;
import inet.networklayer.probabilistic.ProbabilisticBroadcast;


network Simple
{
    parameters:
        @display("bgb=715,638;bgl=42");
        int numSensors;
    submodules:
        lifecycleController: LifecycleController {
            @display("p=54,101");
        }
        scenarioManager: ScenarioManager {
            @display("p=54,160");
        }
        sensor[numSensors]: WSNSensorNode {
        }
        sink: WSNSinkNode {
            @display("p=235,176");
        }
        radioMedium: Ieee802154NarrowbandScalarRadioMedium {
            @display("p=53,51");
        }
        physicalEnvironment: PhysicalEnvironment {
            @display("p=54,218");
        }
        nodeCounter: NodeCounter {
            @display("p=53.927998,274.776");
        }
    connections:
}
