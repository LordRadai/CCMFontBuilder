#include "Debug.h"

void Debug::DebuggerMessage(MsgLevel level, const char* fmt, ...)
{
	va_list args;
	__va_start(&args, fmt);
	std::string msg_level;
	std::string msg_body;
	std::string thread_id = std::to_string(GetCurrentThreadId());

	char msg[256];

	switch (level)
	{
	case LVL_DEBUG:
		msg_level = "[DEBUG]";
		break;
	case LVL_INFO:
		msg_level = "[INFO]";
		break;
	case LVL_WARN:
		msg_level = "[WARN]";
		break;
	case LVL_ERROR:
		msg_level = "[ERROR]";
		break;
	default:
		break;
	}

	msg_body = fmt;
	vsprintf_s(msg, ("(threadId=" + thread_id + ")" +  " " + msg_level + " " + msg_body).c_str(), args);

#ifdef _CONSOLE
	printf_s(msg);
#endif

	std::string txt_log_msg = std::string(msg);

	OutputDebugStringA(msg);
}

void Debug::Panic(const char* src_module, const char* fmt, ...)
{
	va_list args;
	__va_start(&args, fmt);

	char msg[256];

	vsprintf_s(msg, fmt, args);

	std::string txt_log_msg = "FATAL ERROR:" + std::string(msg);
	txt_log_msg.erase(std::remove(txt_log_msg.begin(), txt_log_msg.end(), '\n'));

	try
	{
		throw std::exception(msg);
	}
	catch (std::exception)
	{
		ShowCursor(true);
		MessageBoxA(NULL, msg, src_module, MB_ICONERROR);
	}

	abort();
}

void Debug::Alert(MsgLevel level, const char* src_module, const char* fmt, ...)
{
	va_list args;
	__va_start(&args, fmt);

	char msg[256];
	UINT m_colliderType = MB_ICONASTERISK;

	vsprintf_s(msg, fmt, args);

	Debug::DebuggerMessage(level, fmt, args);

	switch (level)
	{
	case Debug::LVL_DEBUG:
		m_colliderType = MB_ICONINFORMATION;
		break;
	case Debug::LVL_INFO:
		m_colliderType = MB_ICONINFORMATION;
		break;
	case Debug::LVL_WARN:
		m_colliderType = MB_ICONEXCLAMATION;
		break;
	case Debug::LVL_ERROR:
		m_colliderType = MB_ICONERROR;
		break;
	default:
		Panic("Debug.cpp", "Invalid debug level\n");
		return;
	}

	ShowCursor(true);
	MessageBoxA(NULL, msg, src_module, m_colliderType);
}