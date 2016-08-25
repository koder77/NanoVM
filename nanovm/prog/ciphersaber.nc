//  ciphersaber demo
//

func main ()
    string infile[256] = "CKNIGHT.gif.enc";
    string outfile[256] = "CKNIGHT.gif";
    int action = 2;
    string key[256] = "ThomasJefferson";
    
    int ret; int null = 0;
    
    @ciphersaber_run (infile, outfile, action, key, null);
    get (ret);
    
    exit null;
funcend

#include <crypto/ciphersaber.nch>
 
#include <error.nch>
