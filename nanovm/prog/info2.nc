func main ()
    #include <host.nch>

    int null = 0; int one = 1; int two = 2;
    int x;

    print "nano version: ";
    print _version;
    printn one;

    print "register: ";
    print _vmregs;
    printn two;

    print "host machine: ";

    x = _machine == machine_68k;
    if x;
        print "Motorola 68k";
    else;
        print "X86";
    endif;

    printn one;

    print "host os: ";

    x = _os == os_amiga;
    if x;
        print "Amiga OS";
    endif;

    x = _os == os_aros;
    if x;
        print "AROS";
    endif;

    x = _os == os_linux;
    if x;
        print "Linux";
    endif;

    x = _os == os_windows;
    if x;
        print "Windows";
    endif;

    x = _os == os_android;
    if x;
        print "Android";
    endif;
    
    x = _os == os_dragonfly;
    if x;
		print "DragonFly BSD";
	endif;
    
    printn two;
    exit null;
funcend;
