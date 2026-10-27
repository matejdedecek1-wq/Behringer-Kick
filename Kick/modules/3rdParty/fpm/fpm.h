
#pragma once

#include "math.hpp"

namespace fpm
{
    template<typename T>
    T fast_log10( T Value ) noexcept
    {
        using P = fpm::fixed<std::int32_t, std::int64_t, 12>;
        return static_cast<T>( fpm::log10( P( Value ) ) );
    }

    template<typename T>
    T fast_pow( T Base, T Value ) noexcept
    {
        using P = fpm::fixed<std::int32_t, std::int64_t, 12>;
        return static_cast<T>( fpm::pow( P( Base ), P( Value ) ) );
    }

}
