#ifndef WHITGL_RANDOM_H_
#define WHITGL_RANDOM_H_

#include <tinymt/tinymt64.h>
#include <whitgl/math.h>

typedef tinymt64_t whitgl_random_seed;
#define whitgl_random_seed_zero {{},0,0,0}

whitgl_random_seed whitgl_random_seed_init(whitgl_int data);
whitgl_random_seed whitgl_random_seed_init_from_seed(whitgl_random_seed* in);
whitgl_int whitgl_random_int(whitgl_random_seed* seed, whitgl_int size);
whitgl_float whitgl_random_float(whitgl_random_seed* seed);

#endif // WHITGL_RANDOM_H_
