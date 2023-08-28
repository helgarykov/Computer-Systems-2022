#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <stdint.h>
#include <errno.h>
#include <assert.h>

#include "record.h"
#include "id_query.h"

struct naive_data {
  struct record *rs;                  //pointer to the whole array of records (lines/records themselves)
  int n;                              // the size of the array
};

struct naive_data* mk_naive(struct record* rs, int n) {
  
  struct naive_data* my_naive = malloc(sizeof(struct naive_data)); //pointer to the 1.node of the array
  my_naive->rs = rs;
  my_naive->n = n;
  
  return my_naive;
}

void free_naive(struct naive_data* data) {

  if(data == NULL) return; 
  free(data);
}

const struct record* lookup_naive(struct naive_data* data, int64_t needle) {

  for(int i = 0; i < data->n; ++i){
    if(data->rs[i].osm_id == needle) {
    
      return &(data->rs[i]);       // return the address of the cell
    }

  }
  return NULL;
}

int main(int argc, char** argv) {

  // Testing whether lookup_indexed ever returns a NULL pointer
  int n;
  struct record* rs = read_records("20000records.tsv", &n);
  assert(rs != NULL);
  struct naive_data* data = mk_naive(rs, n);
  assert(data != NULL);
  int64_t needle;
  for(int i = 0; i < data->n; ++ i){

    needle = data->rs[i].osm_id;

    assert(lookup_naive(data, needle) != NULL);

  }
  free_naive(data);
  free_records(rs, n);

  return id_query_loop(argc, argv,
                    (mk_index_fn)mk_naive,
                    (free_index_fn)free_naive,
                    (lookup_fn)lookup_naive);
}
