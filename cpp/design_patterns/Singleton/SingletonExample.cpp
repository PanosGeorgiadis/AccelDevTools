// Singleton is a creational design pattern, which ensures that only one object of its kind exists and
// provides a single point of access to it for any other code.
// - It is useful when exactly one object is needed to coordinate actions across a system.
// - A Facade object (design pattern) is often a singleton because only one facade object is required
// - Logging is a common use of a singleto, since any & all objects that log messages would need a
//   uniform point of access, and a single destination to write to

// thread-safe implementation, C++11

#include <mutex>

class ApplicationState
{
public:
    ApplicationState(ApplicationState const&) = delete;
    ApplicationState& operator=(ApplicationState const&) = delete;
    ~ApplicationState() {}

    // Set value has to be thread-safe.
    void set_value(int value)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_value = value;
    }

    // We don't want get value to be thread-safe in our case.
    int get_value() const { return m_value; }

    static ApplicationState* get_instance()
    {
        // Static local variable initialization is thread-safe
        // and will be initialized only once.
        static ApplicationState instance{};
        return &instance;
    }

private:
    explicit ApplicationState() : m_value{0} {}
    std::mutex m_mutex;
    int m_value;
};
