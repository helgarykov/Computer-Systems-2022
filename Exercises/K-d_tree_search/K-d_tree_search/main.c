#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <stdint.h>
#include <errno.h>
#include <assert.h>
#include <math.h>

#include "record.h"
#include "coord_query.h"

/* Structure aka array, containing the pointer to the 1st cell in the array
and its length */
struct data {
  struct record *rs;
  int n;                             // the length of the array -- here depth (of the k-d tree)
};

/* Structure aka node of the tree */
struct node_of_tree {

    struct node_of_tree* left;
    struct node_of_tree* right;

    double lat;                     // x-coordinate
    double lon;                     // y-coordinate
    const struct record* record;    // a pointer to a node in the arrays rs
    int axis;                       // If axis = -1, vertical; if axis = 1, horizontal

};

struct point{
    double lat;
    double lon;
};


void add_node(struct record* city, int previous_axis, struct node_of_tree** node){

    if(*node == NULL) {

        
        *node = malloc(sizeof(struct node_of_tree));   // add a new node to a tree
        struct node_of_tree* new_node = *node;
         // filling in the new node with data
        new_node->axis = -1 * previous_axis;           // change the axis to the opposite
        new_node->lon = city->lon;
        new_node->lat = city->lat;
        new_node->record = city;
        new_node->left = NULL;
        new_node->right = NULL;
    }
    else {
        
        struct node_of_tree* this_node = *node;
        if(this_node->axis == -1) {             // compare with x-coord

            if(this_node->lat < city->lat) {    // if city-lat is larger than the actual node-lat, insert city-lat into the right leaf of the this->node

                add_node(city, this_node->axis, &(this_node->right));
            }
            else{

                add_node(city, this_node->axis, &(this_node->left));
            }

        }
        else {                                  // compare with y-coord

            if(this_node->lon < city->lon) {

                add_node(city, this_node->axis, &(this_node->right));
            }
            else{

                add_node(city, this_node->axis, &(this_node->left));
            }
        }
    }
}

struct node_of_tree* fill_in_tree(struct data* data){
    
    // Create the root of the tree
    struct node_of_tree* kd_node = malloc(sizeof(struct node_of_tree));

    kd_node->axis = -1;

    kd_node->record = data->rs;

    kd_node->lat = data->rs->lat;

    kd_node->lon = data->rs->lon;

    kd_node->left = NULL;
    kd_node->right = NULL;

    for(int i = 1; i < data->n; ++i){

        add_node(&(data->rs[i]), kd_node->axis, &(kd_node));
        //printf("Latitude (%f) and longtitude (%f)\n", data->rs[i].lat, data->rs[i].lon);

    }
    return kd_node;
}


struct data* mk_kd_tree(struct record* rs, int n) {
  struct data* my_data = malloc(sizeof(struct data));
  my_data->rs = rs;
  my_data->n = n;

  //printf("lontitude (%lf) and latitude (%lf)\n", my_data->rs[50].lon, my_data->rs[50].lat );

  return my_data;
}

void free_kd_tree(struct data* data) {
  if(data == NULL)
    return;
  if (data->rs != NULL)
    free(data->rs);
  free(data);
}

double distance (struct point* one, double lat_point, double lon_point) {
    return pow(one->lat - lat_point, 2.0) + pow(one->lon - lon_point, 2.0);
}

struct node_of_tree* closest_point(struct point* target,
                                   struct node_of_tree* candidate_one,
                                   struct node_of_tree* candidate_two){

    double distance_one = distance(target, candidate_one->lat, candidate_one->lon);
    double distance_two = distance(target, candidate_two->lat, candidate_two->lon);
    if(distance_one > distance_two) return candidate_two;
    return candidate_one;
}

struct node_of_tree* search_nearest_neighbour(struct node_of_tree* node, struct point* target) {
    
    if(node == NULL) return NULL;

    struct node_of_tree* next_branch;    // the target is most likely within this subtree
    struct node_of_tree* other_branch;   // the target is less likely in the alternative subtree, though possible

    if(node->axis == -1) {

        if(target->lat > node->lat) {

            next_branch = node->right;
            other_branch = node->left;
        } else {

            next_branch = node->right;
            other_branch = node->left;
        }
    }else {

        if(target->lon > node->lon) {

            next_branch = node->right;
            other_branch = node->left;
        } else {

            next_branch = node->right;
            other_branch = node->left;
        }
    }
    // search the target recursively in the main subtree, with the largest possibility to find the target
    struct node_of_tree* best_point_of_subtree = search_nearest_neighbour(next_branch, target); //

    // search the best candidate point both in the main subtree and axis point (the point on the axis)
    struct node_of_tree* best_actual_point = closest_point(target, best_point_of_subtree, node); //

    double r = distance(target, best_actual_point->lat, best_actual_point->lon); // r

    double r_axis;
    if(node->axis == -1){  // compare by the x-coord on the x-axis
        r_axis = target->lat - node->lat;
    } else{                 // compare by the y-coord on the y-axis
         r_axis = target->lon - node->lon;
    }
    
    struct node_of_tree* very_best_node = best_actual_point;
    if(r_axis < r) {
        
        struct node_of_tree* best_point_of_alternative_subtree = search_nearest_neighbour(other_branch, target);

        very_best_node = closest_point(target, best_point_of_alternative_subtree, best_actual_point); //2.param
    }
    
    return very_best_node;
}



const struct record* lookup_kd_tree(struct data *data, double lon, double lat){

    struct node_of_tree* root = fill_in_tree(data);
    printf("The tree is created %f %f\n", root->lon, root->lat);

    struct point* target = malloc(sizeof(struct point));
    target->lat = lat;
    target->lon = lon;
    struct node_of_tree* closest_node = search_nearest_neighbour(root, target);

    printf("The city is found %f %f\n", closest_node->lon, closest_node->lat);

    return closest_node->record;  // return search_nearest_neighbour(root, target)->record;
}

int main(int argc, char** argv) {

  int n;
  double lon = 0.0, lat = 0.0;
  printf("lon and lat\n");

  scanf("%lf", &lon);
  scanf("%lf", &lat);
  printf("%f %f\n", lon, lat);
  struct record* rs = read_records("20000records.tsv", &n);
  struct data* data = mk_kd_tree(rs, n);
  //free_kd_tree(data);
  lookup_kd_tree(data, lon, lat);
  //fill_in_tree(data);
  
    
  
  return 0;/*coord_query_loop(argc, argv,
                          (mk_index_fn)mk_kd_tree,
                          (free_index_fn)free_kd_tree,
                          (lookup_fn)lookup_kd_tree);*/
}
