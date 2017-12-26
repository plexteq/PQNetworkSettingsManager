#pragma once
#include <vector>
#include "AdapterEntity.h"
#include "GatewayInfo.h"

class OutPutPrinter
{
public:
	OutPutPrinter();
	~OutPutPrinter();
	void printNetInterfaces(std::vector<AdapterEntity> &params);
	void printDefaultGateways(std::vector<GateWayInfo> &gateWays);
	void printMessage(string message);
private:
	void printAdapterEntity(AdapterEntity &entity);
};

