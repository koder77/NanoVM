//  packetman 1.2 - part of the Nano VM packet manager
//  packetmanager frontend
//
// now *.tar.bz2 compressed archives can be installed and removed

func main ()
    #include <file.nch>

    string packetname[256];
    string packetnameuser[256];
    string action[256];
    string action_install[256] = "install";
    string action_remove[256] = "remove";
    string end[256] = "END";
    int packetfile;
    int packetfilecloud;
    string read[2] = "r";
    string write[2] = "w";
    string processcall[256] = "";
    string nanovm[256] = "nanovm cloudget ";
    string stack[256] = "-stacks=1000";
    string packetbase[256] = "base.np";
    string packetman[256] = "packetman.np";
    
    string nanopath[2] = "/";
    
    string packets_installed[4] = "np/";
    string packets_download[256] = "download/";
    
    string dotasc[5] = ".asc";
    int dotasc_len;
    string gpg_verify[256] = "gpg2 --verify ";
    
    string dotbzip2[5] = ".bz2";
    int dotbzip2_len;
    string unbzip[256] = "tar -xjf ";
    string unbzip2[256] = " -C ";
    string listbzip[256] = "tar -jtf ";
    string listbzip2[256] = " >> ";
   
    string source[256];
    string dest[256];
    
    string slash_newline[3];
    int newlinecode = 10;
    
    string yes[2] = "y";
    string no[2] = "n";
    string buf[256];
    string space[2];
    int spacecode = 32;
    
    int ret = 0;
    int do_action = -1;
    
    _err_file = 1;
    
    string packetfilename[256];
    string packetversion[256];
    string packetversioninst[256];
    string removepath[256];
    string packets_installed_path[256];
    string packets_download_path[256];
    int packetfile;
    
    int null = 0; int one = 1; int two = 2;
    int f; int args;
    int error;
    
    int installed = 0;
    int len;
    
//  threading
    int i; int unused = -1;
    int thread;
    int maxthreads = 9;
    int _threadexit[10];
    int threadfree;
    int threadret;
    int _threadmutex;
    int pret;
    int verify_packet = 1;

    #NESTED_CODE_ON;
    
    strlen (dotasc, dotasc_len);
    strlen (dotbzip2, dotbzip2_len);
    
    slash_newline = "/";
    char (newlinecode, buf);
    slash_newline = slash_newline + buf;
    
    print "packetman V 1.2";
    printn one;
    
//  init threads list
    i = 0;
    for;
        var2array (unused, _threadexit, i);
    
        i = ++;
        f = i <= maxthreads;
    next f;

    char (spacecode, space);
    
    argnum (args);
    f = args < one;
    if f;
        print "packetman <packet-name> <install | remove>";
        printn one;
        exit one;
    endif;
    
    argstr (null, packetnameuser);
    argstr (one, action);
    
    packetname = packetnameuser;
    
    f = action == action_install;
    if f;
        do_action = 1;
    endif;
    
    f = action == action_remove;
    if f;
        do_action = 0;
    endif;
    
    f = do_action < null;
    if f;
        print "packetman <packet-name> <install | remove>";
        printn one;
        print "unknown action!";
        printn one;
        print "I can't do everything...";
        printn one;
        exit one;
    endif;
    
//	make sure /np path exists

	packets_installed_path = nanopath;
	packets_installed_path = packets_installed_path + packets_installed;
	
	try_makedir (packets_installed_path);

//	make sure /download path exists
    packets_download_path = nanopath;
	packets_download_path = packets_download_path + packets_download;
	
	try_makedir (packets_download_path);
    
    
//	check if packet installed
	packetfilename = nanopath;
	packetfilename = packetfilename + packets_installed;
	packetfilename = packetfilename + packetname;

	fopen (packetfile, packetfilename, read);
	@file_error ();
	get (error);
	f = error != err_file_ok;
	if f;
		ret = 1;
		print "error: can't open packetfile: ";
		print packetfilename;
		printn one;

		installed = 0;
		goto check_installed_end;
	endif;

