#ifndef __PAN_NET_PUBSUB_STORAGE_BASE_HPP__
#define __PAN_NET_PUBSUB_STORAGE_BASE_HPP__

#include <map>
#include <list>
#include <string>
#include <memory>
#include <pan/net/pub_sub/pubsub.pb.h>

namespace pan { namespace net { namespace pubsub {

class storage_base {
public:
    typedef std::string name_type;
    typedef std::string session_context_type;
    typedef std::list<session_context_type> session_context_list;
    typedef std::map<name_type, session_context_list> subscriber_map;
    // 
    typedef Pango::PubSub::Topic topic_type;
    typedef std::shared_ptr<topic_type> topic_ptr;
    typedef std::vector<topic_ptr> topic_list;
    typedef std::map<name_type, topic_list> topic_map;

    virtual ~storage_base() { }
    //
    virtual void load() { }
    virtual void subscribed_topics(const std::string& context, std::vector<std::string>& topics) { }
    virtual void add_subscriber(const std::string& context, const std::string& topic) { }
    virtual void remove_subscriber(const std::string& context, const std::string& topic) { }
    virtual void add_topic(const std::string& context, const topic_type& topic) { }
    virtual void find_topic(const std::string& context, const std::string& topic_name, int64_t start_time, int32_t count, topic_list& topics) { }

};

class storage : public storage_base {
public:
    storage();
};

}}}

#endif // __PAN_NET_PUBSUB_STORAGE_BASE_HPP__