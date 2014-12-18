// UPDATE: 1.2 computer and player can start the game -> do the first move.

func main ()
    #NESTED_CODE_ON;
    
    int null = 0; int one = 1; int two = 2;
    lint _client;
    lint _gamesock; lint _gameaccept; int _gameserver = 0;
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
    
    int _playerstartsgame = 0;
    int _playermove = 0;
    int _game_over = 0;
    
//  fields
    int _f0; int _f1; int _f2; int _f3; int _f4; int _f5; int _f6; int _f7; int _f8;
    int _t0; int _t1; int _t2; int _t3; int _t4; int _t5; int _t6; int _t7; int _t8;
    
    int next_move; int check;
    
    int _win_x = 440; int _win_y = 440;
    
    int _server_mode = 0; int _client_mode = 0; int _singleplayer_mode = 1;
    string _game_ip[256] = ""; int _game_port = 2010; string _playername[10] = "player"; string _second_playername[10];
    
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

	gadgetnum = 1000; s_value = "";
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
           @gadget_0 (one);
           get (gadgetret);
           
           f = gadgetret == null;
           if f;
              goto gadget_event;
           endif;
        endif;

        f = gadgetnum == 1;
        if f;
           @gadget_1 (one);
           get (gadgetret);
           
           f = gadgetret == null;
           if f;
              goto gadget_event;
           endif;
        endif;

        f = gadgetnum == 2;
        if f;
           @gadget_2 (one);
           get (gadgetret);
           
           f = gadgetret == null;
           if f;
              goto gadget_event;
           endif;
        endif;

        f = gadgetnum == 3;
        if f;
           @gadget_3 (one);
           get (gadgetret);
           
           f = gadgetret == null;
           if f;
              goto gadget_event;
           endif;
        endif;

        f = gadgetnum == 4;
        if f;
           @gadget_4 (one);
           get (gadgetret);
           
           f = gadgetret == null;
           if f;
              goto gadget_event;
           endif;
        endif;

        f = gadgetnum == 5;
        if f;
           @gadget_5 (one);
           get (gadgetret);
           
           f = gadgetret == null;
           if f;
              goto gadget_event;
           endif;
        endif;

        f = gadgetnum == 6;
        if f;
           @gadget_6 (one);
           get (gadgetret);
           
           f = gadgetret == null;
           if f;
              goto gadget_event;
           endif;
        endif;

        f = gadgetnum == 7;
        if f;
           @gadget_7 (one);
           get (gadgetret);
           
           f = gadgetret == null;
           if f;
              goto gadget_event;
           endif;
        endif;

        f = gadgetnum == 8;
        if f;
           @gadget_8 (one);
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
        
        f = _singleplayer_mode == null;
		if f;
			multiplayer ();
			goto gadget_event;
		endif;
        
        f = _playerstartsgame == one;
        if f;
			_playerstartsgame = 0;
			print "computer starts game!"; printn one;
		else;
			_playerstartsgame = 1;
			print "player starts game!"; printn one;
		endif;
    endif;
    
    f = gadgetnum == 10;
    if f;
        _expertmode = l_value;
        goto gadget_event;
    endif;
    
    f = gadgetnum == 11; 
    if f; end (); endif;
    
