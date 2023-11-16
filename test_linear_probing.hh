#pragma once

#include "linear_probing.hh"

#include <iostream>
#include <random>
#include <set>
#include <unordered_map>
#include <vector>
#include "util.h"
#include <cstdlib>
#include <cstring>

#include <algorithm>
bool contains(std::vector<u64> vec, u64 x){
    return std::find(vec.begin(), vec.end(), x) != vec.end();
}

template <uint64_t epsilon>
struct TestLinearProbingPBS {

    struct List {
        u64 elements[epsilon*epsilon];
        u64 n = 0;
    };

    List empty_list;
    LinearProbing<List> table;

    TestLinearProbingPBS(){
        empty_list.n = 0;
    }

    ~TestLinearProbingPBS(){
        std::cout << "Destructor not implemented for TestLinearProbingPBS!\n";
    }

    TestLinearProbingPBS(const TestLinearProbingPBS& other)
    {
        std::cout << "Copy constructor not implemented for TestLinearProbingPBS\n";
        exit(1);
    }

    TestLinearProbingPBS& operator=(const TestLinearProbingPBS& other){
        std::cout << "= operator not implemented for TestLinearProbingPBS\n";
        exit(1);
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


    inline bool tryInsertInPage(u64 x, u64 id){
        id = get_id(x);
        auto res = table.get_or_insert(id, empty_list);
        
        bool found = false;
        for(int i = 0; i < res->value.n; i++){
           if (res->value.elements[i] == x){
            found = true;
            break;
           } 
        } 
        if (!found) {
            res->value.elements[res->value.n++] = x;  
        } 
        return true;        
    }

    inline u64 tryPredecessorInPage(u64 x, u64 id){
        auto res = table.get(id);
        if (res == nullptr) return 0;
        
        u64 best = 0, tmp;
        for (int i = 0; i < res->value.n; i++){
            tmp = res->value.elements[i];
            if (tmp <= x && tmp > best ) best = tmp;
        }
        return best;
    }
/*
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
    */

};