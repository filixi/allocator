
#include <iterator>
#include <iostream>
#include <vector>

#include "allocator.h"

void Test() {
  struct SmallObject {
    int data[10];
    int *ptr = nullptr;
    std::string str = "abc";
    
    SmallObject() {}
    SmallObject(const std::string string) : str(string) {}
  };
  
  {
    allocator::Allocator<int> allo;
    auto *p = allo.Allocate(10);
    std::cout << p << " : " << *p << std::endl;
    allo.Deallocate(p);
  }
  
  {
    allocator::Allocator<int> allo;
    std::vector<int *> ints_allocated;
    for (int i=0; i<10; i++)
      ints_allocated.emplace_back( allo.Allocate(i*7) );
    for (const auto &element : ints_allocated)
      std::cout << element << " : " << *element << std::endl;
    for (const auto &element : ints_allocated)
      allo.Deallocate(element);
    ints_allocated.clear();
    
    for (int i=0; i<10; i++)
      ints_allocated.emplace_back( allo.Allocate(i*7) );
    for (const auto &element : ints_allocated)
      std::cout << element << " : " << *element << std::endl;
    for (const auto &element : ints_allocated)
      allo.Deallocate(element);
    ints_allocated.clear();
  }
}

int main() {
  Test();
  return 0;
}
