func main ()
    #NESTED_CODE_ON;
    
    int null = 0; int one = 1; int two = 2;
    lint _client;
    lint port; int _client_open = 0;
    
    lint args; int f; lint i;

    string ip[256]; string buf[256]; string s[256];
    lint bit = 24L; lint alpha = 255L;
    lint width = 640L; lint height = 480L;
    int r; int g; int b;

    lint x; int y; lint x2; lint y2;
    int _ret = 0;
    
    lint randstart = 2012L;
    int _expertmode = 0;
    
    int _playermove = 0;
    int _game_over = 0;
    
//  fields
    int _f0; int _f1; int _f2; int _f3; int _f4; int _f5; int _f6; int _f7; int _f8;
    int _t0; int _t1; int _t2; int _t3; int _t4; int _t5; int _t6; int _t7; int _t8;
    
    int next_move; int check;
    
    int _win_x = 440; int _win_y = 440;
    
//  gadget event
	lint gadgetnum;
    lint dgadget;
    lint l_value;
    double d_value;
    string s_value[256];
    int gadgetret;
    
    srand (randstart);
    
    waitsec (two);
    
    screen (width, height);
    init_gui_font ();
    init_gui_colors ();
    
    update_screen ();
   
	draw_gui ();

    init_field ();
	
    update_screen ();
    
lab gadget_event;
    print "playermove: ";
    print _playermove;
    printn one;
    
    @nanogfx_gadget_event (_client);

    getmulti (s_value, d_value, l_value, gadgetnum, _ret);
//    getmulti (_ret, gadgetnum, l_value, d_value, s_value);
    getmultiend ();
    
    f = _game_over == null;
    if f;
        f = gadgetnum == 0; 
        if f;
           @gadget_0 ();
           get (gadgetret);
           
           f = gadgetret == null;
           if f;
              goto gadget_event;
           endif;
        endif;

        f = gadgetnum == 1;
        if f;
           @gadget_1 ();
           get (gadgetret);
           
           f = gadgetret == null;
           if f;
              goto gadget_event;
           endif;
        endif;

        f = gadgetnum == 2;
        if f;
           @gadget_2 ();
           get (gadgetret);
           
           f = gadgetret == null;
           if f;
              goto gadget_event;
           endif;
        endif;

        f = gadgetnum == 3;
        if f;
           @gadget_3 ();
           get (gadgetret);
           
           f = gadgetret == null;
           if f;
              goto gadget_event;
           endif;
        endif;

        f = gadgetnum == 4;
        if f;
           @gadget_4 ();
           get (gadgetret);
           
           f = gadgetret == null;
           if f;
              goto gadget_event;
           endif;
        endif;

        f = gadgetnum == 5;
        if f;
           @gadget_5 ();
           get (gadgetret);
           
           f = gadgetret == null;
           if f;
              goto gadget_event;
           endif;
        endif;

        f = gadgetnum == 6;
        if f;
           @gadget_6 ();
           get (gadgetret);
           
           f = gadgetret == null;
           if f;
              goto gadget_event;
           endif;
        endif;

        f = gadgetnum == 7;
        if f;
           @gadget_7 ();
           get (gadgetret);
           
           f = gadgetret == null;
           if f;
              goto gadget_event;
           endif;
        endif;

        f = gadgetnum == 8;
        if f;
           @gadget_8 ();
           get (gadgetret);
           
           f = gadgetret == null;
           if f;
              goto gadget_event;
           endif;
        endif;
    endif;
    
    f = gadgetnum == 9;
    if f;
        init_field ();
    endif;
    
    f = gadgetnum == 10;
    if f;
        _expertmode = l_value;
        goto gadget_event;
    endif;
    
    f = gadgetnum == 11; 
    if f; end (); endif;
    
    f = _game_over == null;
    if f;
        _playermove = ++;

        i = 5;
        f = _playermove <= i;
        if f;
			think ();
        else;
            check_winner ();
        endif;
    endif;
    
    goto gadget_event;
funcend

func init_gui_font ()
	int one = 1;
	
	string font[256];
	int size;

//  load font
    print "loading font...";
    printn one;

    size = 25;
    font = "truetype/freefont/FreeMono.ttf";
    
    @nanogfx_loadfont_ttf (size, font, _client);
    get (_ret);
    check_answer ();
    
    print "font loaded!";
    printn one;
funcend

