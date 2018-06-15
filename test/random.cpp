#include "test/test.h"

template <typename T>
class RandomTest :
	public ::testing::Test
{};

typedef ::testing::Types<char, int, unsigned int, long, float, double> random_types;
TYPED_TEST_CASE(RandomTest, random_types);


TYPED_TEST(RandomTest, Range)
{
	srand(time(nullptr));
	REPEAT(100)
	{
		TypeParam const max = rand();
		TypeParam const min = rand();

		if (!(min < max))
			continue;



		REPEAT(100)
		{
			TypeParam const val = random(max, min);
			ASSERT_GE(max, val) << min << " " << max;
			ASSERT_LE(min, val) << min << " " << max;
		}
	}
}
