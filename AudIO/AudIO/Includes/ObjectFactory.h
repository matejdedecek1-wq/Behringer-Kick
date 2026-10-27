#pragma once

#include "Platform.h"
#include "AudioDataTypes.h"

#include <string>
#include <functional>
#include <memory>
#include <map>

#define REGISTER_OBJECT( type, Object )                                   static AudIO::ObjectFactoryRegistration<Object> _##Object( type )
#define REGISTER_OBJECT_WITH_ARG( type, Object, Arg )                     static AudIO::ObjectFactoryRegistration<Object> _##Object( type, Arg )

#define REGISTER_OBJECT_WITH_ARGS( type, Object, Arg, Man, Cat, ObjType ) static AudIO::ObjectFactoryRegistration<Object> _##Object( type, Arg, Man, Cat, ObjType )

#define CREATE_OBJECT( Type )                                             AudIO::ObjectFactory::Get().Create( Type )

#define ADD_OBJECT_INFO( Type, M, T, C, N )                               AudIO::ObjectFactory::Get().AddInfo( Type, { M, T, C, N } )
#define GET_OBJECT_INFO()                                                 AudIO::ObjectFactory::Get().GetObjectsInfo()

namespace AudIO
{

    class Object;
    class ObjectFactory
    {
    public:
        using TCreateMethod = std::function<std::unique_ptr<Object>()>;
        using TCreatorMap   = std::map<const std::string, TCreateMethod>;

        using TObjectFactoryMap = std::map<const std::string, ObjectInfo>;
        using TOFMultiMap       = std::multimap<const std::string, ObjectInfo>;

        static ObjectFactory& Get()
        {
            static ObjectFactory _instance;
            return _instance;
        }

        bool Register( const std::string& type, const TCreateMethod& creator )
        {
            if( auto it = m_factory.find( std::string( type ) ); it == m_factory.end() )
            {
                m_factory[std::string( type )] = creator;
                return true;
            }
            return false;
        }

        void AddInfo( const std::string& type, const ObjectInfo& info )
        {
            m_factoryObjectInfo[std::string( type )] = info;
            m_factoryManufMap.emplace( std::make_pair( info.Manufacturer, info ) );
            m_factoryCategoryMap.emplace( std::make_pair( info.Category, info ) );
        }

        const TObjectFactoryMap& GetObjectsInfo() const { return m_factoryObjectInfo; }
        const TOFMultiMap&       GetManufacturerMap() const { return m_factoryManufMap; }
        const TOFMultiMap&       GetCategoryMap() const { return m_factoryCategoryMap; }

        std::unique_ptr<Object> Create( const std::string& type )
        {
            autolock guard{ m_lock };
            if( auto it = m_factory.find( std::string( type ) ); it != m_factory.end() )
            {
                return it->second();
            }
            return nullptr;
        }

        std::vector<std::string> List() const
        {
            std::vector<std::string> list;
            for( auto& [identifier, ceator] : m_factory )
            {
                list.push_back( identifier );
            }
            return list;
        }

    private:
        ObjectFactory()                       = default;
        ~ObjectFactory()                      = default;
        ObjectFactory( const ObjectFactory& ) = default;

        TCreatorMap       m_factory;
        TObjectFactoryMap m_factoryObjectInfo;
        TOFMultiMap       m_factoryManufMap;
        TOFMultiMap       m_factoryCategoryMap;
        recursive_mutex   m_lock;
    };

    template<class T>
    class ObjectFactoryRegistration
    {
    public:
        explicit ObjectFactoryRegistration( const char* type )
        {
            ObjectFactory::Get().Register(
                std::string( type ),
                []() { return std::make_unique<T>(); } );
        }
        explicit ObjectFactoryRegistration( const char* type, int arg )
        {
            ObjectFactory::Get().Register(
                std::string( type ),
                [arg]() { return std::make_unique<T>( arg ); } );
        }
        explicit ObjectFactoryRegistration( const char* type, int arg, const char* manufacturer, const char* category, const char* objectType )
        {
            auto ret = ObjectFactory::Get().Register(
                std::string( type ),
                [arg]() { return std::make_unique<T>( arg ); } );

            if( ret )
                ObjectFactory::Get().AddInfo( type, { manufacturer, objectType, category, type } );
        }
    };
} // Namespace AudIO
