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

package routing.leach;

import inet.networklayer.contract.INetworkProtocol;
import inet.networklayer.base.NetworkProtocolBase;


//
// TODO auto-generated module
//
simple LEACH extends NetworkProtocolBase like INetworkProtocol
{
	int maxNetFrameSize = default(0);
	//int netDataFrameOverhead = default(14);
	int netBufferSize = default(32);
	bool isSink = default(false);
	int headerLength @unit(bytes) = default(14B);
	double aggrCoefficient = default(0.05);
	string radioModule = default(absPath("^.^.nic.radio"));

	double percentage;
	double roundLength @unit(s);

	string sinkAddr = default("");

	//routing layer packet sizes
	int advPacketSize @unit(byte) = default(9B); // Type + Source + Destination = 9 bytes
	int joinPacketSize @unit(byte) = default(9B);
	int tdmaPacketSize @unit(byte) = default(150B); // Type + Source + Destination + tdma = 150 bytes
	int aggrPacketSize @unit(byte) = default(14B);

	double sigmaCPUClockDrift = default (0.00003);

	string powers = default("3.5 1.5 -2.5 -16.5"); // @unit(dBm)
	string powerConsumptions = default("43.5 30 27 24"); // @unit(mW)

	@signal[transmitterPowerChanged](type=double);

    @class(LEACH);
}
