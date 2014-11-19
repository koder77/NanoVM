//  txtread.nc
//  show a textfile

func main ()
    #include <file.nch>

//  switch error code handling on
    _err_file = 1;
    
    string filename[256];
    string buf[256]; string ch[2];
    int file; int f; int line = 0; int maxline = 20;
    string read[2] = "r";
    int null = 0; int one = 1; int error;

    print "file to read? ";
    input (filename);
    fopen (file, filename, read);
    @file_error ();
    get (error);
    f = error == err_file_open;
    if f;
        print "error: can't open file!";
        printn one;
        exit one;
    endif;

lab read;
    freadls (file, buf);
    @file_error ();
    get (error);
    f = error == err_file_read;
    if f;
        print "error: can't read file!";
        printn one;
        exit one;
    endif;
    
    f = error == err_file_eof;
    if f;
        print "[END OF FILE]";
        printn one;
        fclose (file);
        exit null;
    endif;
    
    print buf;
    printn one;
    
    line = ++;
    f = line == maxline;
    if f;
        ch = "";
        print "< [RETURN]: next lines | [r]: rewind | [q]: quit >";
        input (ch);
        f = ch == "q;
        if f;
            print "[QUIT]";
            printn one;
            fclose (file);
            exit null;
        endif;
    
        f = ch == "r";
        if f;
            frewind (file);
        endif;
        
        line = 0;
    endif;
    
    goto read;
funcend

#include <error.nch>
