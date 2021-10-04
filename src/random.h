#pragma once

namespace Random
{
	void init();

	float scalef(); // 0-1 range float
	int rangei(int min, int max); // min included, max excluded
	float rangef(float min, float max);
}
