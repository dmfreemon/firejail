 /*
 * Copyright (C) 2014-2020 Firejail Authors
 *
 * This file is part of firejail project
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define MAXBUF 2048
// stats
static int cnt_profiles = 0;
static int cnt_apparmor = 0;
static int cnt_seccomp = 0;
static int cnt_caps = 0;
static int cnt_dotlocal = 0;
static int cnt_globalsdotlocal = 0;
static int cnt_netnone = 0;
static int cnt_noexec = 0;	// include disable-exec.inc
static int cnt_privatedev = 0;
static int cnt_privatetmp = 0;
static int cnt_whitelistvar = 0;	// include whitelist-var-common.inc
static int cnt_ssh = 0;

static int level = 0;
static int arg_debug = 0;
static int arg_apparmor = 0;
static int arg_caps = 0;
static int arg_seccomp = 0;
static int arg_noexec = 0;
static int arg_privatedev = 0;
static int arg_privatetmp = 0;
static int arg_whitelistvar = 0;
static int arg_ssh = 0;

static void usage(void) {
	printf("proftool - print profile statistics\n");
	printf("Usage: proftool [options] file[s]\n");
	printf("Options:\n");
	printf("   --apparmor - print profiles without apparmor\n");
	printf("   --caps - print profiles without caps\n");
	printf("   --ssh - print profiles without \"include disable-common.inc\"\n");
	printf("   --noexec - print profiles without \"include disable-exec.inc\"\n");
	printf("   --private-dev - print profiles without private-dev\n");
	printf("   --private-tmp - print profiles without private-tmp\n");
	printf("   --seccomp - print profiles without seccomp\n");
	printf("   --whitelist-var - print profiles without \"include whitelist-var-common.inc\"\n");
	printf("   --debug\n");
	printf("\n");
}

void process_file(const char *fname) {
	assert(fname);

	if (arg_debug)
		printf("processing #%s#\n", fname);
	level++;
	assert(level < 32); // to do - check in firejail code

	FILE *fp = fopen(fname, "r");
	if (!fp) {
		fprintf(stderr, "Error: cannot open %s\n", fname);
		exit(1);
	}

	char buf[MAXBUF];
	while (fgets(buf, MAXBUF, fp)) {
		char *ptr = strchr(buf, '\n');
		if (ptr)
			*ptr = '\0';
		ptr = buf;

		while (*ptr == ' ' || *ptr == '\t')
			ptr++;
		if (*ptr == '\n' || *ptr == '#')
			continue;

		if (strncmp(ptr, "seccomp", 7) == 0)
			cnt_seccomp++;
		else if (strncmp(ptr, "caps", 4) == 0)
			cnt_caps++;
		else if (strncmp(ptr, "include disable-exec.inc", 24) == 0)
			cnt_noexec++;
		else if (strncmp(ptr, "include whitelist-var-common.inc", 32) == 0)
			cnt_whitelistvar++;
		else if (strncmp(ptr, "include disable-common.inc", 26) == 0)
			cnt_ssh++;
		else if (strncmp(ptr, "net none", 8) == 0)
			cnt_netnone++;
		else if (strncmp(ptr, "apparmor", 8) == 0)
			cnt_apparmor++;
		else if (strncmp(ptr, "private-dev", 11) == 0)
			cnt_privatedev++;
		else if (strncmp(ptr, "private-tmp", 11) == 0)
			cnt_privatetmp++;
		else if (strncmp(ptr, "include ", 8) == 0) {
			// not processing .local files
			if (strstr(ptr, ".local")) {
//printf("dotlocal %d, level %d - #%s#, redirect #%s#\n", cnt_dotlocal, level, fname, buf + 8);
				if (strstr(ptr, "globals.local"))
					cnt_globalsdotlocal++;
				else
					cnt_dotlocal++;
				continue;
			}
			process_file(buf + 8);
		}
	}

	fclose(fp);
	level--;
}

int main(int argc, char **argv) {
	if (argc <= 1) {
		usage();
		return 1;
	}

	int start = 1;
	int i;
	for (i = 1; i < argc; i++) {
		if (strcmp(argv[i], "--help") == 0) {
			usage();
			return 0;
		}
		else if (strcmp(argv[i], "--debug") == 0)
			arg_debug = 1;
		else if (strcmp(argv[i], "--apparmor") == 0)
			arg_apparmor = 1;
		else if (strcmp(argv[i], "--caps") == 0)
			arg_caps = 1;
		else if (strcmp(argv[i], "--seccomp") == 0)
			arg_seccomp = 1;
		else if (strcmp(argv[i], "--noexec") == 0)
			arg_noexec = 1;
		else if (strcmp(argv[i], "--private-dev") == 0)
			arg_privatedev = 1;
		else if (strcmp(argv[i], "--private-tmp") == 0)
			arg_privatetmp = 1;
		else if (strcmp(argv[i], "--whitelist-var") == 0)
			arg_whitelistvar = 1;
		else if (strcmp(argv[i], "--ssh") == 0)
			arg_ssh = 1;
		else if (*argv[i] == '-') {
			fprintf(stderr, "Error: invalid option %s\n", argv[i]);
		 	return 1;
		 }
		 else
		 	break;
	}

	start = i;
	if (i == argc) {
		fprintf(stderr, "Error: no porfile file specified\n");
		return 1;
	}

	for (i = start; i < argc; i++) {
		cnt_profiles++;

		// watch seccomp
		int seccomp = cnt_seccomp;
		int caps = cnt_caps;
		int apparmor = cnt_apparmor;
		int noexec = cnt_noexec;
		int privatetmp = cnt_privatetmp;
		int privatedev = cnt_privatedev;
		int dotlocal = cnt_dotlocal;
		int globalsdotlocal = cnt_globalsdotlocal;
		int whitelistvar = cnt_whitelistvar;
		int ssh = cnt_ssh;

		// process file
		process_file(argv[i]);

		// warnings
		if ((caps + 2) <= cnt_caps) {
			printf("Warning: multiple caps in %s\n", argv[i]);
			cnt_caps = caps + 1;
		}

		// fix redirections
		if (cnt_dotlocal > (dotlocal + 1))
			cnt_dotlocal = dotlocal + 1;
		if (cnt_globalsdotlocal > (globalsdotlocal + 1))
			cnt_globalsdotlocal = globalsdotlocal + 1;

		if (arg_apparmor && apparmor == cnt_apparmor)
			printf("No apparmor found in %s\n", argv[i]);
		if (arg_caps && caps == cnt_caps)
			printf("No caps found in %s\n", argv[i]);
		if (arg_seccomp && seccomp == cnt_seccomp)
			printf("No seccomp found in %s\n", argv[i]);
		if (arg_noexec && noexec == cnt_noexec)
			printf("No include disable-exec.inc found in %s\n", argv[i]);
		if (arg_privatedev && privatedev == cnt_privatedev)
			printf("No private-dev found in %s\n", argv[i]);
		if (arg_privatetmp && privatetmp == cnt_privatetmp)
			printf("No private-tmp found in %s\n", argv[i]);
		if (arg_whitelistvar && whitelistvar == cnt_whitelistvar)
			printf("No include whitelist-var-common.inc found in %s\n", argv[i]);
		if (arg_ssh && ssh == cnt_ssh)
			printf("No include disable-common.inc found in %s\n", argv[i]);

		assert(level == 0);
	}

	printf("\n");
	printf("Stats:\n");
	printf("    profiles\t\t\t%d\n", cnt_profiles);
	printf("    include local profile\t%d   (include profile-name.local)\n", cnt_dotlocal);
	printf("    include globals\t\t%d   (include globals.local)\n", cnt_dotlocal);
	printf("    blacklist ~/.ssh\t\t%d   (include disable-common.inc)\n", cnt_ssh);
	printf("    seccomp\t\t\t%d\n", cnt_seccomp);
	printf("    capabilities\t\t%d\n", cnt_caps);
	printf("    noexec\t\t\t%d   (include disable-exec.inc)\n", cnt_noexec);
	printf("    apparmor\t\t\t%d\n", cnt_apparmor);
	printf("    private-dev\t\t\t%d\n", cnt_privatedev);
	printf("    private-tmp\t\t\t%d\n", cnt_privatetmp);
	printf("    whitelist var directory\t%d   (include whitelist-var-common.inc)\n", cnt_whitelistvar);
	printf("    net none\t\t\t%d\n", cnt_netnone);
	printf("\n");
	return 0;
}