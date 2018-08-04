#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <sys/wait.h>

using namespace std;

int main()
{
    pid_t pid = fork();
    if(pid == 0)
    {
        cout << "child id is: " << getpid() << endl;
        for(int i = 0; i < 30; i++)
        {
            cout << "child " << getpid() << " : " << i << endl;
            sleep(1);
        }
	    exit(0);
    }
    else if(pid > 0)
    {
        cout << "parent id is: " << getpid() << endl;
        while(int i = 1)
        {
            cout << "parent " << getpid() << " : " << i << endl;
            sleep(1);
        }
        cout << "exit: " << wait(NULL) << endl;
        getchar();
        return 0;
    }
    else
    {
	    cout << "error" << endl;
	}
    
}
