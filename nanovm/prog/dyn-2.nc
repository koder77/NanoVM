//  dynamic array demo

func main ()
    qint i = 0Q; qint n = 1Q; qint x = 77Q; qint y; qint type;
    string s[256]; string read[256];
    
//  allocate dynamic array
    dynamic dyn[10];

//  store qint
    var2dynarray (x, dyn, i);
    
    x = 2012Q;
    
    var2dynarray (x, dyn, i);
    
    dynarray2var (dyn, i, y);
    
    print y;
    printn n;
    
//  store string
//  in the same array!!!
//  this is really possible

    i = 1Q;
    s = "Hello world!";
    
    var2dynarray (s, dyn, i);
    
    dynarray2var (dyn, i, read);
    
    print read;
    printn n;
    
    
//  printing out the array element type
    dyngettype (dyn, i, type);
    
    print type;
    printn n;
    
    exit i;
funcend
