#include "stdafx.h"
#include "WmiMgr.h"
#include <atlsafe.h>
#include <WbemCli.h>

WmiMgr::WmiMgr()
{
}

WmiMgr::~WmiMgr()
{
}

wstring WmiMgr::getNetInstancePath(int index) {
	wstring wIndex;
	char indexString[10];
	itoa(index, indexString, 10);

	char instanceString[100];
	wchar_t w_instanceString[100];
	strcpy(instanceString, "Win32_NetworkAdapterConfiguration.Index='");
	strcat(instanceString, indexString);
	strcat(instanceString, "'");
	mbstowcs(w_instanceString, instanceString, 100);

	return wstring(w_instanceString);
}

std::vector<AdapterEntity> WmiMgr::GetAllNetInterfaces() {
	vector<AdapterEntity> aecol;

	HRESULT hres = Initialize();
	if (SUCCEEDED(hres)) {
		// Make special action to get information.
		getEnabledNetInterfaces(aecol);
		Release();
	}

	return aecol;
}

HRESULT WmiMgr::DefaultGateways(std::vector<GateWayInfo> &gateWays) {
	HRESULT hres = E_FAIL;
	hres = Initialize();
	if (SUCCEEDED(hres)) {
		hres = getDefaultGateways(gateWays);
		Release();
	}
	return hres;
}
HRESULT WmiMgr::getDefaultGateways(std::vector<GateWayInfo> &gateWays) {
	HRESULT hres = E_FAIL;
	vector<AdapterEntity> aecol;
	if (SUCCEEDED(getEnabledNetInterfaces(aecol, true))) {
		vector<RouteTable> routes;
		hres = getIP4RouteTable(routes);
		if (SUCCEEDED(hres)) {
			for (int i = 0; i < routes.size(); i++) {
				if (routes[i].NextHop != L"0.0.0.0") {
					GateWayInfo gi;
					gi.Metric1 = routes[i].Metric1;
					gi.Mask = routes[i].Mask;
					gi.NextHop = routes[i].NextHop;
					gi.InterfaceIndex = routes[i].InterfaceIndex;

					// assign interface name
					for (int j = 0; j < aecol.size(); j++) {
						if (aecol[j].InterfaceIndex == routes[i].InterfaceIndex) {
							gi.InterfaceName = aecol[j].Caption;
							break;
						}
					}
					gateWays.push_back(gi);
				}
			}
		}
	}
	return hres;
}

HRESULT WmiMgr::DhcpToStatic(wstring adapterUUID) {
	if (adapterUUID.size()) {
		HRESULT hres = Initialize();
		if (SUCCEEDED(hres)) {
			hres = setDhcpToStatic(adapterUUID);
			Release();
		}
		return hres;
	}
	return E_FAIL;
}

