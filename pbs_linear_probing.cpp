#include <iostream>
#include <random>
#include <set>
#include <unordered_map>
#include <vector>
#include "util.h"

template <uint64_t epsilon>
struct PBSLinearProbing {

    static const u64 DEFAULT_CAPACITY = 1024;
    u64 capacity;
    u64 n_elements;
    u64 *table; 
    

    inline size_t table_size(){
        return sizeof(*table)*capacity;
    }

    PBSLinearProbing(){
        if (epsilon < 2) std::cout << "ERROR. Epsilon must be at least 2\n", exit(1);
        
        capacity   = DEFAULT_CAPACITY;
        n_elements = 0;
        table = (u64*)malloc(table_size());
        memset(table, 0, table_size());
        // ---------------------  TODO: insert 0 ----------------
        // ---------------------  TODO: insert 0 ----------------
        // ---------------------  TODO: insert 0 ----------------
        // ---------------------  TODO: insert 0 ----------------
    }

    ~PBSLinearProbing(){
        std::cout << "Destructor not implemented for PBSLinearProbing\n";
        exit(1);
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


    inline static u64 pbHash(u64 x) {
        //x &= 0xFFFFFFFE; // Odd numbers are never page bearers 
        const u64 a = 2187650952262969439;
        //const u64 b = 2349073786287317910;
        return (a * x); // + b;
    }

    inline static u64 get_id(u64 x){
        return x/epsilon;
    }

    inline static bool is_id_page_bearer(u64 id){
        return pbHash(id) % epsilon == 0;
    }

    inline bool tryInsertInPage(u64 x, u64 id){
        if (!is_id_page_bearer(id)) return false;


        return false;
    }

    inline u64 tryPredecessorInPage(u64 x, u64 id){
       return 0;
    }

    bool tryDeleteInPage(u64 x, u64 id){
        std::cout << "Delete not implemented\n";
        exit(1);
    }

};