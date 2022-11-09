#ifndef CIRI_MEM_H
#define CIRI_MEM_H

namespace Ciri
{
    template <typename T>
    using U = std::unique_ptr<T>;
    template <typename T, typename... Args>
    constexpr U<T> CreateU(Args &&...args)
    {
        return std::make_unique<T>(std::forward<Args>(args)...);
    }

    template <typename T>
    using S = std::shared_ptr<T>;
    template <typename T>
    using ThisS = std::enable_shared_from_this<T>;
    template <typename T, typename... Args>
    constexpr S<T> CreateS(Args &&...args)
    {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }

    template <typename T>
    using W = std::weak_ptr<T>;
    template <typename T>
    bool operator==(const std::weak_ptr<T> &lhs, const std::weak_ptr<T> &rhs)
    {
        return !rhs.owner_before(lhs) && !lhs.owner_before(rhs);
    }
}

#endif