HRESULT WmiMgr::setDhcpToStatic(wstring adapterUUID) {
	HRESULT hres = E_FAIL;
	if (pLoc != NULL && pSvc != NULL) {

		AdapterEntity ae;
		if (SUCCEEDED(this->getInterfaceByID(adapterUUID, ae))) {

			// convert index to int and make instance identifier
			wstring wNetInstancePath = getNetInstancePath(ae.Index);
			//ae.DefaultIPGateway = wstring(L"15.15.15.15");

			BSTR instancePath = SysAllocString(wNetInstancePath.c_str());

			IWbemClassObject* pClass = NULL;
			IWbemClassObject * pOutInst = NULL;

			BSTR ClassPath = SysAllocString(L"Win32_NetworkAdapterConfiguration");
			hres = pSvc->GetObject(ClassPath, 0, NULL, &pClass, NULL);

			BSTR MethodName_ES = SysAllocString(L"EnableStatic");
			IWbemClassObject *pInClass_ES = NULL;
			hres = pClass->GetMethod(MethodName_ES, 0, &pInClass_ES, NULL);
			if (WBEM_S_NO_ERROR == hres) {

				IWbemClassObject *pInInst_ES = NULL;
				hres = pInClass_ES->SpawnInstance(0, &pInInst_ES);
				if (WBEM_S_NO_ERROR == hres)
				{
					// Prepare call of EnableStatic method

					// init input parameter for method
					SAFEARRAY *ip_list = SafeArrayCreateVector(VT_BSTR, 0, 1);
					BSTR ip = SysAllocString(ae.IPAddress.c_str());
					long idx[] = { 0 };
					SafeArrayPutElement(ip_list, idx, ip);
					SysFreeString(ip);

					SAFEARRAY *netmask_list = SafeArrayCreateVector(VT_BSTR, 0, 1);
					BSTR mask = SysAllocString(ae.NetMask.c_str());
					SafeArrayPutElement(netmask_list, idx, mask);
					SysFreeString(mask);

					// Now wrap each safe array in a VARIANT so that it can be passed to COM function
					VARIANT arg1_ES;
					VariantInit(&arg1_ES);
					arg1_ES.vt = VT_ARRAY | VT_BSTR;
					arg1_ES.parray = ip_list;

					VARIANT arg2_ES;
					VariantInit(&arg2_ES);
					arg2_ES.vt = VT_ARRAY | VT_BSTR;
					arg2_ES.parray = netmask_list;

					if ((WBEM_S_NO_ERROR == pInInst_ES->Put(L"IPAddress", 0, &arg1_ES, 0)) &&
						(WBEM_S_NO_ERROR == pInInst_ES->Put(L"SubNetMask", 0, &arg2_ES, 0)))
					{
						hres = pSvc->ExecMethod(instancePath, MethodName_ES, 0, NULL, pInInst_ES, &pOutInst, NULL);
					}
					else {
						hres = E_FAIL;
					}

					// Clear the variants - does this actually get ride of safearrays?
					VariantClear(&arg1_ES);
					VariantClear(&arg2_ES);
				}

				if (pInInst_ES) {
					pInInst_ES->Release();
					pInInst_ES = NULL;
				}
			}

			BSTR MethodName_SG = SysAllocString(L"SetGateways");
			IWbemClassObject *pInClass_SG = NULL;
			hres = pClass->GetMethod(MethodName_SG, 0, &pInClass_SG, NULL);
			if (WBEM_S_NO_ERROR == hres) {

				IWbemClassObject *pInInst_SG = NULL;
				hres = pInClass_SG->SpawnInstance(0, &pInInst_SG);
				if (WBEM_S_NO_ERROR == hres)
				{
					// Prepare call of SetGateways method

					// init input parameter for method
					SAFEARRAY *ip_gateWaylist = SafeArrayCreateVector(VT_BSTR, 0, 1);
					BSTR ip = SysAllocString(ae.DefaultIPGateway.c_str());
					long idx[] = { 0 };
					SafeArrayPutElement(ip_gateWaylist, idx, ip);
					SysFreeString(ip);

					SAFEARRAY *gateWayMetric_list = SafeArrayCreateVector(VT_I4, 0, 1);
					int metric = 1;
					SafeArrayPutElement(gateWayMetric_list, idx, &metric);

					// Now wrap each safe array in a VARIANT so that it can be passed to COM function
					VARIANT arg1_SG;
					VariantInit(&arg1_SG);
					arg1_SG.vt = VT_ARRAY | VT_BSTR;
					arg1_SG.parray = ip_gateWaylist;

					VARIANT arg2_SG;
					VariantInit(&arg2_SG);
					arg2_SG.vt = VT_ARRAY | VT_I4;
					arg2_SG.parray = gateWayMetric_list;

					if ((WBEM_S_NO_ERROR == pInInst_SG->Put(L"DefaultIPGateway", 0, &arg1_SG, 0)) &&
						(WBEM_S_NO_ERROR == pInInst_SG->Put(L"GatewayCostMetric", 0, &arg2_SG, 0)))
					{
						hres = pSvc->ExecMethod(instancePath, MethodName_SG, 0, NULL, pInInst_SG, &pOutInst, NULL);
					}
					else {
						hres = E_FAIL;
					}

					// Clear the variants - does this actually get ride of safearrays?
					VariantClear(&arg1_SG);
					VariantClear(&arg2_SG);
				}

				if (pInInst_SG) {
					pInInst_SG->Release();
					pInInst_SG = NULL;
				}
			}

			SysFreeString(ClassPath);
			SysFreeString(instancePath);
			SysFreeString(MethodName_ES);
			SysFreeString(MethodName_SG);

			if (pOutInst) {
				pOutInst->Release();
				pOutInst = NULL;
			}

			if (pInClass_SG) {
				pInClass_SG->Release();
				pInClass_SG = NULL;
			}

			if (pInClass_ES)
			{
				pInClass_ES->Release();
				pInClass_ES = NULL;
			} //if

			if (pClass)
			{
				pClass->Release();
				pClass = NULL;
			} //if
		}
	}
	return hres;
}

HRESULT WmiMgr::StaticToDhcp(wstring adapterUUID) {

	if (adapterUUID.size()) {
		HRESULT hres = Initialize();
		if (SUCCEEDED(hres)) {
			hres = setStaticToDhcp(adapterUUID);
			Release();
		}
		return hres;
	}

	return E_FAIL;
}

