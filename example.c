#include <stdio.h>
#include "hashtable.h"

int main() {
    hashtable_t ht;
    hashtable_init(&ht, sizeof(char[6]), sizeof(int));

    char key[] = "hello";
    int val = 42;

    ht.insert(&ht, key, &val);

    int *pv = ht.get(&ht, key);
    if (pv) printf("value = %d\n", *pv);

    ht.remove(&ht, key);

    ht.destroy(&ht);
    return 0;
}