// game mode
    f = gadgetnum == 13;
    if f;
		f = l_value == null;
		if f;
			_singleplayer_mode = one;
		endif;
		
		f = l_value == one;
		if f;
			_singleplayer_mode = null;
			_server_mode = one;
			_client_mode = null;
		endif;
		
		f = l_value == two;
		if f;
			_singleplayer_mode = null;
			_client_mode = one;
			_server_mode = null;
		endif;
		
		goto gadget_event;
	endif;
    
    f = gadgetnum == 14;
    if f;
		_game_ip = s_value;
		goto gadget_event;
	endif;
	
	f = gadgetnum == 15;
	if f;
		_playername = s_value;
		goto gadget_event;
	endif;
    
    f = _playermove == null;
    if f;
		f = _playerstartsgame == one;
		if f;
			_playermove = ++;
			waitsec (one);
			goto gadget_event;
		endif;
	endif;
    
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

    size = 20;
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
    string copyright[80] = "(C) 2014 by Stefan Pietzonke";
	
	string singleplayer[80] = "singleplayer";
	string server[80] = "server";
	string client[80] = "client";
	
	string name[80] = "name";
	string player[80] = "player";
	int player_len = 9;
	
	string ip[80] = "ip";

	int ipval_len = 15;
	
	int null = 0; int one = 1; int two = 2; int three = 3;
	int gadgets = 16;

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
    x = 400; y = 10;
    i = 9;
    s = "Start";
    
    @nanogfx_set_gadget_button (s, one, y, x, i, _client);
    get (_ret);
    check_answer ();
    
    y = 60;
    i = 10;
    @nanogfx_set_gadget_cycle (two, expert, normal, null, one, y, x, i, _client);
    get (_ret);
    check_answer ();
    
    y = 110;
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
    
    
// multiplayer settings
	y = 200;
    i = 13;
    @nanogfx_set_gadget_cycle (three, client, server, singleplayer, null, one, y, x, i, _client);
    get (_ret);
    check_answer ();
	
	x = 10;
	y = 400;
	i = 14;
	@nanogfx_set_gadget_string (ipval_len, ipval_len, _game_ip, ip, one, y, x, i, _client);
    get (_ret);
    check_answer ();
	
	x = 400;
	y = 250;
	i = 15;
	
	@nanogfx_set_gadget_string (player_len, player_len, player, name, one, y, x, i, _client);
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



func gadget_0 (int player)
    int x = 10; int y = 10;
    int one = 1; int f; int null = 0;
    int ret;

    f = _f0 != null;
    if f;
         ret = null;
         goto gadget_0_end;
    endif;
	
	f = _singleplayer_mode == one;
	if f;
		draw_cross (x, y);
	else;
		f = player != one;
		if f;
			draw_circle (x, y);
		else;
			draw_cross (x, y);
		endif;
	endif;
	
    print "gadget 0";
    printn one;

    _f0 = player;
	
    check_winner ();
    ret = 1;
lab gadget_0_end;
    return (ret);
funcend;

func gadget_1 (int player)
    int x = 130; int y = 10;
    int one = 1; int f; int null = 0;
    int ret;

    f = _f1 != null;
    if f;
         ret = null;
         goto gadget_1_end;
    endif;

    f = _singleplayer_mode == one;
	if f;
		print "draw cross"; printn one;
		draw_cross (x, y);
	else;
		f = player != one;
		if f;
			draw_circle (x, y);
		else;
			draw_cross (x, y);
		endif;
	endif;
	
    print "gadget 1";
    printn one;

    _f1 = player;
    
    check_winner ();
    ret = 1;
lab gadget_1_end;
    return (ret);
funcend;

func gadget_2 (int player)
    int x = 250; int y = 10;
    int one = 1; int f; int null = 0;
    int ret;

    f = _f2 != null;
    if f;
         ret = null;
         goto gadget_2_end;
    endif;

    f = _singleplayer_mode == one;
	if f;
		draw_cross (x, y);
	else;
		f = player != one;
		if f;
			draw_circle (x, y);
		else;
			draw_cross (x, y);
		endif;
	endif;
	
    print "gadget 2";
    printn one;

    _f2 = player;
   
    check_winner ();
    ret = 1;
lab gadget_2_end;
    return (ret);
funcend;
    
func gadget_3 (int player)
    int x = 10; int y = 130;
    int one = 1; int f; int null = 0;
    int ret;

    f = _f3 != null;
    if f;
         ret = null;
         goto gadget_3_end;
    endif;
	
	f = _singleplayer_mode == one;
	if f;
		draw_cross (x, y);
	else;
		f = player != one;
		if f;
			draw_circle (x, y);
		else;
			draw_cross (x, y);
		endif;
	endif;
	
    print "gadget 3";
    printn one;

    _f3 = player;
    
    check_winner ();
    ret = 1;
lab gadget_3_end;
    return (ret);
funcend;

