/*
 * Copyright (c) 2021 MUSIC Tribe
 *
 * This file is part of Kick.
 *
 * Kick is free software: you can redistribute it and/or modify it under the
 * terms of the GNU Affero General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * Kick is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for
 * more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with Kick. If not, see <https://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#pragma once

#define REGISER_FACTORY(baseClass, derivedClass, type) \
    namespace { auto registry_ ## baseClass ## derivedClass = ConcreteFactory<baseClass, derivedClass>(type);  }

template<typename Base>
class Factory {
public:
    using FactoryMap = std::map<int, Factory<Base>*>;
    
    static FactoryMap& getFactoryMap()
    {
        static FactoryMap fMap;
        return fMap;
    }
    
    static bool registerFactory(int type, Factory<Base>* factory)
    {
        FactoryMap& fMap = getFactoryMap();
        fMap[type] = factory;
        return true;
    }

    static Base* makeObject(int type)
    {
        auto it = getFactoryMap().find(type);
        if (it != getFactoryMap().end())
            return it->second->create();
        
        return nullptr;
    }
        
    virtual Base* create() const = 0;

private:
};

template<typename Base,  typename DerivedClass>
class ConcreteFactory : Factory<Base>
{
public:
    // Register this global object on the Factory register
    explicit ConcreteFactory(int type) {
        Factory<Base>::registerFactory(type, this);
    }

    Base* create() const override
    {
        return new DerivedClass;
    }
};
