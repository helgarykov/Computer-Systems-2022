#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <stdint.h>
#include <errno.h>
#include <assert.h>
#include <math.h>

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

int id_comparator(const void *p, const void *q) {

    int64_t left = ((struct index_record*) p)->osm_id;
    int64_t right = ((struct index_record*) q)->osm_id;
    if (left > right) return 1;
    if (left < right) return -1;
    return 0;
}

void irs_sort(struct indexed_data* data) {
    qsort(data->irs, data->n, sizeof(struct index_record), id_comparator);
}


const struct record* binary_search (struct indexed_data* data, int64_t needle) {
    int high = (data->n)-1;
    int low = 0;
    int mid;
    while (low <= high) {
        mid = floor(((double)(high + low)) / 2);
        if (needle == (data->irs[mid].osm_id)) {
            return data->irs[mid].record;
        } else if (needle > data->irs[mid].osm_id) {
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }
    return NULL;
}

  
struct indexed_data* mk_indexed_sorted (struct record* rs, int n){
  struct indexed_data* sorted = mk_indexed(rs, n);
  irs_sort(sorted);

  return sorted;
}

const struct record* lookup_indexed(struct indexed_data *data,
                                    int64_t needle){
    return binary_search(data, needle);
}

int main(int argc, char** argv) {

  // Testing whether lookup_indexed ever returns a NULL pointer  
  int n;
  struct record* rs = read_records("20000records.tsv", &n);
  assert(rs != NULL);
  struct indexed_data* sorted = mk_indexed_sorted(rs, n);
  assert(sorted != NULL);
  int64_t needle;
  for(int i = 0; i < sorted->n; ++ i){

    needle = sorted->rs[i].osm_id;

    assert(lookup_indexed(sorted, needle) != NULL);
  }
  free_indexed(sorted);
  free_records(rs, n);
  

   return id_query_loop(argc, argv,
                    (mk_index_fn)mk_indexed_sorted,
                    (free_index_fn)free_indexed,
                    (lookup_fn)lookup_indexed);
} 