HRESULT WmiMgr::setStaticToDhcp(wstring adapterUUID) {
	HRESULT hres = E_FAIL;
	if (pLoc != NULL && pSvc != NULL) {

		AdapterEntity ae;
		if (SUCCEEDED(this->getInterfaceByID(adapterUUID, ae))) {
			
			// convert index to int and make instance identifier
			wstring wNetInstancePath = getNetInstancePath(ae.Index);

			BSTR instancePath = SysAllocString(wNetInstancePath.c_str());

			IWbemClassObject* pClass = NULL;
			IWbemClassObject * pOutInst = NULL;

			BSTR ClassPath = SysAllocString(L"Win32_NetworkAdapterConfiguration");
			hres = pSvc->GetObject(ClassPath, 0, NULL, &pClass, NULL);

			BSTR MethodName_ED = SysAllocString(L"EnableDHCP");
			IWbemClassObject *pInClass_ED = NULL;
			if (WBEM_S_NO_ERROR == pClass->GetMethod(MethodName_ED, 0, &pInClass_ED, NULL)) {
				hres = pSvc->ExecMethod(instancePath, MethodName_ED, 0, NULL, NULL, &pOutInst, NULL);
			}

			SysFreeString(ClassPath);
			SysFreeString(instancePath);
			SysFreeString(MethodName_ED);

			if (pOutInst) {
				pOutInst->Release();
				pOutInst = NULL;
			}

			if (pInClass_ED)
			{
				pInClass_ED->Release();
				pInClass_ED = NULL;
			} //if

			if (pClass)
			{
				pClass->Release();
				pClass = NULL;
			} //if
		}
	}
	return hres;
}

HRESULT WmiMgr::SetDNS(wstring adapterUUID, vector<wstring> dns) {
	if (adapterUUID.size() > 0 && dns.size() > 0) {
		HRESULT hres = Initialize();
		if (SUCCEEDED(hres)) {
			// Make special action to get information.
			hres = setDnsValues(adapterUUID, dns);
			Release();
		}
		return hres;
	}
	return E_FAIL;
}

HRESULT WmiMgr::SetDefaultGatewayForAddress(string address, string gatewayAddress) {
	HRESULT hres = E_FAIL;
	int interfaceId = 0;
	int metric = 0;

	hres = Initialize();

	if (SUCCEEDED(hres)) {
		interfaceId = GetBestNetworkInterfaceIndex("8.8.4.4");
		metric = GetMetricByNetworkInterfaceIndex(interfaceId);
		metric++;
		hres = CreateIPForwardEntry(address, gatewayAddress, "255.255.255.255", metric, interfaceId, false);
		Release();
	}
	return hres;
}

HRESULT WmiMgr::setDnsValues(wstring adapterUUID, vector<wstring> dns) {
	HRESULT hres = E_FAIL;
	if (pLoc != NULL && pSvc != NULL) {

		AdapterEntity ae;
		if (SUCCEEDED(this->getInterfaceByID(adapterUUID, ae))) {

			// convert index to int and make instance identifier
			wstring wNetInstancePath = getNetInstancePath(ae.Index);
			//ae.DefaultIPGateway = wstring(L"15.15.15.15");

			BSTR instancePath = SysAllocString(wNetInstancePath.c_str());

			IWbemClassObject* pClass = NULL;
			IWbemClassObject * pOutInst = NULL;

			BSTR ClassPath = SysAllocString(L"Win32_NetworkAdapterConfiguration");
			hres = pSvc->GetObject(ClassPath, 0, NULL, &pClass, NULL);

			BSTR MethodName_SDNS = SysAllocString(L"SetDNSServerSearchOrder");
			IWbemClassObject *pInClass_SDNS = NULL;
			hres = pClass->GetMethod(MethodName_SDNS, 0, &pInClass_SDNS, NULL);
			if (WBEM_S_NO_ERROR == hres) {

				IWbemClassObject *pInInst_SDNS = NULL;
				hres = pInClass_SDNS->SpawnInstance(0, &pInInst_SDNS);
				if (WBEM_S_NO_ERROR == hres)
				{
					// init input parameter for method
					SAFEARRAY *ip_list = SafeArrayCreateVector(VT_BSTR, 0, dns.size());
					long idx[] = { 0 };

					for (int i = 0; i < dns.size(); i++) {
						idx[0] = i;
						BSTR ip = SysAllocString(dns[i].c_str());
						SafeArrayPutElement(ip_list, idx, ip);
						SysFreeString(ip);
					}

					// Now wrap each safe array in a VARIANT so that it can be passed to COM function
					VARIANT arg1_SDNS;
					VariantInit(&arg1_SDNS);
					arg1_SDNS.vt = VT_ARRAY | VT_BSTR;
					arg1_SDNS.parray = ip_list;

					if ((WBEM_S_NO_ERROR == pInInst_SDNS->Put(L"DNSServerSearchOrder", 0, &arg1_SDNS, 0)))
					{
						hres = pSvc->ExecMethod(instancePath, MethodName_SDNS, 0, NULL, pInInst_SDNS, &pOutInst, NULL);
					}
					else {
						hres = E_FAIL;
					}

					// Clear the variants - does this actually get ride of safearrays?
					VariantClear(&arg1_SDNS);
				}

				if (pInInst_SDNS) {
					pInInst_SDNS->Release();
					pInInst_SDNS = NULL;
				}
			}

			SysFreeString(ClassPath);
			SysFreeString(instancePath);
			SysFreeString(MethodName_SDNS);

			if (pOutInst) {
				pOutInst->Release();
				pOutInst = NULL;
			}

			if (pInClass_SDNS)
			{
				pInClass_SDNS->Release();
				pInClass_SDNS = NULL;
			} //if

			if (pClass)
			{
				pClass->Release();
				pClass = NULL;
			} //if
		}
	}
	return hres;
}

