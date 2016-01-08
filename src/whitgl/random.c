#include <whitgl/math.h>
#include <whitgl/random.h>

whitgl_random_seed whitgl_random_seed_init(whitgl_int data)
{
	whitgl_random_seed seed;
	tinymt64_init(&seed, data);
	return seed;
}
whitgl_random_seed whitgl_random_seed_init_from_seed(whitgl_random_seed* in)
{
	whitgl_random_seed out;
	tinymt64_init(&out, tinymt64_generate_uint64(in));
	return out;
}
whitgl_int whitgl_random_int(whitgl_random_seed* seed, whitgl_int size)
{
	return tinymt64_generate_uint64(seed) % size;
}

whitgl_float whitgl_random_float(whitgl_random_seed* seed)
{
	return tinymt64_generate_doubleOO(seed);
}