func gadget_4 (int player)
    int x = 130; int y = 130;
    int one = 1; int f; int null = 0;
    int ret;

    f = _f4 != null;
    if f;
         ret = null;
         goto gadget_4_end;
    endif;
	
	f = _singleplayer_mode == one;
	if f;
		draw_cross (x, y);
	else;
		f = player != one;
		if f;
			draw_circle (x, y);
		else;
			draw_cross (x, y);
		endif;
	endif;
	
    print "gadget 4";
    printn one;

    _f4 = player;
   
    check_winner ();
    ret = 1;
lab gadget_4_end;
    return (ret);
funcend;

func gadget_5 (int player)
    int x = 250; int y = 130;
    int one = 1; int f; int null = 0;
    int ret;

    f = _f5 != null;
    if f;
         ret = null;
         goto gadget_5_end;
    endif;

    f = _singleplayer_mode == one;
	if f;
		draw_cross (x, y);
	else;
		f = player != one;
		if f;
			draw_circle (x, y);
		else;
			draw_cross (x, y);
		endif;
	endif;
	
    print "gadget 5";
    printn one;

    _f5 = player;
    
    check_winner ();
    ret = 1;
lab gadget_5_end;
    return (ret);
funcend;

func gadget_6 (int player)
    int x = 10; int y = 250;
    int one = 1; int f; int null = 0;
    int ret;

    f = _f6 != null;
    if f;
         ret = null;
         goto gadget_6_end;
    endif;

    f = _singleplayer_mode == one;
	if f;
		draw_cross (x, y);
	else;
		f = player != one;
		if f;
			draw_circle (x, y);
		else;
			draw_cross (x, y);
		endif;
	endif;
	
    print "gadget 6";
    printn one;

    _f6 = player;
    
    check_winner ();
    ret = 1;
lab gadget_6_end;
    return (ret);
funcend;

func gadget_7 (int player)
    int x = 130; int y = 250;
    int one = 1; int f; int null = 0;
    int ret;

    f = _f7 != null;
    if f;
         ret = null;
         goto gadget_7_end;
    endif;

    f = _singleplayer_mode == one;
	if f;
		draw_cross (x, y);
	else;
		f = player != one;
		if f;
			draw_circle (x, y);
		else;
			draw_cross (x, y);
		endif;
	endif;
	
    print "gadget 7";
    printn one;

    _f7 = player;
    
    check_winner ();
    ret = 1;
lab gadget_7_end;
    return (ret);
funcend;

func gadget_8 (int player)
    int x = 250; int y = 250;
    int one = 1; int f; int null = 0;
    int ret;

    f = _f8 != null;
    if f;
         ret = null;
         goto gadget_8_end;
    endif;

    f = _singleplayer_mode == one;
	if f;
		draw_cross (x, y);
	else;
		f = player != one;
		if f;
			draw_circle (x, y);
		else;
			draw_cross (x, y);
		endif;
	endif;
	
    print "gadget 8";
    printn one;

    _f8 = player;
    
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

    x2 = _win_x + 190;
    y2 = _win_y + 25;
    
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

    #NESTED_CODE_ON;
    
    x1 = x + i; y1 = y + i;

    i = 115;
    x2 = x + i; y2 = y + i;
    
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
    
    
    f = _playerstartsgame == one;
    if f;
		f = _playermove == two;
		if f;
			f = _expertmode == one;
			if f;
				f = _f1 == one;
				if f;	
					goto move_4;
				endif;
				
				f = _f3 == one;
				if f;
					goto move_4;
				endif;
				
				f = _f5 == one;
				if f;
					goto move_4;
				endif;
				
				f = _f7 == one;
				if f;
					goto move_4;
				endif;
			endif;
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
   
    #NESTED_CODE_ON;
   
    f = _playerstartsgame == null;
    if f;
    
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
    
    else;

    i = 4;
    f = i == _val; 
    if f; _val = 2;
		progress = 25;
		set_move_progress (progress);
    
		goto think_2; 
	endif;
    
    i = 2;
    f = i == _val; 
    if f; _val = 8;
		progress = 50;
		set_move_progress (progress);
    
		goto think_2
	endif;
    
    i = 8;
    f = i == _val; 
    if f; 
		progress = 75;
		set_move_progress (progress);
    
		goto move_random; 
    endif;
    
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




// MULTIPLAYER ================================================================

