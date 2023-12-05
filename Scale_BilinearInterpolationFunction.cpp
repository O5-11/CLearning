void Scale_BilinearInterpolation(IMAGE* src, IMAGE* dst, double targetWidth, double targetHeight)
{
	dst->Resize(targetWidth, targetHeight);
	double widthScaleRate = (double)src->getwidth() / targetWidth;
	double heightScaleRate = (double)src->getheight() / targetHeight;

	DWORD* readimgmemHead = GetImageBuffer(src);
	DWORD* readimgmem = readimgmemHead;
	DWORD* writeimgmemHead = GetImageBuffer(dst);
	DWORD* writeimgmem = writeimgmemHead;

	for (int i = 0; i < targetHeight; ++i)
	{
		//[i,j]对应原图点精确位置
		double x = (i + 0.5f) * heightScaleRate - 0.5f;

		//防越界
		int fx = (int)x;
		if (fx < 0)fx = 0;
		if (fx >= src->getheight())fx = src->getheight() - 2;

		//取得坐标小数部分为权重
		//左移22位保留更多精度
		x -= fx;
		x *= 2048;

		int x1 = fx, x2 = fx + 1;
		if (x2 > src->getheight() - 1)x2 = src->getheight() - 1;

		for (int j = 0; j < targetWidth; ++j)
		{
			//[i, j] 对应原图点精确位置
			double y = (j + 0.5f) * widthScaleRate - 0.5f;

			//防越界
			int fy = (int)y;
			if (fy < 0)fy = 0;
			if (fy >= src->getwidth())fy = src->getwidth() - 2;

			//取得坐标小数部分为权重
			//左移22位保留更多精度
			y -= fy;
			y *= 2048;

			int y1 = fy, y2 = fy + 1;
			if (y2 > src->getwidth() - 1)y2 = src->getwidth() - 1;

			int color[4];
			readimgmem = readimgmemHead;

			//色值
			color[0] = readimgmem[x1 * src->getwidth() + y1];
			color[1] = readimgmem[x2 * src->getwidth() + y1];
			color[2] = readimgmem[x1 * src->getwidth() + y2];
			color[3] = readimgmem[x2 * src->getwidth() + y2];

			//x方向乘以权重
			//分通道插值
			int Rvalue_X1 = GetRValue(color[0]) * (2048 - x) + GetRValue(color[2]) * x;
			int Gvalue_X1 = GetGValue(color[0]) * (2048 - x) + GetGValue(color[2]) * x;
			int Bvalue_X1 = GetBValue(color[0]) * (2048 - x) + GetBValue(color[2]) * x;
			int Rvalue_X2 = GetRValue(color[1]) * (2048 - x) + GetRValue(color[3]) * x;
			int Gvalue_X2 = GetGValue(color[1]) * (2048 - x) + GetGValue(color[3]) * x;
			int Bvalue_X2 = GetBValue(color[1]) * (2048 - x) + GetBValue(color[3]) * x;

			//目标图像素内存位置
			long long dstpos = (i * targetWidth) + j;

			////y方向乘以权重
			long long RYres = Rvalue_X1 * (2048 - y) + Rvalue_X2 * y;
			long long GYres = Gvalue_X1 * (2048 - y) + Gvalue_X2 * y;
			long long BYres = Bvalue_X1 * (2048 - y) + Bvalue_X2 * y;

			RYres = RYres >> 22;
			GYres = GYres >> 22;
			BYres = BYres >> 22;

			writeimgmem[dstpos] = RGB(RYres, GYres, BYres);
		}
	}
}
