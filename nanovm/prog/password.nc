// password.nc - password input demo

func main ()
    int null = 0; int one = 1;

    string password_in[256];
    
    
//  secret hash of password:
    qint hash_secret = 728921584Q;

    int f; int len = 30;
    
    byte p[30]; string ch; byte b;
    int plen; int i;
    qint hash_in;

//  get password
    print "password? ";
    password (password_in, len);

//  copy password to byte array
    strlen (password_in, plen);
    
    i = 0;
    for;
        part2string (password_in, i, ch);
        asc (ch, b);
        var2array (b, p, i);
    
        i = ++;
        f = i < plen;
    next f;

//  generate hash_in
    hash64ab (p, plen, hash_in);
    

//  compare hashes
    f = hash_in == hash_secret;
    if f;
        print "access granted!";
        printn one;
    else;
        print "access denied!";
        printn one;
    endif;

    exit null;
funcend
