#include <stdio.h>
#include <stdlib.h>
#include <sys/ptrace.h>
#include <signal.h>
#include <sys/user.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

void cmd(char *arg);

main(int argc, char *argv[]){
    printf("*****************************************************\n");
    printf("*****************************************************\n");
    printf("STARTING MY GDB\n");
    printf("LOADING %s\n", argv[1]);
    printf("TYPE help TO GET COMMAND HELP\n");
    printf("*****************************************************\n");
    cmd(argv[1]);
}

void read_args(char *s, char **args, char *delim){
	int i = 0,j = 0, k = 0;
	while(!fgets(s,100,stdin));
	args[i] = strtok(s,delim);
	while(args[i] != NULL){
		//printf("%s",args[i]);
		args[++i] = strtok(NULL,delim);
	}
}

void b_cmd(int pid){
    char *cmds[10], s[100], *endptr;
    long unsigned int address;
    int i = 0;
    while(1){
	printf("->");
	read_args(s, cmds, " \n");
	if(strcmp(cmds[0], "get") == 0){
	    address = strtol(cmds[1], &endptr, 16);
	    int data = ptrace(PTRACE_PEEKDATA, pid, address, 0);
	    printf("Value at %x = %lu\n", address, data);
	}
	else if(strcmp(cmds[0], "set") == 0){
	    address = strtol(cmds[1], &endptr, 16);
	    int dat = strtol(cmds[2], &endptr, 10);
	    ptrace(PTRACE_POKEDATA, pid, address, dat);
	}
	else if(strcmp(cmds[0], "continue") == 0)
	    break;
	else if(strcmp(cmds[0], "exit") == 0)
	    return 0;
	else if(strcmp(cmds[0], "help") == 0){
	    printf("get address : to get data at given address, address should be in hexadecimal format\n");
	    printf("set address data : to set data at given address, data should be in intiger format and  address should be in hexadecimal format\n");
	    printf("continue : to continue running the program\n");
	    printf("exit : to exit mygdb\n");
	}
	else
	    printf("invalid command\n");
    }
}

void cmd(char *arg){
    char *cmds[10], s[100], *endptr;
    long unsigned int address[100];
    int i = 0;
    while(1){
	printf(">>");
	read_args(s, cmds, " \n");
	if(strcmp(cmds[0], "break") == 0){
	    address[i++] = strtol(cmds[1], &endptr, 16);
	    printf("break point set at %x\n",address[i-1]);
	}
	else if(strcmp(cmds[0], "exit") == 0)
	    return 0;
	else if(strcmp(cmds[0], "run") == 0)
	    break;
	else if(strcmp(cmds[0], "help") == 0){
	    printf("break address : to set break point, address should be in hexadecimal format\n");
	    printf("run : to start running the program\n");
	    printf("exit : to exit mygdb\n");
	}
	else
	    printf("invalid command\n");
    }
    int j = 0, status = 0, pid;
    struct user_regs_struct uregs;
    printf("running %s...\n",arg);
    if ((pid=fork())==0) {
        ptrace(PTRACE_TRACEME, 0, 0, 0);
        execl(arg, arg, 0);
        printf("execl error...\n");
    } else {
        wait(&status);
	while(1){
	    j = 0;
	    if(WIFEXITED(status)) break;
	    ptrace(PTRACE_SINGLESTEP, pid, 0, 0); 
	    wait(&status);
	    ptrace(PTRACE_GETREGS, pid, 0, &uregs);
	    while(j < i){
		if(address[j++] == uregs.rip){
		    printf("break at %x\n", uregs.rip);
		    b_cmd(pid);
		}
	    }
	}
	printf("Finished running %s...\n", arg);
	cmd(arg);
    }
}
