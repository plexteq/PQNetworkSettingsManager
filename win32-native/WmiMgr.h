#pragma once
#include "AdapterEntity.h"
#include "GatewayInfo.h"
#include <vector>
using namespace std;
#define _WIN32_DCOM
#include <iostream>
using namespace std;
#include <comdef.h>
#include <Wbemidl.h>

#pragma comment(lib, "wbemuuid.lib")

struct RouteTable {
	wstring Destination;
	wstring NextHop;
	wstring Mask;
	int Metric1;
	int InterfaceIndex;
};

class WmiMgr;

typedef bool(WmiMgr::*CheckForwardEntry)(PMIB_IPFORWARDROW, DWORD);

class WmiMgr
{
public:
	WmiMgr();
	~WmiMgr();
	std::vector<AdapterEntity> GetAllNetInterfaces();
	HRESULT GetNetInterfaceByID(wstring adapterUUID, AdapterEntity &ae);
	HRESULT SetDNS(wstring adapterUUID, vector<wstring> dns);
	HRESULT DhcpToStatic(wstring adapterUUID);
	HRESULT StaticToDhcp(wstring adapterUUID);
	HRESULT DefaultGateways(std::vector<GateWayInfo> &gateWays);
	HRESULT SetDefaultGateway(string ipAddress, string mask, int metric, int interfaceIndex);
	HRESULT RemoveDefaultGateway();
	HRESULT SetDefaultGatewayForAddress(string gatewayAddress, string address);
	HRESULT RemoveDefaultGatewayForAddress(string address);
private:
	// Initialize COM
	HRESULT Initialize();
	void Release();
	HRESULT getEnabledNetInterfaces(vector<AdapterEntity> &aec, bool bAll = false);
	HRESULT assignValue(wstring propName, AdapterEntity &ae, IWbemClassObject *pclsObj);
	HRESULT assignValueToRt(wstring propName, RouteTable & rt, IWbemClassObject * pclsObj);
	HRESULT getWbemClassObjectEnumerator(IEnumWbemClassObject** pEnumeratro, string className);
	HRESULT setStaticToDhcp(wstring adapterUUID);
	HRESULT setDhcpToStatic(wstring adapterUUID);
	HRESULT setDnsValues(wstring adapterUUID, vector<wstring> dns);

	HRESULT getDefaultGateways(std::vector<GateWayInfo> &gateWays);
	wstring getNetInstancePath(int index);
	HRESULT getInterfaceByID(wstring adapterUUID, AdapterEntity &ae);
	HRESULT getIP4RouteTable(vector<RouteTable> &routes);
	PMIB_IPFORWARDROW getForwardRowByInterfaceIndex(int index);
	PMIB_IPFORWARDROW getForwardRowByDestAddress(string address);
	PMIB_IPFORWARDROW getForwardRowByFunc(CheckForwardEntry pFunc, DWORD value);
	bool CheckNetworkIndex(PMIB_IPFORWARDROW pRow, DWORD index);
	bool CheckDestAddress(PMIB_IPFORWARDROW pRow, DWORD address);
	HRESULT CreateIPForwardEntry(string networkAddress, string gatewayAddress, string mask, int metric, int interfaceIndex, bool toCreateEntry);
	int GetBestNetworkInterfaceIndex(string destAddress);
	int GetMetricByNetworkInterfaceIndex(int networkInterfaceIndex);
private:
	IWbemLocator *pLoc;
	IWbemServices *pSvc;
};

