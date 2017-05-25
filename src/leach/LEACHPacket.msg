//
// This file is part of wsn_routing
//
// Modified work Copyright (C) 2017 Mateusz Czarnecki <mateusz.czarnecki92@gmail.com>
//
//****************************************************************************
//*  Original work Copyright (c) Federal University of Para, brazil - 2011   *
//*  Developed at the Research Group on Computer Network and Multimedia      *
//*  Communication (GERCOM)                                  *
//*  All rights reserved                                         *
//*                                                                          *
//*  Permission to use, copy, modify, and distribute this protocol and its   *
//*  documentation for any purpose, without fee, and without written         *
//*  agreement is hereby granted, provided that the above copyright notice,  *
//*  and the author appear in all copies of this protocol.                   *
//*                                                                          *
//*  Module:   LEACH Clustering Protocol for Castalia Simulator              *
//*  Version:  0.2                                                           *
//*  Author(s): Adonias Pires <adonias@ufpa.br>                              *
//*             Claudio Silva <claudio.silva@itec.ufpa.br>                   *
//****************************************************************************/
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'LICENSE'
// for details on this and other legal matters.
//
cplusplus {{
#include "inet/networklayer/base/NetworkDatagramBase_m.h"
}}

class inet::NetworkDatagramBase;
class noncobject inet::L3Address;

enum LEACHPacketType
{
	LEACH_ADV_PACKET 	= 1;		
	LEACH_JOIN_PACKET 	= 2;		
	LEACH_TDMA_PACKET 	= 3;		
	LEACH_DATA_PACKET 	= 4;
};

packet LEACHPacket extends inet::NetworkDatagramBase
{	
	int type @enum(LEACHPacketType);   
	inet::L3Address schedule[];
	int transportProtocol = -1;
	double rssi;
	unsigned long seqNum = 0;
	int aggrNum = 1;
};