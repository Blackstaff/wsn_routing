//
// This file is part of wsn_routing
//
// Copyright (C) 2017 Mateusz Czarnecki <mateusz.czarnecki92@gmail.com>
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'LICENSE'
// for details on this and other legal matters.
//

package routing.leach_dchs;

import routing.leach.LEACH;


simple LEACH_DCHS extends LEACH
{
    parameters:
        string energyStorageModule = default("^.^.energyStorage");
        
        @class(LEACH_DCHS);
}
