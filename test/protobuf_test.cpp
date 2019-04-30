#include <gtest/gtest.h>
#include <net/protobuf/codec.hpp>
#include "address_book.pb.h"

// a temp session for unit testing.
class protobuf_session : 
    public std::enable_shared_from_this<protobuf_session> {
public:
    using pointer = std::shared_ptr<protobuf_session>;
    using codec_type = pan::net::protobuf::codec<protobuf_session>; 
    explicit protobuf_session(codec_type& codec) 
        : codec_(codec)
    { }
    void write(const void* data, std::size_t size) 
    {
        // a circuit
        codec_.on_message(shared_from_this(), data, size);
    }
private:
    codec_type& codec_;
};


static std::shared_ptr<tutorial::AddressBook> g_address_book;


std::shared_ptr<tutorial::AddressBook> create_address_book()
{
    auto address_book = std::make_shared<tutorial::AddressBook>();
    auto people = address_book->add_people();
    people->set_id(10001);
    people->set_name("pandora");
    people->set_email("pandora@gmail.com");
    auto phone1 = people->add_phones();
    phone1->set_type(tutorial::Person::MOBILE);
    phone1->set_number("11111111111");
    auto phone2 = people->add_phones();
    phone2->set_type(tutorial::Person::HOME);
    phone2->set_number("22222222222");
    auto phone3 = people->add_phones();
    phone3->set_type(tutorial::Person::WORK);
    phone3->set_number("33333333333");
    return address_book;
}


template <typename T>
bool be_in_range(const T& t1, const T& t2, const T& t3, const T& t4)
{
    return t1 == t2 || t1 == t3 || t1 == t4;
}


void on_address_book(protobuf_session::pointer /*session*/, std::shared_ptr<tutorial::AddressBook> address_book)
{
    // EXPECT_EQ(*address_book, *g_address_book);
    for (int i = 0; i != address_book->people_size(); ++i) {
        const tutorial::Person& person1 = address_book->people(i);
        const tutorial::Person& person2 = g_address_book->people(i);
        EXPECT_EQ(person1.id(), 10001);
        EXPECT_EQ(person1.name(), "pandora");
        EXPECT_EQ(person1.email(), "pandora@gmail.com");

        EXPECT_EQ(person1.id(), person2.id());
        EXPECT_EQ(person1.name(), person2.name());
        EXPECT_TRUE(person1.has_email());
        EXPECT_TRUE(person2.has_email());
        EXPECT_EQ(person1.email(), person2.email());

        for (int j = 0; j < person1.phones_size(); j++) {
            const tutorial::Person::PhoneNumber& phone_number1 = person1.phones(j);
            const tutorial::Person::PhoneNumber& phone_number2 = person2.phones(j);

            EXPECT_EQ(phone_number1.type(), phone_number2.type());
            EXPECT_EQ(phone_number1.number(), phone_number2.number());
        }
    }
}

TEST(PROTOBUF_TEST, BaseTest)
{
    pan::net::protobuf::codec<protobuf_session> codec;
    auto session = std::make_shared<protobuf_session>(codec);
    g_address_book = create_address_book();
    codec.register_callback<tutorial::AddressBook>(std::bind(on_address_book, std::placeholders::_1, std::placeholders::_2));
    try {
        codec.send(session, g_address_book);
    }
    catch (std::exception& /*e*/) {
        EXPECT_TRUE(false);
    }
    std::this_thread::sleep_for(std::chrono::seconds(2));
}