func draw_gui ()
	string s[256];
	
	string normal[80] = "normal";
    string expert[80] = "expert";
    string copyright[80] = "(C) 2013 by Stefan Pietzonke";
	
	int null = 0; int one = 1; int two = 2;
	int gadgets = 13;

	int x; int y = 10; int x2; int y2; int width = 120;
	int i; int j = 0;
	int gadgetnum = 0;
	int f;
	
//  set number of gadgets
    
    nanogfx_gadgets (gadgets, _client);

    color_black ();
    
	for;
		x = 10;
		i = 0;
		
		for;
			x2 = x + width;
			y2 = y + width;
			
			@nanogfx_rectangle (y2, x2, y, x, _client);
			get (_ret);
			check_answer ();
    
			update_screen ();

			@nanogfx_set_gadget_box (one, y2, x2, y, x, gadgetnum, _client);
			get (_ret);
			check_answer ();

			gadgetnum = ++;
			
			x = x + width;
			i = ++;
			f = i <= two;
		next f;
		
		y = y + width;
		j = ++;
		f = j <= two;
	next f;
	
// set button gadgets
    x = 440; y = 10;
    i = 9;
    s = "Start";
    
    @nanogfx_set_gadget_button (s, one, y, x, i, _client);
    get (_ret);
    check_answer ();
    
    y = 130;
    i = 10;
    @nanogfx_set_gadget_cycle (two, expert, normal, null, one, y, x, i, _client);
    get (_ret);
    check_answer ();
    
    y = 250;
    i = 11;
    s = "Close";
    
    @nanogfx_set_gadget_button (s, one, y, x, i, _client);
    get (_ret);
    check_answer ();
    
    y = 400;
    
	s = "my move";
	i = 12;
	int progress = 0;

	@nanogfx_set_gadget_progress_bar (progress, s, one, y, x, i, _client);
    get (_ret);
    check_answer ();
    
    
	x = 10; y = 440;
    
    color_black ();
    @nanogfx_text_ttf (copyright, y, x, _client);
    get (_ret);
    check_answer ();   
funcend	

func set_move_progress (int progress)
	int one = 1;
	int gadget = 12;
	
	@nanogfx_change_gadget_progress_bar (progress, one, gadget, _client);
	get (_ret);
	check_answer ();
funcend
	

func init_gui_colors ()
	int one = 1;
	int r; int g; int b;

	#NESTED_CODE_ON;
	
//  colors
    print "setting colors...";
    printn one;

    r = 232; g = 239; b = 247;

    nanogfx_color (b, g, r, _client);
    nanogfx_gadget_color_background_light (_client);

    r = 194; g = 201; b = 209;

    nanogfx_color (b, g, r, _client);
    nanogfx_gadget_color_background_shadow (_client);

    r = 143; g = 147; b = 151;

    nanogfx_color (b, g, r, _client);
    nanogfx_gadget_color_border_light (_client);
    
    r = 85; g = 87; b = 90;

    nanogfx_color (b, g, r, _client);
    nanogfx_gadget_color_border_shadow (_client);

    r = 0; g = 0; b = 0;

    nanogfx_color (b, g, r, _client);
    nanogfx_gadget_color_text_light (_client);
   
    r = 255; g = 255; b = 255;

    nanogfx_color (b, g, r, _client);
    nanogfx_gadget_color_text_shadow (_client);
    
    print "colors set!";
    printn one;
    
    clear_screen ();
    
    print "clear_screen ok";
    printn one;
funcend
 
func update_screen ()
    @nanogfx_updatescreen (_client);

    get (_ret);
    check_answer ();
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
    int error;
    
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

	@socket_error ();
    get (error);

    f = error == err_sock_ok;
    if f;
        goto ip_ok;
    endif;

    buf = ip;
    hostbyname (buf, ip);
    scopen (_client, ip, port);
    check_err ();
    
 lab ip_ok;
    _client_open = 1;
    s = "nano - gui";

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
funcend

func clear_screen ()
    int null = 0; int alpha = 255;

    color_background ();

    @nanogfx_clearscreen (_client);

    get (_ret);
//    check_answer ();

    nanogfx_alpha (alpha, _client);
funcend

func color_background ()
    int r = 232; int g = 239; int b = 247;
    
    nanogfx_color (b, g, r, _client);
funcend

func color_white ()
    int r = 255; int g = 255; int b = 255;

    nanogfx_color (b, g, r, _client);
funcend

func color_black ()
    int null = 0;
    
    nanogfx_color (null, null, null, _client);
