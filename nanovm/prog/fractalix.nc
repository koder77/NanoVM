// 	based on a C sourcecode from www.computerix.info - Klaus Kusche
//
//	calculation speedup written in assembly
//	turn -O2 optimization on: $ nanoc fractalix -O2

func main ()
    #include <math.nch>
    #include <socket.nch>

    int _err_sock;
    int _err_sock_ok = err_sock_ok;

    int _err_connrefused = err_connrefused;
    int _err_nosys = err_nosys;


    int null = 0; int one = 1;
    int _r[192]; int _g[192]; int _b[192];
    
    int _client;
    int _client_open = null;
    int _ret;
    
    int width = 320; int height = 240;
    
    init_colors ();
//    debug_colors ();
    screen (width, height);
    draw_fract (width, height);
    
lab loop;
    waitsec (one);

    update_screen ();
    goto loop;
    
    exit null;
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

func draw_fract (int screen_width, int screen_height)
	double screen_width_d; double screen_height_d;
	
	int null = 0; int one = 1;
	int i; int j; int k; int l;
	
	double m_d; double n_d; double o_d; double p_d; double q_d; double r_d;
	int f;
	
	qint limit = 500Q;
	qint x; qint y;
	qint pixel;
	int type = 1;
	int two = 2; int four = 4;
	
	double xcenter = -0.10126177818;
	double ycenter = 0.9542199377;
	
	double xpos; double ypos;
	
	double xconst = 0.0;
	double yconst = 0.0;
	double zoom = 1.0;
	double zoomStep = 1.05;
	double zoomEnd = 10000000000000.0;
	double two_d = 2.0;
	double four_d = 4.0;
	double pixel_d;
	
	double zx; double zy;
	double cx; double cy;
	double zxpwr; double zypwr;  
	
	#NESTED_CODE_ON;
	
    2double (screen_width, screen_width_d);
    2double (screen_height, screen_height_d);
	
	f = screen_width < screen_height;
	if f;
		pixel = screen_width;
	else;
		pixel = screen_height;
	endif;
	
	2double (pixel, pixel_d);
	
	x = 0;
	for;
		y = 0;
		for;
			calc_fract ();
	
//			print "xpos: ";
//			print xpos;
//			printsp (one);
//			print "ypos: ";
//			print ypos;
//			printn one;
		
			y = ++;
			f = y < screen_height;
		next f;
		
		update_screen ();
		
		x = ++;
		f = x < screen_width;
	next f;
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

	push_i			type@draw_fract, type;
	push_i			0, null;
	push_q			limit@draw_fract, limit;

	push_d			xconst@draw_fract, xconst;
	push_d			yconst@draw_fract, yconst;

	push_q			x@draw_fract, x;
	2double			x, n_d;

	push_d			screen_width_d@draw_fract, screen_width;
	push_d			screen_height_d@draw_fract, screen_height;
	push_d			two_d@draw_fract, two;
	push_d			four_d@draw_fract, four;
	push_d			pixel_d@draw_fract, pixel;
	push_d			zoom@draw_fract, zoom;
	push_d			xcenter@draw_fract, xcenter;
	push_d			ycenter@draw_fract, ycenter;

	div_d			screen_width, two, r_d;
	sub_d			n_d, r_d, o_d;
	mul_d			o_d, four, m_d;

	mul_d			pixel, zoom, p_d;
	div_d			m_d, p_d, q_d;

	add_d			xcenter, q_d, xpos;
	pull_d			xpos, xpos@draw_fract;


	push_q			y@draw_fract, y;			
	2double			y, n_d;

	div_d			screen_height, two, r_d;
	sub_d			n_d, r_d, o_d;
	mul_d			o_d, four, m_d;

	div_d			m_d, p_d, q_d;

	add_d			ycenter, q_d, ypos;
	pull_d			ypos, ypos@draw_fract;

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

	stpush_all_l;
    stpush_all_d;

    stpush_l		i;
    stpush_l		y;
    stpush_l		x;

    jsr				draw_cyclic_point;

    stpull_all_d;
    stpull_all_l;
    jmp next_y;

lab not_cyclic_point;

	sub_d			m_d, n_d, zxpwr;
	mul_d			two, zx, zypwr;
	mul_d			zypwr, zy, zypwr;

	add_d			zxpwr, cx, zx;
	add_d			zypwr, cy, zy;

	dec_greq_jmp_l	i, null, for_i;

lab next_y;
	rts;

	#unsetreg_all_l;
	#unsetreg_all_d;
#ASSEMB_END


func draw_cyclic_point (qint x, qint y, qint color)
    int f; int null = 0;
    int draw_color;
    int max_color = 192;
    int r; int g; int b;
    
    int one = 1;
    
    #NESTED_CODE_ON;
    
    f = color < null;
    if f;
        nanogfx_color (null, null, null, _client);
        @nanogfx_pixel (y, x, _client);
    
        get (_ret);
        check_answer ();
    else;
        draw_color = color % max_color;
        array2var (_r, draw_color, r);
        array2var (_g, draw_color, g);
        array2var (_b, draw_color, b);
        
        nanogfx_color (b, g, r, _client);
        @nanogfx_pixel (y, x, _client);
    
        get (_ret);
        check_answer ();
    endif;
    
    update_screen ();
funcend

func update_screen ()
    int one = 1;

    @nanogfx_updatescreen (_client);

    get (_ret);
    check_answer ();

    printn one;
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
    
    print "hostbyname...";
    printn one;
    
    hostbyname (buf, ip);
    scopen (_client, ip, port);
    check_err ();
    
 lab ip_ok;
	print "ip ok!";
	printn one;
 
    _client_open = 1;
    s = "nano - fractalix";

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

func update_screen ()
    int one = 1;

    @nanogfx_updatescreen (_client);

    get (_ret);
    check_answer ();

    printn one;
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
