#include "array.h"

// ����Ԫ��ǰ��һ��λ�ã�0��Ԫ���Ƶ����
void loopMove(int *dat, int len)
{
	int tem[3];
	tem = dat[0]
		for (int i = 0; i < len - 1; i++)
			dat[i] = dat[i + 1];
	dat[len - 1] = tem;
}