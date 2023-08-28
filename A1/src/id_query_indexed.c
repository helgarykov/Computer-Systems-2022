#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <stdint.h>
#include <errno.h>
#include <assert.h>

#include "record.h"
#include "id_query.h"

struct indexed_data {
    struct index_record *irs; //pointer to the array, cointains the id and pointer to the one cell of the array rs  irs[0].osm_id ()
    int n;                    // the size of array
    struct record* rs;        // original array of records
};

struct index_record {            // the node of the array
    int64_t osm_id;
    const struct record *record; //pointer to the one cell of the array rs
};

/* The array of records (each cell of rs contains one record); 
the array irs (each cell contains record + osm_id).
 */
struct index_record* map(struct record* rs, int n){
  struct index_record* irs = malloc(n*sizeof(struct index_record));
  for(int i = 0; i < n; i++){
    irs[i].osm_id = rs[i].osm_id; 
    irs[i].record = &rs[i];   
  }
  return irs;
} 

struct indexed_data* mk_indexed(struct record* rs, int n){
    
  struct indexed_data* my_indexed = malloc(sizeof(struct indexed_data)); //pointer to the 1.node of the array
  my_indexed->irs = map(rs, n);
  my_indexed->n = n;
  my_indexed->rs = rs;
  return my_indexed;
}


void free_indexed(struct indexed_data* data){
    
    free(data->irs);   // array with pointers and IDs
    free(data);        // struct with pointers to arrays
}


const struct record* lookup_indexed(struct indexed_data *data,
                                    int64_t needle){
    
    
    for(int i = 0; i < data->n; ++i) {

      if(data->irs[i].osm_id == needle) {
    
        return (data->irs[i].record); 
      }      
    }
    return NULL;
}

int main(int argc, char** argv) {

  // Testing whether lookup_indexed ever returns a NULL pointer
  int n;
  struct record* rs = read_records("20000records.tsv", &n);
  assert(rs != NULL);
  struct indexed_data* data = mk_indexed(rs, n);
  assert(data != NULL);

  int64_t needle;
  for(int i = 0; i < data->n; ++ i){

    needle = data->rs[i].osm_id;

    assert(lookup_indexed(data, needle) != NULL);

  }
  free_indexed(data);
  free_records(rs, n);   // original array with records

  return id_query_loop(argc, argv,
                    (mk_index_fn)mk_indexed,
                    (free_index_fn)free_indexed,
                    (lookup_fn)lookup_indexed);               
}