func multiplayer_event ()
	//  gadget event
	int gadgetnum;
    lint dgadget;
    lint l_value;
    double d_value;
    string s_value[256];
    int gadgetret;
    int ret = 0;
    int player;
    
    int f;
    int null = 0; int one = 1;
    
    int progress = 100;
    
    
    #NESTED_CODE_ON;
    
    f = _client_mode == one;
    if f;
		player = 4;
	else;
		player = 1;
	endif;
    
    set_move_progress (progress);
    update_screen ();
    
lab multiplayer_event_get;
    @nanogfx_gadget_event (_client);

    getmulti (s_value, d_value, l_value, gadgetnum, _ret);
    getmultiend ();
    
    print "gadget: "; print gadgetnum; printn one;
    
    waitsec (one);
    
    f = _game_over == null;
    if f;
        f = gadgetnum == 0; 
        if f;
           @gadget_0 (player);
           get (gadgetret);
           
           f = gadgetret == null;
           if f;
              goto multiplayer_event_get;
           endif;
           
           f = _client_mode == one;
           if f;
			   swrite (_gamesock, gadgetnum);
			   swrite (_gamesock, _playername); swriten (_gamesock, one);
		   else;
				swrite (_gameaccept, gadgetnum);
				swrite (_gameaccept, _playername); swriten (_gameaccept, one);
		   endif;
        endif;

        f = gadgetnum == 1;
        if f;
           @gadget_1 (player);
           get (gadgetret);
           
           f = gadgetret == null;
           if f;
              goto gadget_event;
           endif;
           
           f = _client_mode == one;
           if f;
			   swrite (_gamesock, gadgetnum);
			   swrite (_gamesock, _playername); swriten (_gamesock, one);
		   else;
				swrite (_gameaccept, gadgetnum);
				swrite (_gameaccept, _playername); swriten (_gameaccept, one);
		   endif;
        endif;

        f = gadgetnum == 2;
        if f;
           @gadget_2 (player);
           get (gadgetret);
           
           f = gadgetret == null;
           if f;
              goto multiplayer_event_get;
           endif;
           
           f = _client_mode == one;
           if f;
			   swrite (_gamesock, gadgetnum);
			   swrite (_gamesock, _playername); swriten (_gamesock, one);
		   else;
				swrite (_gameaccept, gadgetnum);
				swrite (_gameaccept, _playername); swriten (_gameaccept, one);
		   endif;
        endif;

        f = gadgetnum == 3;
        if f;
           @gadget_3 (player);
           get (gadgetret);
           
           f = gadgetret == null;
           if f;
              goto gadget_event;
           endif;
           
           f = _client_mode == one;
           if f;
			   swrite (_gamesock, gadgetnum);
			   swrite (_gamesock, _playername); swriten (_gamesock, one);
		   else;
				swrite (_gameaccept, gadgetnum);
				swrite (_gameaccept, _playername); swriten (_gameaccept, one);
		   endif;
        endif;

        f = gadgetnum == 4;
        if f;
           @gadget_4 (player);
           get (gadgetret);
           
           f = gadgetret == null;
           if f;
              goto multiplayer_event_get;
           endif;
           
           f = _client_mode == one;
           if f;
			   swrite (_gamesock, gadgetnum);
			   swrite (_gamesock, _playername); swriten (_gamesock, one);
		   else;
				swrite (_gameaccept, gadgetnum);
				swrite (_gameaccept, _playername); swriten (_gameaccept, one);
		   endif;
        endif;

        f = gadgetnum == 5;
        if f;
           @gadget_5 (player);
           get (gadgetret);
           
           f = gadgetret == null;
           if f;
              goto multiplayer_event_get;
           endif;
           
           f = _client_mode == one;
           if f;
			   swrite (_gamesock, gadgetnum);
			   swrite (_gamesock, _playername); swriten (_gamesock, one);
		   else;
				swrite (_gameaccept, gadgetnum);
				swrite (_gameaccept, _playername); swriten (_gameaccept, one);
		   endif;
        endif;

        f = gadgetnum == 6;
        if f;
           @gadget_6 (player);
           get (gadgetret);
           
           f = gadgetret == null;
           if f;
              goto multiplayer_event_get;
           endif;
           
          f = _client_mode == one;
           if f;
			   swrite (_gamesock, gadgetnum);
			   swrite (_gamesock, _playername); swriten (_gamesock, one);
		   else;
				swrite (_gameaccept, gadgetnum);
				swrite (_gameaccept, _playername); swriten (_gameaccept, one);
		   endif;
        endif;

        f = gadgetnum == 7;
        if f;
           @gadget_7 (player);
           get (gadgetret);
           
           f = gadgetret == null;
           if f;
              goto multiplayer_event_get;
           endif;
           
           f = _client_mode == one;
           if f;
			   swrite (_gamesock, gadgetnum);
			   swrite (_gamesock, _playername); swriten (_gamesock, one);
		   else;
				swrite (_gameaccept, gadgetnum);
				swrite (_gameaccept, _playername); swriten (_gameaccept, one);
		   endif;
        endif;

        f = gadgetnum == 8;
        if f;
           @gadget_8 (player);
           get (gadgetret);
           
           f = gadgetret == null;
           if f;
              goto multiplayer_event_get;
           endif;
           
           f = _client_mode == one;
           if f;
			   swrite (_gamesock, gadgetnum);
			   swrite (_gamesock, _playername); swriten (_gamesock, one);
		   else;
				swrite (_gameaccept, gadgetnum);
				swrite (_gameaccept, _playername); swriten (_gameaccept, one);
		   endif;
        endif;
        
        f = gadgetnum == 11;
        if f;
			ret = 1;
		endif;
		
		progress = 50;
		set_move_progress (progress);
		update_screen ();
		
		waitsec (one);
    endif;
    
    return (ret);
