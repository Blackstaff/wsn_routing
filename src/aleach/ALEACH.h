//
// This file is part of wsn_routing
//
// Copyright (C) 2017 Mateusz Czarnecki <mateusz.czarnecki92@gmail.com>
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'LICENSE'
// for details on this and other legal matters.
//

#ifndef __ROUTING_ALEACH_H_
#define __ROUTING_ALEACH_H_

#include <omnetpp.h>

#include "../leach/LEACH.h"

#include "inet/power/contract/IEnergyStorage.h"

using namespace omnetpp;
using namespace inet;

class ALEACH : public LEACH
{
  protected:
    power::IEnergyStorage *energyStorage;
    int numSensors;
    int expectedCHNum;
    double maxEnergy;

  protected:
    virtual void initialize(int stage) override;
    virtual void selectCH() override;
};

#endif
