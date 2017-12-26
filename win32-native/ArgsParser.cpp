#include "stdafx.h"
#include "ArgsParser.h"


ArgsParser::ArgsParser()
{
}

ArgsParser::~ArgsParser()
{
}

void ArgsParser::parserCmd(int &argc, char **argv) {
	this->tokens.clear();
	for (int i = 1; i < argc; ++i)
		this->tokens.push_back(string(argv[i]));

	pi.params.clear();
	pi.type = getQueryType();
	pi.params = getParamsBelongQueryType(pi.type);
}

ParamInfo ArgsParser::getCmdParams() {
	return this->pi;
}

std::vector<string> ArgsParser::getParamsBelongQueryType(QueryType type) {

	std::vector<string> params;
	std::vector<string>::const_iterator itr;
	itr = std::find(this->tokens.begin(), this->tokens.end(), getTypeAsStr(type));

	if (itr != this->tokens.end()) {
		itr++;
		for (std::vector<string>::const_iterator i = itr; i != this->tokens.end(); i++) {
			params.push_back(*i);
		}
	}

	return params;
}

bool ArgsParser::cmdOptionExists(const std::string &option) {
	return std::find(this->tokens.begin(), this->tokens.end(), option) != this->tokens.end();
}

QueryType ArgsParser::getQueryType() {

	if (this->cmdOptionExists(this->getTypeAsStr(QueryType::dts)))
		return QueryType::dts;
	if (this->cmdOptionExists(this->getTypeAsStr(QueryType::gai)))
		return QueryType::gai;
	if (this->cmdOptionExists(this->getTypeAsStr(QueryType::gdg)))
		return QueryType::gdg;
	if (this->cmdOptionExists(this->getTypeAsStr(QueryType::goai)))
		return QueryType::goai;
	if (this->cmdOptionExists(this->getTypeAsStr(QueryType::rdg)))
		return QueryType::rdg;
	if (this->cmdOptionExists(this->getTypeAsStr(QueryType::sdg)))
		return QueryType::sdg;
	if (this->cmdOptionExists(this->getTypeAsStr(QueryType::sdns)))
		return QueryType::sdns;
	if (this->cmdOptionExists(this->getTypeAsStr(QueryType::stod)))
		return QueryType::stod;
	if (this->cmdOptionExists(this->getTypeAsStr(QueryType::sdga)))
		return QueryType::sdga;
	if (this->cmdOptionExists(this->getTypeAsStr(QueryType::rdga)))
		return QueryType::rdga;


	return QueryType::none;
}

string ArgsParser::getTypeAsStr(QueryType type) {
	switch (type)
	{
	case QueryType::gai:
		return "/gai";
	case QueryType::dts:
		return "/dts";
	case QueryType::gdg:
		return "/gdg";
	case QueryType::goai:
		return "/goai";
	case QueryType::none:
		return "/none";
	case QueryType::rdg:
		return "/rdg";
	case QueryType::sdg:
		return "/sdg";
	case QueryType::sdns:
		return "/sdns";
	case QueryType::stod:
		return "/std";
	case QueryType::sdga:
		return "/sdga";
	case QueryType::rdga:
		return "/rdga";
	default:
		break;
	}
	return "";
}

