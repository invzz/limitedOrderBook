#pragma once
#include <any>

namespace market
{

    /**
     * @class BaseCommand
     * @brief Abstract base class for command pattern implementation.
     *
     * This class serves as an interface for all command objects in the system.
     * It provides a pure virtual function `execute` that must be overridden
     * by derived classes to define specific command behavior.
     */
    class BaseCommand
    {
        public:
        virtual ~BaseCommand()                     = default;
        virtual void execute(const std::any &body) = 0;
    };

    /**
     * @brief A template class that represents a command.
     *
     * This class inherits from BaseCommand and provides an interface for executing commands
     * with a specific type.
     *
     * @tparam T The type of the command body.
     */
    template <typename T> class Command : public BaseCommand
    {
        public:
        virtual ~Command()                  = default;
        virtual void execute(const T &body) = 0;

        void execute(const std::any &body) override { execute(std::any_cast<const T &>(body)); }
    };
} // namespace market