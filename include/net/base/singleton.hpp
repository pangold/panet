#ifndef __PAN_SINGLETON_HPP__
#define __PAN_SINGLETON_HPP__

namespace pan {

template <typename T>
struct singleton {
private:
	struct object_creator 
	{
		object_creator() { singleton<T>::instance(); }
		inline void do_nothing() const { }
	};
	static object_creator create_object;
	singleton();
public:
	typedef T object_type;
	static object_type& instance() 
	{
		static object_type obj;
		create_object.do_nothing();
		return obj;
	}
};

template <typename T>
typename singleton<T>::object_creator singleton<T>::create_object;

}

#endif // __PAN_SINGLETON_HPP__