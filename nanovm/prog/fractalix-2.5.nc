// 	based on a C sourcecode from www.computerix.info - Klaus Kusche
//
//	calculation speedup written in assembly
//
//  multithreading version (8 threads)

func main ()
    #include <math.nch>
    #include <socket.nch>
    
    int _err_sock;
    int _err_sock_ok = err_sock_ok;

    int _err_connrefused = err_connrefused;
    int _err_nosys = err_nosys;

    #NESTED_CODE_ON;

    int null = 0; int one = 1; int two = 2; int three = 3; int four = 4;
    int five = 5; int six = 6; int seven = 7; int eight = 8;
    int _r[192]; int _g[192]; int _b[192];
    int loopcount = 0; int loopmax = 60;
    int f;
    
    int _client;
    int _client_open = null;
    int _ret;
    
    int thread1; int thread2; int thread3; int thread4;
    int thread5; int thread6; int thread7; int thread8;
    int xstart; int xend;
    
    int width = 320; int height = 240;
    int width_thread = width;
    width_thread = width_thread / 8;
    
    int _draw_mutex;
    
    qint hundred_points_ticks = 2858687Q;
    qint hundred = 100Q;

    qint p;
    
    init_colors ();
//    debug_colors ();
    screen (width, height);
    
    jit	(jit_start1, jit_end1);
	jit	(jit_start2, jit_end2);
	jit (jit_start3, jit_end3);
    
    timeron ();
    
    threadcreate (thread1, draw_fract);
    threadsync (thread1);
    
    @get_xparam (width_thread, one);
    getmulti (xstart, xend); getmultiend ();
    threadpush (thread1, xend);
    threadpush (thread1, xstart);
    threadpush (thread1, height);
    threadpush (thread1, width);
    threadpushsync (thread1);
    
    
    threadcreate (thread2, draw_fract);
    threadsync (thread2);
    
    @get_xparam (width_thread, two);
    getmulti (xstart, xend); getmultiend ();
    threadpush (thread2, xend);
    threadpush (thread2, xstart);
    threadpush (thread2, height);
    threadpush (thread2, width);
    threadpushsync (thread2);
    
    
    threadcreate (thread3, draw_fract);
    threadsync (thread3);
    
    @get_xparam (width_thread, three);
    getmulti (xstart, xend); getmultiend ();
    threadpush (thread3, xend);
    threadpush (thread3, xstart);
    threadpush (thread3, height);
    threadpush (thread3, width);
    threadpushsync (thread3);
    
    
    threadcreate (thread4, draw_fract);
    threadsync (thread4);
    
	@get_xparam (width_thread, four);
    getmulti (xstart, xend); getmultiend ();
    threadpush (thread4, xend);
    threadpush (thread4, xstart);
    threadpush (thread4, height);
    threadpush (thread4, width);
    threadpushsync (thread4);
    
    
    threadcreate (thread5, draw_fract);
    threadsync (thread5);
    
	@get_xparam (width_thread, five);
    getmulti (xstart, xend); getmultiend ();
    threadpush (thread5, xend);
    threadpush (thread5, xstart);
    threadpush (thread5, height);
    threadpush (thread5, width);
    threadpushsync (thread5);
    
    
    threadcreate (thread6, draw_fract);
    threadsync (thread6);
    
	@get_xparam (width_thread, six);
    getmulti (xstart, xend); getmultiend ();
    threadpush (thread6, xend);
    threadpush (thread6, xstart);
    threadpush (thread6, height);
    threadpush (thread6, width);
    threadpushsync (thread6);
    
    
    threadcreate (thread7, draw_fract);
    threadsync (thread7);
    
	@get_xparam (width_thread, seven);
    getmulti (xstart, xend); getmultiend ();
    threadpush (thread7, xend);
    threadpush (thread7, xstart);
    threadpush (thread7, height);
    threadpush (thread7, width);
    threadpushsync (thread7);
    
    
    threadcreate (thread8, draw_fract);
    threadsync (thread8);
    
	@get_xparam (width_thread, eight);
    getmulti (xstart, xend); getmultiend ();
    threadpush (thread8, xend);
    threadpush (thread8, xstart);
    threadpush (thread8, height);
    threadpush (thread8, width);
    threadpushsync (thread8);
    
    threadjoin ();
    
    timeroff ();
    print "ms: ";
    print _timer;
    printn one;
    
    p = hundred_points_ticks * hundred / _timer;
    
    print "points: ";
    print p;
    printn two;
    
