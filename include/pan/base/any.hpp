#ifndef __PAN_ANY_HPP__
#define __PAN_ANY_HPP__

#include <cassert>
#include <typeinfo>

namespace pan {

class holder {
public:
    virtual holder* clone() const = 0;
    virtual const std::type_info& type() const = 0;
    virtual ~holder() { }
};

template <typename T>
class dataholder : public holder {
    typedef dataholder<T> self;
public:
    dataholder(const T& v) : val(v) { }
    dataholder(const self&) = delete;
    self& operator = (const self& rhs) = delete;
    virtual dataholder* clone() const { return new dataholder(val); }
    virtual const std::type_info& type() const { return typeid(val); }
    T val;
};

class any {
    template <typename T>
    friend T& any_cast(const any& rhs);
public:
    any() : content_(nullptr) { }

    template <typename T>
    any(const T& val) 
        : content_(new dataholder<T>(val)) 
    { }

    any(const any& rhs) 
    {
        content_ = rhs.content_->clone(); 
    }

    any& operator = (const any& rhs) 
    { 
        if (content_) delete content_;
        content_ = rhs.content_->clone();
        return *this; 
    }

    ~any() 
    { 
        delete content_; 
    }

    const std::type_info& type() const
    {
        return content_ == nullptr ? typeid(void) : content_->type();
    }

private:
    holder* content_;
};

template <typename T>
T& any_cast(const any& rhs)
{
    assert(typeid(typename T) == rhs.type());
    return static_cast<dataholder<T>*>(rhs.content_)->val;
}

}

#endif // __PAN_ANY_HPP__