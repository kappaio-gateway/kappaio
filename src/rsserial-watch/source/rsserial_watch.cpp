//====================================================================================
//     The MIT License (MIT)
//
//     Copyright (c) 2011 Kapparock LLC
//
//     Permission is hereby granted, free of charge, to any person obtaining a copy
//     of this software and associated documentation files (the "Software"), to deal
//     in the Software without restriction, including without limitation the rights
//     to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//     copies of the Software, and to permit persons to whom the Software is
//     furnished to do so, subject to the following conditions:
//
//     The above copyright notice and this permission notice shall be included in
//     all copies or substantial portions of the Software.
//
//     THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//     IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//     FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//     AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//     LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//     OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//     THE SOFTWARE.
//====================================================================================
#include "kjson.h"
#include "kutil.h"
#include <fcntl.h>
#include <syslog.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/stat.h>

#define RSSERIAL_KEEP_ALIVE_COUNT_FILE "/tmp/rsserial-kla.json"
#define PIDF "/var/run/rsserial-watch.pid"
#define RSSERIAL_PIDF "/var/run/rsserial.pid"

int write_pid()
{

	int fd = open(PIDF, O_WRONLY|O_CREAT, 0600);

	if (fd < 0)
	{
		syslog(LOG_INFO, "write_pid open fd failed:%s", strerror(errno));
	}

	if (flock(fd,LOCK_EX|LOCK_NB) < 0)
	{
		syslog(LOG_INFO, "only one instance of please");
		close(fd);
		return -1;
	}
	char pidstr[16];
	int pidstr_len;
	pidstr_len = sprintf(pidstr, "%d\n", getpid());
	write(fd, pidstr, pidstr_len);
	//close(fd);
	return 0;

}

void daemon_signal_handler(int sig)
{
	if (sig == SIGTERM)
	{
		syslog(LOG_INFO, "SIGTERM catched and terminating rsserial-watch");
		system("/etc/init.d/rsserial stop > /dev/null");
		unlink(PIDF);
		closelog();
		exit(0);
	}
}

void daemonize()
{
	pid_t pid, sid;

	signal(SIGTERM, daemon_signal_handler);

	pid = fork();

	if (pid < 0)
	{
		exit(EXIT_FAILURE);
	}

	if (pid > 0)
	{
		exit(EXIT_SUCCESS);
	}

	umask(0);
	sid = setsid();
	if (sid < 0) {
		exit(EXIT_FAILURE);
	}

	if ((chdir("/")) < 0) {
		exit(EXIT_FAILURE);
	}
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);
}

//void read_ints (const char* file_name)
//{
//  FILE* file = fopen (file_name, "r");
//  int i = 0;
//
//  fscanf (file, "%d", &i);
//  while (!feof (file))
//    {
//      printf ("%d ", i);
//      fscanf (file, "%d", &i);
//    }
//  fclose (file);
//}
int readKLA()
{
	if (access(RSSERIAL_KEEP_ALIVE_COUNT_FILE, F_OK) <0)
	{
		return -1;
	}

	int kla;
	FILE *file = fopen (RSSERIAL_KEEP_ALIVE_COUNT_FILE, "r");

	if (file == NULL)
	{
		fclose(file);
		return -1;
	}

	fscanf(file, "%d", &kla);
	fclose(file);

	return kla;

}
int checkByPIDFile()
{
	/*determine if process is running by check PID*/
	if (access(RSSERIAL_PIDF, F_OK) <0)
	{
		return -1;
	}
	int pid;
	FILE *pidFile = fopen (RSSERIAL_PIDF, "r");

	if (pidFile == NULL)
	{
		fclose(pidFile);
		return -1;
	}

	fscanf(pidFile, "%d", &pid);
	fclose(pidFile);

	/* Got PID , now check if this PID is running */
	struct stat s;
	char fileString[32];

	sprintf(fileString, "/proc/%d", pid);
	//syslog(LOG_INFO, "%s", fileString);
	int err = stat(fileString, &s);
	if(-1 == err) {
	    if(ENOENT == errno) {
	        syslog(LOG_INFO, "does not exist ");
	    	return -1;
	    } else {
	        syslog(LOG_INFO, "stat error");
	        return 0;
	    }
	} else {
	  return 0;
	}
	return 0;
}
int  main()
{

	openlog("rsserial-watch", LOG_CONS|LOG_PID, LOG_USER);
	syslog(LOG_INFO, "rsserial-watch v1.01");

	daemonize();

	if (write_pid() < 0)
	{
		exit(0);
	}

	int keepAliveCountRef = 0;
	int keepAliveCount;
	int tolerance = 0;
	while(1)
	{
		if (checkByPIDFile() < 0)
		{
			syslog(LOG_INFO, "process is not running");
			/* restart process immediately*/
			system("/etc/init.d/rsserial restart > /dev/null");
			goto SKIP_THE_REST;
		}

		keepAliveCount = readKLA();
		if (keepAliveCount < 0)
		{
			syslog(LOG_INFO, "keep alive counter < 0, restarting...");
			/* restart process immediately*/
			system("/etc/init.d/rsserial restart > /dev/null");
			goto SKIP_THE_REST;
		}

		if (keepAliveCount != keepAliveCountRef)
		{
			keepAliveCountRef = keepAliveCount;
			tolerance = 0;
		} else
		{
			if (tolerance > 0)
			{
				syslog(LOG_INFO, "tolerance > 0, restarting...");
				/* restart process immediately*/
				system("/etc/init.d/rsserial restart > /dev/null");
				tolerance = 0;
			} else {
				tolerance++;
			}
		}

		SKIP_THE_REST:
		kSleep(10);
	}
}
