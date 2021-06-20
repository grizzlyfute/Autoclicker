#include "test.h"

// int main(int argc, char *argv[])
int main(void)
{
	TEST_BEGIN();
	assertInt(1, 1);
	assertArray("1", "10", sizeof("1") - 1);
	assertString("1", "1");
	TEST_END();
}
