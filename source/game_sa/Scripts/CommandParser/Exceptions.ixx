export module notsa.script.CommandParser.Exceptions;

#include <exception>

export namespace notsa::script::command_parser::exceptions {
    /*!
     * @brief Command is not implemented yet.
     */
    export class CommandNotImplemented : public std::exception {
        using std::exception::exception;
    };
}
