enum FillStyleEnum
{
    eNormalFillStyle        = 0x00,
    eSortedFillStyle        = 0x01,
    eReverseSortedFillStyle = 0x03,
    eDiscreteFillStyle      = 0x04, //use 2nd byte to specify num values
};

void printdoublearray(double* array, int size,int focusstart = 0, int focussize=-1, int swapaindex=-1, int swapbindex=-1);
void printintarray(int* array, int size);

void fillIntArray(int* array, int size, int minval = -99, int maxval = 99);
void fillDoubleArray(double* array, int size, unsigned int fillStyle = 0, double minval = -99.99, double maxval = 99.99);

void printseperator(int depth);
