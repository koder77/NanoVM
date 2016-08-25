func main ()
    string testdir[256];
    int null = 0;

    testdir = "/home/stefan/nano/test/foobar";
    try_makedir (testdir);

    exit null;
funcend

func try_makedir (string path[256])
    int len;
    int dirend;
    int i = 0;
    int startpos = 0;
    int one = 1;
    int f;
    
    string slash[2] = "/";
    string ch[2];
    string makedirpath[256];
    
    strlen (path, len);

lab try_makedir_loop;
    part2string (path, i, ch);
    
    f = ch == slash;
    if f;
        f = i > 0;
        if f;
//  skip first slash: / home/stefan/nano
            dirend = i + 1;
            strleft (path, dirend, makedirpath);
    
            print "creating directory: ";
            print makedirpath;
            printn one;

            makedir (makedirpath);
        endif;
     endif;
    
    i = ++;
    
    f = i < len;
    if f;
        goto try_makedir_loop;
    endif;
funcend
