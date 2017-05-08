#include <signal.h>
#include "GPIOClass.h"
 
void sig_handler(int sig);

bool ctrl_c_pressed = false;

using namespace std;
 
int main (void)
{
	struct sigaction sig_struct;
	sig_struct.sa_handler = sig_handler;
	sig_struct.sa_flags = 0;
	sigemptyset(&sig_struct.sa_mask);

	if (sigaction(SIGINT, &sig_struct, NULL) == -1) {
		cout << "Problem with sigaction" << endl;
		exit(1);
	}
 
    string inputstate;
    int i = 0;
    GPIOClass* gled = new GPIOClass("6"); //create new GPIO object to be attached to  GPIO4
    GPIOClass* rled = new GPIOClass("13"); //create new GPIO object to be attached to  GPIO17
 
    cout << " GPIO pins exported" << endl;
 
    gled->setdir_gpio("out"); //GPIO4 set to output
    rled->setdir_gpio("out"); // GPIO17 set to input
 
    cout << " Set GPIO pin directions" << endl;
 
    while(i < 20)
    {
        usleep(500000);  // wait for 0.5 seconds
        gled->setval_gpio("1");
        usleep(500000);  // wait for 0.5 seconds
        gled->setval_gpio("0");
    	i++;
        cout << i << endl;
        if(ctrl_c_pressed)
        {
            break;
        }
    }
    
    cout << "Releasing heap memory and exiting....." << endl;
    delete gled;
    delete rled;
    gled = NULL;
    rled = NULL;
    
    return 0;
}

void sig_handler(int sig)
{
    write(0,"\nCtrl^C pressed in sig handler\n",32);
    ctrl_c_pressed = true;
}
