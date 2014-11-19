// Intel Core2 Duo Processor SU7300 @ 1.3GHz == 100 Points
// faster = more points
// optimize with -O2

func main ()
    #include <math.nch>

    qint marray = 400000000Q;
    qint a[marray];

    int f; qint x; int n = 1; int null = 0; int two = 2; qint ten = 10Q;
    qint i = 0Q; qint v;
    qint max = marray;
    max = --;
    
    qint total_ticks = 0Q;
    qint hundred_points_ticks = 4722Q;
    qint hundred = 100Q;
    qint points;

//  check vmul_l    
    #NESTED_CODE_GLOBAL_OFF;
    
    print "vmul_l check: initializing array ...";
    printn n;
    
    timeron ();
    
    for;
        var2array (i, a, i);
        
        i = ++;
        f = i <= max;
    next f;

    i = --;
    
    print "vmul_l ...";
    printn n;
    
    vmul (a, ten, a);
    
    array2var (a, i, v);
    
    timeroff ();
    
    print "last index: ";
    print v;
    printn n;
    
    print "ticks: ";
    print _timer;
    printn two;
    
    total_ticks = total_ticks + _timer;

    
//  check vmul2_d

    dealloc (a);
    marray = 50000000Q;
    max = marray;
    max = --;
    
    double da[marray]; double db[marray]; double dc[marray];
    double dv;
    i = 0Q;
    
    print "vmul2_d check: initializing arrays ...";
    printn n;
    
    timeron ();
    
    for;
        2double (i, dv);
        var2array (dv, da, i);
        var2array (m_pi, db, i);
        
        i = ++;
        f = i <= max;
    next f;
    
    i = --;
    
    print "vmul2_d ...";
    printn n;
    
    vmul2 (da, db, dc);
    
    array2var (dc, i, dv);
    
    timeroff ();
    
    print "last index: ";
    print dv;
    printn n;
    
    print "ticks: ";
    print _timer;
    printn two;
    
    total_ticks = total_ticks + _timer;
    
    
//  loops test

    print "loops test ...";
    printn n;

    timeron ();

    qint j;
    qint maxloop = 50000Q;
    
    i = 0;
    for;
        j = 0;
        for;
            j = ++;
            f = j <= maxloop;
        next f;
    
        i = ++;
        f = i <= maxloop;
    next f;
    
    timeroff ();
    
    print "ticks: ";
    print _timer;
    printn two;
    
    total_ticks = total_ticks + _timer;
    
    print "total ticks: ";
    print total_ticks;
    printn two;
    
    print "Nano benchm V1.1 [2013] POINTS: ";
    
    points = hundred_points_ticks * hundred / total_ticks;
    
    print points;
    printn two;
    
    exit null;
funcend
