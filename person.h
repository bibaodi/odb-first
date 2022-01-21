// file      : hello/Person.h
// copyright : not copyrighted - public domain

#ifndef PERSON_HXX
#define PERSON_HXX

#include <cstddef> // std::size_t
#include <string>

#include <odb/core.hxx>

#pragma db object
class Person {
  public:
    Person(const std::string &first, const std::string &last, unsigned short age)
        : first_(first), last_(last), age_(age) {}

    const std::string &first() const { return first_; }

    const std::string &last() const { return last_; }

    unsigned short age() const { return age_; }

    void age(unsigned short age) { age_ = age; }

  private:
    friend class odb::access;

    Person() {}

#pragma db id auto
    unsigned long id_;

    std::string first_;
    std::string last_;
    unsigned short age_;
};

#pragma db view object(Person)
struct person_stat {
#pragma db column("count(" + Person::id_ + ")")
    std::size_t count;

#pragma db column("min(" + Person::age_ + ")")
    unsigned short min_age;

#pragma db column("max(" + Person::age_ + ")")
    unsigned short max_age;
};

#endif // PERSON_HXX
