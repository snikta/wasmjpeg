struct bit_set {
	int n;
	int length;
} typedef bit_set;

bit_set bit_set_init(bit_set bs, int length);
bit_set bit_set_set(bit_set bs, int p);
int bit_set_test(bit_set bs, int p);
bit_set bit_set_clear(bit_set bs, int p);
bit_set bit_set_toggle(bit_set bs, int p);
bit_set bit_set_invert(bit_set bs);
char *bit_set_to_string(bit_set bs);