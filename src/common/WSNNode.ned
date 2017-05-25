//
// This file is part of wsn_routing
//
// Copyright (C) 2017 Mateusz Czarnecki <mateusz.czarnecki92@gmail.com>
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'LICENSE'
// for details on this and other legal matters.
//

package routing.common;

import inet.common.lifecycle.NodeStatus;
import inet.linklayer.configurator.L2NodeConfigurator;
import inet.linklayer.contract.IWirelessNic;
import inet.linklayer.contract.INic;
import inet.linklayer.loopback.LoopbackInterface;
import inet.mobility.contract.IMobility;
import inet.networklayer.common.InterfaceTable;
import inet.networklayer.contract.INetworkLayer;
import inet.node.inet.INetworkNode;
import inet.node.inet.NodeBase;
import inet.power.contract.IEnergyStorage;


//
// TODO auto-generated type
//
module WSNNode like INetworkNode
{
    @networkNode();
    @display("bgb=327,252;i=device/device");
    string nicType = default("Ieee802154NarrowbandNic");
    string batteryType = default("IdealEnergyStorage");
    string networkLayerType = default("");
    string mobilityType = default("StationaryMobility");
    int numRadios = default(1);
    *.interfaceTableModule = absPath(".interfaceTable");
    gates:
        input radioIn[numRadios] @directIn;
        inout pppg[] @labels(PPPFrame-conn) @loose;
        inout ethg[] @labels(EtherFrame-conn) @loose;
    submodules:
        interfaceTable: InterfaceTable {
            @display("p=45,161");
        }
        status: NodeStatus {
            @display("p=39,220");
        }
        lo0: LoopbackInterface {
            @display("p=259,193");
        }
        nic: <nicType> like IWirelessNic {
            @display("p=147,193");
        }
        networkLayer: <networkLayerType> like INetworkLayer {
            @display("p=147,111");
        }
        battery: <batteryType> like IEnergyStorage {
            @display("p=45,86");
        }
        mobility: <mobilityType> like IMobility {
            @display("p=45,23");
        }
    connections:
        networkLayer.ifOut++ --> lo0.upperLayerIn;
        lo0.upperLayerOut --> networkLayer.ifIn++;

        radioIn++ --> nic.radioIn;

        networkLayer.ifOut++ --> nic.upperLayerIn;
        nic.upperLayerOut --> networkLayer.ifIn++;
}
