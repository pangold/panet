#ifndef __PAN_NET_PUBSUB_STORAGE_BASE_HPP__
#define __PAN_NET_PUBSUB_STORAGE_BASE_HPP__

#include <map>
#include <list>
#include <string>
#include <memory>

namespace pan { namespace net { namespace pubsub {

template <typename Session>
class storage_base {
public:
    storage_base() { }
    virtual ~storage_base() { }

    typedef Session session_type;
    typedef typename session_type session_ptr;

    typedef std::string topic_key;
    typedef std::list<session_ptr> subscriber_list;
    typedef std::map<topic_key, subscriber_list> subscriber_map;
    //
    typedef std::shared_ptr<Pango::PubSub::Topic> topic_ptr;
    typedef std::list<topic_ptr> topic_list;
    typedef std::map<topic_key, topic_list> topic_map;

};

}}}

#endif // __PAN_NET_PUBSUB_STORAGE_BASE_HPP__