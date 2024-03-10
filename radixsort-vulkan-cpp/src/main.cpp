#include "app_params.hpp"
#include "init.hpp"
#include "morton.hpp"

//#include "unique.hpp"
#include "radixsort.hpp"
#include "radix_tree.hpp"
#include "edge_count.hpp"
#include "prefix_sum.hpp"
#include "octree.hpp"

#include <vulkan/vulkan.hpp>

#define BUFFER_ELEMENTS 131072

int main(){
    AppParams app_params;
    app_params.n = BUFFER_ELEMENTS;
    app_params.min_coord = 0.0f;
    app_params.max_coord = 1.0f;
    app_params.seed = 114514;
    app_params.n_threads = 4;
    app_params.n_blocks = 16;
    
    std::vector<uint32_t> morton_keys(BUFFER_ELEMENTS, 0);
    std::vector<glm::vec4> data(BUFFER_ELEMENTS ,glm::vec4(0,0,0,0));
    std::vector<uint8_t> prefix_n(app_params.n, 0);
    std::vector<uint32_t> edge_count(app_params.n, 0);
    int n_brt_nodes = BUFFER_ELEMENTS;
    std::vector<int> parents(app_params.n, 0);
    std::vector<int> left_child(app_params.n, 0);
    bool has_leaf_left[app_params.n] = {0};
    bool has_leaf_right[app_params.n] = {0};
    
    Init init_stage = Init();
    init_stage.run(app_params.n_blocks, data.data(), app_params.n, app_params.min_coord, app_params.getRange(), app_params.seed);
    for (int i = 0; i < 1024; ++i){
        std::cout << data[i].x << " " << data[i].y << " " << data[i].z << " " << data[i].w << std::endl;
    }
    
    Morton morton_stage = Morton();
    morton_stage.run(app_params.n_blocks, data.data(), morton_keys.data(), app_params.n, app_params.min_coord, app_params.getRange());
    
    for (int i = 0; i < 1024; ++i){
        std::cout << morton_keys[i] << std::endl;
    }
    
    auto radixsort_stage = RadixSort();
    radixsort_stage.run(app_params.n_blocks, morton_keys.data(), app_params.n);


	for (int i = 0; i < 1024; i++){
		printf("sorted_key[%d]: %d\n", i, morton_keys[i]);
	}

    auto build_radix_tree_stage = RadixTree();
    build_radix_tree_stage.run(app_params.n_blocks, morton_keys.data(), prefix_n.data(), has_leaf_left, has_leaf_right, left_child.data(), parents.data(), n_brt_nodes);

    for (int i = 0; i < 1024; i++){
        printf("prefix_n[%d]: %d\n", i, prefix_n[i]);
    }
    auto edge_count_stage = EdgeCount();
    edge_count_stage.run(app_params.n_blocks, prefix_n.data(), parents.data(),edge_count.data(), n_brt_nodes);
    
    for (int i = 0; i < 1024; i++){
        printf("edge_count[%d]: %d\n", i, edge_count[i]);
    }
    auto prefix_sum_stage = PrefixSum();
    /*
    prefix_sum_stage.run(app_params.n_blocks, edge_count.data(), n_brt_nodes);

    for (int i = 0; i < 1024; i++){
        printf("scanededge_count[%d]: %d\n", i, edge_count[i]);
    }
    */
    
}