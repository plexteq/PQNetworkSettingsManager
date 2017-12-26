// NativeNetSettingsAPI.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <vector>
#include <string>
#include <sstream>

using namespace std;

#include "ArgsParser.h"
#include "WmiMgr.h"
#include "OutPutPrinter.h"

int main(int argc, char * argv[]);

void TestArgsParser();

vector<string> split(string str, char delimiter) {
	vector<string> internal;
	stringstream ss(str); // Turn the string into a stream.
	string tok;

	while (getline(ss, tok, delimiter)) {
		internal.push_back(tok);
	}

	return internal;
}

string getStringFromNum(long code) {
	char buff[100];
	snprintf(buff, sizeof(buff), "%d", code);
	std::string buffAsStdStr = buff;
	return buffAsStdStr;
}


int main(int argc, char *argv[])
{
	//TestArgsParser();
	ArgsParser parser;
	WmiMgr wmi;
	OutPutPrinter printer;

	parser.parserCmd(argc, argv);
	ParamInfo pi = parser.getCmdParams();
	std::vector<AdapterEntity> collAE;
	std::vector<GateWayInfo> gateWays;
	HRESULT hr;

	switch (pi.type)
	{
		case QueryType::gai:
		{
			collAE = wmi.GetAllNetInterfaces();
			printer.printNetInterfaces(collAE);
			break;
		}
		case QueryType::goai: {
			AdapterEntity out;
			hr = wmi.GetNetInterfaceByID(std::wstring(pi.params[0].begin(), pi.params[0].end()), out);
			if (SUCCEEDED(hr)) {
				collAE.push_back(out);
				printer.printNetInterfaces(collAE);
			}
			break;
		}
		case QueryType::dts:{
			std::wstring UUID = std::wstring(pi.params[0].begin(), pi.params[0].end());
			hr = wmi.DhcpToStatic(UUID);
			if (SUCCEEDED(hr)) {
				printer.printMessage("Changes have been applied");
			}
			break;
		}
		case QueryType::stod: {
			std::wstring UUID = std::wstring(pi.params[0].begin(), pi.params[0].end());
			hr = wmi.StaticToDhcp(UUID);
			if (SUCCEEDED(hr)) {
				printer.printMessage("Changes have been applied");
			}
			break;
		}
		case QueryType::gdg: {
			hr = wmi.DefaultGateways(gateWays);
			if (SUCCEEDED(hr)) {
				printer.printDefaultGateways(gateWays);
			}
			break;
		}
		case QueryType::rdg: {
			hr = wmi.RemoveDefaultGateway();
			if (SUCCEEDED(hr)) {
				printer.printMessage("DefaultGetway was removed");
			}
			else {
				printer.printMessage("Failed to remove DefaultGetway" + getStringFromNum(hr));
			}
			break;
		}
		case QueryType::sdg: {
			if (pi.params.size() <= 4) {
				std::string address = pi.params[0];
				std::string mask = pi.params[1];
				int metric = atoi(pi.params[2].c_str());
				int interfaceIndex = -1;
				if (pi.params.size() >= 4)
					interfaceIndex = atoi(pi.params[3].c_str());

				hr = wmi.SetDefaultGateway(address, mask, metric, interfaceIndex);
				if (SUCCEEDED(hr)) {
					printer.printMessage("DefaultGetway was set");
				}
				else {
					printer.printMessage("Failed to set DefaultGetway. Error code: " + getStringFromNum(hr));
				}
			}
			else {
				printer.printMessage("Not all parameters were set");
			}
			
			break;
		}
		case QueryType::sdns: {

			std::wstring adresses = std::wstring(pi.params[0].begin(), pi.params[0].end());
			vector<string> sep = split(pi.params[0], ',');
			
			std::vector<std::wstring> addressArray;
			for (size_t i = 0; i < sep.size(); i++)
			{
				addressArray.push_back(std::wstring(sep[i].begin(), sep[i].end()));
			}
			
			hr = wmi.SetDNS(std::wstring(pi.params[1].begin(), pi.params[1].end()), addressArray);
			if (SUCCEEDED(hr)) {
				printer.printMessage("Changes have been applied");
			}
			else {
				printer.printMessage("Something wrong." + getStringFromNum(hr));
			}
			break;
		}
		case QueryType::sdga:{
			if (pi.params.size() == 2) {
				hr = wmi.SetDefaultGatewayForAddress(pi.params[0], pi.params[1]);
				if (SUCCEEDED(hr)) {
					printer.printMessage("Route Table was updated");
				}
				else {
					printer.printMessage("Something wrong." + getStringFromNum(hr));
				}
			}
			else
				printer.printMessage("Not all parameters were set");

			break;
		}
		case QueryType::rdga: {
			if (pi.params.size() == 1) {
				hr = wmi.RemoveDefaultGatewayForAddress(pi.params[0]);
				if (SUCCEEDED(hr)) {
					printer.printMessage("Geteway was removed. Address - " + pi.params[0]);
				}
				else {
					printer.printMessage("Failed to remove Gateway for address - " + pi.params[0]);
				}
			}
			
			break;
		}
		default:
			break;
	}
	return 0;
}

