#pragma once

#include <string>
#include <vector>
using namespace std;

enum QueryType {
	none = 0, // none
	gai, // Get information about all available interfaces
	goai, // Get information about specified interface.
	sdns, // Change DNS Servers
	dts, // Switch from dhcp to static mode
	stod, // Switch from static to dhcp mode
	gdg, // Get DefaultGateway and Metric 
	sdg, // Change DefaultGateway 
	rdg, // Removes DefaultGateway 
	sdga, // Set Default Gateway for specific address
	rdga // Removes Gateway for specific address
};

struct ParamInfo {
	QueryType  type;
	std::vector<string> params;
};

class ArgsParser
{
public:
	ArgsParser();
	~ArgsParser();
	void parserCmd(int &argc, char **argv);
	ParamInfo getCmdParams();

private:
	QueryType getQueryType();
	std::vector<string> getParamsBelongQueryType(QueryType type);
	string getTypeAsStr(QueryType type);
	bool cmdOptionExists(const std::string &option);

private:
	string emptyStr;
	std::vector<string> tokens;
	ParamInfo pi;
};

