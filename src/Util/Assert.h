#ifndef CIRI_ASSERT_H
#define CIRI_ASSERT_H

// Commented out version without a condition because to make it work like this is very messy.
#ifdef CIRI_DEBUG
// #   define CIRI_ASSERT(message) CIRI_ASSERT(false, message)
#   define CIRI_ASSERT(condition, message) \
    do { \
        if (!(condition)) { \
            CIRI_ERROR("Assertion '{0}' failed at {1}:{2}", #condition, __FILE__, __LINE__); \
            std::terminate(); \
        } \
    } while (false)
#else\
// #   define CIRI_ASSERT(message) do { } while (false)
#   define CIRI_ASSERT(condition, message) do { } while (false)
#endif

#endif