funcend

func check_answer ()
    int f; int null = 0; int one = 1;
    
    print _ret; printn one;
    
    f = _ret != null;
    if f;
        print "ERROR: server error!";
        printn one;
        end ();
    endif;

//    print "OK!";
//    printn one;
funcend



func gadget_0 ()
    int x = 10; int y = 10;
    int n = 1; int f; int null = 0;
    int ret;

    f = _f0 != null;
    if f;
         ret = null;
         goto gadget_0_end;
    endif;

    draw_cross (x, y);
    print "gadget 0";
    printn n;

    _f0 = 1;
    check_winner ();
    ret = 1;
lab gadget_0_end;
    return (ret);
funcend;

func gadget_1 ()
    int x = 130; int y = 10;
    int n = 1; int f; int null = 0;
    int ret;

    f = _f1 != null;
    if f;
         ret = null;
         goto gadget_1_end;
    endif;

    draw_cross (x, y);
    print "gadget 1";
    printn n;

    _f1 = 1;
    check_winner ();
    ret = 1;
lab gadget_1_end;
    return (ret);
funcend;

func gadget_2 ()
    int x = 250; int y = 10;
    int n = 1; int f; int null = 0;
    int ret;

    f = _f2 != null;
    if f;
         ret = null;
         goto gadget_2_end;
    endif;

    draw_cross (x, y);
    print "gadget 2";
    printn n;

    _f2 = 1;
    check_winner ();
    ret = 1;
lab gadget_2_end;
    return (ret);
funcend;
    
func gadget_3 ()
    int x = 10; int y = 130;
    int n = 1; int f; int null = 0;
    int ret;

    f = _f3 != null;
    if f;
         ret = null;
         goto gadget_3_end;
    endif;

    draw_cross (x, y);
    print "gadget 3";
    printn n;

    _f3 = 1;
    check_winner ();
    ret = 1;
lab gadget_3_end;
    return (ret);
funcend;

func gadget_4 ()
    int x = 130; int y = 130;
    int n = 1; int f; int null = 0;
    int ret;

    f = _f4 != null;
    if f;
         ret = null;
         goto gadget_4_end;
    endif;

    draw_cross (x, y);
    print "gadget 4";
    printn n;

    _f4 = 1;
    check_winner ();
    ret = 1;
lab gadget_4_end;
    return (ret);
funcend;

func gadget_5 ()
    int x = 250; int y = 130;
    int n = 1; int f; int null = 0;
    int ret;

    f = _f5 != null;
    if f;
         ret = null;
         goto gadget_5_end;
    endif;

    draw_cross (x, y);
    print "gadget 5";
    printn n;

    _f5 = 1;
    check_winner ();
    ret = 1;
lab gadget_5_end;
    return (ret);
funcend;

func gadget_6 ()
    int x = 10; int y = 250;
    int n = 1; int f; int null = 0;
    int ret;

    f = _f6 != null;
    if f;
         ret = null;
         goto gadget_6_end;
    endif;

    draw_cross (x, y);
    print "gadget 6";
    printn n;

    _f6 = 1;
    check_winner ();
    ret = 1;
lab gadget_6_end;
    return (ret);
funcend;

func gadget_7 ()
    int x = 130; int y = 250;
    int n = 1; int f; int null = 0;
    int ret;

    f = _f7 != null;
    if f;
         ret = null;
         goto gadget_7_end;
    endif;

    draw_cross (x, y);
    print "gadget 7";
    printn n;

    _f7 = 1;
    check_winner ();
    ret = 1;
lab gadget_7_end;
    return (ret);
funcend;

func gadget_8 ()
    int x = 250; int y = 250;
    int n = 1; int f; int null = 0;
    int ret;

    f = _f8 != null;
    if f;
         ret = null;
         goto gadget_8_end;
    endif;

    draw_cross (x, y);
    print "gadget 8";
    printn n;

    _f8 = 1;
    check_winner ();
    ret = 1;
lab gadget_8_end;
    return (ret);
funcend;

func draw_circle (int x, int y)
    int i = 60;
    
    color_black ();
    
    x = x + i;
    y = y + i;

    i = 55;

    @nanogfx_circle (i, y, x, _client);
    
    get (_ret);
    check_answer ();
    
    update_screen ();
funcend;

func init_field ();
    int y = 10; int width = 120; int height;
    int i = 0; int j; int f; int two = 2;
    
    int x; int y; int x1; int y1; int x2; int y2;
    height = width;
    
    color_background ();
    
