func main ()
    int null = 0; int one = 1; int two = 2;
    int maxspace = 4;
    int x; int y;
    int xmax = 10; int ymax = 10;
    int numlen; int spaces; int num;
    int f;

    string numstr[80];
 
    y = 1;
    for;
		#NESTED_CODE_OFF;
        x = 1;
        for;
			#NESTED_CODE_OFF;
            @mul (x, y);
            get (num);
            str (num, numstr);
            strlen (numstr, numlen);
            spaces = maxspace - numlen;
            printsp (spaces);
            print num;

            x = ++;
            f = x <= xmax;
        next f;
        printn one;
        y = ++;
        f = y <= ymax;
    next f;

    exit null;
funcend

func mul (int x, int y)
    int ret;

    ret = x * y;
    return (ret);
funcend
    