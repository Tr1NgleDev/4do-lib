#pragma once

#include "basicIncludes.h"

namespace fdo
{
	class Logger
	{
	private:
		inline static size_t _errors = 0;
		inline static size_t _warnings = 0;
		inline static size_t _messages = 0;
	public:
		// Toggles ANSI colors on logs.
		inline static bool ansiColors = false;
		// Toggles logging to console.
		inline static bool logToConsole = false;
		// Will contain any thrown errors/warnings/messages. May contain ANSI escape codes if `ansiColors` is set to `true`
		inline static std::vector<std::string> logs{};

		/**
		 * Logs an error.
		 * @param msg The contents of the error.
		 * @see fdo::Logger::errors
		 */
		inline static void logError(const std::string& msg)
		{
			std::string formatted =
				ansiColors
				? std::format("\x1B[37;1m4DO-Lib: \x1B[31;5mError: \x1B[0m{}", msg)
				: std::format("4DO-Lib: Error: {}", msg);

			if(logToConsole) printf("%s\n", formatted.c_str());
			logs.push_back(formatted);

			_errors++;
		}

		/**
		 * Logs a warning.
		 * @param msg The contents of the warning.
		 * @see fdo::Logger::warnings
		 */
		inline static void logWarning(const std::string& msg)
		{
			std::string formatted =
				ansiColors
				? std::format("\x1B[37;1m4DO-Lib: \x1B[33;1mWarning: \x1B[0m{}", msg)
				: std::format("4DO-Lib: Warning: {}", msg);

			if(logToConsole) printf("%s\n", formatted.c_str());
			logs.push_back(formatted);

			_warnings++;
		}

		/**
		 * Logs a message/note.
		 * @param msg The contents of the message/note.
		 * @see fdo::Logger::messages
		 */
		inline static void logMessage(const std::string& msg)
		{
			std::string formatted =
				ansiColors
				? std::format("\x1B[37;1m4DO-Lib: \x1B[37;0mMessage: \x1B[0m{}", msg)
				: std::format("4DO-Lib: Message: {}", msg);

			if(logToConsole) printf("%s\n", formatted.c_str());
			logs.push_back(formatted);

			_messages++;
		}

		/**
		 * @returns The amount of errors logged since last check.
		 * @see fdo::Logger::logError
		 */
		inline static size_t errors()
		{
			size_t result = _errors;
			_errors = 0;
			return result;
		}
		/**
		 * @returns The amount of warnings logged since last check.
		 * @see fdo::Logger::logWarning
		 */
		inline static size_t warnings()
		{
			size_t result = _warnings;
			_warnings = 0;
			return result;
		}
		/**
		 * @returns The amount of messages logged since last check.
		 * @see fdo::Logger::logMessage
		 */
		inline static size_t messages()
		{
			size_t result = _messages;
			_messages = 0;
			return result;
		}
	};
}
