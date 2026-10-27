#pragma once

#include <array>
#include <atomic>

namespace AudIO {
template <typename T, size_t size>
class SPSCQUeue {
public:
    bool push(const T& dataIn)
    {
        auto oldWritePos = writePos.load();
        auto newWritePos = getNextPosition(oldWritePos);
        if (newWritePos == readPos.load())
            return false;
        ringBuffer[oldWritePos] = dataIn;
        writePos.store(newWritePos);
        return true;
    }
    bool pop(T& dataOut)
    {
        auto oldWritePos = writePos.load();
        auto oldReadPos = readPos.load();
        if (oldReadPos == oldWritePos)
            return false;
        dataOut = std::move(ringBuffer[oldReadPos]);
        readPos.store(getNextPosition(oldReadPos));
        return true;
    }

private:
    constexpr size_t getNextPosition(size_t pos)
    {
        return ++pos == ringBuffer.size() ? 0 : pos;
    }

    std::array<T, size> ringBuffer {};
    std::atomic<size_t> readPos { 0 }, writePos { 0 };
};
} // Namespace AudIO
