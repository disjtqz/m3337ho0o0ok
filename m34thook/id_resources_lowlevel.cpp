#include <cstdint>
std::uint64_t checksum_override_data(std::uint8_t* a1, int a2, int a3)
{
	unsigned int v4; // er11
	unsigned int v5; // er8
	unsigned __int64 v6; // r10
	unsigned int v7; // ecx
	int v8; // eax
	int v9; // eax
	int v10; // edx
	int v11; // edx

	v4 = a3 ^ a2;
	v5 = 0;
	if (a2 >= 8)
	{
		v6 = (unsigned __int64)(unsigned int)a2 >> 3;
		a2 -= 8 * ((unsigned int)a2 >> 3);
		do
		{
			v7 = 1540483477 * ((1540483477 * *(unsigned int*)a1) ^ ((unsigned int)(1540483477 * *(unsigned int*)a1) >> 24));
			v8 = *((unsigned int*)a1 + 1);
			a1 += 8;
			v4 = v7 ^ (1540483477 * v4);
			v5 = (1540483477 * ((1540483477 * v8) ^ ((unsigned int)(1540483477 * v8) >> 24))) ^ (1540483477 * v5);
			--v6;
		} while (v6);
	}
	if (a2 >= 4)
	{
		v9 = *(unsigned int*)a1;
		a1 += 4;
		v4 = (1540483477 * ((1540483477 * v9) ^ ((unsigned int)(1540483477 * v9) >> 24))) ^ (1540483477 * v4);
		a2 -= 4;
	}
	v10 = a2 - 1;
	if (!v10)
		goto LABEL_11;
	v11 = v10 - 1;
	if (!v11)
	{
	LABEL_10:
		v5 ^= a1[1] << 8;
	LABEL_11:
		v5 = 1540483477 * (v5 ^ *a1);
		return ((unsigned __int64)(1540483477
			* ((1540483477 * (v4 ^ (v5 >> 18))) ^ ((1540483477
				* (v5 ^ ((1540483477 * (v4 ^ (v5 >> 18))) >> 22))) >> 17))) << 32) | (1540483477 * ((1540483477 * (v5 ^ ((1540483477 * (v4 ^ (v5 >> 18))) >> 22))) ^ ((1540483477 * ((1540483477 * (v4 ^ (v5 >> 18))) ^ ((1540483477 * (v5 ^ ((1540483477 * (v4 ^ (v5 >> 18))) >> 22))) >> 17))) >> 19)));
	}
	if (v11 != 1)
		return ((unsigned __int64)(1540483477
			* ((1540483477 * (v4 ^ (v5 >> 18))) ^ ((1540483477
				* (v5 ^ ((1540483477 * (v4 ^ (v5 >> 18))) >> 22))) >> 17))) << 32) | (1540483477 * ((1540483477 * (v5 ^ ((1540483477 * (v4 ^ (v5 >> 18))) >> 22))) ^ ((1540483477 * ((1540483477 * (v4 ^ (v5 >> 18))) ^ ((1540483477 * (v5 ^ ((1540483477 * (v4 ^ (v5 >> 18))) >> 22))) >> 17))) >> 19)));
	v5 ^= a1[2] << 16;
	goto LABEL_10;
}