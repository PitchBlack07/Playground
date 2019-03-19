
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <hash/fnv1a.h>
#include <hash/robinhood.h>
#include <hash/chaining.h>
#include <pcg/pcg_basic.h>
#include <test/dataset.h>

static const uint32_t DataCount = (1U << 24);
static uint64_t PerformanceFrequency;

static void LoadPerformanceFrequency()
{
	QueryPerformanceFrequency((LARGE_INTEGER*)&PerformanceFrequency);
}

static void run_robin_hood_insertion_test(struct dataset* dataset_, uint32_t size_)
{
	uint32_t i, j;
	uint64_t t0, t1, tdiff, freq;
	QueryPerformanceFrequency((LARGE_INTEGER*)&freq);

	FILE* f;
	if (fopen_s(&f, "robin_hood_insertion_test.txt", "w") != 0)
	{
		return;
	}

	for (i = 32768; i <= size_; i += 32768)
	{
		struct robin_hood_hashmap hashmap;
		robin_hood_hashmap_init_sized(&hashmap, 25);

		QueryPerformanceCounter((LARGE_INTEGER*)&t0);

		for (j = 0; j < i; ++j)
		{
			robin_hood_hashmap_insert(&hashmap, dataset_[j].key, dataset_[j].value);
		}

		QueryPerformanceCounter((LARGE_INTEGER*)&t1);

		tdiff = t1 - t0;

		double deltaT = (double)(tdiff) / freq;

		fprintf(f, "%u, %f\n", i, deltaT);
		printf("Executing run %u of %u (%f)\n", i, size_, (double)i / (double)size_);

		robin_hood_hashmap_deinit(&hashmap);
	}

	fclose(f);
}

static void run_chaining_insertion_test(struct dataset* dataset_, uint32_t size_)
{
	uint32_t i, j;
	uint64_t t0, t1, tdiff, freq;
	QueryPerformanceFrequency((LARGE_INTEGER*)&freq);

	FILE* f;
	if (fopen_s(&f, "chaining_insertion_test.txt", "w") != 0)
	{
		return;
	}

	for (i = 32768; i <= size_; i += 32768)
	{
		struct chaining_hashmap hashmap;
		chaining_hashmap_init_sized(&hashmap, 25);

		QueryPerformanceCounter((LARGE_INTEGER*)&t0);

		for (j = 0; j < i; ++j)
		{
			chaining_hashmap_insert(&hashmap, dataset_[j].key, dataset_[j].value);
		}

		QueryPerformanceCounter((LARGE_INTEGER*)&t1);

		tdiff = t1 - t0;

		double deltaT = (double)(tdiff) / freq;

		fprintf(f, "%u, %f\n", i, deltaT);
		printf("Executing run %u of %u (%f)\n", i, size_, (double)i / (double)size_);

		chaining_hashmap_deinit(&hashmap);
	}

	fclose(f);
}

int main(int argc, char** argv)
{
	pcg32_srandom(42u, 54u);
	LoadPerformanceFrequency();

	struct dataset* ds = generate_random_data_set(DataCount);

	//struct chaining_hashmap hashmap;
	//chaining_hashmap_init(&hashmap);

	//for (uint32_t i = 0; i < DataCount; ++i)
	//{
	//	chaining_hashmap_insert(&hashmap, ds[i].key, ds[i].value);
	//}

	//uint32_t maxlen = 0;
	//uint32_t lencnt = 0;

	//for (uint32_t i = 0; i < hashmap.size; ++i)
	//{
	//	uint32_t t = chaining_hashmap_keycount(&hashmap, i);
	//	maxlen = max(maxlen, t);
	//}

	//uint32_t* buckets = malloc(sizeof(uint32_t) * (maxlen+1));
	//memset(buckets, 0, sizeof(uint32_t) * (maxlen + 1));
	//for (uint32_t i = 0; i < hashmap.size; ++i)
	//{
	//	uint32_t t = chaining_hashmap_keycount(&hashmap, i);
	//	buckets[t]++;
	//}



	//chaining_hashmap_deinit(&hashmap);

	//run_robin_hood_insertion_test(ds, DataCount);
	run_chaining_insertion_test(ds, DataCount);

	free(ds);

	//uint64_t freq, time0, time1;
	//uint32_t i;
	//double time;
	//
	//struct robin_hood_hashmap hashmap;

	//struct dataset* ds = generate_random_data_set(DataCount);
	//int hashmapOk = 0;

	//hashmapOk = robin_hood_hashmap_init_sized(&hashmap, 25);

	//QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
	//QueryPerformanceCounter((LARGE_INTEGER*)&time0);
	//int32_t succeeded = 0;
	//int32_t collisions = 0;
	//for (i = 0; i < DataCount; ++i)
	//{
	//	int32_t val = robin_hood_hashmap_insert(&hashmap, ds[i].key, ds[i].value);
	//	if (val == -1)
	//	{
	//		ds[i].key   = 0xffffffff;
	//		ds[i].value = 0xffffffff;
	//		collisions++;
	//	}
	//	else
	//	{
	//		succeeded += val;
	//	}
	//}

	//QueryPerformanceCounter((LARGE_INTEGER*)&time1);

	//uint32_t maxdist = 0;
	//uint32_t distsum = 0;

	//for (int i = 0; i < (1 << hashmap.shift); ++i)
	//{
	//	if (!(hashmap.keys[i] & 0x80000000))
	//	{
	//		maxdist = max(maxdist, hashmap.values[i].distance);
	//		distsum += hashmap.values[i].distance;
	//	}
	//}

	//uint32_t* distbuckets = malloc(sizeof(uint32_t) * (maxdist+1));
	//memset(distbuckets, 0, sizeof(uint32_t) * (maxdist+1));

	//for (int i = 0; i < (1 << hashmap.shift); ++i)
	//{
	//	if (hashmap.values[i].distance != 0xffffffff)
	//	{
	//		distbuckets[hashmap.values[i].distance]++;
	//	}
	//}


	//time = (double)(time1 - time0) / (double)(freq);

	//printf("Insertion time %f ms (%i)\n", time * 1000.0, succeeded);
	//printf("Maximum distance (%u)\n", maxdist);
	//printf("Average distance (%f)\n", (double)distsum / (double)succeeded);
	//printf("Collisions %i\n", collisions);
	//for (uint32_t i = 0; i < maxdist; ++i)
	//{
	//	printf("Distance %u: %u, (%f)\n", i, distbuckets[i], (double)distbuckets[i] / (double)succeeded);
	//}
	//free(distbuckets);
	//shuffle_data_set(ds, DataCount);

	//QueryPerformanceCounter((LARGE_INTEGER*)&time0);

	//for (uint32_t i = 0; i < DataCount; ++i)
	//{
	//	if (ds[i].key < 0xffffffff && ds[i].value < 0xffffffff)
	//	{
	//		uint32_t val = robin_hood_hashmap_remove(&hashmap, ds[i].key);
	//		//if (val != ds[i].value)
	//		//{
	//		//	printf("Value mismatch!\n");
	//		//}
	//	}
	//}
	//QueryPerformanceCounter((LARGE_INTEGER*)&time1);


	//time = (double)(time1 - time0) / (double)(freq);

	//printf("Removal time %f ms\n", time * 1000.0);

	//robin_hood_hashmap_deinit(&hashmap);

	return 0;
}