//  read packet version, ignored for now...
	freadls (packetfile, packetversioninst);
	@file_error ();
	get (error);
	f = error != err_file_ok;
	if f;
		ret = one;
		goto main_end_ok;
	endif;
    
	installed = 1;
	
lab check_installed_end;
        
    f = do_action == 1;
    if f;
//  download packet list: start cloudget
    
		processcall = "";
		processcall = processcall + nanovm;
		processcall = processcall + packetname;
		processcall = processcall + space;
		processcall = processcall + nanopath;
		processcall = processcall + space;
		processcall = processcall + stack;
    
		runsh (processcall, ret);
    
		print "RETURN CODE: ";
		print ret;
		printn one;
    
		f = ret != null;
		if f;
			print "error: can't load package list!";
			printn one;
			ret = one;
			goto packetlist_error;
		endif;
		
		print "packet list loaded...";
		printn one;
    
//  do action for each file in packet list
    
		packetfilename = nanopath;
		packetfilename = packetfilename + packetname;
    
		fopen (packetfilecloud, packetfilename, read);
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

//  read packet version...
		freadls (packetfilecloud, packetversion);
		@file_error ();
		get (error);
		f = error != err_file_ok;
		if f;
			goto main_end_ok;
		endif;
    
		print "packet version in cloud: ";
		print packetversion;
		printn two;
	endif;	
		

	f = installed == one;
	if f;
		print "packet version installed: ";
		print packetversioninst;
		printn two;
	endif;
		
    f = do_action == null;
    if f;
		f = installed == null;
		if f;
			print "ERROR: packet not installed!";
			printn one;
			goto main_end_ok;
		endif;
    
        f = packetnameuser == packetbase;
        if f;
// user wants to remove base package -> ask
        
            print "You want to remove the base package. Are you sure y|n ? ";
            input (buf);
            f = buf == no;
            if f;
                print "user aborted: no packages are removed!";
                printn one;
                
                do_action = -1;
                goto main_end_ok;
            endif;
        endif;
        
        f = packetnameuser == packetman;
        if f;
// user wants to remove packetman package ??? -> ask
        
            print "You want to remove the packetman package. Are you sure?";
            printn one;
            print "Without me it's impossible to do packet management!!!";
            printn one;
            print "DO YOU REALLY WANT TO REMOVE packetman y|n ? ";
            input (buf);
            f = buf == no;
            if f;
                print "user aborted: no packages are removed!";
                printn one;
                
                do_action = -1;
                goto main_end_ok;
            endif;
            
            print "Good luck! bye bye!";
            printn one;
        endif;
        
        print "You want to remove packages. Are you sure y|n ? ";
        input (buf);
        f = buf == no;
        if f;
            print "user aborted: no packages are removed!";
            printn one;
            
            do_action = -1;
            goto main_end_ok;
        endif;
    else;
        print "You want to install the ";
        print packetnameuser;
        print "-package. Are you sure y|n ? ";
        printn one;
        input (buf);
        f = buf == no;
        if f;
            print "user aborted: no packages will be installed!";
            printn one;
            
//	remove *.np packetlist

			do_action = -1;

			removepath = "/";
			removepath = removepath + packetnameuser;
        
			print "removing: ";
			print removepath;
			printn one;
        
			fremove (removepath);
			@file_error ();
			get (error);
			f = error != err_file_ok;
			if f;
				fclose (packetfilecloud);
        
				print "error can't remove: ";
				print removepath;
				printn one;
				ret = one;
				goto packetlist_error;
			endif;
            
            goto main_end_ok;
        endif;  
    endif;


lab loop;
	f = do_action == null;
	if f;
		freadls (packetfile, packetname);
		@file_error ();
		get (error);
		f = error != err_file_ok;
		if f;
			goto main_end_ok;
		endif;
	else;
		freadls (packetfilecloud, packetname);
		@file_error ();
		get (error);
		f = error != err_file_ok;
		if f;
			goto main_end_ok;
		endif;
	endif;
    
    print "package name: ";
    print packetname;
    printn one;
    
    f = do_action == null;
    if f;
