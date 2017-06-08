//
// This file is part of wsn_routing
//
// Copyright (C) 2017 Mateusz Czarnecki <mateusz.czarnecki92@gmail.com>
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'LICENSE'
// for details on this and other legal matters.
//

package routing.common.misc;

simple NodeCounter
{
    parameters:
        @display("i=block/cogwheel;is=s");
        int numSensors;
        @signal[sinkDown](type=simtime_t);
        @signal[aliveNodesChanged];
        @statistic[aliveNodes](title="Number of alive nodes"; source=aliveNodesChanged; record=vector; interpolationmode=sample-hold);
        @statistic[sinkDownTime](title="Sink shutdown time"; source=sinkDown; unit=s; record=last; interpolationmode=none);
}
