#pragma once
#include <mutex>

namespace AudIO
{
    using recursive_mutex = std::recursive_mutex;
    //using autolock = std::lock_guard<recursive_mutex>;
    using autolock = std::scoped_lock<recursive_mutex>;
}
