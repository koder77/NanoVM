func main ()
    int n = 1; int ret = 0;
    
//	declare arrays
    qint x[10]; qint a[10]; qint b[10]; 
    
    qint y; int ind = 2;
    
//	declare z
    int z = 77;

//	declare array lists
//	the zero after the closing bracket ] is the index offset
    x = [ 10 9 z 7 6 5 4 3 2 1 ] 0;
    
    a = [ 2 4 6 1 2 3 7 8 9 5 ] 0;

//	multiply lists x and a
    vmul2 (x, a, b);

//	print the third element of array b
    array2var (b, ind, y);
    print y;
    printn n;

    exit ret;
funcend
