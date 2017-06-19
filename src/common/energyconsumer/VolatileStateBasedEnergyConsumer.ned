//
// This file is part of wsn_routing
//
// Copyright (C) 2017 Mateusz Czarnecki <mateusz.czarnecki92@gmail.com>
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'LICENSE'
// for details on this and other legal matters.
//

package routing.common.energyconsumer;

import inet.physicallayer.energyconsumer.StateBasedEnergyConsumer;

//
// This module provides a radio power consumer model. The power consumption is
// determined by the radio mode, the transmitter state and the receiver state
// using constant parameters.
//
simple VolatileStateBasedEnergyConsumer extends StateBasedEnergyConsumer
{
    parameters:
        double baseConsumption @unit(W) = default(0W);
        @class(VolatileStateBasedEnergyConsumer);
}