HRESULT WmiMgr::getInterfaceByID(wstring adapterUUID, AdapterEntity &ae) {
	HRESULT hres = E_FAIL;
	vector<AdapterEntity> aecol;
	if (SUCCEEDED(getEnabledNetInterfaces(aecol))) {
		for (std::vector<AdapterEntity>::const_iterator i = aecol.begin(); i != aecol.end(); i++) {
			if ((*i).UUID == adapterUUID) {
				ae = (*i);
				hres = S_OK;
				break;
			}
		}
	}
	return hres;
}

HRESULT WmiMgr::GetNetInterfaceByID(wstring adapterUUID, AdapterEntity &ae) {

	if (adapterUUID.size() > 0)
		return E_FAIL;

	HRESULT hres = Initialize();
	if (SUCCEEDED(hres)) {
		hres = getInterfaceByID(adapterUUID, ae);
		Release();
	}
	return hres;
}

HRESULT WmiMgr::getIP4RouteTable(vector<RouteTable> &routes) {
	if (pLoc != NULL && pSvc != NULL) {
		HRESULT hres;
		IEnumWbemClassObject* pEnumerator = NULL;
		hres = getWbemClassObjectEnumerator(&pEnumerator, "Win32_IP4RouteTable");
		if (FAILED(hres)) {
			return hres;               // Program has failed.
		}

		IWbemClassObject *pclsObj = NULL;
		ULONG uReturn = 0;

		while (pEnumerator)
		{
			HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1,
				&pclsObj, &uReturn);

			if (0 == uReturn)
			{
				break;
			}

			RouteTable rt;
			hr = assignValueToRt(L"Destination", rt, pclsObj);
			hr = assignValueToRt(L"NextHop", rt, pclsObj);
			hr = assignValueToRt(L"Mask", rt, pclsObj);
			hr = assignValueToRt(L"Metric1", rt, pclsObj);
			hr = assignValueToRt(L"InterfaceIndex", rt, pclsObj);

			routes.push_back(rt);
			
			pclsObj->Release();
		}

		if (pEnumerator != NULL) {
			pEnumerator->Release();
			pEnumerator = NULL;
		}

		return hres;
	}

	return E_FAIL;
}

HRESULT WmiMgr::getEnabledNetInterfaces(vector<AdapterEntity> &collection, bool bAll) {

	if (pLoc != NULL && pSvc != NULL) {
		HRESULT hres;
		IEnumWbemClassObject* pEnumerator = NULL;
		hres = getWbemClassObjectEnumerator(&pEnumerator, "Win32_NetworkAdapterConfiguration");
		if (FAILED(hres)) {
			return hres;               // Program has failed.
		}

		IWbemClassObject *pclsObj = NULL;
		ULONG uReturn = 0;

		while (pEnumerator)
		{
			HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1,
				&pclsObj, &uReturn);

			if (0 == uReturn)
			{
				break;
			}

			VARIANT vtIPEnabled;

			// Get the value of properties
			hr = pclsObj->Get(L"IPEnabled", 0, &vtIPEnabled, 0, 0);
			if (SUCCEEDED(hr) && (vtIPEnabled.boolVal || bAll)) {

				// IPAddress IPSubnet DefaultIPGateway SettingID DHCPEnabled DNSServerSearchOrder
				AdapterEntity ae;
				hr = assignValue(L"IPAddress", ae, pclsObj);
				hr = assignValue(L"IPSubnet", ae, pclsObj);
				hr = assignValue(L"DefaultIPGateway", ae, pclsObj);
				hr = assignValue(L"SettingID", ae, pclsObj);
				hr = assignValue(L"DHCPEnabled", ae, pclsObj);
				hr = assignValue(L"DNSServerSearchOrder", ae, pclsObj);
				hr = assignValue(L"Description", ae, pclsObj);
				hr = assignValue(L"Caption", ae, pclsObj);
				hr = assignValue(L"Index", ae, pclsObj);
				hr = assignValue(L"InterfaceIndex", ae, pclsObj);
				hr = assignValue(L"IPConnectionMetric", ae, pclsObj);

				collection.push_back(ae);
			}
			VariantClear(&vtIPEnabled);
			pclsObj->Release();
		}

		if (pEnumerator != NULL) {
			pEnumerator->Release();
			pEnumerator = NULL;
		}

		return hres;
	}

	return E_FAIL;
}

