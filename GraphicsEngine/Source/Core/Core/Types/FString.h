#pragma once

class  FString
{
public:
	FString();
	CORE_API FString(std::string input, bool Hash = false);
	CORE_API FString(const char* text, bool Hash = false);
	CORE_API ~FString();
	CORE_API bool Contains(FString Target);
	std::string& ToSString();
	std::wstring& ToWideString();
	CORE_API bool operator==(const FString &b) const;
	CORE_API void SetupHash();
#if RUNTESTS
	static void RunFStringTests();
#endif
private:
#if RUNTESTS
	static void RunPerfTests();
#endif
	bool WideCached = false;
	std::string UnderlyingString;
	std::wstring WideString;
	int64_t Hash = -1;
};

