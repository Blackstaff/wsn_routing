//
// This file is part of wsn_routing
//
// Copyright (C) 2017 Mateusz Czarnecki <mateusz.czarnecki92@gmail.com>
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'LICENSE'
// for details on this and other legal matters.
//

package routing.common.misc;

simple NodeMonitor
{
    parameters:
        @display("i=block/cogwheel;is=s");
        string energyStorageModule = default(absPath("^.energyStorage"));
        double timerInterval @unit(s) = default(100ms);
        @signal[storedEnergyChanged](type=double);
        @statistic[storedEnergy](title="Current energy stored in node"; source=storedEnergyChanged; record=vector; interpolationmode=sample-hold);
}