HRESULT WmiMgr::assignValueToRt(wstring propName, RouteTable & rt, IWbemClassObject * pclsObj)
{
	HRESULT hr = E_FAIL;
	VARIANT vtProp;

	if (propName == L"Mask") {
		hr = pclsObj->Get(propName.c_str(), 0, &vtProp, 0, 0);
		if (SUCCEEDED(hr)) {
			rt.Mask = std::wstring(vtProp.bstrVal, SysStringLen(vtProp.bstrVal));
		}
	}
	if (propName == L"Destination") {
		hr = pclsObj->Get(propName.c_str(), 0, &vtProp, 0, 0);
		if (SUCCEEDED(hr)) {
			rt.Destination = std::wstring(vtProp.bstrVal, SysStringLen(vtProp.bstrVal));
		}
	}

	if (propName == L"NextHop") {
		hr = pclsObj->Get(propName.c_str(), 0, &vtProp, 0, 0);
		if (SUCCEEDED(hr)) {
			rt.NextHop = std::wstring(vtProp.bstrVal, SysStringLen(vtProp.bstrVal));
		}
	}
	if (propName == L"Metric1") {
		hr = pclsObj->Get(propName.c_str(), 0, &vtProp, 0, 0);
		if (SUCCEEDED(hr)) {
			rt.Metric1 = vtProp.intVal;
		}
	}
	if (propName == L"InterfaceIndex") {
		hr = pclsObj->Get(propName.c_str(), 0, &vtProp, 0, 0);
		if (SUCCEEDED(hr)) {
			rt.InterfaceIndex = vtProp.intVal;
		}
	}

	if (SUCCEEDED(hr))
		VariantClear(&vtProp);

	return hr;
}

HRESULT WmiMgr::assignValue(wstring propName, AdapterEntity & ae, IWbemClassObject * pclsObj)
{
	// IPAddress IPSubnet DefaultIPGateway SettingID DHCPEnabled DNSServerSearchOrder
	HRESULT hr = E_FAIL;
	VARIANT vtProp;

	if (propName == L"IPAddress") {
		hr = pclsObj->Get(propName.c_str(), 0, &vtProp, 0, 0);
		if (SUCCEEDED(hr) && vtProp.parray != NULL) {
			if (vtProp.parray->cDims >= 1 && (vtProp.vt == (VT_ARRAY | VT_BSTR))){
				CComSafeArray<BSTR> sa(vtProp.parray);
				ae.IPAddress = std::wstring(sa[0]);
			}
		}
	}
	if (propName == L"IPSubnet") {

		hr = pclsObj->Get(propName.c_str(), 0, &vtProp, 0, 0);
		if (SUCCEEDED(hr) && vtProp.parray != NULL) {
			if (vtProp.parray->cDims >= 1 && (vtProp.vt == (VT_ARRAY | VT_BSTR))) {
				CComSafeArray<BSTR> sa(vtProp.parray);
				ae.NetMask = std::wstring(sa[0]);
			}
		}
	}
	if (propName == L"DefaultIPGateway") {

		hr = pclsObj->Get(propName.c_str(), 0, &vtProp, 0, 0);
		if (SUCCEEDED(hr) && vtProp.parray != NULL && vtProp.vt != VT_NULL) {
			if (vtProp.parray->cDims >= 1 && (vtProp.vt == (VT_ARRAY | VT_BSTR))) {
				CComSafeArray<BSTR> sa(vtProp.parray);
				ae.DefaultIPGateway = std::wstring(sa[0]);
			}
		}
	}
	if (propName == L"SettingID") {
		hr = pclsObj->Get(propName.c_str(), 0, &vtProp, 0, 0);
		if (SUCCEEDED(hr)) {
			ae.UUID = std::wstring(vtProp.bstrVal, SysStringLen(vtProp.bstrVal));
		}
	}
	if (propName == L"Index") {
		hr = pclsObj->Get(propName.c_str(), 0, &vtProp, 0, 0);
		if (SUCCEEDED(hr)) {
			ae.Index = vtProp.intVal;
		}
	}
	if (propName == L"InterfaceIndex") {
		hr = pclsObj->Get(propName.c_str(), 0, &vtProp, 0, 0);
		if (SUCCEEDED(hr)) {
			ae.InterfaceIndex = vtProp.intVal;
		}
	}
	if (propName == L"Description") {
		hr = pclsObj->Get(propName.c_str(), 0, &vtProp, 0, 0);
		if (SUCCEEDED(hr)) {
			ae.Description = std::wstring(vtProp.bstrVal, SysStringLen(vtProp.bstrVal));
		}
	}

	if (propName == L"Caption") {
		hr = pclsObj->Get(propName.c_str(), 0, &vtProp, 0, 0);
		if (SUCCEEDED(hr)) {
			ae.Caption = std::wstring(vtProp.bstrVal, SysStringLen(vtProp.bstrVal));
		}
	}

	if (propName == L"DHCPEnabled") {
		hr = pclsObj->Get(propName.c_str(), 0, &vtProp, 0, 0);
		if (SUCCEEDED(hr)) {
			ae.IsDHCPEnabled = vtProp.boolVal;
		}
	}
	if (propName == L"DNSServerSearchOrder") {
		hr = pclsObj->Get(propName.c_str(), 0, &vtProp, 0, 0);
		if (SUCCEEDED(hr) && vtProp.parray != NULL) {
			if (vtProp.parray->cDims >= 1 && (vtProp.vt == (VT_ARRAY | VT_BSTR))) {
				CComSafeArray<BSTR> sa(vtProp.parray);

				for (int i = 0; i < sa.GetCount(); i++) {
					ae.DNS.push_back(std::wstring(sa[i]));
				}
			}
		}
	}
	if (propName == L"IPConnectionMetric") {
		hr = pclsObj->Get(propName.c_str(), 0, &vtProp, 0, 0);
		if (SUCCEEDED(hr)) {
			ae.Metric = vtProp.intVal;
		}
	}

	if(SUCCEEDED(hr))
		VariantClear(&vtProp);

	return hr;
}

