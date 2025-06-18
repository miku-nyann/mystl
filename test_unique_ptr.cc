#include <cstdio>
#include <vector>

#include "unique_ptr.h"

class Animal {
public:
    virtual void speak() = 0;
    virtual ~Animal() = default;
};

class Dog : public Animal {
public:
    Dog(int age) : _age(age) {}

    virtual void speak() {
        printf("Bark! I'm %d year old.\n", _age);
    }
    
private:
    int _age;
};

class Cat : public Animal {
public:
    Cat(int& age) : _age(age) {}

    virtual void speak() {
        printf("Meow! I'm %d year old.\n", _age);
    }

private:
    int& _age;
};

int main() {
    std::vector<UniquePtr<Animal>> zoo;
    int age = 3;
    zoo.emplace_back(makeUnique<Cat>(age));
    zoo.emplace_back(makeUnique<Dog>(age));
    for (const auto& a : zoo)
        a->speak();
    ++age;
    for (const auto& a : zoo)
        a->speak();
    return 0;
}
