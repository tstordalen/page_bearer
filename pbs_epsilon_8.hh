#pragma once

#include "util.h"
#include <cstdlib>
#include "linear_probing.hh"


// The same as pbs_bit_tricks but with epilson=8 fixed. Sorry.

// With epsilon = 8, we have epsilon^2 = 64, and we can
// store a single 64-bit bitvector word for each 'page'
struct PBSEpsilon8 {

    static const u64 epsilon = 8;
    static const u64 bits_per_word = 64;
    u64 zero = 0;

    LinearProbing<u64> table;

    PBSEpsilon8(){};


    std::string name(){
        return "PBS - fixed epislon 8";
    }

    inline static u64 get_id(u64 x){
        return x / (epsilon*epsilon); 
    }

    inline static u64 recover_element(u64 id){
        return id * (epsilon * epsilon);
    }

    inline static u64 get_index_in_page(u64 x){
        return x % (epsilon * epsilon);
    }

    inline static bool is_id_page_bearer(u64){
        // we store everything using their respective IDs. 
        return true; 
    }

    inline bool try_insert_in_page(u64 x, u64){
        u64 x_id = get_id(x);       
        // 0: initialize with empty bitvector if the page does not exist
        auto result = table.get_or_insert(x_id, zero);
        result->value |= ((u64)(1) << get_index_in_page(x));
        return true;
    }

    inline u64 try_predecessor_in_page(u64 x, u64 id){
        auto result = table.get(id);
        if (result == nullptr) return 0;

        

        u64 elements = result->value;
        u64 x_id = get_id(x);
        
        
        // mask out the elements that are not predecessors
        if (x_id == id){
            const u64 index = get_index_in_page(x);

            // Note; left-shifting u64 by 64 bits is undefined behaviour,
            // so we have to do it this way instead of left shifting by
            // index + 1 and then subtracting 1
            const u64 lsh   = (u64)(1) << index;
            const u64 mask  = (lsh - 1) | lsh;  
            
            elements = elements & mask;
        }

        // We might have masked out all the 1-bits in element
        if (elements == 0) return 0;

        const u64 base_element = recover_element(id);
        const u64 index_of_largest_element = bits_per_word - 1 - std::__countl_zero(elements);

        auto ret =  base_element + index_of_largest_element;
        return ret;
    }
};