HRESULT WmiMgr::Initialize() {
	HRESULT hres = S_OK;

	// Initialize COM.
	hres = CoInitializeEx(0, COINIT_MULTITHREADED);
	if (FAILED(hres))
	{
		cout << "Failed to initialize COM library. Error code = 0x"
			<< hex << hres << endl;
		return hres;                  // Program has failed.
	}

	// Set general COM security levels
	hres = CoInitializeSecurity(
		NULL,
		-1,                          // COM authentication
		NULL,                        // Authentication services
		NULL,                        // Reserved
		RPC_C_AUTHN_LEVEL_DEFAULT,   // Default authentication 
		RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation  
		NULL,                        // Authentication info
		EOAC_NONE,                   // Additional capabilities 
		NULL                         // Reserved
	);


	if (FAILED(hres))
	{
		cout << "Failed to initialize security. Error code = 0x"
			<< hex << hres << endl;
		CoUninitialize();
		return hres;                    // Program has failed.
	}

	// Obtain the initial locator to WMI -------------------------
	pLoc = NULL;

	hres = CoCreateInstance(
		CLSID_WbemLocator,
		0,
		CLSCTX_INPROC_SERVER,
		IID_IWbemLocator, (LPVOID *)&pLoc);

	if (FAILED(hres))
	{
		cout << "Failed to create IWbemLocator object."
			<< " Err code = 0x"
			<< hex << hres << endl;
		CoUninitialize();
		return hres;                 // Program has failed.
	}

	pSvc = NULL;

	// Connect to the root\cimv2 namespace with
	// the current user and obtain pointer pSvc
	// to make IWbemServices calls.
	hres = pLoc->ConnectServer(
		_bstr_t(L"ROOT\\CIMV2"), // Object path of WMI namespace
		NULL,                    // User name. NULL = current user
		NULL,                    // User password. NULL = current
		0,                       // Locale. NULL indicates current
		NULL,                    // Security flags.
		0,                       // Authority (for example, Kerberos)
		0,                       // Context object 
		&pSvc                    // pointer to IWbemServices proxy
	);

	if (FAILED(hres))
	{
		cout << "Could not connect. Error code = 0x"
			<< hex << hres << endl;
		pLoc->Release();
		CoUninitialize();
		return hres;                // Program has failed.
	}

	//cout << "Connected to ROOT\\CIMV2 WMI namespace" << endl;

	// Set security levels on the proxy -------------------------

	hres = CoSetProxyBlanket(
		pSvc,                        // Indicates the proxy to set
		RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx
		RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx
		NULL,                        // Server principal name 
		RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx 
		RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
		NULL,                        // client identity
		EOAC_NONE                    // proxy capabilities 
	);

	if (FAILED(hres))
	{
		cout << "Could not set proxy blanket. Error code = 0x"
			<< hex << hres << endl;
		pSvc->Release();
		pLoc->Release();
		CoUninitialize();
		return hres;               // Program has failed.
	}

	return hres;
}

HRESULT WmiMgr::getWbemClassObjectEnumerator(IEnumWbemClassObject** pEnumerator, string className)
{
	HRESULT hres = E_FAIL;
	//IEnumWbemClassObject* pEnumerator = NULL;
	*pEnumerator = NULL;
	if (pLoc != NULL && pSvc != NULL) {
		string query = "SELECT * FROM " + className;
		hres = pSvc->ExecQuery(
			bstr_t("WQL"),
			bstr_t(query.c_str()),
			WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
			NULL,
			pEnumerator);

		if (FAILED(hres))
		{
			cout << "Query for operating system name failed."
				<< " Error code = 0x"
				<< hex << hres << endl;
			return hres;               // Program has failed.
		}
	}
	return hres;
}

