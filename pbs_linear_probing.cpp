#include <iostream>
#include <random>
#include <set>
#include <unordered_map>
#include <vector>
#include "util.h"
#include <cstdlib>
#include <cstring>

template <uint64_t epsilon>
struct PBSLinearProbing {

    // Capacity = 1 << k for some k to support fast mod 
    static const u64 DEFAULT_CAPACITY = (1 << 10);
    static const u64 ALL_ONES   = 0xFFFFFFFFFFFFFFFF;
    static const u64 EMPTY_CELL = ALL_ONES;
    constexpr static const double MAX_FILL_RATIO = 0.8;
    
    u64 capacity;
    u64 mod_capacity_bitmask;
    u64 n_elements;
    u64 max_n_supported;
    u64 *table; 
    
    // ------------- TODO --------------
    // ------ Implement shrinking ------
    // ---------------------------------

    inline size_t table_size(){
        return sizeof(*table)*capacity;
    }

    PBSLinearProbing(){
        if (epsilon < 2) std::cout << "ERROR. Epsilon must be at least 2\n", exit(1);
        
        capacity   = DEFAULT_CAPACITY;
        mod_capacity_bitmask = capacity - 1;
        n_elements = 0;
        max_n_supported = (u64)(MAX_FILL_RATIO * capacity);
        table = (u64*)malloc(table_size());

        memset(table, (unsigned char)EMPTY_CELL, table_size()); // set all ones

        // ---------------------  TODO: insert 0 ----------------
        // ---------------------  TODO: insert 0 ----------------
        // ---------------------  TODO: insert 0 ----------------
        // Only necessary if we no longer use the divide by epsilon^2 method.

        verify_valid_capacity();

    }

    ~PBSLinearProbing(){
        std::cout << "Destructor not implemented for PBSLinearProbing!\n";
    }

    PBSLinearProbing(const PBSLinearProbing& other)
    {
        std::cout << "Copy constructor not implemented for PBSLinearProbing\n";
        exit(1);
    }

    PBSLinearProbing& operator=(const PBSLinearProbing& other){
        std::cout << "= operator not implemented for PBSLinearProbing\n";
        exit(1);
    }

    void resize_table(){
        //std::cout << "c\n";
        // TODO: Remember to implement shrinking if necessary

        u64 *old_table = table;
        u64 old_capacity = capacity;

        // Ensure capacity is (1 << k) for some k 
        this->capacity = this->capacity * 2;
        this->mod_capacity_bitmask = this->capacity - 1;
        u64 new_size = sizeof(u64)*capacity;
        this->table = (u64*)malloc(new_size);
        memset(this->table, (unsigned char)EMPTY_CELL, new_size);
        this->n_elements = 0;
        this->max_n_supported = (u64)(MAX_FILL_RATIO * capacity);
        verify_valid_capacity();

        //std::cout << "d\n";

        u64 tmp;
        for(size_t i = 0; i < old_capacity; i++){
            tmp = old_table[i];
            if (tmp != EMPTY_CELL){
                tryInsertInPage(tmp, get_id(tmp));
            }
        }
        //std::cout << "e\n";
        
        free(old_table);
    }

    void verify_valid_capacity(){
        // capacity should be a power of two to support fast modulo
        const u64 n = capacity;
        bool is_power_of_two = (n != 0) && ((n & (n-1)) == 0);
        if (!is_power_of_two){
            std::cout << "ERROR: capacity is not one smaller than a power of two. Exiting.\n";
            std::cout << "Capacity is " << capacity << "\n";
            std::cout << "Capacity + 1 != 0 " << ((capacity + 1) != 0) << "\n";
            std::cout << "capacity + 1: " << std::hex << (capacity + 1 ) << "\n";
            std::cout << "Capacity: " << capacity << "\n";
            std::cout << "Binary and: " << ((capacity + 1) & capacity) << "\n";
            std::cout << (((capacity + 1) & capacity) == 0) << "\n";
            exit(1);
        }
    }

    inline static u64 hash(u64 x) {
        //x &= 0xFFFFFFFE; // Odd numbers are never page bearers 
        const u64 a = 2187650952262969439;
        //const u64 b = 2349073786287317910;
        return (a * x); // + b;
    }

    inline static u64 get_id(u64 x){
        return x / (epsilon * epsilon);
        //return x/epsilon;
    }

    inline static bool is_id_page_bearer(u64 id){
        // all ids are page bearers because we are setting ID = x / epsilon^2
        //return pbHash(id) % epsilon == 0;
        return true; 
    }


    // Assumes there is at least one free position in the table
    inline bool tryInsertInPage(u64 x, u64 id){
        //std::cout << "a\n   " << x << ", " << id << "\n";
        u64 current = hash(get_id(x)) & mod_capacity_bitmask;
        bool found = false;
        u64 tmp;
        while (true) {
            tmp = *(table + current);
            found = (tmp == x);
            if (found || tmp == EMPTY_CELL) break;
            // Modulo capacity, assuming capacity = (1 << k) - 1. We cannot rely on 
            // the compiler since capacity changes at runtime.
            current = (current + 1) & mod_capacity_bitmask;
        }

        if (!found) *(table + current) = x;
        n_elements++;

        if (n_elements >= max_n_supported) resize_table();

        return true;
    }

    inline u64 tryPredecessorInPage(u64 x, u64 id){
        u64 best = 0;
        u64 current = hash(id) & mod_capacity_bitmask;
        u64 tmp;
        while (true){
            tmp = *(table + current);
            if (tmp == EMPTY_CELL) break;
            if (tmp <= x && tmp > best) best = tmp;
            current = (current + 1) & mod_capacity_bitmask;
        }
       return best;
    }
    
    bool tryDeleteInPage(u64 x, u64 id){
        std::cout << "Delete not implemented\n";
        exit(1);
    }

};