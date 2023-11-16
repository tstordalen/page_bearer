#pragma once

#include "util.h"
#include <cstdlib>
#include "linear_probing.hh"
#include <sstream>


// Stores a bitvector consisting of  CEIL(epsilon^2/64) words for each page.
template <u64 epsilon>
struct PBSBitTricks {

    static const u64 epsilon_squared      = epsilon*epsilon;
    static const u64 bits_per_word        = 64;
    static const u64 words_per_large_word = (epsilon_squared + bits_per_word - 1)/bits_per_word;

    struct LargeWord {
        u64 words[words_per_large_word];

        LargeWord() {
            memset(words, 0, sizeof(words));
        }

        inline void set_bit(u64 i){
            const u64 word_i = i / bits_per_word;
            const u64 remainder = i % bits_per_word;
            words[word_i]  |= ((u64)(1) << remainder);
        }

        inline u64 predecessor(u64 i){
            const u64 word_i = i / bits_per_word;
            const u64 index  = i % bits_per_word;

            const u64 lsh     = (u64)(1) << index;
            const u64 mask    = (lsh - 1) | lsh;

            // NOTE: REMEMBER TO ALWAYS RESET words[word_i]. 
            const u64 tmp = words[word_i];
            words[word_i] &= mask;

            i64 best = -1;
            for(int i = 0; i <= word_i; i++){
                best = words[i] ? i : best;        
            }

            if (best == -1) {
                words[word_i] = tmp;
                return 0;
            } 

            const u64 base          = bits_per_word * best;
            const u64 pred_in_word  = bits_per_word - 1 - std::__countl_zero(words[best]);
            words[word_i] = tmp;
            return base + pred_in_word;
        }

        inline u64 get_largest(){
            return predecessor(bits_per_word * words_per_large_word - 1);
        }
    };


    LinearProbing<LargeWord> table;
    LargeWord empty_large_word;

    PBSBitTricks(){
        empty_large_word = LargeWord();
    };


    std::string name(){
        std::stringstream sstm;
        sstm << "PBSBitTricks<" << epsilon << ">";
        return sstm.str();
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
        auto result = table.get_or_insert(x_id, empty_large_word);
        const u64 index = get_index_in_page(x);
        result->value.set_bit(index);
        return true;
    }

    inline u64 tryPredecessorInPage(u64 x, u64 id){
        auto result = table.get(id);
        if (result == nullptr) return 0;

        const u64 x_id = get_id(x);
        u64 index_of_pred = x_id > id ? 
                            result->value.get_largest() :
                            result->value.predecessor(get_index_in_page(x)); 

        return recover_element(id) + index_of_pred;
    }
};