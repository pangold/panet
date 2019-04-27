#ifndef __PAN_NONCOPYABLE_HPP__
#define __PAN_NONCOPYABLE_HPP__

namespace pan {

class noncopyable {
protected:
    noncopyable() {}
    virtual ~noncopyable() {}

private:  
    noncopyable(const noncopyable&);
    const noncopyable& operator=(const noncopyable&);

};

}

#endif // __PAN_NONCOPYABLE_HPP__