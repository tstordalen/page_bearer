#include <iostream>
#include <random>
#include <set>
#include <unordered_map>
#include <vector>
#include <algorithm>

#include "util.h"

#include "pbs_map_and_vec.cpp"
#include "pbs_linear_probing.cpp"

typedef std::mt19937 MTRng;  
const u32 seed_val = 53315113;    
MTRng rng;

struct TestData {
    enum Op {Query, Insert};
    std::vector<Op> ops;
    std::vector<u64> xs;
};

template <typename pbs_structure>
struct PbsTestData {
    enum Op {Query, Insert};
    std::vector<Op> ops;
    std::vector<u64> xs;
    std::vector<u64> page_id;
};

struct TestResult {
    u64 sum;
    u64 insertion_time;
    u64 query_time;
};


TestData generate_test_data(u64 universe_size, u64 n_insertions_per_block, u64 n_queries_per_block, u64 n_blocks){
    TestData data;
    using Op = TestData::Op;
    std::uniform_int_distribution<u64> uniform(0,universe_size);
    std::vector<Op> ops;
    std::vector<u64> xs;

    for (int block = 0; block < n_blocks; block++){
        for(int ins_i = 0; ins_i < n_insertions_per_block; ins_i++){
            ops.push_back(Op::Insert);
            xs.push_back(uniform(rng));
        }
        for(int pred_i = 0; pred_i < n_queries_per_block; pred_i++){
            ops.push_back(Op::Query);
            xs.push_back(uniform(rng));
        }
    }
    return {.ops = ops, .xs = xs};
}


template <typename pbs_structure>
PbsTestData<pbs_structure> generate_pbs_test_data(TestData& data){

    using Data = PbsTestData<pbs_structure>;

    constexpr auto get_id               = pbs_structure::get_id;
    constexpr auto is_id_page_bearer    = pbs_structure::is_id_page_bearer;

    Data pbs_data;
    std::set<u64> all_identifiers; 
    all_identifiers.insert(0);

    const u64 data_N = data.ops.size();
    for(int64_t data_i = 0; data_i < data_N; data_i++){
        const TestData::Op data_op = data.ops[data_i];
        const u64 x    = data.xs[data_i];
        const u64 id_x = get_id(x);
        auto pbs_op = 
            (data_op == TestData::Op::Insert) ? Data::Op::Insert : 
            (data_op == TestData::Op::Query)  ? Data::Op::Query  :
            throw "Unhandled operation"; 
        
        // Element immediately after the predecessor of x
        // Becomes predecessor with next decrement
        auto pt = all_identifiers.upper_bound(x);
        u64 last_id = 0xFFFFFFFFFFFFFFFF;
        u64 current_id; 
        do {
            pt--;
            current_id = get_id(*pt);
            if (current_id == last_id) continue; // skip multiple ops in the same page
            last_id = current_id;
            pbs_data.ops.push_back(pbs_op);
            pbs_data.xs.push_back(x);
            pbs_data.page_id.push_back(last_id);
        } while ( !is_id_page_bearer(last_id) );

        if (data_op == TestData::Insert) all_identifiers.insert(x);
    }
    return pbs_data;
}



TestResult test_set_data_structure(TestData& data){
    
    std::set<u64> set;
    set.insert(0);
    u64 insertion_time = 0;
    u64 query_time = 0;
    u64 sum = 0;
    i64 current = 0;
    const i64 N = data.ops.size();
    while (current < N){
        if (data.ops[current] == TestData::Op::Insert){
            const u64 start = nowMicros();
            while (current < N && data.ops[current] == TestData::Op::Insert){
                #ifdef DEBUGGING 
                std::cout << "SET: inserting " << data.xs[current] << "\n"; 
                #endif
                set.insert(data.xs[current]);
                current++;
            }
            const u64 end = nowMicros();
            insertion_time += end - start;
        }
        else if (data.ops[current] == TestData::Op::Query){
            const u64 start = nowMicros();
            while (current < N && data.ops[current] == TestData::Op::Query){
                auto pt = set.upper_bound(data.xs[current]);
                pt--;
                sum += *pt;
                #ifdef DEBUGGING
                std::cout << "SET: pred of " << data.xs[current] << " is " << *pt << "\n";
                #endif
                current++;
            }
            const u64 end = nowMicros();
            query_time += end - start;
        }
        else {
            std::cout << "Unsupported operation. Exiting.\n";
            exit(1);
        }
    }

    std::cout << "Testing regular set\n";
    std::cout << "Insertion time: " << insertion_time << "us\n";
    std::cout << "Query time: " << query_time << "us\n";
    std::cout << "Sum: " << sum << "\n";
    std::cout << "--------------------\n";

    return {.sum = sum, .insertion_time = insertion_time, .query_time = query_time};
}

template <typename pbs_structure>
TestResult test_pbs_data_structure(PbsTestData<pbs_structure> data){
    using Data = PbsTestData<pbs_structure>;
    auto pbs = pbs_structure();
    u64 insertion_time = 0;
    u64 query_time = 0;
    u64 sum = 0;
    i64 current = 0;
    const i64 N = data.ops.size();
    while (current < N){
        if (data.ops[current] == Data::Op::Insert){
            const u64 start = nowMicros();
            while (current < N && data.ops[current] == Data::Op::Insert){
                auto res = pbs.tryInsertInPage(data.xs[current], data.page_id[current]);
                #ifdef DEBUGGING
                std::cout << "PBS: inserting " << data.xs[current] << " at " << data.page_id[current] << " with result " << res << "\n";
                #endif
                current++;
            }
            const u64 end = nowMicros();
            insertion_time += end - start;
        }
        else if (data.ops[current] == Data::Op::Query){
            const u64 start = nowMicros();
            while (current < N && data.ops[current] == Data::Op::Query){
                auto res = pbs.tryPredecessorInPage(data.xs[current], data.page_id[current]);
                sum += res;
                #ifdef DEBUGGING
                std::cout << "PBS: querying " << data.xs[current] << " at " << data.page_id[current] << " with result " << res << "\n";
                #endif
                current++;
                
            }
            const u64 end = nowMicros();
            query_time += end - start;
        }
        else {
            std::cout << "Unsupported operation. Exiting.\n";
            exit(1);
        }
    }

    std::cout << "Testing PBS structure\n";
    std::cout << "Insertion time: " << insertion_time << "us\n";
    std::cout << "Query time: " << query_time << "us\n";
    std::cout << "Sum: " << sum << "\n";
    std::cout << "--------------------\n";

    return {.sum = sum, .insertion_time = insertion_time, .query_time = query_time};
}


int main(void){

    TestData data = generate_test_data(0xFFFFFFFFFFFFFFFF,10000,10000,100);
    auto res_set = test_set_data_structure(data);
    
  
    using PBS = MapAndVecPBS<32>;
    PbsTestData<PBS> pbs_data = generate_pbs_test_data<PBS>(data);
    auto res_pbs = test_pbs_data_structure<PBS>(pbs_data);


    if (res_set.sum == res_pbs.sum) std::cout << "All good!\n";
    else std::cout << "BUG! They differ\n";

    std::cout << "Time PBS / Set\nInsertion: " 
              << (double)res_pbs.insertion_time / (double)res_set.insertion_time 
              << "\nQuery: " << (double)res_pbs.query_time / (double)res_set.query_time << "\n";




    return 0;
}


