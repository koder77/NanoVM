//  cloudget 1.2 - part of the Nano VM packet manager
//  downloads files from repository
//

func main ()
    #include <file.nch>

    int null = 0; int one = 1; int two =  2;
    int _client; string _ip[256] = ""; int _port = 80;
    string repo[256] = "/nano/repo/";
    
    string _redirect[256] = "/nano/repo/redirect";
    string redirect_file[256] = "redirect";
    string redirect_server[256];
    int redirect_filep;
    
    string _filename[256];
    string _reponame[256];
    string _nanopath[256];
    string newrepopath[256];
    string download_filename[256];
    
    int args; int f; int ret = 0;
    
    int config;
    string configfilename[256] = "cloudget.conf";
    string read[2] = "r";
    int error;
    
    #NESTED_CODE_ON;
    
    set_linefeed ();
    
    _err_file = 1;
    
    fopen (config, configfilename, read);
    @file_error ();
    get (error);
    f = error != err_file_ok;
    if f;
        ret = 1;
        print "error: can't open config: ";
        print configfilename;
        printn one;

        goto main_end;
    endif;
    
    argnum (args);
    f = args < one;
    if f;
        print "cloudget <filename> <nanopath>";
        printn one;
        ret = 1;
        goto main_end;
    endif;
    
    argstr (null, _filename);
    argstr (one, _nanopath);
    
    _reponame = repo;
    _reponame = _reponame + _filename;

lab loop;
//  try to download file from repositories from configlist

    freadls (config, _ip);
    @file_error ();
    get (error);
    f = error != err_file_ok;
    if f;
        ret = 1;
        print "error: can't read config: ";
        print configfilename;
        printn one;
        fclose (config);
        goto main_end;
    endif;

lab do_redirect;
    @socket_connect ();
    get (ret);
    f = ret != 0;
    if f;
        print "error: can't connect to repository server: ";
        print _ip;
        printn one;
        ret = 1;
        goto main_end;
    endif;
    
    @convert_repo_path (_redirect);
    get (newrepopath);
    
    print "check for redirection: ";
    print _redirect;
    printsp (one);
    print "on: ";
    print _ip;
    printn one;
    
    http_send_request (_client, _ip, newrepopath);
    
    print "downloading file...";
    printn one;
    
    download_filename = _nanopath;
    download_filename = download_filename + redirect_file;
   
	try_makedir (download_filename);
   
    @http_download_file (_client, download_filename);
    socket_close ();
    get (ret);
    f = ret != 0;
    if f;
        print "error: can't download file: ";
        print download_filename;
        printn one;
        goto no_redirect;
    endif;

    print "downloaded file. All OK! -> REDIRECTED";
    printn one;
    
    fopen (redirect_filep, download_filename, read);
    @file_error ();
    get (error);
    f = error != err_file_ok;
    if f;
        ret = 1;
        print "error: can't open redirect file: ";
        print download_filename;
        printn one;

        goto no_redirect;
    endif;
    
    freadls (redirect_filep, _ip);
    @file_error ();
    get (error);
    f = error != err_file_ok;
    if f;
        ret = 1;
        print "error: can't read redirect file: ";
        print redirect_file;
        printn one;
        fclose (redirect_filep);
        goto no_redirect;
    endif;
    
    goto do_redirect;
    
lab no_redirect;
    @convert_repo_path (_reponame);
    get (newrepopath);
    
    _reponame = newrepopath;

    print "sending request: ";
    print _reponame;
    printsp (one);
    print "to: ";
    print _ip;
    printn one;
    
    @socket_connect ();
    get (ret);
    f = ret != 0;
    if f;
        print "error: can't connect to repository server: ";
        print _ip;
        printn one;
        ret = 1;
        goto main_end;
    endif;
    
    http_send_request (_client, _ip, _reponame);
    
    print "downloading file...";
    printn one;
    
    download_filename = _nanopath;
    download_filename = download_filename + _filename;
    
    print "downloading file ";
    print download_filename;
    printn one;
    
    try_makedir (download_filename);
    
    @http_download_file (_client, download_filename);
    socket_close ();
    get (ret);
    f = ret != 0;
    if f;
        print "error: can't download file!";
        printn one;
        
        ret = 1;
        goto loop;
    endif;

    print "downloaded file. All OK!";
    printn one;
    
    fclose (config);
    socket_close ();
    
lab main_end;
    exit ret;
funcend

func convert_repo_path (string path[256])
    string newpath[256];
    string ch[2];
    string slash[2] = "/";
    string backslash[2] = "\";
    int i; int f; int pathlen;
    
    strlen (path, pathlen);
    i = 0;

    newpath = "";

    for;
        part2string (path, i, ch);
        f = ch == backslash;
        if f;
            newpath = newpath + slash;
        else;
            newpath = newpath + ch;
        endif;

        i = ++;
        f = i < pathlen;
    next f;

    return (newpath);
funcend

func set_linefeed ()
//  newline = CRLF
    int ncr = 13; int nlf = 10;
    string cr[2]; string lf[2];
    
    char (ncr, cr);
    char (nlf, lf);
    
    _fnewline = cr;
    _fnewline = _fnewline + lf;
funcend

func socket_connect ()
    #include <socket.nch>
    
    int error; int f;
    int ret = 0;
    
    scopen (_client, _ip, _port);
    @socket_error ();
    get (error);

    f = error != err_sock_ok;
    if f;
        ret = 1;
        goto connect_end;
    endif;
    
lab connect_end;
    return (ret);
funcend
    
func socket_close ()
    scclose (_client);
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

#include <error.nch>
#include <net/download.nch>
