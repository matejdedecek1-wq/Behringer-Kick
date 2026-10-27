#pragma once

#include <string>
#include <functional>
#include <vector>
#include <unordered_map>
#include <thread>

namespace AudIO{

    typedef std::function<void( const std::vector<std::string>& ) > CmdHandler_t;

    struct ConsoleCommand{
        std::string     cmd;
        std::string     desc;
        CmdHandler_t    handler;
    };

    class Console{
    public:
        static Console& get();
        ~Console();

        void addCommand( const std::string& Command, CmdHandler_t Func, const std::string& Description = "" );
        void removeCommand( const std::string& Command );
        void clear();
        void help();

        void reInit();
        void close();

    private:
        Console();
        void run();
        int parse( const std::string& Input );
        static void exec( Console* thread );

        std::unordered_map<std::string, ConsoleCommand> commands;
        std::thread* pThread{ nullptr };
    };

} // namespace AudIO