//  remove file...
        removepath = "/";
        removepath = removepath + packetname;
        
        strright (removepath, one, buf);
        f = buf != "/";
        if f;
			print "removing: ";
			print removepath;
			printn one;
        
			fremove (removepath);
			@file_error ();
			get (error);
			f = error != err_file_ok;
			if f;
				fclose (packetfile);
        
				print "error can't remove: ";
				print removepath;
				printn one;
				ret = one;
				goto packetlist_error;
			endif;
		endif;
    else;
        processcall = "";
        processcall = processcall + nanovm;
        processcall = processcall + packetname;
        processcall = processcall + space;
        processcall = processcall + nanopath;
        processcall = processcall + packets_download;
        processcall = processcall + space;
        processcall = processcall + stack;
lab wait;
        
        threadfree = unused;
        i = 0;
        for;
            varlock (_threadmutex);
            array2var (_threadexit, i, threadfree);
            varunlock (_threadmutex);

            f = threadfree == unused;
            if f;
                threadfree = i;
                goto found_free_thread;
            endif;
                
            i = ++;
            f = i <= maxthreads;
        next f;
        
        
        f = threadfree == unused;
        if f;
            goto wait;
        endif;

lab found_free_thread;
        threadcreate (thread, run_process);
        threadsync (thread);
        threadpush (thread, threadfree);
        threadpush (thread, packetname);
        threadpush (thread, processcall);

        threadpushsync (thread);

//  check return codes
        varlock (_threadmutex);
        i = 0;
        for;
            array2var (_threadexit, i, threadret);
            f = threadret == null;
            if f;
                var2array (unused, _threadexit, i);
            else;
                f = threadret == one;
                if f;
                    print "ERROR: can't download file!"
                    ret = one;
                    fclose (packetfile);
                    varunlock (_threadmutex);
                    goto main_end;
                endif;
            endif;

            i = ++;
            f = i <= maxthreads;
        next f;
        varunlock (_threadmutex);
    endif;

    goto loop;
       
lab main_end_ok;
    threadjoin ();
    
//	check user aborted:
	f = do_action == -1;
	if f;
		goto main_end;
	endif;
    
    f = do_action == null;
    if f;
		f = installed == null;
		if f;
			goto main_end;
		endif;
	endif;
    


	f = do_action == one;
    if f;
//  verify files with gpg2
//	dummy read
		frewind (packetfilecloud);
		freadls (packetfilecloud, buf);
		@file_error ();
		get (error);
		f = error != err_file_ok;
		if f;
			ret = one;
			goto main_end_postinst;
		endif;
	
lab verify;
		freadls (packetfilecloud, packetname);
		@file_error ();
		get (error);
		f = error != err_file_ok;
		if f;
			goto install;
		endif;
		
		strright (packetname, dotasc_len, buf);
		f = buf == dotasc;
		if f;
//	file is .asc signature: do verify with gpg2
			processcall = "";
			processcall = processcall + gpg_verify;
			processcall = processcall + _nanopath;
			processcall = processcall + packets_download;
			processcall = processcall + packetname;
			
			runsh (processcall, pret);
			f = pret != null;
			if f;
				print "ERROR: verify of packet: ";
				print packetname;
				printsp (one);
				print "FAILED!!!";
				printn one;
				print
				ret = 1; verify_packet = 0;
				goto main_end_postinst;
			endif;
         endif;
         
         goto verify;

lab install;
//	copy files to nano directory (make install)        
//	dummy read
		frewind (packetfilecloud);
		freadls (packetfilecloud, buf);
		@file_error ();
		get (error);
		f = error != err_file_ok;
		if f;
			ret = one;
			goto main_end_postinst;
		endif;
		
		print "installing...";
		printn one;
		
 lab do_install;
		freadls (packetfilecloud, packetname);
		@file_error ();
		get (error);
		f = error != err_file_ok;
		if f;
			goto main_end_postinst;
		endif;

		source = "";
		source = source + packets_download;
		source = source + packetname;
		
		strright (packetname, dotbzip2_len, buf);
		f = buf == dotbzip2;
		if f;
