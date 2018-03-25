#include "heap_allocator.hpp"

#include <algorithm>
#include <cassert>

namespace memory {

heap_allocator::heap_allocator(raw_memory_ptr base, std::size_t size)
: base_allocator()
, base_memory(base)
, capacity(size)
, root(static_cast<header*>(base_memory)){
    root->next = nullptr;
    root->prev = nullptr;
    root->allocation_size = size - sizeof(header);
    root->used = false;
}

raw_memory_ptr heap_allocator::allocate(std::size_t size) {
    // search for the first fit
    header* it;
    for(it = root; it != nullptr && (it->used || it->allocation_size < size) ;it = it->next);

    if(!it) {
        return nullptr;
    }

    header* after = it + 1;
    uint8_t* alloc_spot = reinterpret_cast<uint8_t*>(after);
    uint8_t* end_of_alloc = alloc_spot + size;

    // If allocation_size > size create a new, else reuse
    if(it->allocation_size > size + sizeof(header)) {
        header* next_header = reinterpret_cast<header *>(end_of_alloc);
        next_header->allocation_size = it->allocation_size - size - sizeof(header);
        next_header->prev = it;
        next_header->used = false;
        next_header->next = it->next;

        it->next = next_header;
        it->allocation_size = size;
    }

    it->used = true;
    used_space += size;

    return alloc_spot;
}

void heap_allocator::free(raw_memory_ptr memory, std::size_t s) {
    if(memory == nullptr) return;

    header* head = static_cast<header*>(memory) - 1;

    assert(head->allocation_size >= s);
    assert(head->used);

    // Mark the space as not used anymore
    head->used = false;

    // Compress current cell with previous
    if(head->prev && !head->prev->used) {
        header* temp = head;
        head = head->prev;
        head->next = temp->next;
        if(temp->next){
            temp->next->prev = head;
        }

        head->allocation_size += temp->allocation_size + sizeof(header);
    }

    if(head->next && !head->next->used) {
        // Compress head with next
        header* temp = head->next;
        head->next = temp->next;
        head->allocation_size += temp->allocation_size + sizeof(header);

        if(head->next) {
            head->next->prev = head;
        }
    }

    used_space -= s;
}

}