lab loop;
    waitsec (one);

    update_screen ();
    loopcount = ++;
    
    f = loopcount <= loopmax;
    if f;
		goto loop;
    endif;
    
    exit null;
funcend

func get_xparam (int width_thread, int threadn)
	int xstart; int xend;
	int one = 1;
	
	xend = threadn * width_thread;
	xstart = xend - width_thread;
	xend = --;
	
	print "xstart: "; print xstart; print " xend: "; print xend; printn one;
	
	returnmulti (xstart, xend);
funcend

func debug_colors ()
    int i = 0; int max = 192;
    int r; int g; int b;
    int one = 1;
    int f;
    
    for;
        array2var (_r, i, r);
        array2var (_g, i, g);
        array2var (_b, i, b);
    
        print r; printn one;
        print g; printn one;
        print b; printn one;
    
        i = ++;
        f = i < max;
    next f;
    
    print "COLORS END";
    printn one;
    printn one;
funcend

lab init_colors;
#ASSEMB
	#setreg_l		L0, i;
	#setreg_l		L1, j;
	#setreg_l		L2, max;
	#setreg_l		L3, color;
	#setreg_l		L4, c;
	#setreg_l		L5, x;

	push_i			0, i;
	push_i			64, max;

lab init_colors_loop;	
	push_i			0, color;
	move_i_a		color, _r, i;

	push_i			4, x;
	mul_l			x, i, x;
	move_i_a		x, _g, i;

	push_i			255, c;
	sub_l			c, x, color;
	move_i_a		color, _b, i;

	push_i			64, j;
	add_l			j, i, j;
	move_i_a		x, _r, j;

	push_i			255, c;
	sub_l			c, x, color;
	move_i_a		color, _g, j;

	push_i			0, color;
	move_i_a		color, _b, j;


	push_i			128, j;
	add_l			j, i, j;
	push_i			255, c;
	sub_l			c, x, color;
	move_i_a		color, _r, j;

	push_i			0, color;
	move_i_a		color, _b, j;

	move_i_a		x, _g, j;

	inc_ls_jmp_l 	i, max, init_colors_loop;
	rts;
#ASSEMB_END