lab init_field_loop_y;
    j = 0;
    x = 10;
    
lab init_field_loop_x;
    x2 = x + width - 5;
    y2 = y + height - 5;
    x1 = x + 5;
    y1 = y + 5;

    @nanogfx_rectangle_filled (y2, x2, y1, x1, _client);
    get (_ret);
    check_answer ();
    
    x = x + width;
    j = ++;
    f = j <= two;
    if f; goto init_field_loop_x; endif;
    
    y = y + height;
    i = ++;
    f = i <= two;
    if f; goto init_field_loop_y; endif;

    x2 = _win_x + 150;
    y2 = _win_y + 20;
    
    @nanogfx_rectangle_filled (y2, x2, _win_y, _win_x, _client);
    get (_ret);
    check_answer ();
    
    update_screen ();

    color_black ();

//  clear field table
    _f0 = 0; _f1 = 0; _f2 = 0; _f3 = 0; _f4 = 0; _f5 = 0; _f6 = 0; _f7 = 0; _f8 = 0;
    _game_over = 0;
    _playermove = 0;
funcend;

func draw_cross (int x, int y)
    int i = 5;

    int x1; int y1; int x2; int y2;

    x1 = x + i; y1 = y + i;

    i = 115;
    x2 = x + i; y2 = y + i;
    
    
    int one = 1;
    print "draw_cross"; printn one;
    
    
    @nanogfx_line (y2, x2, y1, x1, _client);

    get (_ret);
    check_answer ();
        
    i = 115;
    x1 = x + i;
    
    i = 5;
    y1 = y + i;
    
    i = 5;
    x2 = x + i;

    i = 115;
    y2 = y + i;

    @nanogfx_line (y2, x2, y1, x1, _client);
   
    get (_ret);
    check_answer ();

    update_screen ();
funcend;

func last_move ()
	int f;
	int null = 0;
	
	
funcend
	
func think ()
//  initialize think memory
    _t0 = 0; _t1 = 0; _t2 = 0; _t3 = 0; _t4 = 0; _t5 = 0; _t6 = 0; _t7 = 0; _t8 = 0;
    
    int _val = 2; int i; int j; int f; int null = 0; int one = 1;
    int check; int next_move; int two = 2; int three = 3; int four = 4; int n = 1;
    int x; int y;
    
    double random; double deight = 8.0; double dthree = 3.0;
    
    int progress = 0;
    
    set_move_progress (progress);
    
    f = _playermove == 5;
    if f;
		progress = 100;
		set_move_progress (progress);
    
		f = _f0 == null;
		if f;
			goto move_0;
		endif;
	
		f = _f1 == null;
		if f;
			goto move_1;
		endif;
	
		f = _f2 == null;
		if f;
			goto move_2;
		endif;
	
		f = _f3 == null;
		if f;
			goto move_3;
		endif;
	
		f = _f4 == null;
		if f;
			goto move_4;
		endif;

		f = _f5 == null;
		if f;
			goto move_5;
		endif;
	
		f = _f6 == null;
		if f;
			goto move_6;
		endif;
	
		f = _f7 == null;
		if f;
			goto move_7;
		endif;
	
		f = _f8 == null;
		if f;
			goto move_8;
		endif;
    endif;
    
    
lab think_2;
    print "thinking ";
    print _val;
    printn two;
  
//  xline 0
    i = _f0 + _f1 + _f2;
    f = i == _val;
    if f; xline_0 (); endif;
    
//  xline 1
    i = _f3 + _f4 + _f5;
    f = i == _val;
    if f; xline_1 (); endif;
    
//  xline 2;
    i = _f6 + _f7 + _f8;
    f = i == _val;
    if f; xline_2 (); endif;
  
//  yline 0;
    i = _f0 + _f3 + _f6;
    f = i == _val;
    if f; yline_0 (); endif;
    
//  yline 1
    i = _f1 + _f4 + _f7;
    f = i == _val;
    if f; yline_1 (); endif;

//  yline 2
    i = _f2 + _f5 + _f8;
    f = i == _val;
    if f; yline_2 (); endif;

//  cross 0
    i = _f0 + _f4 + _f8;
    f = i == _val;
    if f; cross_0 (); endif;