funcend

func multiplayer_player_two ()
	int f;
	int gadgetnum;
	int gadgetret;
	int player;
	int one = 1;
	
	int progress = 0;
	
	f = _client_mode == one;
    if f;
		player = 1;
	else;
		player = 4;
	endif;
	
	set_move_progress (progress);
    update_screen ();
    
    f = _client_mode == one;
	if f;
		sread (_gamesock, gadgetnum);
		sreadls (_gamesock, _second_playername);
	else;
		sread (_gameaccept, gadgetnum);
		sreadls (_gameaccept, _second_playername);
	endif;
	
	print "player two: "; print gadgetnum; printn one;
	
	f = gadgetnum == 0; 
    if f;
       @gadget_0 (player);
       get (gadgetret);
    endif;

    f = gadgetnum == 1; 
    if f;
       @gadget_1 (player);
       get (gadgetret);
    endif;
	
	f = gadgetnum == 2; 
    if f;
       @gadget_2 (player);
       get (gadgetret);
    endif;
    
    f = gadgetnum == 3; 
    if f;
       @gadget_3 (player);
       get (gadgetret);
    endif;
    
    f = gadgetnum == 4; 
    if f;
       @gadget_4 (player);
       get (gadgetret);
    endif;
    
    f = gadgetnum == 5; 
    if f;
       @gadget_5 (player);
       get (gadgetret);
    endif;
    
    f = gadgetnum == 6; 
    if f;
       @gadget_6 (player);
       get (gadgetret);
    endif;
    
    f = gadgetnum == 7; 
    if f;
       @gadget_7 (player);
       get (gadgetret);
    endif;
    
    f = gadgetnum == 8; 
    if f;
       @gadget_8 (player);
       get (gadgetret);
    endif;
funcend

func multiplayer_msg_clear ()
	int x1 = 400; int y1 = 350;
	int x2 = 639; int y2 = 390;
	
	color_background ();
	
	@nanogfx_rectangle_filled (y2, x2, y1, x1, _client);
    get (_ret);
    check_answer ();
    
    update_screen ();
funcend

func multiplayer_waiting ()
	int x; int y;

	string conn[80] = "waiting...";
	
	multiplayer_msg_clear ();
	
	x = 400; y = 350;
    
    color_black ();
    @nanogfx_text_ttf (conn, y, x, _client);
    get (_ret);
    check_answer ();
    
    update_screen ();
funcend

