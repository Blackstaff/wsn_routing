//
// This file is part of wsn_routing
//
// Copyright (C) 2017 Mateusz Czarnecki <mateusz.czarnecki92@gmail.com>
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'LICENSE'
// for details on this and other legal matters.
//

package routing.aleach;

import routing.leach.LEACH;


simple ALEACH extends LEACH
{
    parameters:
        string energyStorageModule = default("^.^.energyStorage");
        int numSensors;
        
        @class(ALEACH);
}