//  cross 1
    i = _f2 + _f4 + _f6;
    f = i == _val;
    if f; cross_1 (); endif;
   
    print "two: ";
    print two;
    printn n;
   
    i = 2;
    f = i == _val; 
    if f; _val = 8;
		progress = 25;
		set_move_progress (progress);
    
		goto think_2; 
	endif;
    
    i = 8;
    f = i == _val; 
    if f; _val = 4; 
		progress = 50;
		set_move_progress (progress);
    
		goto think_2; 
    endif;

//  no two found: search 4

    i = 4;
    f = i == _val; 
    if f; 
		progress = 75;
		set_move_progress (progress);
    
		goto move_random; 
	endif;
    
    
lab move_random;
    print "random two:";
    print two;
    printn n;
    
    f = _playermove == one;
    if f;
        f = _expertmode == one;
        if f;
//  first playermove, check edges

            print "expert...";
            printn n;

            f = _f0 == one;
            if f;
                goto move_4;
            endif;
    
            f = _f2 == one;
            if f;
                goto move_4;
            endif;
            
            f = _f6 == one;
            if f;
                goto move_4;
            endif;
            
            f = _f8 == one;
            if f;
                goto move_4;
            endif;
            
//  check middle side

            f = _f1 == one;
            if f;
                rand (random);
                2int (random, i);
            
                f = i == null;
                if f;
                    goto move_0;
                else
                    goto move_2;
                endif;
            endif;
            
            f = _f3 == one;
            if f;
                rand (random);
                2int (random, i);
            
                f = i == null;
                if f;
                    goto move_0;
                else
                    goto move_6;
                endif;
            endif;
            
            f = _f5 == one;
            if f;
                rand (random);
                2int (random, i);
            
                f = i == null;
                if f;
                    goto move_2;
                else
                    goto move_8;
                endif;
            endif;
            
            f = _f7 == one;
            if f;
                rand (random);
                2int (random, i);
            
                f = i == null;
                if f;
                    goto move_6;
                else
                    goto move_8;
                endif;
            endif;
        
//  check middle

            f = _f4 == one;
            if f;
                rand (random);
                random = random * dthree;
                2int (random, i);
    
                f = i == null;
                if f;
                    goto move_0;
                endif;
    
                f = i == one;
                if f;
                    goto move_2;
                endif;
        
                f = i == two;
                if f;
                    goto move_6;
                endif;
        
                f = i == three;
                if f;
                    goto move_8;
                endif;
            endif;
        endif;
    endif;

    f = _playermove == two;
    if f;
        f = _expertmode == one;
        if f;
//  second playermove: check middle field

            print "expert...";
            printn n;

            f = _f4 == null;
            if f;
               goto move_4;
            endif;
        endif;
    endif;

    goto check_think;
    
lab move_random_2;
    print "random two:";
    print two;
    printn n;

    print "move random...";
    printn n;

    rand (random);
    
    print random;
    printn n;
    
    random = random * deight;
    2int (random, i);
    
    print i;
    printn n;
    
    j = 0;
    f = i == j;
    if f;
        f = _f0 == null;
        if f;
            goto move_0;
        endif;
    endif;
    
    j = 1;
    f = i == j;
    if f;
        f = _f1 == null;
        if f;
            goto move_1;
        endif;
    endif;
    
    j = 2;
    f = i == j;
    if f;
        f = _f2 == null;
        if f;
            goto move_2;
        endif;
    endif;
    
    j = 3;
    f = i == j;
    if f;
        f = _f3 == null;
        if f;
            goto move_3;
        endif;
    endif;
    
    j = 4;
    f = i == j;
    if f;
        f = _f4 == null;
        if f;
            goto move_4;
        endif;
    endif;
    
    j = 5;
    f = i == j;
    if f;
        f = _f5 == null;
        if f;
            goto move_5;
        endif;
    endif;
    
    j = 6;
    f = i == j;
    if f;
        f = _f6 == null;
        if f;
            goto move_6;
        endif;
    endif;
    
    j = 7;
    f = i == j;
    if f;
        f = _f7 == null;
        if f;
            goto move_7;
        endif;
    endif;
    
    j = 8;
    f = i == j;
    if f;
        f = _f8 == null;
        if f;
            goto move_8;
        endif;
    endif;

//  random field was not free, go back!
    goto move_random_2;
 
