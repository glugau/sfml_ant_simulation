#include "random.h"

#include <stdlib.h>
#include <time.h>
#include <mutex>

static std::mutex mutex;

void Random::init()
{
	srand(time(NULL));
	int _disposed = rand();
}

float Random::scalef()
{
	std::unique_lock<std::mutex> lock(mutex);
	return (float)rand() / (float)RAND_MAX;
}

int Random::rangei(int min, int max)
{
	std::unique_lock<std::mutex> lock(mutex);
	int range = max - min;
	return ((rand() * range) / RAND_MAX) + min;
}

float Random::rangef(float min, float max)
{
	std::unique_lock<std::mutex> lock(mutex);
	float range = max - min;
	return (((float)rand() * range) / (float)RAND_MAX) + min;
}