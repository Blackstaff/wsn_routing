//
// This file is part of wsn_routing
//
// Copyright (C) 2017 Mateusz Czarnecki <mateusz.czarnecki92@gmail.com>
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'LICENSE'
// for details on this and other legal matters.
//

#ifndef __VOLATILESTATEBASEDENERGYCONSUMER_H
#define __VOLATILESTATEBASEDENERGYCONSUMER_H

#include "inet/physicallayer/energyconsumer/StateBasedEnergyConsumer.h"

using namespace inet;
using namespace physicallayer;

/**
 * This is a simple radio power consumer model. The power consumption is
 * determined by the radio mode, the transmitter state and the receiver state
 * using constant parameters.
 *
 * @author Levente Meszaros
 */
class VolatileStateBasedEnergyConsumer : public StateBasedEnergyConsumer
{
    protected:
        W baseConsumption;

    public:
        VolatileStateBasedEnergyConsumer();
        virtual void receiveSignal(cComponent *source, simsignal_t signalID, double value, cObject *details) override;

    protected:
        virtual void initialize(int stage) override;
};

#endif // ifndef __VOLATILESTATEBASEDENERGYCONSUMER_H