lab check_think;
    print "check think...";
    printn n;

    print_think ();
    
    next_move = 0;
    f = _f0 == null;
    if f;
        check = _t0;
    else;
        check = 0;
    endif;
    
    f = _t1 > check;
    if f;
        f = _f1 == null;
        if f;
            next_move = 1; check = _t1;
        endif;
    endif;
    
    f = _t2 > check;
    if f;
        f = _f2 == null;
        if f;
            next_move = 2; check = _t2;
        endif;
    endif;
    
    f = _t3 > check;
    if f;
        f = _f3 == null;
        if f;
            next_move = 3; check = _t3;
        endif;
    endif;
   
    f = _t4 > check;
    if f;
        f = _f4 == null;
        if f;
            next_move = 4; check = _t4;
        endif;
    endif;
    
    f = _t5 > check;
    if f;
        f = _f5 == null;
        if f;
            next_move = 5; check = _t5;
        endif;
    endif;
    
    f = _t6 > check;
    if f;
        f = _f6 == null;
        if f;
            next_move = 6; check = _t6;
        endif;
    endif;
    
    f = _t7 > check;
    if f;
        f = _f7 == null;
        if f;
            next_move = 7; check = _t7;
        endif;
    endif;
    
    f = _t8 > check;
    if f;
        f = _f8 == null;
        if f;
            next_move = 8; check = _t8;
        endif;
    endif;
    
    f = check == null;
    if f;
        goto move_random_2;
    endif;

    f = _playermove == two;
    if f;
        f = _expertmode == one;
        if f;
     
            f = check <= four;
            if f;
     
//  check if edges are set = found
                f = _t0 == four;
                if f;
                    next_move = 0; check = _t0;
                    goto think_do_move;
                endif;
            
                f = _t2 == four;
                if f;
                    next_move = 2; check = _t2;
                    goto think_do_move;
                endif;
            
                f = _t6 == four;
                if f;
                    next_move = 6; check = _t6;
                    goto think_do_move;
                endif;
            
                f = _t8 == four;
                if f;
                    next_move = 8; check = _t8;
                    goto think_do_move;
                endif;
            endif;
        endif;
    endif;
    
    
// do move
lab think_do_move;
    i = 0;
    f = i == next_move;
    if f; goto move_0; endif;
    
    i = 1;
    f = i == next_move;
    if f; goto move_1; endif;
    
    i = 2;
    f = i == next_move;
    if f; goto move_2; endif;

    i = 3;
    f = i == next_move;
    if f; goto move_3; endif;

    i = 4;
    f = i == next_move;
    if f; goto move_4; endif;
    
    i = 5;
    f = i == next_move;
    if f; goto move_5; endif;
    
    i = 6;
    f = i == next_move;
    if f; goto move_6; endif;

    i = 7;
    f = i == next_move;
    if f; goto move_7; endif;

    i = 8;
    f = i == next_move;
    if f; goto move_8; endif;   


lab move_0;
    _f0 = 4;
    
    x = 10; y = 10;
    draw_circle (x, y);
    
    check_winner ();
    print_field ();
    
    goto think_end;

lab move_1;
     _f1 = 4;
    
    x = 130; y = 10;
    draw_circle (x, y);
    
    check_winner ();
    print_field ();
    
    goto think_end;

lab move_2;
     _f2 = 4;
    
    x = 250; y = 10;
    draw_circle (x, y);
    
    check_winner ();
    print_field ();
    
    goto think_end;

lab move_3;
     _f3 = 4;
    
    x = 10; y = 130;
    draw_circle (x, y);
    
    check_winner ();
    print_field ();
    
    goto think_end;

lab move_4;
     _f4 = 4;
    
    x = 130; y = 130;
    draw_circle (x, y);
    
    check_winner ();
    print_field ();
    
    goto think_end;

lab move_5;
     _f5 = 4;
    
    x = 250; y = 130;
    draw_circle (x, y);
    
    check_winner ();
    print_field ();
    
    goto think_end;

lab move_6;
     _f6 = 4;
    
    x = 10; y = 250;
    draw_circle (x, y);
    
    check_winner ();
    print_field ();
    
    goto think_end;

lab move_7;
    _f7 = 4;
    
    x = 130; y = 250;
    draw_circle (x, y);
    
    check_winner ();
    print_field ();
    
    goto think_end;

lab move_8;
    _f8 = 4;
    
    x = 250; y = 250;
    draw_circle (x, y);
    
    check_winner ();
    print_field ();
    
lab think_end;
	progress = 100;
	set_move_progress (progress);
funcend;

