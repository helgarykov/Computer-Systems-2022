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

struct naive_data {
  struct record *rs;
  int n;
};

struct naive_data* mk_naive(struct record* rs, int n) {
  struct naive_data* my_data = malloc(sizeof(struct naive_data));
  my_data->n = n;
  my_data->rs = rs;

  return my_data;
}

void free_naive(struct naive_data* data) {
  if(data == NULL) 
    return; 
  if (data->rs != NULL) 
    free(data->rs); 
  free(data);
}

const struct record* lookup_naive(struct naive_data *data, double lon, double lat) {
  double distance = 10000.0;
  struct record *found_rs = NULL;
  for(int i = 0; i < data->n; ++i){
    double test = sqrt(pow((data->rs[i].lon - lon), 2.0)+ pow((lat - data->rs[i].lat), 2.0));
    if(test < distance){
      distance = test;
      found_rs = &(data->rs[i]);
    }
  }
  return found_rs;
}

int main(int argc, char** argv) {
  return coord_query_loop(argc, argv,
                          (mk_index_fn)mk_naive,
                          (free_index_fn)free_naive,
                          (lookup_fn)lookup_naive);
}
