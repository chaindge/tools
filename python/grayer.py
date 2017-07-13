from PIL import Image
import sys

def get_histogram(histogram,pix_table,w,h):
    for x in range(w):
        for y in range(h):
            r = pix_table[x,y][0]
            histogram[r] += 1


def get_threshold(histogram):
    amount = 0
    s = 0

    for i in range(len(histogram)):
        amount += histogram[i]
        s += i * histogram[i]

    return s / amount


def get_ostu(histogram):
    X = 0
    Y = 0
    Amount = 0
    PixelBack = 0
    PixelFore = 0
    PixelIntegralBack = 0
    PixelIntegralFore = 0
    PixelIntegral = 0

    OmegaBack = 0.0
    OmegaFore = 0.0
    MicroBack = 0.0
    MicroFore = 0.0
    SigmaB = 0.0
    Sigma = 0.0

    MinValue = 0
    MaxValue = len(histogram)-1
    Threshold = 0

    while MinValue < MaxValue and histogram[MinValue] == 0:
        MinValue += 1

    while MaxValue > MinValue and histogram[MaxValue] == 0:
        MaxValue -= 1

    if MaxValue == MinValue:
        return MaxValue

    if MinValue + 1 == MaxValue:
        return MinValue


    Y = MinValue
    while Y <= MaxValue:
        Amount += histogram[Y]
        Y += 1

    PixelIntegral = 0
    Y = MinValue
    while Y <= MaxValue:
        PixelIntegral += histogram[Y] * Y
        Y += 1

    SigmaB = -1.0

    Y = MinValue
    while Y < MaxValue:
        PixelBack = PixelBack + histogram[Y]
        PixelFore = Amount - PixelBack
        OmegaBack = float(PixelBack) / Amount
        OmegaFore = float(PixelFore) / Amount
        PixelIntegralBack += float(histogram[Y]) * Y
        PixelIntegralFore = float(PixelIntegral - PixelIntegralBack)
        MicroBack = PixelIntegralBack / float(PixelBack)
        MicroFore = PixelIntegralFore / float(PixelFore)
        Sigma = OmegaBack * OmegaFore * (MicroBack - MicroFore) * (MicroBack - MicroFore)
        if Sigma > SigmaB:
            SigmaB = Sigma
            Threshold = Y
        Y += 1

    return Threshold


def binarization(pix_table, w, h, mode, hold):
    for x in range(w):
        for y in range(h):
            r = 0
            g = 0
            b = 0
            a = 255

            if mode == 'RGB':
                r, g, b = pix_table[x, y]
            elif mode == 'RGBA':
                r, g, b, a = pix_table[x, y]

            if r > hold:
                r = 255
            else:
                r =0

            if mode == 'RGB':
                pix_table[x,y] = r, r, r
            elif mode == 'RGBA':
                pix_table[x, y] = r, r, r, 255


def binarization_pic(pic_name):
    im = Image.open(pic_name)
    w,h = im.size
    pix = im.load()
    print im.mode

    # grayer pixel
    for x in range(w):
        for y in range(h):
            r = 0
            g = 0
            b = 0
            a = 255

            if im.mode == 'RGB':
                r, g, b = pix[x, y]
            elif im.mode == 'RGBA':
                r, g, b, a = pix[x,y]

            if a != 255 :
                r = r * a / 255
                b = b * a / 255
                g = g * a / 255

            gc = (r * 38 + g * 75 + b * 15) >> 7

            if im.mode == 'RGB':
                pix[x,y] = gc, gc, gc
            elif im.mode == 'RGBA':
                pix[x, y] = gc, gc, gc, 255

    hist = []
    for i in range(256):
        hist.append(0)


    # get gray histogram
    get_histogram(hist,pix,w,h)
    threshold = get_ostu(hist)

    # print threshold
    print 'threshold=' + str(threshold)

    binarization(pix,w,h,im.mode,threshold)
    im.save(str(threshold) + '_' + pic_name)


# entry point
if __name__ == '__main__':
    for index in range(1,len(sys.argv)):
        src_pic = sys.argv[index]
        binarization_pic(src_pic)