func set_0 ()
    _t0 = _t0 + _val;
    
    int two = 2; int fac = 3;
    int f;
    f = _val == two;
    if f; _t0 = _t0 * fac; endif;
    
    int eight = 8;
    f = _val == eight;
    if f; _t0 = _t0 * fac * fac; endif;
funcend

func set_1 ()
    _t1 = _t1 + _val;
    
    int two = 2; int fac = 3;
    int f;
    f = _val == two;
    if f; _t1 = _t1 * fac; endif;
    
    int eight = 8;
    f = _val == eight;
    if f; _t1 = _t1 * fac * fac; endif;
funcend

func set_2 ()
    _t2 = _t2 + _val;
    
    int two = 2; int fac = 3;
    int f;
    f = _val == two;
    if f; _t2 = _t2 * fac; endif;
    
    int eight = 8;
    f = _val == eight;
    if f; _t2 = _t2 * fac * fac; endif;
funcend

func set_3 ()
    _t3 = _t3 + _val;
   
    int two = 2; int fac = 3;
    int f;
    f = _val == two;
    if f; _t3 = _t3 * fac; endif;
    
    int eight = 8;
    f = _val == eight;
    if f; _t3 = _t3 * fac * fac; endif;
funcend 

func set_4 ()
    _t4 = _t4 + _val;
   
    int two = 2; int fac = 3;
    int f;
    f = _val == two;
    if f; _t4 = _t4 * fac; endif;
    
    int eight = 8;
    f = _val == eight;
    if f; _t4 = _t4 * fac * fac; endif;
funcend

func set_5 ()
    _t5 = _t5 + _val;
    
    int two = 2; int fac = 3;
    int f;
    f = _val == two;
    if f; _t5 = _t5 * fac; endif;
    
    int eight = 8;
    f = _val == eight;
    if f; _t5 = _t5 * fac * fac; endif;
funcend
    
func set_6 ()
    _t6 = _t6 + _val;
    
    int two = 2; int fac = 3;
    int f;
    f = _val == two;
    if f; _t6 = _t6 * fac; endif;
    
    int eight = 8;
    f = _val == eight;
    if f; _t6 = _t6 * fac * fac; endif; 
funcend

func set_7 ()
    _t7 = _t7 + _val;
    
    int two = 2; int fac = 3;
    int f;
    f = _val == two;
    if f; _t7 = _t7 * fac; endif;
    
    int eight = 8;
    f = _val == eight;
    if f; _t7 = _t7 * fac * fac; endif;
funcend

func set_8 ()
    _t8 = _t8 + _val;
    
    int two = 2; int fac = 3;
    int f;
    f = _val == two;
    if f; _t8 = _t8 * fac; endif;
    
    int eight = 8;
    f = _val == eight;
    if f; _t8 = _t8 * fac * fac; endif;
funcend
 
func xline_0 ()
    int n = 1; int null = 0; int f;
    print "xline 0";
    printn n;
    
    f = _f0 == null;
    if f; set_0 (); endif;
    
    f = _f1 == null;
    if f; set_1 (); endif;
    
    f = _f2 == null;
    if f; set_2 (); endif;
funcend
    
func xline_1 ()
    int n = 1; int null = 0; int f;
    print "xline 1";
    printn n;

    f = _f3 == null;
    if f; set_3 (); endif;
    
    f = _f4 == null;
    if f; set_4 (); endif;
    
    f = _f5 == null;
    if f; set_5 (); endif;
funcend

func xline_2 ()
    int n = 1; int null = 0; int f;
    print "xline 2";
    printn n;

    f = _f6 == null;
    if f; set_6 (); endif;
    
    f = _f7 == null;
    if f; set_7 (); endif;
    
    f = _f8 == null;
    if f; set_8 (); endif;
funcend

func yline_0 ()
    int n = 1; int null = 0; int f;
    print "yline 0";
    printn n;
    
    f = _f0 == null;
    if f; set_0 (); endif;
    
    f = _f3 == null;
    if f; set_3 (); endif;
    
    f = _f6 == null;
    if f; set_6 (); endif;
funcend

func yline_1 ()
    int n = 1; int null = 0; int f;
    print "yline 1";
    printn n;
    
    f = _f1 == null;
    if f; set_1 (); endif;
    
    f = _f4 == null;
    if f; set_4 (); endif;
    
    f = _f7 == null;
    if f; set_7 (); endif;
funcend

