// Fill out your copyright notice in the Description page of Project Settings.

#include "SpaceStationGame.h"
#include "StringHelpers.h"
#include <iostream>
#include <sstream>
#include <string.h>
#include <locale.h>


using namespace std;

std::wstring StringHelpers::widen(const std::string& str)
{
	wostringstream wstm;
	const ctype<wchar_t>& ctfacet =
		use_facet< ctype<wchar_t> >(wstm.getloc());
	for (size_t i = 0; i < str.size(); ++i)
		wstm << ctfacet.widen(str[i]);
	return wstm.str();
};

std::string StringHelpers::narrow(const std::wstring& str)
{
	ostringstream stm;
	const ctype<char>& ctfacet =
		use_facet< ctype<char> >(stm.getloc());
	for (size_t i = 0; i < str.size(); ++i)
		stm << ctfacet.narrow(str[i], 0);
	return stm.str();
};

std::wstring StringHelpers::ConvertToWString(FString str)
{
	return StringHelpers::widen((TCHAR_TO_UTF8(*str)));
};

std::string StringHelpers::ConvertToString(FString str)
{
	return (TCHAR_TO_UTF8(*str));
};

FString StringHelpers::ConvertToFString(std::string str)
{
	return (FString(str.c_str()));
};

FString StringHelpers::ConvertToFString(std::wstring str)
{
	string o = narrow(str);
	return (FString(o.c_str()));
};

const char* StringHelpers::ConvertToCCP(FString str)
{
	string t = TCHAR_TO_UTF8(*str);
	char * returnvalue = (char *)malloc(sizeof(char)* (t.length() + 1));
	strncpy_s(returnvalue, t.length(), t.c_str(), t.length());
	return returnvalue;
}

const char* StringHelpers::ConvertToCCP(std::string str)
{
	char * returnvalue = (char *)malloc(sizeof(char)* (str.length() + 1));
	strncpy_s(returnvalue, str.length(), str.c_str(), str.length());
	return returnvalue;
}

const char* StringHelpers::ConvertToCCP(std::wstring str)
{
	std::string t = narrow(str);
	char * returnvalue = (char *)malloc(sizeof(char)* (t.length() + 1));
	strncpy_s(returnvalue, t.length(), t.c_str(), t.length());
	return returnvalue;
}


std::string StringHelpers::int64ToString(int64 number)
{
	std::stringstream stream;
	__int64 value(number);
	stream << value;
	std::string strValue(stream.str());
	return strValue;
};

std::wstring StringHelpers::int64ToWString(int64 number)
{
	std::stringstream stream;
	__int64 value(number);
	stream << value;
	std::string strValue(stream.str());
	return widen(strValue);
}
