#ifndef HEADERS_H
#define HEADERS_H
#include <stdio.h>
#include <stdbool.h>
#include <signal.h> // kill(), SIGKILL, SIGSTOP, SIGCONT, etc.// struct sigaction, sigaction(), SIGINT, SIGTSTP
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <pwd.h> // struct passwd, getpwuid
#include <ctype.h>
#include <stddef.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h> //for pid_t, uid_t (needed by getpwuid/getuid)
#include <sys/wait.h>
#include <unistd.h> // for chdir,getcwd,gethostname
#include <time.h>
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
// #include <grp.h>
#include "prompt.h"
#include "log.h"
#include "hop.h"
#include "input.h"
#include "parser.h"
#include "reveal.h"
// #include "io.h"
#include "pipes.h"
// #include "execution.h"
// #include "signals.h"
// #include "jobs.h"
#include "background.h"
#include "sequential.h"
// #include "ping.h"
// #include "activities.h"
#define MAX_LOG_ENTRIES 15
#define MAX_TOKEN_LEN 256
#define MAX_TOKENS 128
#define MAX_PATH 1024

#endif
