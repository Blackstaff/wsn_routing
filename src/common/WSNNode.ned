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
import routing.common.misc.NodeMonitor;


//
// TODO auto-generated type
//
module WSNNode like INetworkNode
{
    @networkNode();
    @display("bgb=327,252;i=device/device");
    string nicType = default("Ieee802154NarrowbandNic");
    string energyStorageType = default("SimpleEnergyStorage");
    string networkLayerType = default("");
    string mobilityType = default("StationaryMobility");
    int numRadios = default(1);
    *.interfaceTableModule = absPath(".interfaceTable");
    nic.radio.energyConsumerType = default("VolatileStateBasedEnergyConsumer");
    nic.**.energySourceModule = default(absPath(".energyStorage"));

    //Energy Consumer parameters - based on ATmega256RFR2 (pages 555-566)
    double radioMicrocontrollerConsumption @unit(W) = default(12.3mW);
    nic.radio.energyConsumer.baseConsumption = radioMicrocontrollerConsumption;
    nic.radio.energyConsumer.offPowerConsumption = default(0mW);
    nic.radio.energyConsumer.sleepPowerConsumption = default(0.00225mW);
    nic.radio.energyConsumer.switchingPowerConsumption = default(radioMicrocontrollerConsumption);
    nic.radio.energyConsumer.receiverIdlePowerConsumption = default(18mW + radioMicrocontrollerConsumption);
    nic.radio.energyConsumer.receiverBusyPowerConsumption = default(18mW + radioMicrocontrollerConsumption);
    nic.radio.energyConsumer.receiverReceivingPowerConsumption = default(18mW + radioMicrocontrollerConsumption);
    nic.radio.energyConsumer.transmitterIdlePowerConsumption = default(17.1mW + radioMicrocontrollerConsumption);
    nic.radio.energyConsumer.transmitterTransmittingPowerConsumption = default(43.5mW + radioMicrocontrollerConsumption);

    //    switchingTimes parameter should be regarded as a 5x5 matrix:
    //
    //               OFF SLEEP RECEIVER TRANSMITTER TRANSCEIVER
    // OFF           0 170000 520000 520000 520000
    // SLEEP         0 0 350000 350000 350000
    // RECEIVER      1000 1000 0 1000 1000
    // TRANSMITTER   1000 1000 1000 0 1000
    // TRANSCEIVER   1000 1000 1000 1000 0
    //nic.radio.switchingTimes = default("ns 0 170000 520000 520000 520000 0 0 350000 350000 350000 1000 1000 0 1000 1000 1000 1000 1000 0 1000 1000 1000 1000 1000 0");

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
        energyStorage: <energyStorageType> like IEnergyStorage {
            @display("p=45,86");
        }
        mobility: <mobilityType> like IMobility {
            @display("p=45,23");
        }
        nodeMonitor: NodeMonitor {
            @display("p=290,23");
        }
    connections:
        networkLayer.ifOut++ --> lo0.upperLayerIn;
        lo0.upperLayerOut --> networkLayer.ifIn++;

        radioIn++ --> nic.radioIn;

        networkLayer.ifOut++ --> nic.upperLayerIn;
        nic.upperLayerOut --> networkLayer.ifIn++;
}
