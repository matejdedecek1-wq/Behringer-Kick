#include "Console.h"

#include <sstream>
#include <iostream>
#include <iomanip>

namespace AudIO {

    Console& Console::get(){
        static Console _instance;
        return _instance;
    }
    
    Console::Console(){
        addCommand( "help", [&]( const std::vector<std::string>& ){ help(); }, "Displays Help" );
        addCommand( "?", [&]( const std::vector<std::string>& ){ help(); }, "Displays Help" );
        addCommand("clear", [&](const std::vector<std::string>&) { clear(); }, "Displays Help");
    }

    Console::~Console(){
        close();
    }

    void Console::reInit(){
        // Close currently thread if at all...
        close();
        // Restart console thread...
        if( nullptr == pThread ){
            pThread = new std::thread( Console::exec, this );
            if( pThread )
                pThread->detach();
        }
    }

    void Console::close(){
        if( nullptr != pThread ){
            pThread->join();
            delete pThread;
            pThread = nullptr;
        }
    }

    void Console::exec( Console* thread ){
        if( thread )    thread->run();
    }

    void Console::addCommand( const std::string& Command, CmdHandler_t Func, const std::string& Description ){
        commands[Command] = ConsoleCommand{ Command, Description, Func };
    }

    void Console::removeCommand( const std::string& Command ){
        commands.erase( Command );
    }

    void Console::clear() {
        std::cout << "\x1B[2J\x1B[H";
    }

    void Console::help(){
        for( const auto& it : commands ){
            std::cout << std::left
                << std::setw( 20 ) << it.second.cmd
                << std::setw( 40 ) << it.second.desc
                << std::endl;
        }
    }

    void Console::run(){

        std::string userInput;
        help();
        while( 1 ){
            std::getline( std::cin, userInput );
            if(userInput.size() && 0 != parse( userInput ) )
                break;
        }
    }

    int Console::parse( const std::string& Input ){
        std::vector<std::string> result;
        std::stringstream ss( Input );
        std::string item;

        while( std::getline( ss, item, ' ' ) ){
            if( item.size() ){
                result.push_back( item );
                //std::cout << item << std::endl;
            }
        }

        if( result.size() ){
            std::string cmd = result.front();
            result.erase( result.begin() );

            const auto& it = commands.find( cmd );
            if( it != commands.end() ){
                it->second.handler( result );
                std::cout << std::endl;
            }
            else if( (0 == cmd.compare( "Q" ))
                || (0 == cmd.compare( "q" ))
                || (0 == cmd.compare( "exit" ))
                || (0 == cmd.compare( "EXIT" ))
                || (0 == cmd.compare( "quit" ))
                || (0 == cmd.compare( "QUIT" ))
                ){
                std::cout << "Closing Console..." << std::endl;
                return -1;
            } else{
                std::cout << "Unknown Command (" << cmd << ")..." << std::endl;
            }
        }
        return 0;
    }

} // namespace AudIO
