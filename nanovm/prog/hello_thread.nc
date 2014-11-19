func main ()
    int null = 0; int one = 1;
    string hellostr[13] = "Hello world!";

    int thread1; int thread2;

    threadcreate (thread1, hello);

    print "created thread: ";
    print thread1;
    printn one;

    threadsync (thread1);

    threadcreate (thread2, hello);

    print "created thread: ";
    print thread2;
    printn one;

    threadsync (thread2);

//  send string hello to threads
    threadpush (thread1, hellostr);
    threadpushsync (thread1);

    threadpush (thread2, hellostr);
    threadpushsync (thread2);

//  wait till all threads are done
    threadjoin ();

    exit null;
funcend

func hello (#sync, string Phello[256])
    int Pnull = 0; int Pone = 1;
    int Pthreadn;
    
    threadnum (Pthreadn);
    
    print "thread: ";
    print Pthreadn;
    printn Pone;
    print Phello;
    printn Pone;
    
    exit Pnull;
funcend

    