func draw_fract (#sync, int Pscreen_width, int Pscreen_height, int Pxstart, int Pxend)
	double Pscreen_width_d; double Pscreen_height_d;
	
	int Pnull = 0; int Pone = 1;
	int Pi; int Pj; int Pk; int Pl;
	
	double Pm_d; double Pn_d; double Po_d; double Pp_d; double Pq_d; double Pr_d;
	int Pf;
	
	qint Plimit = 500Q;
	qint Px; qint Py;
	qint Ppixel;
	int Ptype = 1;
	int Ptwo = 2; int Pfour = 4;
	
	double Pxcenter = -0.10126177818;
	double Pycenter = 0.9542199377;
	
	double Pxpos; double Pypos;
	
	double Pxconst = 0.0;
	double Pyconst = 0.0;
	double Pzoom = 1.0;
	double PzoomStep = 1.05;
	double PzoomEnd = 10000000000000.0;
	double Ptwo_d = 2.0;
	double Pfour_d = 4.0;
	double Ppixel_d;
	
	double Pzx; double Pzy;
	double Pcx; double Pcy;
	double Pzxpwr; double Pzypwr;  
	
	#NESTED_CODE_GLOBAL_OFF;
	
    2double (Pscreen_width, Pscreen_width_d);
    2double (Pscreen_height, Pscreen_height_d);
	
	Pf = Pscreen_width < Pscreen_height;
	if Pf;
		Ppixel = Pscreen_width;
	else;
		Ppixel = Pscreen_height;
	endif;
	
	2double (Ppixel, Ppixel_d);
	
	print "width: ";
	print Pscreen_width;
	printn Pone;
	
	print "height: ";
	print Pscreen_height;
	printn Pone;
	
	print "pixel: ";
	print Ppixel_d;
	printn Pone;
	
	Px = Pxstart;
	for;
		Py = 0;
		for;
			calc_fract ();
	
//			print "xpos: ";
//			print xpos;
//			printsp (one);
//			print "ypos: ";
//			print ypos;
//			printn one;
		
			Py = ++;
			pull Py;
			Pf = Py < Pscreen_height;
		next Pf;
		
		update_screen ();
		
		Px = ++;
		pull Px;
		Pf = Px <= Pxend;
	next Pf;
	
	exit Pnull;
funcend

lab calc_fract;
#ASSEMB
	#setreg_d		D0, screen_width;
	#setreg_d		D1, screen_height;
	#setreg_d		D2, two;
	#setreg_d		D3, o_d;
	#setreg_d		D4, n_d;
	#setreg_d		D5, four;
	#setreg_d		D6, p_d;
	#setreg_d		D7, pixel;
	#setreg_d		D8, m_d;
	#setreg_d		D9, q_d;
	#setreg_d		D10, r_d;
	#setreg_d		D11, xpos;
	#setreg_d		D12, ypos;
	#setreg_d		D13, xcenter;
	#setreg_d		D14, ycenter;
	#setreg_d		D15, zoom;
	#setreg_d		D16, zx;
	#setreg_d		D17, zy;
	#setreg_d		D18, cx;
	#setreg_d		D19, cy;
	#setreg_d		D20, xconst;
	#setreg_d		D21, yconst;
	#setreg_d		D22, zxpwr;
	#setreg_d		D23, zypwr;

	#setreg_l		L0, x;
	#setreg_l		L1, y;
	#setreg_l		L2, i;
	#setreg_l		L3, limit;
	#setreg_l		L4, type;
	#setreg_l		L5, null;
	#setreg_l		L6, f;
	#setreg_l		L7, one;
	#setreg_l		L8, max_color;
	#setreg_l		L9, client;
	#setreg_l		L10, ret;
	#setreg_l		L11, color;
	#setreg_l		L12, r;
	#setreg_l		L13, g;
	#setreg_l		L14, b;

	ppush_i			Ptype@draw_fract, type;
	push_i			0, null;
	push_i			1, one;
	push_i			192, max_color;
	push_i			_client, client;
	ppush_q			Plimit@draw_fract, limit;

	ppush_d			Pxconst@draw_fract, xconst;
	ppush_d			Pyconst@draw_fract, yconst;

	ppush_q			Px@draw_fract, x;
	2double			x, n_d;

	ppush_d			Pscreen_width_d@draw_fract, screen_width;
	ppush_d			Pscreen_height_d@draw_fract, screen_height;
	ppush_d			Ptwo_d@draw_fract, two;
	ppush_d			Pfour_d@draw_fract, four;
	ppush_d			Ppixel_d@draw_fract, pixel;
	ppush_d			Pzoom@draw_fract, zoom;
	ppush_d			Pxcenter@draw_fract, xcenter;
	ppush_d			Pycenter@draw_fract, ycenter;

lab jit_start1;
	div_d			screen_width, two, r_d;
	sub_d			n_d, r_d, o_d;
	mul_d			o_d, four, m_d;

	mul_d			pixel, zoom, p_d;
	div_d			m_d, p_d, q_d;

	add_d			xcenter, q_d, xpos;
lab jit_end1;	

	ppull_d			xpos, Pxpos@draw_fract;


	ppush_q			Py@draw_fract, y;			
	2double			y, n_d;

lab jit_start2;
	div_d			screen_height, two, r_d;
	sub_d			n_d, r_d, o_d;
	mul_d			o_d, four, m_d;

	div_d			m_d, p_d, q_d;

	add_d			ycenter, q_d, ypos;
lab jit_end2;

	ppull_d			ypos, Pypos@draw_fract;

lab jit_start3;
	eq_jmp_l		type, null, type_null;

	move_d			xpos, cx;
	move_d			ypos, cy;
	move_d			xconst, zx;
	move_d			yconst, zy;
	jmp				type_set;

lab type_null;
	move_d			xpos, zx;
	move_d			ypos, zy;
	move_d			xconst, cx;
	move_d			yconst, cy;

lab type_set;	

	move_l			limit, i;

lab for_i;
	mul_d			zx, zx, m_d;
	mul_d			zy, zy, n_d;
	add_d			m_d, n_d, o_d;

	ls_d			o_d, four, f;
	jmp_l			f, not_cyclic_point;

	jmp    			cyclic_point;

lab not_cyclic_point;

	sub_d			m_d, n_d, zxpwr;
	mul_d			two, zx, zypwr;
	mul_d			zypwr, zy, zypwr;

	add_d			zxpwr, cx, zx;
	add_d			zypwr, cy, zy;

	dec_greq_jmp_l	i, null, for_i;


lab cyclic_point;
lab jit_end3;
	nop;

	var_lock		_draw_mutex;

	ls_jmp_l		i, null, draw_color_black;

	jmp				draw_color;

lab draw_done;	

	var_unlock		_draw_mutex;
	rts;


lab draw_color_black;
	stpush_all_l;
    stpush_all_d;

	stpush_l		client;
	stpush_l		null;
	stpush_l		null;
	stpush_l		null;

	jsr				nanogfx_color;

	stpush_l		client;
	stpush_l		x;
	stpush_l		y;

	jsr				nanogfx_pixel;

	stpull_l		ret;
	pull_i			ret, _ret;


	jsr				check_answer;

	stpull_all_d;
	stpull_all_l;

	jmp 			draw_done;

lab draw_color;
	mod_l			i, max_color, color;

	move_a_i		_r, color, r;
	move_a_i		_g, color, g;
	move_a_i		_b, color, b;

	stpush_all_l;
    stpush_all_d;

	stpush_l		client;
	stpush_l		r;
	stpush_l		g;
	stpush_l		b;

	jsr				nanogfx_color;

	stpush_l		client;
	stpush_l		x;
	stpush_l		y;

	jsr				nanogfx_pixel;

	stpull_l		ret;
	pull_i			ret, _ret;

	jsr				check_answer;

	stpull_all_d;
	stpull_all_l;

	jmp 			draw_done;

	#unsetreg_all_l;
	#unsetreg_all_d;
#ASSEMB_END


func update_screen ()
	varlock (_draw_mutex);
    
    @nanogfx_updatescreen (_client);

    get (_ret);
    check_answer ();

    varunlock (_draw_mutex);
funcend

func screen (int width, int height)
    #include <socket.nch>
    
    lint nlf; lint ncr;
    string cr[2]; string lf[2];
    string fnewline[3];
    
    int args; int null = 0; int one = 1; int two = 2;
    string ip[256]; string portstr[256]; string buf[256];
    int port;
    
    int f;
    
    string s[256];
    int alpha = 255; int bit = 24;
    int _ret;
    
    
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

    f = _err_sock == _err_sock_ok;
    if f;
        goto ip_ok;
    endif;

    buf = ip;
    hostbyname (buf, ip);
    scopen (_client, ip, port);
    check_err ();
    
 lab ip_ok;
    _client_open = 1;
    s = "nano - fractalix parallel";

    @nanogfx_openscreen (alpha, bit, height, width, s, s, null, _client);
    get (_ret);
    f = _ret != null;
    if f;
        print "no screen!";
        printn one;
        end ();
    endif;

    scclose (_client);

    waitsec (two);
    
    scopen (_client, ip, port);
    check_err ();
    
    clear_screen ();
    color_white ();
funcend
 
func circle (int x, int y, int radius)
    @nanogfx_circle (radius, y, x, _client);
    get (_ret);
    check_answer ();
funcend

func line (int x, int y, int x2, int y2)
    @nanogfx_line (y2, x2, y, x, _client);
    get (_ret);
    check_answer ();
funcend
 
func clear_screen ()
    int null = 0; int alpha = 255;

    nanogfx_color (null, null, null, _client);

    @nanogfx_clearscreen (_client);

    get (_ret);
    check_answer ();

    nanogfx_alpha (alpha, _client);
funcend

func color_white ()
    int r = 255; int g = 255; int b = 255;

    nanogfx_color (b, g, r, _client);
funcend


func check_answer ()
    int f; int null = 0; int one = 1;

    f = _ret != null;
    if f;
        print "ERROR: server error!";
        printn one;
        end ();
    endif;

//    print "OK!";
//    printn one;
funcend

func show_args ()
    int n = 1;
    print "fractalix <ip> <port>";
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
	int error;

	@nanogfx_shutdown (_client);
	get (error);

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