/*
This function is used as Unit Test
*/
void TestArgsParser() {
	int argc = 0;
	std::vector<char*> args;
	ArgsParser parser;
	ParamInfo pi;

	// Test cmd line with gai parameter

	// gai
	argc = 2;
	args.push_back("AppName.exe");
	args.push_back("/gai");

	parser.parserCmd(argc, &args[0]);
	pi = parser.getCmdParams();

	if (pi.type != QueryType::gai || pi.params.size() != 0)
		cout<< "-gai param info failed.." << endl;

	// goai {0EB0E025-3D65-4BFE-8F69-01A4B88F1A73}
	args.clear();
	argc = 3;
	args.push_back("AppName.exe");
	args.push_back("/goai");
	args.push_back("{0EB0E025-3D65-4BFE-8F69-01A4B88F1A73}");

	parser.parserCmd(argc, &args[0]);
	pi = parser.getCmdParams();

	if (pi.type != QueryType::goai || pi.params.size() != 1 || pi.params[0] != "{0EB0E025-3D65-4BFE-8F69-01A4B88F1A73}")
		cout << "-goai param info failed.." << endl;

	///sdns 8.8.8.8,8.8.8.4 {0EB0E025-3D65-4BFE-8F69-01A4B88F1A73}
	args.clear();
	argc = 4;
	args.push_back("AppName.exe");
	args.push_back("/sdns");
	args.push_back("8.8.8.8,8.8.8.4");
	args.push_back("{0EB0E025-3D65-4BFE-8F69-01A4B88F1A73}");

	parser.parserCmd(argc, &args[0]);
	pi = parser.getCmdParams();

	if (pi.type != QueryType::sdns || pi.params.size() != 2 || pi.params[0] != "8.8.8.8,8.8.8.4" || pi.params[1] != "{0EB0E025-3D65-4BFE-8F69-01A4B88F1A73}")
		cout << "-sdns param info failed.." << endl;

	// dts {0EB0E025-3D65-4BFE-8F69-01A4B88F1A73}
	args.clear();
	argc = 3;
	args.push_back("AppName.exe");
	args.push_back("/dts");
	args.push_back("{0EB0E025-3D65-4BFE-8F69-01A4B88F1A73}");

	parser.parserCmd(argc, &args[0]);
	pi = parser.getCmdParams();

	if (pi.type != QueryType::dts || pi.params.size() != 1 || pi.params[0] != "{0EB0E025-3D65-4BFE-8F69-01A4B88F1A73}")
		cout << "-dts param info failed.." << endl;

	// std { 0EB0E025 - 3D65 - 4BFE - 8F69 - 01A4B88F1A73 }
	args.clear();
	argc = 3;
	args.push_back("AppName.exe");
	args.push_back("/std");
	args.push_back("{0EB0E025-3D65-4BFE-8F69-01A4B88F1A73}");

	parser.parserCmd(argc, &args[0]);
	pi = parser.getCmdParams();

	if (pi.type != QueryType::stod || pi.params.size() != 1 || pi.params[0] != "{0EB0E025-3D65-4BFE-8F69-01A4B88F1A73}")
		cout << "-std param info failed.." << endl;

	///gdg
	args.clear();
	argc = 2;
	args.push_back("AppName.exe");
	args.push_back("/gdg");

	parser.parserCmd(argc, &args[0]);
	pi = parser.getCmdParams();

	if (pi.type != QueryType::gdg || pi.params.size() != 0)
		cout << "-gdg param info failed.." << endl;

	// sdg 192.168.8.8 0.0.0.0 12
	args.clear();
	argc = 5;
	args.push_back("AppName.exe");
	args.push_back("/sdg");
	args.push_back("192.168.8.8");
	args.push_back("0.0.0.0");
	args.push_back("12");


	parser.parserCmd(argc, &args[0]);
	pi = parser.getCmdParams();

	if (pi.type != QueryType::sdg || pi.params.size() != 3 || pi.params[0] != "192.168.8.8" || pi.params[1] != "0.0.0.0" || pi.params[2] != "12")
		cout << "-sdg param info failed.." << endl;

	// sdga 8.8.8.8 192.168.25.16
	args.clear();
	argc = 4;
	args.push_back("AppName.exe");
	args.push_back("/sdga");
	args.push_back("8.8.8.8");
	args.push_back("192.168.25.16");

	parser.parserCmd(argc, &args[0]);
	pi = parser.getCmdParams();

	if (pi.type != QueryType::sdga || pi.params.size() != 2 || pi.params[0] != "8.8.8.8" || pi.params[1] != "192.168.25.16")
		cout << "-sdga param info failed.." << endl;

	///rdg
	args.clear();
	argc = 2;
	args.push_back("AppName.exe");
	args.push_back("/rdg");

	parser.parserCmd(argc, &args[0]);
	pi = parser.getCmdParams();

	if (pi.type != QueryType::rdg || pi.params.size() != 0)
		cout << "-rdg param info failed.." << endl;

	cout << "Press eny keys to exit" << endl;
	cin.get();
}