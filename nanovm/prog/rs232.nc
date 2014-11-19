// RS232 read/send demo
// flow 0.5.5 required!
//
// to run this demo:
// shell 1: $ flow 2000
// shell 2: $ nanovm rs232 127.0.0.1 2000 -stacks=1000

func main ()
    #include <socket.nch>

    int _err_sock;
    int _err_sock_ok = err_sock_ok;

    int _err_connrefused = err_connrefused;
    int _err_nosys = err_nosys;

    lint null = 0L; lint one = 1L; lint two = 2L;
    lint _client; lint _ret;
    lint port; lint _client_open = 0L;
    
    string s[256]; string ch[2]; string nch[2]; string cr[2]; string lf[2]; string ip[256];
    string buf[256]; string portstr[256]; string val[256]; string sh[256]; string lg[256];
    lint f; lint f1; lint f2; lint args; lint nlf; lint ncr;
    string fnewline[3];
    int delay = 10;

//	comport = /dev/ttyUSB0 on Linux, see flow: rs232.c
    int comport = 16; lint baudrate = 19200L;
    string buf[16];
    
    byte comportbuf[4096]; byte code; byte oldcode; int codeinit = 0; int codecount;
    int comportbuf_addr;
    
    int i; int max; int j;
    
    getaddress (comportbuf, comportbuf_addr);
    
//  nl = CRLF
    ncr = 13L;
    char (ncr, cr);
    fnewline = cr;
    nlf = 10L;
    char (nlf, lf);
    fnewline = fnewline + lf;
    _fnewline = fnewline;

//  check arguments
    argnum (args);
    f = args != one;
    if f;
        show_args ();
    endif;

    argstr (null, ip);
    argstr (one, portstr);
    val (portstr, port);

    nanogfx_init ();

    scopen (_client, ip, port);

    #ASSEMB
        pull_i      err_sock, _err_sock;
    #ASSEMB_END

    f = _err_sock == err_sock_ok;
    if f;
        goto ip_ok;
    endif;

    #NESTED_CODE_ON;
    
    buf = ip;
    hostbyname (buf, ip);
    scopen (_client, ip, port);
    check_err ();

lab ip_ok;
    _client_open = 1L;

    @rs232_open_comport (baudrate, comport, _client);
    get (_ret);
    f = _ret == 0;
    if f;
		print "comport opened reading port...";
		printn one;
	else;
		print "error: can't open comport!";
		printn one;
	endif;
    
//	send A
    code = 65;
    @rs232_send_byte (code, comport, _client);
    get (_ret);
    
    i = 66; max = 70; j = 0;
    
	for;
		code = i;
		var2array (code, comportbuf, j);
		i = ++; j = ++;
		f = i <= max;
	next f;
	
	j = 5;

//	send B C D E F
	@rs232_send_buf (j, comportbuf_addr, comport, _client);
	get (_ret);
    
    codecount = 0;
    
//	read from RS232 port: send "X" to end loop!!
    
lab loop;
    @rs232_poll_comport (one, comportbuf_addr, comport, _client);
    get (_ret);
    
    f = _ret > 0;
    if f;
		codecount = ++;
		array2var (comportbuf, null, code);
		f = codeinit == 0;
		if f;
			oldcode = code;
			codeinit = 1;
		else;
			f1 = codecount == 2;
			if f1;
				f2 = code != oldcode;
				if f2;
					print code;
					printn one;
		    
					oldcode = code;
					codecount = 0;
				endif;
			endif;
		endif;
	endif;
	
	waittick (delay);
    
    f = code != 88;
    if f;
//	user sended not a X -> continue reading from serial port
		goto loop;
	endif;
    
    waitsec (delay);

//	shutdown flow
    @nanogfx_shutdown (_client);

    get (_ret);
    check_answer ();

    end ();

    exit null;
funcend

func check_answer ()
    int f; int null = 0; int one = 1;

    f = _ret != null;
    if f;
        print "ERROR: server error!";
        printn one;
        end ();
    endif;

    print "OK!";
    printn one;
funcend

func show_args ()
    int n = 1;
    print "rs232 <ip> <port>";
    printn n;

    end ();
funcend

func end ()
    int ret = 0;
    int f; int one = 1;
    f = _client_open == one;
    if f;
        close_client ();
    endif;

    exit ret;
funcend

func close_client ()
    scclose (_client);
funcend

func check_err ()
    int f; int n = 1;

    #ASSEMB
        pull_i      err_sock, _err_sock;
    #ASSEMB_END

    f = _err_sock != _err_sock_ok;
    if f;
        goto error;
    endif;
    goto check_err_end;

lab error;
    print "socket error: ";
    print _err_sock;
    printn n;

    f = _err_sock == _err_connrefused;
    if f;
        print "server not found!";
        printn n;
    endif;

    f = _err_sock == _err_nosys;
    if f;
        print "tcp stack not running!";
        printn n;
    endif;

    end ();
lab check_err_end;
funcend

#include <flow.nah>