void WmiMgr::Release() {

	if (pSvc != NULL) {
		pSvc->Release();
		pSvc = NULL;
	}
	if (pLoc != NULL) {
		pLoc->Release();
		pLoc = NULL;
	}
	
	CoUninitialize();
}

HRESULT WmiMgr::RemoveDefaultGateway() {
	HRESULT hres = E_FAIL;
	PMIB_IPFORWARDROW pRow = NULL;
	DWORD dwStatus = 0;

	// get table row item with mask = 0.0.0.0 and destination = 0.0.0.0
	
	pRow = getForwardRowByInterfaceIndex(-1);
	if (pRow != NULL) {
		dwStatus = DeleteIpForwardEntry(pRow);

		if (dwStatus == ERROR_SUCCESS) {
			hres = S_OK;
		}
		else {
			printf("Could not delete gateway. codeError [%d]\n", dwStatus);
		}

		if (pRow)
			free(pRow);
	}
	return hres;
}

HRESULT WmiMgr::RemoveDefaultGatewayForAddress(string address) {
	HRESULT hres = E_FAIL;
	PMIB_IPFORWARDROW pRow = NULL;
	DWORD dwStatus = 0;

	pRow = getForwardRowByDestAddress(address);

	if (pRow != NULL) {
		dwStatus = DeleteIpForwardEntry(pRow);

		if (dwStatus == ERROR_SUCCESS) {
			hres = S_OK;
		}
		else {
			printf("Could not delete gateway. codeError [%d]\n", dwStatus);
		}

		if (pRow)
			free(pRow);
	}

	return hres;
}

HRESULT WmiMgr::SetDefaultGateway(string ipAddress, string mask, int metric, int interfaceIndex) {
	HRESULT hres = E_FAIL;
	PMIB_IPFORWARDROW pRow = NULL;
	MIB_IPFORWARDROW newRow;
	DWORD dwStatus = 0;
	bool toCreate = false;

	hres = CreateIPForwardEntry("0.0.0.0", ipAddress, mask, metric, interfaceIndex, false);

	return hres;
}

HRESULT WmiMgr::CreateIPForwardEntry(string networkAddress, string gatewayAddress, string mask, int metric, int interfaceIndex, bool toUpdate) {

	HRESULT hres = E_FAIL;
	PMIB_IPFORWARDROW pRow = NULL;
	DWORD dwStatus = 0;

	if(interfaceIndex != 0 && toUpdate)
	{
		pRow = getForwardRowByInterfaceIndex(interfaceIndex);
		if (pRow != NULL) {
			toUpdate = false;
			// Delete the old default gateway entry.
			//dwStatus = DeleteIpForwardEntry(pRow);

			if (dwStatus != ERROR_SUCCESS) {
				if (pRow)
					free(pRow);
				printf("Could not delete old gateway\n");
				return hres;
			}
		}
	}

	if (pRow == NULL && !toUpdate) {
		pRow = (PMIB_IPFORWARDROW)malloc(sizeof(MIB_IPFORWARDROW));
		ZeroMemory(pRow, sizeof(MIB_IPFORWARDROW));
	}

	in_addr tmp_Addr;
	tmp_Addr.s_addr = inet_addr(mask.c_str());
	pRow->dwForwardMask = tmp_Addr.S_un.S_addr;
	tmp_Addr.s_addr = inet_addr(gatewayAddress.c_str());
	pRow->dwForwardNextHop = tmp_Addr.S_un.S_addr;
	tmp_Addr.s_addr = inet_addr(networkAddress.c_str());
	pRow->dwForwardDest = tmp_Addr.S_un.S_addr;
	pRow->dwForwardPolicy = 0;
	pRow->dwForwardIfIndex = (DWORD)interfaceIndex;

	pRow->dwForwardType = 4;
	pRow->dwForwardProto = MIB_IPPROTO_NETMGMT;
	//pRow->dwForwardAge = 0;
	pRow->dwForwardNextHopAS = 0;

	pRow->dwForwardMetric1 = (DWORD)metric;
	pRow->dwForwardMetric2 = 0;
	pRow->dwForwardMetric3 = 0;
	pRow->dwForwardMetric4 = 0;
	pRow->dwForwardMetric5 = 0;

	if (toUpdate) {
		dwStatus = SetIpForwardEntry(pRow);
	}
	else {
		dwStatus = CreateIpForwardEntry(pRow);
	}

	if (dwStatus == NO_ERROR)
		hres = S_OK;
	else if (dwStatus == ERROR_INVALID_PARAMETER) {
		printf("Invalid parameter.\n");
		if (dwStatus == ERROR_BAD_ARGUMENTS)
			printf("Bad arguments.\n");
		hres = dwStatus;
	}

	if (pRow)
		free(pRow);

	return hres;
}

