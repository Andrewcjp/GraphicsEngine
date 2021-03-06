#pragma once
#include "Core/Engine.h"
#include "Core\Platform\PlatformCore.h"
static inline const char * GetFilename(const char * name)
{
	for (const char * Ptr = name; *Ptr != '\0'; ++Ptr)
	{
		if (*Ptr == '\\' || *Ptr == '/')
		{
			name = Ptr + 1;
		}
	}
	return name;
}
template<typename ... Args>
std::string string_format(const std::string& format, Args ... args)
{
	size_t size = snprintf(nullptr, 0, format.c_str(), args ...) + 1; // Extra space for '\0'
	if (size <= 0) { throw std::runtime_error("Error during formatting."); }
	std::unique_ptr<char[]> buf(new char[size]);
	snprintf(buf.get(), size, format.c_str(), args ...);
	return std::string(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside
}
#ifdef PLATFORM_WINDOWS
#define DEBUGBREAK  __debugbreak();
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#else
#define DEBUGBREAK  __debugbreak();
#endif
#define STRINGIZE(x) STRINGIZE2(x)
#define STRINGIZE2(x) #x
#define LINE_STRING STRINGIZE(__LINE__)
#if !BUILD_SHIPPING
#define DebugEnsure(condition) if(!condition){DEBUGBREAK}
#define AssertDebugBreak() if(PlatformApplication::IsDebuggerPresent()){DEBUGBREAK} 
#else
#define DebugEnsure(condition);
#define AssertDebugBreak();
#endif
#define FILE_NAME std::string(GetFilename(__FILE__))+std::string(":")+ std::string(LINE_STRING)
#define ASSERT_STRING(msg,con,x) std::string _AssertMSG_ = std::string(msg)+std::string(con)+std::string(x)+std::string("\nFile: ")+ FILE_NAME;
#define ASSERT_STRING_LOG(x) std::string _LOGMSG_ = "["+std::string(FILE_NAME)+"]: "+std::string(x);
#define VA_ARGS(...) , ##__VA_ARGS__

#if DOFULLCHECK
#define ensure(condition) {if(!(condition)){ AssertDebugBreak();ASSERT_STRING("Ensure Failed: ", #condition, "");\
Log::LogMessage(_AssertMSG_,Log::Severity::Error); PlatformApplication::DisplayAssertBox("Fatal Error", _AssertMSG_); }}

#define ensureMsgf(condition,Message) ensureFatalMsgf(condition,Message);

#define AD_Assert(condition,Message) if(!(condition)){AssertDebugBreak();ASSERT_STRING("Assert Failed: ",#condition" ",Message);\
Log::LogMessage(_AssertMSG_,Log::Severity::Error); PlatformApplication::DisplayAssertBox("Error",_AssertMSG_);}

#define AD_Assert_Always(Message) {AssertDebugBreak();ASSERT_STRING("Assert Failed: ","",Message);\
Log::LogMessage(_AssertMSG_,Log::Severity::Error); PlatformApplication::DisplayAssertBox("Error",_AssertMSG_);}


#define AD_ERROR(Message,...) ASSERT_STRING_LOG(string_format(Message VA_ARGS(__VA_ARGS__))); Log::LogMessage(_LOGMSG_ ,Log::Severity::Error);
#define AD_WARN(Message,...) ASSERT_STRING_LOG(string_format(Message VA_ARGS(__VA_ARGS__))); Log::LogMessage(_LOGMSG_ ,Log::Severity::Warning);

#define LogEnsure(condition,...) if(!(condition)){AD_ERROR(#condition,)};
#define LogEnsureMsgf(condition,Message,...) if(!(condition)){AD_ERROR(#condition" "Message,__VA_ARGS__)};
#define LogEnsure_Always(Message,...) AD_ERROR(Message,__VA_ARGS__);

#define ENUMCONVERTFAIL() AD_Assert_Always("Failed to convert Enum")
#define NoImpl() AD_Assert_Always("Feature Not Implmented \n");
#else
#define AD_Assert(condition);
#define checkMsgf(condition,Message);
#define ensure(condition);
#define ensureMsgf(condition,Message);
#define NoImpl();
#define LogEnsure(condition);
#define LogEnsureMsgf(c,m);
#define LogEnsure_Always(m)
#define ENUMCONVERTFAIL()
#define AD_WARN(msg,...)
#define AD_ERROR(Message,...)
#define AD_Assert_Always(Message)
#endif

#if DOCHECK
#define ensureFatalMsgf(condition,Message) if(!(condition)){ AssertDebugBreak();ASSERT_STRING("Assert Failed: ",#condition " ",Message);\
Log::LogMessage(_AssertMSG_,Log::Severity::Error); PlatformApplication::DisplayMessageBox("Fatal Error",_AssertMSG_);__debugbreak();}
#else
#define ensureFatalMsgf(condition,Message);
#endif


