#pragma once

#include <iostream>
#include <random>
#include <set>
#include <unordered_map>
#include <vector>
#include "util.h"
#include <cstdlib>
#include <cstring>

template <typename Data>
struct LinearProbing {

    struct Entry {
        u64 key;
        Data value;
    };

    static const u64 ALL_ONES   = 0xFFFFFFFFFFFFFFFF;
    static const u64 EMPTY_CELL = ALL_ONES;
    constexpr static const double MAX_FILL_RATIO = 0.8;
    
    // Capacity = 1 << k for some k to support fast mod 
    static const u64 DEFAULT_CAPACITY = (1 << 10);
    u64 capacity;
    u64 mod_capacity_bitmask;
    u64 n_elements;
    u64 max_n_supported;
    Entry *table; 
    
    // ------------- TODO --------------
    // ------ Implement shrinking ------
    // ---------------------------------

    LinearProbing(){
        capacity             = DEFAULT_CAPACITY;
        mod_capacity_bitmask = capacity - 1;
        n_elements           = 0;
        max_n_supported      = (u64)(MAX_FILL_RATIO * capacity);
        u64 size             = sizeof(Entry)*capacity;
        table                = (Entry*)malloc(size);  
        table = (Entry*)memset((void*)table, (unsigned char)EMPTY_CELL, size); 
        verify_valid_capacity();
    }

    ~LinearProbing(){
        std::cout << "Destructor not implemented for LinearProbing!\n";
    }

    LinearProbing(const LinearProbing& other)
    {
        std::cout << "Copy constructor not implemented for LinearProbing\n";
        exit(1);
    }

    LinearProbing& operator=(const LinearProbing& other){
        if(&other != this){
            capacity               = other.capacity;
            mod_capacity_bitmask   = other.mod_capacity_bitmask;
            n_elements             = other.n_elements;
            max_n_supported        = other.max_n_supported;
            table                  = other.table;
            return *this;
        } else return *this; 
    }

    void resize_table(){
        // TODO: Remember to implement shrinking if necessary

        Entry *old_table = table;
        u64 old_capacity = capacity;

        // Ensure capacity is (1 << k) for some k 
        this->capacity             = this->capacity * 2;
        this->mod_capacity_bitmask = this->capacity - 1;
        this->n_elements           = 0;
        this->max_n_supported      = (u64)(MAX_FILL_RATIO * capacity);

        u64 new_size               = sizeof(*table)*capacity;
        this->table                = (typeof(table))malloc(new_size);
        memset(this->table, (unsigned char)EMPTY_CELL, new_size);
        verify_valid_capacity();


        Entry tmp;
        for(size_t i = 0; i < old_capacity; i++){
            tmp = old_table[i];
            if (tmp.key != EMPTY_CELL){
                get_or_insert(tmp.key, tmp.value);
            }
        }
        free(old_table);
    }

    void verify_valid_capacity(){
        // capacity should be a power of two to support fast modulo
        const u64 n = capacity;
        bool is_power_of_two = (n != 0) && ((n & (n-1)) == 0);
        if (!is_power_of_two){
            std::cout << "ERROR: capacity (" << capacity << ") is not a power of two Exiting.\n";
            exit(1);
        }
    }

    inline static u64 hash(u64 x) {
        const u64 a = 2187650952262969439;
        const u64 b = 2349073786287317910;
        return (a * x) + b;
    }

    // Gets the entry, or inserts a new one if it's not in the table 
    inline Entry* get_or_insert(u64 key, Data& init_if_not_found){
        if (n_elements >= max_n_supported) resize_table();
        
        u64 current = hash(key) & mod_capacity_bitmask;
        Entry *ret = nullptr;
        Entry *tmp;
        while (true){
            tmp = table + current;
            if (tmp->key == key){
                ret = tmp;
                break;
            }
            if (tmp->key == EMPTY_CELL) break;
            current = (current + 1) & mod_capacity_bitmask;
        }
        const bool key_was_not_found = ret == nullptr;
        if (key_was_not_found){
            ret = table + current;
            *ret = {.key = key, .value = init_if_not_found};
            n_elements++;
        }
        return ret;
    }

    // nullptr if not found
    inline Entry* get(u64 key){
        u64 current = hash(key) & mod_capacity_bitmask;
        Entry *ret = nullptr;
        Entry *tmp;
        while (true){
            tmp = table + current;
            if (tmp->key == key){
                ret = tmp;
                break;
            }
            if (tmp->key == EMPTY_CELL) break;
            current = (current + 1) & mod_capacity_bitmask;
        }
        return ret;
    }
};