#pragma once

#include <string>
#include <vector>

using namespace std;

/** String to integer, returns Fail on fail. */
inline int StoI(const string& S, int Fail = 0)
{
	char* EP;
	const char* P = S.c_str();
	int R = strtol(P, &EP, 0);
	if (EP == P)
		return Fail;
	return R;
}

/** String to double, returns Fail on fail. */
inline double StoD(const string& S, double Fail = 0.0)
{
	char* EP;
	const char* P = S.c_str();
	double R = strtod(P, &EP);
	if (EP == P)
		return Fail;
	return R;
}

/** Split a string. */
inline vector<string> Split(const string& str, const string& delim)
{
	vector<string> ret;
	
	unsigned int offset = 0;
	
	for (unsigned int delimIndex = str.find(delim, offset); delimIndex != string::npos;
	     delimIndex = str.find(delim, offset))
	{
		ret.push_back(str.substr(offset, delimIndex - offset));
		offset += delimIndex - offset + delim.length();
	}
	
	ret.push_back(str.substr(offset));
	
	return ret;
}
