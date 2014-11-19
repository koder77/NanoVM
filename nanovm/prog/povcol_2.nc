// povcol_2.nc
// convert rgb colors to Povray colors

func main ()
    int null = 0; int one = 1; int two = 2;
    int red; int green; int blue; int ret;
    double povred; double povgreen; double povblue;
    int f;

lab main_input;
    print "povcol";
    printn one;
    print "convert RGB colors 0 - 255 to POV-Ray colors 0.0 - 1.0";
    printn two;
    
    @get_colors_rgb ();
    getmulti (red, green, blue, ret);
    getmultiend ();
    
    f = ret == null;
    if f;
        print "user abort!";
        printn one;
        goto main_end;
    endif;
    
    @get_colors_povray (red, green, blue);
    getmulti (povred, povgreen, povblue);
    getmultiend ();
    
    print "povray:";
    printn one;
    
    print "red:   ";
    print povred;
    printn one;
    
    print "green: ";
    print povgreen;
    printn one;
    
    print "blue:  ";
    print povblue;
    printn two;
    goto main_input;
    
lab main_end;
    exit null;
funcend

func get_colors_rgb ()
    int red = 0; int green = 0; int blue = 0;
    int f; int ret = 1;
    
    string inp[256];
    
    print "red:   ";
    input (inp);
    f = inp == "";
    if f;
        ret = 0;
        goto get_colors_rgb_end;
    endif;
    val (inp, red);
    
    print "green: ";
    input (inp);
    f = inp == "";
    if f;
        ret = 0;
        goto get_colors_rgb_end;
    endif;
    val (inp, green);
    
    print "blue:  ";
    input (inp);
    f = inp == "";
    if f;
        ret = 0;
        goto get_colors_rgb_end;
    endif;
    val (inp, blue);

lab get_colors_rgb_end;
    returnmulti (red, green, blue, ret);
funcend

func get_colors_povray (int red, int green, int blue)
    double _fac;
    
    double dred; double dgreen; double dblue;
    double povred; double povgreen; double povblue;
    
    double one = 1.0;
    double max = 255.0;
    
    2double (red, dred);
    2double (green, dgreen);
    2double (blue, dblue);
  
	_fac = one / max;
  
    povred = dred * _fac;
    povgreen = dgreen * _fac;
    povblue = dblue * _fac;
    
    returnmulti (povred, povgreen, povblue);
funcend

        
