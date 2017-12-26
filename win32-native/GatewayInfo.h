#pragma once
#include <string>
#include <vector>
using namespace std;

struct GateWayInfo {
	wstring NextHop;
	wstring Mask;
	int Metric1;
	int InterfaceIndex;
	wstring InterfaceName;
};
