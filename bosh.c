#include "bosh_utilities.h"
#include "bosh_parser.h"

char banner[64], pidstr[16];
pid_t shell_pid;

void initialize_stuff(){
    testlink();
    shell_pid = getpid();
    sprintf(pidstr, "%d", shell_pid);
    banner[0] = '[';
    strcat(banner, pidstr);
    strcat(banner,"] ø > ");
    signal(SIGINT, sigint_handler);
    signal(SIGTSTP, sigtstp_handler);
}


int main(void)
{
	pid_t pid;
	TOKEN term = T_NL;
    initialize_stuff();

	while (true) {
		if (term == T_NL)
			printf("%s", banner);
		term = command(&pid, false, NULL);
		if (term == T_ERROR) {
			fprintf(stderr, "Bad command\n");
			term = T_NL;
		}
		if (term != T_AMP && pid > 0)
			wait_and_display(pid);
		fd_check();
	}
}