//	file is tar.bz2 -> decompress it
			print "DECOMPRESSING bzip2 archive...";
			printn one;

			processcall = "";
			processcall = processcall + unbzip;
			processcall = processcall + space;
			processcall = processcall + _nanopath;
			processcall = processcall + source;
			processcall = processcall + unbzip2;
			processcall = processcall + _nanopath;
			
			runsh (processcall, pret);
			f = pret != null;
			if f;
				print "ERROR: unbzip2 of packet: ";
				print packetname;
				printsp (one);
				print "FAILED!!!";
				printn one;
				print
				ret = 2;
			else;
				print "packetman: file: "
				print source;
				printsp (one);
				print "successfully INSTALLED: OK";
				printn one;
				
				fclose (packetfilecloud);
				fremove (packetfilename);
				fopen (packetfilecloud, packetfilename, write);
				@file_error ();
				get (error);
				f = error != err_file_ok;
				if f;
					ret = 2;
					print "error: can't open packetfile: ";
					print packetfilename;
					printn one;
					goto create_list_end;
				endif;
				
				fwrite (packetfilecloud, packetversion);
				fwriten (packetfilecloud, one);
				fclose (packetfilecloud);
		
				processcall = "";
				processcall = processcall + listbzip;
				processcall = processcall + _nanopath;
				processcall = processcall + source;
				processcall = processcall + listbzip2;
				processcall = processcall + _nanopath;
				
				strlen (packetfilename, len);
				len = --;
				strright (packetfilename, len, buf);
				processcall = processcall + buf;
				
				runsh (processcall, pret);
				f = pret != null;
				if f;
					print "ERROR: file list of packet: ";
					print packetname;
					printsp (one);
					print "FAILED!!!";
					printn one;
					print
					ret = 2;
				endif;
lab skip;
			endif;
			
lab create_list_end;
		else;
			dest = nanopath;
			dest = dest + packetname;
	
			@copy_file (source, dest);
			get (i);
			f = i != null;
			if f;
				print "packetman: ERROR can't copy file: ";
				print source;
				printsp (one);
				print "to ";
				print dest;
				printn one;
				ret = 2;
			else;
				print "packetman: file: "
				print source;
				printsp (one);
				print "successfully INSTALLED: OK";
				printn one;
			endif;
		endif;
		
		buf = "/" + source;
		fremove (buf);
		@file_error ();
		get (error);
		f = error != err_file_ok;
		if f;
			fclose (packetfile);
        
			print "error can't remove: ";
			print source;
			printn one;
			ret = 2;
		endif;		
		goto do_install;
    endif; 
		
		
lab main_end_postinst;
    print "POSTINSTALL...";
    printn one;
    
    f = do_action == null;
    if f;
//	remove *.np packetlist

		removepath = "/";
		removepath = removepath + packets_installed;
		removepath = removepath + packetnameuser;
        
		print "removing: ";
		print removepath;
		printn one;
        
		fremove (removepath);
		@file_error ();
		get (error);
		f = error != err_file_ok;
		if f;
			fclose (packetfile);
        
			print "error can't remove: ";
			print removepath;
			printn one;
			ret = one;
			goto packetlist_error;
		endif;
	else;
//	copy packetfile *.np to np/ directory
		f = verify_packet == null;
		if f;
			goto remove_packet;
		endif;
		
		source = nanopath;
		source = source + packetnameuser;
	
		dest = nanopath;
		dest = dest + packets_installed;
		dest = dest + packetnameuser;
	
		@copy_file (source, dest);
		get (i);
		f = i != null;
		if f;
			print "packetman: ERROR can't copy packetlist to: ";
			print dest;
			printn one;
			ret = 2;
		else;
			print "packetman: packet successfully INSTALLED: OK";
			printn one;
		endif;
		
