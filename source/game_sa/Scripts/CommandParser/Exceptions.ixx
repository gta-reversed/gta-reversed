export module notsa.script.CommandParser.Exceptions;

#include <exception>
#include <string>
#include <format>

export namespace notsa::script::command_parser::exceptions {
    /*!
     * @brief Command is not implemented yet.
     */
    export class CommandNotImplemented : public std::exception {
        using std::exception::exception;

        const char* what() const noexcept override {
            return "Command not implemented";
        }
    };

    /*!
     * @brief Command arguments are invalid (e.g. out of range, or wrong type).
     */
    export class InvalidCommandArgument : public std::exception {
        /*!
         * @param argument The argument's name that is invalid.
         * @param reason The reason why the argument is invalid. (Eg.: `Expected value in range [0, 10]`, or `Expected a vehicle model name`, etc...)
         */
        InvalidCommandArgument(std::string_view argument, std::string_view reason) :
            m_What(std::format("Invalid argument `{}` - {}", argument, reason)) {}

        const char* what() const noexcept override {
            return m_What.c_str();
        }

    private:
        std::string m_What;
    };
}
