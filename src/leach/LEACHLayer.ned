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
import routing.common.energyconsumer.SimpleEnergyConsumer;


module LEACHLayer extends SimpleNetworkLayer like INetworkLayer
{
    @display("bgb=288,210");
    networkProtocolType = default("LEACH");
    np.energyConsumerModule = default(absPath(".simpleConsumer"));
    **.consumptionInterval = default(1ns);
    submodules:
        simpleConsumer: SimpleEnergyConsumer {
            @display("p=237.49333,32.12");
            energySourceModule = default(absPath("^.^.energyStorage"));
        }
}
