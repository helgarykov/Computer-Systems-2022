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
    if(this_node->axis == -1) {             // compare after x-coord
      if(this_node->lat < city->lat) {    // if city-lat is larger than the actual node-lat, insert city-lat into the right leaf of the this->node
        add_node(city, this_node->axis, &(this_node->right)); 
      }
      else{                               
        add_node(city, this_node->axis, &(this_node->left));
      }
    }
    else {                                  // compare after y-coord
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

  }
  return kd_node;
}


struct data* mk_kd_tree(struct record* rs, int n) {
  struct data* my_data = malloc(sizeof(struct data));
  my_data->rs = rs;
  my_data->n = n;

  return my_data;
}

void free_kd_tree(struct data* data) {
  if(data == NULL) 
    return; 
  if (data->rs != NULL) 
    free(data->rs); 
  free(data);
}

double Distance(double lonX1, double lonX2, double latY1, double latY2){
  return pow((lonX1 - lonX2), 2.0)+ pow((latY1 - latY2), 2.0);
}

void lookup(struct node_of_tree *closest, double lon, double lat, struct node_of_tree* node){
  if(node == NULL) return;
  
  double closestToPoint = Distance(closest->lon,lon,closest->lat,lat);
  double nodeToPoint = Distance(node->lon,lon,node->lat,lat);

  if(closestToPoint > nodeToPoint){
    *closest = *node;
  }

  double diff = 0.0;
  if(node->axis == -1){
    diff = node->lat - lat;
  }else{
    diff = node->lon - lon;
  }

  double radius = Distance(closest->lon,lon,closest->lat,lat);

  if(diff >= 0 || radius > diff){
    lookup(closest, lon, lat, node->left);
  }
  if(diff <= 0 || radius > diff){
    lookup(closest, lon, lat, node->right);
  }
}

const struct record* lookup_kd_tree(struct data *data, double lon, double lat){
  struct node_of_tree* tree = fill_in_tree(data);
  struct node_of_tree closest = *tree;

  lookup(&closest,lon,lat,tree);

  return closest.record;

}

int main(int argc, char** argv) {

  return coord_query_loop(argc, argv,
                          (mk_index_fn)mk_kd_tree,
                          (free_index_fn)free_kd_tree,
                          (lookup_fn)lookup_kd_tree); 
}