func multiplayer_connecting ()
	int x; int y;

	string conn[80] = "-> '";
	conn = conn + _game_ip;
	conn = conn + "'";
	
	multiplayer_msg_clear ();
	
	x = 400; y = 350;
    
    color_black ();
    @nanogfx_text_ttf (conn, y, x, _client);
    get (_ret);
    check_answer ();
    
    update_screen ();
funcend
    
func multiplayer_error ()
	int x; int y;

	string error[80] = "net error!";
	
	multiplayer_msg_clear ();
	
	x = 400; y = 350;
    
    color_black ();
    @nanogfx_text_ttf (error, y, x, _client);
    get (_ret);
    check_answer ();
    
    update_screen ();
funcend

func multiplayer_ok ();
	int x; int y;

	string error[80] = "net OK!";
	
	multiplayer_msg_clear ();
	
	x = 400; y = 350;
    
    color_black ();
    @nanogfx_text_ttf (error, y, x, _client);
    get (_ret);
    check_answer ();
    
    update_screen ();
funcend

func multiplayer ()
	#include <socket.nch>

	int null = 0; int one = 1;
	int f; 
	
	int i_begin;
	int my_move;
	int play = one;
	int error;
	int ret;
	
	string buf[256];
	int code;
	int startcode = 2014;
	
	_game_over = null;
	

	f = _gameserver == one;
	if f;
		goto play_loop;
	endif;
	
	multiplayer_connecting ();
	
	f = _client_mode == one;
	if f;
		scopen (_gamesock, _game_ip, _game_port);
		
		@socket_error ();
		get (error);

		f = error != err_sock_ok;
		if f;
			buf = _game_ip;
			hostbyname (buf, _game_ip);
			scopen (_gamesock, _game_ip, _game_port);
			
			@socket_error ();
			get (error);

			f = error != err_sock_ok;
			if f;
				multiplayer_error ();
			
				goto multiplayer_end;
			endif;
		endif;
		
		swrite (_gamesock, startcode);
		
		i_begin = null;
		my_move = null;
		
		_gameserver = one;
	else;
		ssopen (_gamesock, _game_ip, _game_port);

		@socket_error ();
		get (error);

		f = error != err_sock_ok;
		if f;
			buf = _game_ip;
			hostbyname (buf, _game_ip);
			ssopen (_gamesock, _game_ip, _game_port);
			
			@socket_error ();
			get (error);

			f = error != err_sock_ok;
			if f;
				multiplayer_error ();
			
				goto multiplayer_end;
			endif;
		endif;
		
		_gameserver = one;
		
		multiplayer_waiting ();
			
		ssopenact (_gamesock, _gameaccept);
		@socket_error ();
		get (error);

		f = error != err_sock_ok;
		if f;
			multiplayer_error ();
			
			goto multiplayer_end;
		endif;
		
lab multiplayer_connect;
		sread (_gameaccept, code);
		@socket_error ();
		get (error);

		f = error != err_sock_ok;
		if f;
			goto multiplayer_connect;
		endif;
			
		f = code != startcode;
		if f;
			multiplayer_error ();
		
			goto multiplayer_end;
		endif;
		
		i_begin = one;
		my_move = one;
	endif;
	
	multiplayer_ok ();
	
lab play_loop;
	f = my_move == one;
	if f;
		@multiplayer_event ();
		get (ret);
		f = ret == one;
		if f;
			goto multiplayer_end;
		endif;
		
		my_move = null;
	else;
		multiplayer_player_two ();
		
		my_move = one;
	endif;

	f = _game_over == null;
	if f;
		goto play_loop;
	endif;
	
	lab multiplayer_end;
funcend;



func check_winner ()
    int val = 3; int i; int f; int null = 0; int two = 2; int channel = 2;
    string player[80] = "";
    string pc[80] = "PC wins!";
    string wav[80] = "Applause.wav";
    
    #NESTED_CODE_ON;
    
    f = _singleplayer_mode == null;
    if f;
		f = _client_mode == null;
		if f;
			player = _playername;
			pc = _second_playername;
		
			player = player + " wins!";
			pc = pc + " wins!";
		else;
			player = _second_playername;
			pc = _playername;
		
			player = player + " wins!";
			pc = pc + " wins!";
		endif;
	else;
		player = _playername + " wins!";
		pc = "PC wins!";
	endif;

	
	
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