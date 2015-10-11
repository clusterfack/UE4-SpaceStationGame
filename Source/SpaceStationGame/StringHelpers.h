// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include <string>

/**
*
*/

class StringHelpers
{
public:
	static std::wstring widen(const std::string& str);
	static std::string narrow(const std::wstring& str);


	static std::wstring ConvertToWString(FString str);
	static std::string ConvertToString(FString str);
	static FString ConvertToFString(std::string str);
	static FString ConvertToFString(std::wstring str);

	static const char* ConvertToCCP(FString str);
	static const char* ConvertToCCP(std::string str);
	static const char* ConvertToCCP(std::wstring str);


	static std::string int64ToString(int64 number);
	static std::wstring int64ToWString(int64 number);
};