func yline_2 ()
    int n = 1; int null = 0; int f;
    print "yline 2";
    printn n;
    
    f = _f2 == null;
    if f; set_2 (); endif;
    
    f = _f5 == null;
    if f; set_5 (); endif;
    
    f = _f8 == null;
    if f; set_8 (); endif;
funcend

func cross_0 ()
    int n = 1; int null = 0; int f;
    print "cross 0";
    printn n;
    
    f = _f0 == null;
    if f; set_0 (); endif;
    
    f = _f4 == null;
    if f; set_4 (); endif;
    
    f = _f8 == null;
    if f; set_8 (); endif;  
funcend

func cross_1 ()
    int n = 1; int null = 0; int f;
    print "cross 1";
    printn n;
    
    f = _f2 == null;
    if f; set_2 (); endif;
    
    f = _f4 == null;
    if f; set_4 (); endif;
    
    f = _f6 == null;
    if f; set_6 (); endif;
funcend

func print_field ()
    int n = 1; int two = 2;

    print _f0;
    print  " ";
    print _f1;
    print " ";
    print _f2;
    printn n;

    print _f3;
    print  " ";
    print _f4;
    print " ";
    print _f5;
    printn n;

    print _f6;
    print  " ";
    print _f7;
    print " ";
    print _f8;
    printn two;
funcend;

func print_think ()
    int n = 1; int two = 2;

    print "think:";
    printn n;
    
    print _t0;
    print  " ";
    print _t1;
    print " ";
    print _t2;
    printn n;

    print _t3;
    print  " ";
    print _t4;
    print " ";
    print _t5;
    printn n;

    print _t6;
    print  " ";
    print _t7;
    print " ";
    print _t8;
    printn two;
funcend;


func check_winner ()
    int val = 3; int i; int f; int two = 2; int channel = 2;
    string player[80] = "player wins!";
    string pc[80] = "PC wins!";
    string wav[80] = "Applause.wav";
    
lab check_winner_2;
//  xline 0
    i = _f0 + _f1 + _f2;
    f = i == val; 
    if f; goto winner; endif;

//  xline 1
    i = _f3 + _f4 + _f5;
    f = i == val;
    if f; goto winner; endif;

//  xline 2
    i = _f6 + _f7 + _f8;
    f = i == val; 
    if f; goto winner; endif; 

//  y line 0
    i = _f0 + _f3 + _f6;
    f = i == val; 
    if f; goto winner; endif;

//  y line 1
    i = _f1 + _f4 + _f7;
    f = i == val; 
    if f; goto winner; endif;

//  y line 2
    i = _f2 + _f5 + _f8;
    f = i == val; 
    if f; goto winner; endif;
  
//  cross 0
    i = _f0 + _f4 + _f8;
    f = i == val; 
    if f; goto winner; endif;

//  cross 1
    i = _f2 + _f4 + _f6;
    f = i == val; 
    if f; goto winner; endif;

    i = 12;
    f = i == val;
    if f; goto check_winner_return; endif;
    
    val = 12;
    goto check_winner_2;
    

lab winner;
    i = 3;
    f = i == val;
    if f;
        color_black ();
        @nanogfx_text_ttf (player, _win_y, _win_x, _client);
    
        get (_ret);
        check_answer ();
    
        update_screen ();
    
//  Play Aplause.wav
        i = 1;
//        nanosfx_play_wav (i, channel, wav, _client); 
        
        _game_over = 1;
    endif;
    
    i = 12;
    f = i == val;
    if f;
        color_black ();
        @nanogfx_text_ttf (pc, _win_y, _win_x, _client);
    
        get (_ret);
        check_answer ();
        
        update_screen ();
    
//  Play Aplause.wav
        i = 1;
//        nanosfx_play_wav (i, channel, wav, _client); 
    
        _game_over = 1;
    endif;
    
    printn two;
    
	lab check_winner_return;
funcend  


func show_args ()
    int n = 1;
    print "ttt <ip> <port>";
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
	#include <socket.nch>

    int f; int n = 1;
	int error;
    
    @socket_error ();
    get (error);

    f = error != err_sock_ok;
    if f;
        goto error;
    endif;
    goto check_err_end;

lab error;
    print "socket error: ";
    print error;
    printn n;

    f = error == err_connrefused;
    if f;
        print "server not found!";
        printn n;
    endif;

    f = error == err_nosys;
    if f;
        print "tcp stack not running!";
        printn n;
    endif;

    end ();
lab check_err_end;
funcend

#include <error.nch>
#include <flow.nah>
