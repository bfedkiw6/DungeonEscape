#pragma once

#include <chrono>
#include <format>
#include <iostream>
#include <source_location>
#include <string>
#include <glm/glm.hpp>

#ifndef NDEBUG
#define DEBUG_ENABLED 1
#else
#define DEBUG_ENABLED 0
#endif

/**
 * @brief Utility functions for debug logging and messages.
 *
 * This namespace provides functions to print debug, info, warning, and error messages
 * to the console. \n
 * @code print()@endcode and @code log()@endcode messages are only printed when the application is built in Debug mode (CMake Debug build). \n
 * The @code error()@endcode macro provides file and line information automatically.
 *
 * Use {} as format specifiers - each {} is replaced with the next argument.
 * @par Example:
 * @code debug::print("ID: {}, Color: {}", 42, glm::vec3(1, 0, 0)); @endcode
 * @par Output:
 * [DEBUG] ID: 42, Color: (1, 0, 0)
 */
namespace debug {
    // ANSI color codes for terminal output
    namespace Color {
        constexpr const char* RESET = "\033[0m";
        constexpr const char* RED = "\033[31m";
        constexpr const char* GREEN = "\033[32m";
        constexpr const char* YELLOW = "\033[33m";
        constexpr const char* BLUE = "\033[34m";
        constexpr const char* MAGENTA = "\033[35m";
        constexpr const char* CYAN = "\033[36m";
        constexpr const char* GRAY = "\033[90m";
    }

    /**
     * Prints a debug message to the console <b>ONLY</b> if CMake Debug build is enabled. \n
     * Use {} as format specifiers - each {} is replaced with the next argument.
     *
     * @par Example:
     * @code debug::print("ID: {}, Color: {}", 42, glm::vec3(1, 0, 0)); @endcode
     *
     * @par Output:
     * [DEBUG] ID: 42, Color: (1, 0, 0)
     *
     *
     * @param fmt Format string with {} placeholders
     * @param args Values to insert into placeholders
     */
    template <typename... Args>
    inline void print(std::format_string<Args...> fmt, Args&&... args) {
        if constexpr (DEBUG_ENABLED) {
            std::cout << Color::CYAN << "[DEBUG] " << Color::RESET
                << std::format(fmt, std::forward<Args>(args)...) << std::endl;
        }
    }

    /**
     * Logs a debug message with timestamp to the console <b>ONLY</b> if CMake Debug build is enabled. \n
     * Use {} as format specifiers - each {} is replaced with the next argument.
     *
     * @par Example:
     * @code debug::log("Position: {}", glm::vec2(1.5, 5)); @endcode
     *
     * @par Output:
     * [LOG] [12:34:56.7890] Pos: (1.5, 5)
     *
     *
     * @param fmt Format string with {} placeholders
     * @param args Values to insert into placeholders
     */
    template <typename... Args>
    inline void log(std::format_string<Args...> fmt, Args&&... args) {
        if constexpr (DEBUG_ENABLED) {
            // Get current time with milliseconds - fully cross-platform
            auto now = std::chrono::system_clock::now();
            // Format time using std::chrono - no platform-specific code needed
            auto time_str = std::format("{:%H:%M:%S}", now);

            std::cout << Color::GRAY << "[LOG] "
                      << std::format("[{}] ", time_str)
                      << Color::RESET
                      << std::format(fmt, std::forward<Args>(args)...) << std::endl;
        }
    }

    /**
     * Prints an info message to the console. \n
     * Use {} as format specifiers - each {} is replaced with the next argument.
     *
     * @par Example:
     * @code debug::info("Loading file: {}", file_name); @endcode
     *
     * @par Output:
     * [INFO] Loading file: example/file.txt
     *
     *
     * @param fmt Format string with {} placeholders
     * @param args Values to insert into placeholders
     */
    template <typename... Args>
    inline void info(std::format_string<Args...> fmt, Args&&... args) {
        std::cout << Color::GREEN << "[INFO] " << Color::RESET
            << std::format(fmt, std::forward<Args>(args)...) << std::endl;
    }

    /**
     * Prints a warning message to the console. \n
     * Use {} as format specifiers - each {} is replaced with the next argument.
     *
     * @par Example:
     * @code debug::warn("Unable to load file: {}", file_name); @endcode
     *
     * @par Output:
     * [WARN] Unable to load file: example/file.txt
     *
     *
     * @param fmt Format string with {} placeholders
     * @param args Values to insert into placeholders
     */
    template <typename... Args>
    inline void warn(std::format_string<Args...> fmt, Args&&... args) {
        std::cerr << Color::YELLOW << "[WARN] " << Color::RESET
            << std::format(fmt, std::forward<Args>(args)...) << std::endl;
    }

    /**
     * Internal error implementation - DO NOT CALL DIRECTLY.
     * Use the debug::error() macro instead (see below).
     */
    namespace internal {
        template <typename... Args>
         inline void impl(const std::source_location& loc, std::format_string<Args...> fmt, Args&&... args) {
            std::cerr << Color::RED << "[ERROR] " << Color::RESET
                << std::format(fmt, std::forward<Args>(args)...) << std::endl;
            std::cerr << Color::GRAY << "  at " << loc.file_name() << ":" << loc.line() << Color::RESET << std::endl;
        }
    }


    /**
     * @par Macro
     * Prints an error message and shows the file/line where it occurred.
     * Use {} as format specifiers - each {} is replaced with the next argument.
     *
     * @par Example:
     * @code debug::error("Failed to open file: {}", file_name); @endcode
     *
     * @par Output:
     * [ERROR] Failed to open file: example/file.txt \n at src/main.cpp:42
     *
     *
     * @param fmt Format string with {} placeholders
     * @param args Values to insert into placeholders
     */
#define error(...) internal::impl(std::source_location::current(), __VA_ARGS__)

}

// Custom formatter for glm vectors, can add more specializations as needed
template <glm::length_t L, typename T, glm::qualifier Q>
struct std::formatter<glm::vec<L, T, Q>> {
    static constexpr auto parse(format_parse_context& ctx) {
        return ctx.begin();
    }

    auto format(const glm::vec<L, T, Q>& vec, format_context& ctx) const {
        auto out = std::format_to(ctx.out(), "(");
        for (glm::length_t i = 0; i < L; ++i) {
            if (i > 0) out = std::format_to(out, ", ");
            out = std::format_to(out, "{}", vec[i]);
        }
        return std::format_to(out, ")");
    }
};
