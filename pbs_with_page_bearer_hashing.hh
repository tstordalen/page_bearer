#pragma once

#include <sstream>

#include "util.h"
#include "linear_probing.hh"



template <u64 epsilon>
struct PBSPageBearerHashing {

    using VEC = std::vector<u64>;

    LinearProbing<VEC*> table;

    PBSPageBearerHashing(){
        VEC *vec = new VEC;
        vec->push_back(0);
        table.get_or_insert(0, vec);
    }

    ~PBSPageBearerHashing(){
        std::cout << "Destructor not implemented for PBSPageBearerHashing!\n";
    }

    PBSPageBearerHashing(const PBSPageBearerHashing& other)
    {
        std::cout << "Copy constructor not implemented for PBSPageBearerHashing\n";
        exit(1);
    }

    PBSPageBearerHashing& operator=(const PBSPageBearerHashing& other){
        if(&other != this){
            table = other.table;
            return *this;
        } else return *this;
    }

    std::string name(){
        std::stringstream sstm;
        sstm << "PBSPageBearerHashing<" << epsilon << ">";
        return sstm.str();
    }

    inline static u64 pb_hash(u64 x){
        uint64_t a = 8163375249528611521;
        return a * x;
    }

    inline static u64 get_id(u64 x){
        return x/epsilon;
    }

    inline static bool is_id_page_bearer(u64 id){
        return pb_hash(id) % epsilon == 0;
    }

    inline void insert_if_not_present(std::vector<u64> *vec, u64 x){
        bool already_present = false;
        for (auto e : *vec){
            if (e == x) {
                already_present = true;
                break; 
            }
        }
        if (!already_present) vec->push_back(x);
    }

    inline bool try_insert_in_page(u64 x, u64 page_id){
        if(!is_id_page_bearer(page_id)) return false;

        auto *entry = table.get(page_id);
        if (entry == nullptr) return false;

        VEC *vec = entry->value;

        const u64 x_id = get_id(x);
        bool should_split_page = is_id_page_bearer(x_id) && x_id != page_id;

        if (!should_split_page) insert_if_not_present(vec, x);            
        else {
            VEC *new_page = new VEC;
            new_page->push_back(x);

            VEC &vec_ref = *vec;
            u64 tmp;
            u64 i = 0;
            while (i < vec_ref.size()){
                tmp = vec_ref[i];
                if (tmp >= x){
                    new_page->push_back(tmp);
                    vec_ref[i] = vec_ref.back();
                    vec_ref.pop_back();
                }
                else i++;
            }
            table.get_or_insert(x_id, new_page);
        }
        return true;
    }

    inline u64 try_predecessor_in_page(u64 x, u64 page_id){
        if(!is_id_page_bearer(page_id)) return 0;

        auto entry = table.get(page_id);
        if (entry == nullptr) return 0;

        VEC *vec = entry->value;

        VEC &vec_ref = *vec;
        u64 best = 0;
        for (auto e : vec_ref){
            if (e <= x && e > best) best = e;
        }
        return best;
    }
};
