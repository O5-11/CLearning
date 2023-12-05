void Scale_BilinearInterpolation(IMAGE* src,IMAGE* dst,double targetWidth,double targetHeight)
{
    if (dst == NULL)
        dst = new IMAGE(targetWidth, targetHeight);
    else
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
        if (x < 0)x = 0;
        if (x >= src->getheight())x = src->getheight() - 2;

        int fx = (int)x;

        //取得坐标小数部分为权重
        x -= fx;

        int x1 = fx, x2 = fx + 1;

        for (int j = 0; j < targetWidth; ++j)
        {
            //[i, j] 对应原图点精确位置
            double y = (j + 0.5f) * widthScaleRate - 0.5f;

            //防越界
            if (y < 0)y = 0;
            if (y >= src->getwidth())y = src->getwidth() - 2;

            int fy = (int)y;

            //取得坐标小数部分为权重
            y -= fy;

            int y1 = fy, y2 = fy + 1;

            int color[4];
            readimgmem = readimgmemHead;

            //色值
            color[0] = readimgmem[x1 * src->getheight() + y1];
            color[1] = readimgmem[x2 * src->getheight() + y1];
            color[2] = readimgmem[x1 * src->getheight() + y2];
            color[3] = readimgmem[x2 * src->getheight() + y2];

            //x方向乘以权重
            //分通道插值
            int Rvalue_X1 = GetRValue(color[0]) * (1 - x) + GetRValue(color[2]) * x;
            int Gvalue_X1 = GetGValue(color[0]) * (1 - x) + GetGValue(color[2]) * x;
            int Bvalue_X1 = GetBValue(color[0]) * (1 - x) + GetBValue(color[2]) * x;
            int Rvalue_X2 = GetRValue(color[1]) * (1 - x) + GetRValue(color[3]) * x;
            int Gvalue_X2 = GetGValue(color[1]) * (1 - x) + GetGValue(color[3]) * x;
            int Bvalue_X2 = GetBValue(color[1]) * (1 - x) + GetBValue(color[3]) * x;

            //目标图像素内存位置
            long long dstpos = (i * targetWidth) + j;

            ////y方向乘以权重
            writeimgmem[dstpos] = RGB(Rvalue_X1 * (1 - y), Gvalue_X1 * (1 - y), Bvalue_X1 * (1 - y)) + RGB(Rvalue_X2 * y, Gvalue_X2 * y, Bvalue_X2 * y);
        }
    }
}