lab remove_packet;
		removepath = "";
		removepath = "/" + packetnameuser;
       
		print "removing ";
		print removepath;
		printn one;
        
		fremove (removepath);
		@file_error ();
		get (error);
		f = error != err_file_ok;
		if f;
			fclose (packetfile);
        
			print "error can't remove: ";
			print removepath;
			printn one;
			ret = one;
			goto packetlist_error;
		endif;
	endif;
	
lab main_end;

lab packetlist_error;
    f = ret == one;
    if f;
        f = do_action == null;
        if f;
            print "packetman: ERROR can't remove some files!";
            printn one;
        else;
			f = verify_packet == null;
			if f;
				print "FATAL ERROR: verify of files failed!!!";
				printn one;
				print "PACKET NOT INSTALLED!";
				printn one;
			else;
				print "packetman: ERROR!!! can't download some files!";
				printn one;
			endif;
        endif;
    endif;
    
    f = ret == null;
    if f;
        print "packetman: all OK!";
        printn one;
    endif;

    exit ret;
funcend

func copy_file (string source[256], string dest[256])
	#include <file.nch>
	
	int src;
	int dst;
	
	byte buf[100];
	int error; int f;
	int ret = 0;
	int one = 1;
	
	qint source_size;
	qint todo_size;
	qint read_size;
	
	string read[2] = "r";
	string write[2] = "w";
	
	#NESTED_CODE_ON;
	
	fopen (src, source, read);
	@file_error ();
	get (error);
	f = error != err_file_ok;
	if f;
		print "copy_file: can't open source!";
		printn one;
		ret = 1;
		goto copy_file_end;
	endif;
	
	fsize (src, source_size);
	
//	print "copy_file: filesize: ";
//	print source_size;
//	printn one;
	
	fopen (dst, dest, write);
	@file_error ();
	get (error);
	f = error != err_file_ok;
	if f;
		print "copy_file: can't open dest!";
		printn one;
		ret = 1;
		goto copy_file_end;
	endif;
	
	f = source_size < 100;
	if f;
		read_size = source_size;
	else;
		read_size = 100Q;
	endif;
	
	todo_size = source_size;
	
lab copy_file_loop;
//	print "copy_file: read_size: ";
//	print read_size;
//	printn one;

	freadab (src, buf, read_size);
	@file_error ();
	get (error);
	f = error != err_file_ok;
	if f;
		print "copy_file: read error!";
		printn one;
	
		f = error == err_file_eof;
		if f;
			ret = 0;
			goto copy_file_end;
		endif;
	endif;
	
	fwriteab (dst, buf, read_size);
	@file_error ();
	get (error);
	f = error != err_file_ok
	if f;
		print "copy_file: can't write to dest!";
		printn one;
		ret = 1;
		goto copy_file_end;
	endif;
	
	todo_size = todo_size - read_size;
	
	f = todo_size > 0;
	if f;
		f = todo_size > 100;
		if f;
			read_size = 100Q;
		else;
			read_size = todo_size;
		endif;
		
		goto copy_file_loop;
	endif;
	
	
lab copy_file_end;
	fclose (src);
	@file_error ();
	get (error);
	f = error != err_file_ok;
	if f;
		print "copy_file: can't close source!";
		printn one;	
	endif;
	
	fclose (dst);
	@file_error ();
	get (error);
	f = error != err_file_ok;
	if f;
		print "copy_file: can't close dest!";
		printn one;	
	endif;
	
	return (ret);
funcend
			
	

func run_process (#sync, string Ppathname[256], string Ppacketname[256], int Pind)
    int Pret = 0;
    int Pf; int Pnull = 0; int Pone = 1;
    
    runsh (Ppathname, Pret);
    
    Pf = Pret != Pnull;
    if Pf;
        print "error can't load package: ";
        print Ppacketname;
        printn Pone;
        Pret = Pone;
    endif;

    varlock (_threadmutex);
    var2array (Pret, _threadexit, Pind);
    varunlock (_threadmutex);
    exit Pret;
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
