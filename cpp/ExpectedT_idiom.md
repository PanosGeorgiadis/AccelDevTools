
## Summary

The Expected<T> idiom is a programming methodology that allows functions to communicate their success or failure along with the returned object.

## Implementation
From Andrei Alexandrescu, 2012

see [channel9 video](https://channel9.msdn.com/Shows/Going+Deep/C-and-Beyond-2012-Andrei-Alexandrescu-Systematic-Error-Handling-in-C)<br/>
[presentation slides](https://onedrive.live.com/?cid=F1B8FF18A2AEC5C5&id=F1B8FF18A2AEC5C5%211158&parId=root&o=OneUp)

```cpp
template<class T>
class Expected
{
protected:
    union
    {
        T result;
        std::exception_ptr spam;
    };
    bool gotResult;
    Expected() {};
    
public:
    // constructors and destructor
    Expected(const T& r) : result(r), gotResult(true) {};
    Expected(T&& r) : result(std::move(r)), gotResult(true) {};
    Expected(const Expected& e) : gotResult(e.gotResult) { ... }
    Expected(Expected&& e) : gotResult(e.gotResult) { ... }
    ~Expected() {};

    void swap(Expected& e) { ... }
    
    // creating expect from exceptions
    template<class E>
    static Expected<T> fromException(const E& exception) { ... }
    
    static Expected<T> fromException(std::exception_ptr p) { ... }
    static Expected<T> fromException() { ... }
    
    // operator overload
    Expected& operator=(const Expected& e) { ... }
    
    // getters
    bool isValid() const { return gotResult; }
    bool wasSuccessful() const { return gotResult; }
    T& get()
    {
        if (!gotResult)
            std::rethrow_exception(spam);
        return result;
    }
    const T& get() const { ... }        
    
    // probe for exception
    template<class E>
    bool hasException() const { ... }
};

// template instantiation for T=void
template<> 
class Expected<void> 
{
    std::exception_ptr spam;
    
public:
    template <typename E>
    Expected(E const& e) : spam(std::make_exception_ptr(e)) { }
    Expected(Expected&& o) : spam(std::move(o.spam)) { }
    Expected() : spam() {}

    Expected& operator=(const Expected& e) { ... }

    bool isValid() const { return !spam; }
    bool wasSuccessful() const { return !spam; }
    void get() const { if (!valid()) std::rethrow_exception(spam); }
};
```