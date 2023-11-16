#pragma once

#include "util.h"
#include <cstdlib>
#include "linear_probing.hh"


// With epsilon = 8, we have epsilon^2 = 64, and we can
// store a single 64-bit bitvector word for each 'page'
struct PBSEpsilon8 {

    static const u64 epsilon = 8;
    static const u64 bits_per_word = 64;

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

    inline static bool is_id_page_bearer(u64 id){
        // we store everything using their respective IDs. 
        return true; 
    }

    inline bool tryInsertInPage(u64 x, u64 id){
        u64 x_id = get_id(x);       
        // 0: initialize with empty bitvector if the page does not exist
        auto result = table.get_or_insert(x_id, 0);
        //std::cout << "------------------\n";
        //std::cout << std::dec << "Inserting " << x << " at id " << x_id << "\n";
        //std::cout << "The index in page is " << get_index_in_page(x) << "\n";
        //std::cout << "initially, elements is " << std::hex << result->value << "\n";
        //std::cout << "The thing we or in is " << ((u64)(1) << get_index_in_page(x)) << "\n";
        result->value |= ((u64)(1) << get_index_in_page(x));
        //std::cout << "after, it is " << result->value << "\n";
        //std::cout << std::dec << "---------------\n";
        return true;
    }

    inline u64 tryPredecessorInPage(u64 x, u64 id){
        auto result = table.get(id);
        if (result == nullptr) return 0;

        

        u64 elements = result->value;
        u64 x_id = get_id(x);
        
        //if (x == 55) std::cout << "elements: " << std::hex << elements << "\n";
        
        // mask out the elements that are not predecessors
        if (x_id == id){
            const u64 index = get_index_in_page(x);

            // Note; left-shifting u64 by 64 bits is undefined behaviour,
            // so we have to do it this way instead of left shifting by
            // index + 1 and then subtracting 1
            const u64 lsh   = (u64)(1) << index;
            const u64 mask  = (lsh - 1) | lsh;  
            
            elements = elements & mask;
            //if (x == 55) std::cout << "after mask: " << std::hex << elements << "\n";
        }

        // We might have masked out all the 1-bits in element
        if (elements == 0) return 0;

        const u64 base_element = recover_element(id);
        //if (x == 55) std::cout << "base: " << std::hex << base_element << "\n";
        const u64 index_of_largest_element = bits_per_word - 1 - std::__countl_zero(elements);
        //if (x == 55) std::cout << "index_of_largest: " << std::dec << index_of_largest_element << "\n";

        auto ret =  base_element + index_of_largest_element;
        return ret;
    }
};