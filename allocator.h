#ifndef _CCPROJECT_ALLOCATOR_ALLOCATOR_H_
#define _CCPROJECT_ALLOCATOR_ALLOCATOR_H_

#include <cassert>

namespace allocator{

// Allocator provides O(1) object allocation and deallocation. Storage arear of
// objects deallocated will be reused. Memory that has been allocated from OS
// will not be freed until the instance is destroyed.
// Each instance of Allocator keeps a number of internal states, and the
// instance of Allocator that used for allocation and deallocation must be the
// same one.
// Exemple :
//    allocator::Allocator<int> allo;
//    auto *p = allo.Allocate(10);
//    std::cout << p << " : " << *p << std::endl; // (an address) : 10
//    allo.Deallocate(p);

template <class T>
class Allocator {
  struct Node {
    void * const memory;
    Node *next = nullptr;
    
    Node() : memory(::operator new(kBlockCapacity_*kItemSize_)) {}
    ~Node() {
      ::operator delete(memory);
    }
  };
public:
  template <class... Args>
  T *Allocate(Args&&... args) {
    if (allocated_ == capacity_) {
      auto new_node = new Node;
      new_node->next = last_node_;
      last_node_ = new_node;
      last_free_ = reinterpret_cast<uint8_t *>(last_node_->memory);
      capacity_ += kBlockCapacity_;
    }
    
    if (last_deleted_ != nullptr) {
      auto result = last_deleted_;
      last_deleted_ = *reinterpret_cast<uint8_t **>(last_deleted_);
      ++allocated_;
      return new(result) T{std::forward<Args>(args)...};
    } else {
      last_free_ += kItemSize_;
      ++allocated_;
      return new(last_free_-kItemSize_) T{std::forward<Args>(args)...};
    }
  }
  
  void Deallocate(T *memory) {
    *reinterpret_cast<uint8_t **>(memory) = last_deleted_;
    last_deleted_ = reinterpret_cast<uint8_t *>(memory);
    memory->~T();
    --allocated_;
    assert(allocated_>=0);
  }
  
  ~Allocator() {
    while (last_node_ != nullptr) {
      auto next = last_node_->next;
      delete last_node_;
      last_node_ = next;
    }
  }
private:
  // round up to sizeof(uint8_t *)
  static constexpr int kItemSize_ =
    ((sizeof(T)+sizeof(uint8_t *)-1) / sizeof(uint8_t *)) * sizeof(uint8_t *);
  static const int kBlockCapacity_ = 32;
  
  int capacity_ = kBlockCapacity_;
  int allocated_ = 0;

  Node *last_node_ = new Node();
  
  uint8_t *last_deleted_ = nullptr;
  uint8_t *last_free_ = reinterpret_cast<uint8_t *>(last_node_->memory);
};

} // namespace allocator

#endif // _CCPROJECT_ALLOCATOR_ALLOCATOR_H_
