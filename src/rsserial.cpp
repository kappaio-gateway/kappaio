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
#include "glbdef.h"
#include "localintf.h"
#include "apsdb.h"
#include "zdo.h"
#include "hal.h"
#include <stdint.h>
#include <time.h>
#include <string.h>
#include <dlfcn.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/file.h>
#include <sys/select.h>
#include <stdio.h>
#include "kutil.h"
#include "kglobal.h"
#include "remoteif.h"
#include <iostream>
#include <string>

#define ENDPOINT_PATH "/usr/lib/rsserial/endpoints"
#define RSSERIAL_KEEP_ALIVE_COUNT_FILE "/tmp/rsserial-kla.json"
#define errExit(msg) do { perror(msg); exit(EXIT_FAILURE); } while (0)

int write_pid()
{
	int fd = open(PIDF, O_WRONLY|O_CREAT, 0600);
	if (fd < 0)
	{
		syslog(LOG_INFO, "write_pid open fd failed:%s", strerror(errno));
	}
	if (flock(fd,LOCK_EX|LOCK_NB) < 0)
	{
		syslog(LOG_INFO, "only one instance please");
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
		syslog(LOG_INFO, "SIGTERM catched and terminating program");
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

int handleWriteFlash(int argc, char **argv)
{
	// flash TI processor
	int exitStatus = -1;
	if (argc > 1)
	{
		char *aargv;
		int argIndex=1;

		processorStatus_t status;
		openlog("rsserial_TI_update", LOG_CONS|LOG_PID, LOG_USER);

		hal_init();
		ProcessorReset();

		kSleep(5);
		aargv = argv[argIndex++];
		if (aargv[0]=='-' && strlen(aargv) > 1)
		{
			// 'w' write image
			if (aargv[1]=='w')
			{
				if (argc < (argIndex + 1))
				{
					LOG_ERROR_MESSAGE("no valid image file path");
					goto handleWriteFlashCleanUp;
				}
				void *sbl_handle = dlopen("/usr/lib/kappaio/libsbl.so",RTLD_LAZY|RTLD_LOCAL );
				if (!sbl_handle)
				{
					LOG_ERROR_MESSAGE("error when loading libsbl.so : %s\n", dlerror());
					goto handleWriteFlashCleanUp;
				}
				int (*fn)(const char*) = (int(*)(const char*))dlsym(sbl_handle, "flashImage");
				if (!fn)
				{
					LOG_ERROR_MESSAGE("error when loading libsbl.so : %s\n", dlerror());
					goto handleWriteFlashCleanUp;
				}
				exitStatus = fn(argv[argIndex]);
			}
		}
		/* issue a systemPing which in turn triggers a CRC consistency check, which takes about 40 seconds */
		printf("Starting CRC check, this may take up to 40 seconds ...\n");
		while (systemPing(&status) <0)
		{
			kSleep(25);
			LOG_MESSAGE("calculating CRC...");
		}
		if (status.capability != 0x3275)
		{
			printf("CRC check failed, try flashing again\n");
		}
		handleWriteFlashCleanUp:
		closelog();
		printf("CRC check ok, flash successful\n");
	}
	return exitStatus;
}

int handleJson(char *argv)
{
	printf(localRpcRequest(argv).c_str());
	return 0;
}
/* install segfault handler, debug only, 06-06-2014 */
void segfault_sigaction(int signal, siginfo_t *si, void *arg)
{
	//=================
	// assist to locate segfault
	char cmd[256];
	sprintf(cmd, " cp /proc/%d/maps /tmp/maps && echo '\npid = %d ' >> /tmp/maps && echo '\nSegfault address = %p ' >> /tmp/maps ", getpid(), getpid(), si->si_addr);
	system(cmd);
	//=================
	LOG_MESSAGE("Caught segfault at address %p", si->si_addr);
	exit(0);
}
/* install segfault handler, debug only, 06-06-2014 */

int klaIncr()
{
	static unsigned short keepAliveCount = 1;
	int fd = open(RSSERIAL_KEEP_ALIVE_COUNT_FILE, O_WRONLY|O_CREAT, 0600);
	char string[32];
	int len = sprintf(string, "%d\n", keepAliveCount++);
	write(fd, string, len);
	close(fd);
	return 0;
}

int main(int argc, char **argv)
{
	if (argc >1)
	{
		int exitStatus = -1;
		int argIndex=1;
		char *aargv = argv[argIndex++];
		if (aargv[0] == '-')
		{
			switch (aargv[1])
			{
			case 'w':
				exitStatus = handleWriteFlash(argc, argv);
				break;
			case 'j':
				exitStatus = handleJson(argv[argIndex++]);
				break;
			default:
				break;
			}
		}
		exit(exitStatus);
	}
	// main application
	openlog("rsserial", LOG_CONS|LOG_PID, LOG_USER);
	syslog(LOG_INFO, "rsserial v1.04");
	daemonize();
	if (write_pid() < 0)
	{
		exit(0);
	}
	/* install segfault handler, debug only, 06-06-2014 */
	struct sigaction sa;
	memset(&sa, 0, sizeof(struct sigaction));
	sigemptyset(&sa.sa_mask);
	sa.sa_sigaction = segfault_sigaction;
	sa.sa_flags   = SA_SIGINFO;
	sigaction(SIGSEGV, &sa, NULL);
	/* install segfault handler, debug only, 06-06-2014 */

	klaIncr();
	// ====================================================

	// initialize the communication channel between the CPU and TI-CC2530
	// if success, CPU should be able to command CC2530 using TI's MT-command set
	// and, based on MT-command, CPU should be able to send Zigbee AF message
	hal_init();
	ProcessorReset();
	StartSystem();
	StartSystemMonitor();

	aps::init();
	LOG_MESSAGE("passed apsdbinit ");
	localIntfInit();
	LOG_MESSAGE("passed localIntfInit ");

	struct dirent *dir;
	DIR *d = opendir(ENDPOINT_PATH);
	if (d)
	{
		while ((dir = readdir(d)) != NULL)
		{
			if (dir->d_type == DT_REG || dir->d_type == DT_LNK)
			{
				char endpoint_plugin[256];
				int len = sprintf(endpoint_plugin, "%s/%s",ENDPOINT_PATH, dir->d_name);
				void *profileHandle = dlopen(endpoint_plugin,RTLD_NOW|RTLD_LOCAL);
				if (profileHandle)
				{
					void (*fn)(void) = (void (*)(void))dlsym(profileHandle, "init");
					if (!fn)
					{
						LOG_ERROR_MESSAGE("error when loading %s : %s\n", endpoint_plugin,dlerror());
					} else {
						fn();
					}
				}
			}
		}
		closedir(d);
	}
	LOG_MESSAGE("passed load endpoints ");
	//======= beta 02-15-2014 ======
	startZDO();
	LOG_MESSAGE("passed load startZDO ");

	remoteIntfInit();

	syslog(LOG_INFO, "main() initialization done -- entering infinite loop");

//	unsigned short keepAliveCount = 0;
	while(1)
	{
//		int fd = open(RSSERIAL_KEEP_ALIVE_COUNT_FILE, O_WRONLY|O_CREAT, 0600);
//		char string[32];
//		int len = sprintf(string, "%d\n", keepAliveCount++);
//		write(fd, string, len);
//		close(fd);
		klaIncr();
		kSleep(5);
	}
}


