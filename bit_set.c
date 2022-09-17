#include <stdlib.h>
#include "bit_set.h"

bit_set bit_set_init(bit_set bs, int length)
{
	bs.n = 0;
	bs.length = length;
	return bs;
}
bit_set bit_set_set(bit_set bs, int p)
{
	bs.n |= (1 << p);
	return bs;
}
int bit_set_test(bit_set bs, int p)
{
	return (bs.n & (1 << p)) != 0;
}
bit_set bit_set_clear(bit_set bs, int p)
{
	bs.n &= ~(1 << p);
	return bs;
}
bit_set bit_set_toggle(bit_set bs, int p)
{
	bs.n ^= (1 << p);
	return bs;
}
bit_set bit_set_invert(bit_set bs)
{
	for (int i = 0; i < bs.length; i++)
	{
		bs = bit_set_toggle(bs, i);
	}
	return bs;
}
char *bit_set_to_string(bit_set bs)
{
	char *bin = (char *) malloc(bs.length + 1);
	for (int i = 0; i < bs.length; i++)
	{
		if (bit_set_test(bs, (bs.length - 1) - i))
		{
			bin[i] = '1';
		}
		else
		{
			bin[i] = '0';
		}
	}
	bin[bs.length] = '\0';
	return bin;
}