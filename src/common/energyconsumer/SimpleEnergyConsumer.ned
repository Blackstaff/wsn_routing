//
// This file is part of wsn_routing
//
// Copyright (C) 2017 Mateusz Czarnecki <mateusz.czarnecki92@gmail.com>
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'LICENSE'
// for details on this and other legal matters.
//

package routing.common.energyconsumer;

import inet.power.base.EnergyConsumerBase;

simple SimpleEnergyConsumer extends EnergyConsumerBase
{
    parameters:
        string energySourceModule = default("^.energyStorage");
        volatile double consumptionInterval @unit(s) = default(1ns);
        @class(SimpleEnergyConsumer);
}
