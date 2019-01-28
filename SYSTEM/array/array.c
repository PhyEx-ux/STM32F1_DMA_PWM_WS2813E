#include "array.h"

// 所有元素前移一个位置，0号元素移到最后
void loopMove(int *dat, int len)
{
	int tem[3];
	tem = dat[0]
		for (int i = 0; i < len - 1; i++)
			dat[i] = dat[i + 1];
	dat[len - 1] = tem;
}