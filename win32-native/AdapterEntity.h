#pragma once

#include <string>
#include <vector>
using namespace std;

struct AdapterEntity {

	wstring Description;
	wstring Caption;
	wstring UUID;
	int Index;
	int InterfaceIndex;
	wstring IPAddress;
	bool IsDHCPEnabled;
	wstring NetMask;
	wstring DefaultIPGateway;
	std::vector<wstring> DNS;
	int Metric;
};