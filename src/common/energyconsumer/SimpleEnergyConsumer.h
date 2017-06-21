//
// This file is part of wsn_routing
//
// Copyright (C) 2017 Mateusz Czarnecki <mateusz.czarnecki92@gmail.com>
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'LICENSE'
// for details on this and other legal matters.
//

#ifndef __SIMPLEENERGYCONSUMER_H
#define __SIMPLEENERGYCONSUMER_H

#include "inet/physicallayer/energyconsumer/StateBasedEnergyConsumer.h"

using namespace inet;
using namespace physicallayer;

class SimpleEnergyConsumer : public cSimpleModule, IEnergyConsumer
{
    protected:
        int energyConsumerId;
        IEnergySource *energySource;
        W powerConsumption;
        cMessage *timer;

    protected:
        virtual void initialize(int stage) override;

        virtual void handleMessage(cMessage *message) override;

        virtual void updatePowerConsumption();

    public:
        SimpleEnergyConsumer();
        virtual ~SimpleEnergyConsumer();

        virtual W getPowerConsumption() const override { return powerConsumption; }
        virtual void setPowerConsumption(W power);
};

#endif // ifndef __SIMPLEENERGYCONSUMER_H