PMIB_IPFORWARDROW WmiMgr::getForwardRowByDestAddress(string address) {
	PMIB_IPFORWARDROW pRow = NULL;
	in_addr tmp_Addr;
	tmp_Addr.s_addr = inet_addr(address.c_str());
	CheckForwardEntry pFunc = &WmiMgr::CheckDestAddress;

	pRow = this->getForwardRowByFunc(pFunc, tmp_Addr.s_addr);
	return pRow;
}

PMIB_IPFORWARDROW WmiMgr::getForwardRowByInterfaceIndex(int index) {
	PMIB_IPFORWARDROW pRow = NULL;
	CheckForwardEntry pFunc = &WmiMgr::CheckNetworkIndex;
	pRow = this->getForwardRowByFunc(pFunc, index);
	return pRow;
}

PMIB_IPFORWARDROW WmiMgr::getForwardRowByFunc(CheckForwardEntry pFunc, DWORD value) {
	PMIB_IPFORWARDROW pRow = NULL;
	PMIB_IPFORWARDTABLE pIpForwardTable = NULL;
	DWORD dwSize = 0;
	BOOL bOrder = FALSE;
	DWORD dwStatus = 0;

	// Find out how big our buffer needs to be.
	dwStatus = GetIpForwardTable(pIpForwardTable, &dwSize, bOrder);
	if (dwStatus == ERROR_INSUFFICIENT_BUFFER) {
		// Allocate the memory for the table
		if (!(pIpForwardTable = (PMIB_IPFORWARDTABLE)malloc(dwSize))) {
			printf("Malloc failed. Out of memory.\n");
			return pRow;
		}
		// Now get the table.
		dwStatus = GetIpForwardTable(pIpForwardTable, &dwSize, bOrder);
	}

	if (dwStatus != ERROR_SUCCESS) {
		printf("getIpForwardTable failed.\n");
		if (pIpForwardTable)
			free(pIpForwardTable);
		return pRow;
	}

	WmiMgr &inst = *this;

	for (int i = 0; i < pIpForwardTable->dwNumEntries; i++) {
		if ((inst.*pFunc)(&pIpForwardTable->table[i], value)) {
			//if (pIpForwardTable->table[i].dwForwardDest == 0 && pIpForwardTable->table[i].dwForwardIfIndex == index) {
			// We have found the default gateway.
			if (!pRow) {
				// Allocate some memory to store the row in; this is easier than filling
				// in the row structure ourselves, and we can be sure we change only the
				// gateway address.
				pRow = (PMIB_IPFORWARDROW)malloc(sizeof(MIB_IPFORWARDROW));
				if (!pRow) {
					printf("Malloc failed. Out of memory.\n");
					free(pIpForwardTable);
					return pRow;
				}
				// Copy the row
				memcpy(pRow, &(pIpForwardTable->table[i]),
					sizeof(MIB_IPFORWARDROW));
			}

			if (dwStatus != ERROR_SUCCESS) {
				printf("Could not delete old gateway\n");
				free(pIpForwardTable);
				return pRow;
			}
			break;
		}
	}
	return pRow;
}

bool WmiMgr::CheckDestAddress(PMIB_IPFORWARDROW pRow, DWORD address) {
	return pRow->dwForwardDest == address;
}

bool WmiMgr::CheckNetworkIndex(PMIB_IPFORWARDROW pRow, DWORD index) {
	if (index > 0) {
		return pRow->dwForwardDest == 0 && pRow->dwForwardIfIndex == index;
	}
	else
		return pRow->dwForwardDest == 0 && pRow->dwForwardMask == 0;
}

int WmiMgr::GetBestNetworkInterfaceIndex(string destAddress) {

	in_addr tmp_Addr;
	tmp_Addr.s_addr = inet_addr(destAddress.c_str());
	DWORD bestIfIndex = -1;
	
	if (SUCCEEDED(GetBestInterface(tmp_Addr.s_addr, &bestIfIndex)))
		return bestIfIndex;
	else
		return -1;
}

int WmiMgr::GetMetricByNetworkInterfaceIndex(int networkInterfaceIndex) {

	vector<AdapterEntity> aecol;
	int metric = 0;

	if (SUCCEEDED(getEnabledNetInterfaces(aecol, true))) {
		for (int j = 0; j < aecol.size(); j++) {
			if (aecol[j].InterfaceIndex == networkInterfaceIndex) {
				metric = aecol[j].Metric;
				break;
			}
		}
	}
	return metric;
}