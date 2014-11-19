// Nano make - build programs downloaded on packet-list
// 1.1

func main ()
    #include <file.nch>

    string packetnameuser[256];
    string packetfilename[256];
    string packetname[256];
    string packetnamewsuffix[256];
    string packetversion[256];
    string filename[256];
    string nanopath[256];
    string processcall[256] = "";
    
    string nanoa[256] = "nanoa ";
    string nanoc[256] = "nanoc ";
    string stack[256] = "-stacks=10000";
    string objs[256] = "-objs=100000";
    string lines[256] = "-lines=1000000";
    string read[2] = "r";
    string dotna[4] = ".na";
    string dotnc[4] = ".nc";
    
    string packets_installed[4] = "np/";
    
    string dotasc[5] = ".asc";
    int dotasclen = 4;
    string gpg[256] = "gpg2 --detach-sign --armor --openpgp ";
    string signflag[256] = "sign";
    string signallflag[256] = "sign-all";
    
    string space[2];
    string buf[256];
    
    string nanopath[2] = "/";
    
    int spacecode = 32;
    
    int null = 0; int one = 1;
    int f; int args;
    int error;
    int config;
    int packetfile;
    int ret = 0;
    int pret;
    int suffixlen = 3;
    int len;
    
    int compile; int assemble; int make; 
    int sign = 0;
    
    char (spacecode, space);
    
    #NESTED_CODE_ON;
    
     _err_file = 1;
    
    argnum (args);
    f = args < null;
    if f;
        print "make <packet-name> [sign]";
        printn one;
        exit one;
    endif;
    
    argstr (null, packetnameuser);
    
    f = args == one;
    if f;
		argstr (one, buf);
		f = buf == signflag;
		if f;
			sign = 1;
			print "signing with gpg2!";
			printn one;
		endif;
		
		f = buf == signallflag;
		if f;
			sign = 2;
			print "signing all files with gpg2!";
			printn one;
		endif;
	endif;
		
    
//  do make for each program in packet list
    
    packetfilename = nanopath;
    packetfilename = packetfilename + packets_installed;
    packetfilename = packetfilename + packetnameuser;
    
    fopen (packetfile, packetfilename, read);
    @file_error ();
    get (error);
    f = error != err_file_ok;
    if f;
        ret = 1;
        print "error: can't open packetfile: ";
        print packetfilename;
        printn one;

        goto main_end;
    endif;
    
//  read packet version, ignored for now...
    freadls (packetfile, packetversion);
    @file_error ();
    get (error);
    f = error != err_file_ok;
    if f;
        goto main_end_ok;
    endif;
    
    print "making files... ";
    printn one;
    
lab loop;
    compile = 0;
    assemble = 0;
    make = 0;

    freadls (packetfile, packetname);
    @file_error ();
    get (error);
    f = error != err_file_ok;
    if f;
        goto main_end_ok;
    endif;
    
    strright (packetname, suffixlen, buf);
    
    f = buf == dotnc;
    if f;
        compile = 1; assemble = 1; make = 1;
    endif;
    f = buf == dotna;
    if f;
        assemble = 1; make = 1;
    endif;
    
// check if compile and assemmble
    
    f = make == null;
    if f;
//  nothing to do ?

		f = sign <= 1;
		if f;
			goto loop;
		endif;
    endif;
    
    strlen (packetname, len);
    len = len - suffixlen;
    strleft (packetname, len, packetnamewsuffix);
    
    f = compile == one;
    if f;
//  start compiler
    
         processcall = "";
         processcall = processcall + nanoc;
         processcall = processcall + nanopath;
         processcall = processcall + packetnamewsuffix;
         
         runsh (processcall, pret);
         f = pret != null;
         if f;
            print "error: can't compile ";
            print packetname;
            printn one;
            fclose (packetfile);
            ret = 1;
            goto main_end;
         endif;
     endif;
     
     f = assemble == one;
     if f;
//  start assembler

        processcall = "";
        processcall = processcall + nanoa;
        processcall = processcall + nanopath;
        processcall = processcall + packetnamewsuffix;
        processcall = processcall + space;
        processcall = processcall + objs;
        processcall = processcall + space;
        processcall = processcall + lines;
        
        runsh (processcall, pret);
        f = pret != null;
        if f;
            print "error: can't assemble ";
            print packetname;
            printn one;
            fclose (packetfile);
            ret = 1;
            goto main_end;
        endif;
    endif;
    
    f = sign >= one;
    if f;
// sign file with gpg2

		strright (packetname, dotasclen, buf);
    
		f = buf != dotasc;
		if f;
			processcall = "";
			processcall = gpg;
			processcall = processcall + nanopath;
			processcall = processcall + packetname;
		
			runsh (processcall, pret);
			f = pret != null;
			if f;
				print "error: can't sign with gpg2 ";
				print packetname;
				printn one;
				fclose (packetfile);
				ret = 1;
				goto main_end;
			endif;
        endif;
    endif;
    
    goto loop;
    
lab main_end_ok;
    fclose (packetfile);
    ret = 0;
    
    print "all files maked: all OK!";
    printn one;

lab main_end;
    exit ret;
funcend

#include <error.nch>
