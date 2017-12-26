#include "stdafx.h"
#include "OutPutPrinter.h"
#include <iostream>
#include <string>
using namespace std;


OutPutPrinter::OutPutPrinter()
{
}


OutPutPrinter::~OutPutPrinter()
{
}

void OutPutPrinter::printMessage(string message) {
	cout << message << endl;
}

void OutPutPrinter::printNetInterfaces(std::vector<AdapterEntity> &params) {
	if (params.size() > 0) {
		for (int i = 0; i < params.size(); i++) {
			printAdapterEntity(params[i]);
		}
	}
}

void OutPutPrinter::printDefaultGateways(std::vector<GateWayInfo> &gateWays) {
	for (int i = 0; i < gateWays.size(); i++) {
		wcout << gateWays[i].NextHop << ";" << gateWays[i].Mask << ";" << gateWays[i].Metric1 << ";" << gateWays[i].InterfaceIndex << ";" << gateWays[i].InterfaceName << endl;
	}
}

void OutPutPrinter::printAdapterEntity(AdapterEntity &entity) {
	wcout << entity.UUID << ";" << entity.IPAddress << ";" << entity.NetMask << ";" << entity.DefaultIPGateway << ";" << (entity.IsDHCPEnabled ? L"True" : L"False") << ";";
	int len = entity.DNS.size();
	for (int i = 0; i < len; i++) {
		if (len - 1 != i)// check for last element
		{
			wcout << entity.DNS[i] << ",";
		}
		else {
			wcout << entity.DNS[i];
		}
	}
	wcout << ";";
	wcout << endl;
}

