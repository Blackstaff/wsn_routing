//
// This file is part of wsn_routing
//
// Copyright (C) 2017 Mateusz Czarnecki <mateusz.czarnecki92@gmail.com>
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'LICENSE'
// for details on this and other legal matters.
//

cplusplus {{
#include "inet/networklayer/common/L3Address.h"
}}

class noncobject inet::L3Address;

packet SinkInfo {
    inet::L3Address address;
}