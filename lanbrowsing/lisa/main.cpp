/*    main.cpp
 *
 *    Copyright (c) 1998-2000 Alexander Neundorf
 *    neundorf@kde.org
 *
 *    You may distribute under the terms of the GNU General Public
 *    License as specified in the COPYING file.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 */

#include "lisadefines.h"
#include "netmanager.h"

#ifdef LISA_DEBUG
#undef LISA_DEBUG
#endif

#ifdef dcerr
#undef dcerr
#endif

#define LISA_DEBUG 0
#define dcerr if (LISA_DEBUG==1) std::cerr<<"main "

#include <iostream>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>

// detect linux/glibc for the gnu style --args
#if defined(__linux__) || defined(__linux) || defined(linux)
#  include <features.h>
#  ifdef __GLIBC__
// only gnu libc has getopt.h... getopt(3) is defined to be in unistd.h
// by POSIX.2
#ifndef _GNU_SOURCE
#    define _GNU_SOURCE
#endif
#    include <getopt.h>
#  endif // __GLIBC__
#  define GNU_GETOPT
#endif // linux


void printVersion()
{
   const char * versionInfo=\
   "\r\nThis is the LAN Information Server LISa "MYVERSION"\r\n"\
   "It is free software according the GNU General Public License\r\n"\
   "Copyright (c) 2000-2003 by Alexander Neundorf\r\n"\
   "email: neundorf@kde.org\r\n";
   std::cout<<versionInfo<<std::endl;
}

void usage()
{
   printVersion();
   const char * usageInfo=\
      "-v, --version      prints out a short version info\n"\
      "-u, --unix         deprecated\n"\
      "-k, --kde1         deprecated\n"\
      "-K, --kde2         deprecated\n"\
      "                   lisa now looks always first for $(HOME)/.lisarc, then for /etc/lisarc\n"\
      "-c, --config=FILE  read this and no other configuration file\n"\
      "-q, --quiet        start quiet without the greeting message\n"\
      "-p, --port PORTNR  start the server on this portnumber\n"\
      "                   if you use this LISa won't be able to\n"\
      "                   cooperate with other LISa's in the network\n"\
      "-h, --help         you are currently reading it ;-)\n";
      std::cout<<usageInfo<<std::endl;
//I thought this would be the way to check wether long options are supported...
//#ifndef _GNU_SOURCE
//      std::cout<<"Please note that the long options are not supported on
//      this system"<<std::endl;
//#endif
}

NetManager *manager(0);

   
void destruct(int sigNumber)
{
   signal(sigNumber,SIG_IGN);
   dcerr<<"signal caught: "<<sigNumber<<", exiting"<<std::endl;
   //signal(sigNumber,&destruct);
   if (manager!=0)
      manager->~NetManager();
   exit(0);
}

void readConfig(int sigNumber)
{
   dcerr<<"readConfig(): signal caught: "<<sigNumber<<std::endl;
   signal(SIGHUP,SIG_IGN);
   if (manager!=0)
      manager->readConfig();
   signal(SIGHUP,&readConfig);
}

void printState(int sigNumber)
{
   dcerr<<"printState(): signal caught: "<<sigNumber<<std::endl;
   signal(SIGUSR1,SIG_IGN);
   if (manager!=0)
      manager->printState();
   signal(SIGUSR1,&printState);
}

void setSignalHandler()
{
   signal(SIGHUP,&readConfig);
	signal(SIGUSR1,&printState);

   signal(SIGINT,&destruct);
	signal(SIGQUIT,&destruct);
	signal(SIGILL,&destruct);
	signal(SIGTRAP,&destruct);
	signal(SIGABRT,&destruct);
	signal(SIGBUS,&destruct);
	signal(SIGSEGV,&destruct);
	signal(SIGUSR2,&destruct);
	signal(SIGPIPE,&destruct);
	signal(SIGALRM,&destruct);
	signal(SIGTERM,&destruct);
	signal(SIGFPE,&destruct);
#ifdef SIGPOLL
   signal(SIGPOLL, &destruct);
#endif
#ifdef SIGSYS
   signal(SIGSYS, &destruct);
#endif
#ifdef SIGVTALRM
   signal(SIGVTALRM, &destruct);
#endif
#ifdef SIGXCPU
   signal(SIGXCPU, &destruct);
#endif
#ifdef SIGXFSZ
   signal(SIGXFSZ, &destruct);
#endif
}

#ifdef GNU_GETOPT 
static struct option const long_opts[] =
{
  {"version", no_argument, 0, 'v'},
  {"quiet", no_argument, 0, 'q'},
  {"unix", no_argument, 0, 'u'},
  {"kde1", no_argument, 0, 'k'},
  {"kde2", no_argument, 0, 'K'},
  {"config", required_argument, 0, 'c'},
  {"port", required_argument, 0, 'p'},
  {"help", no_argument, 0, 'h'},
  {0, 0, 0, 0}
};
#endif

int main(int argc, char** argv)
{
   int quiet(0);
   int c(0);
   int configStyle(UNIXCONFIGSTYLE);
   MyString configFile;
   int portToUse(MYPORT);

//I thought this would be the way to check wether long options are supported...
#ifdef GNU_GETOPT 
   while ((c=getopt_long(argc, argv, "vqukKc:h", long_opts, 0))!=-1)
#else
   while ((c=getopt(argc, argv, "vqukKc:h"))!=-1)
#endif

   {
      char *endp(0);
      switch (c)
      {
      case 0:
         break;
      case 'v':
         printVersion();
         exit(0);
         break;
      case 'q':
         quiet=1;
         break;
      case 'u':
      case 'k':
      case 'K':
         std::cout<<"\a\nThe command line switches -k, -K, -u and \ntheir long versions "\
            "--kde1, --kde2 and --unix are not supported anymore.\n"\
            "Lisa will always first look for $(HOME)/.lisarc , then for /etc/lisarc.\n"\
            "If your lisa configuration file was created using an older version of \n"\
            "the KDE control center, copy the /root/.kde/share/config/lisarc to /etc and \n"\
            "then start lisa without any command line options.\n"<<std::endl;
         return 0;
         break;

      case 'c':
         configFile = optarg;
         configStyle = EXTRACONFIGSTYLE;
         break;

      case 'p':
         portToUse=strtol(optarg,&endp,10);
         if (endp!=0)
         {
            usage();
            exit(0);
         }
         break;

      case 'h':
      default:
         usage();
         exit(0);
         break;
      }
   }

   //fork and let the parent exit
   pid_t pid=fork();
   if (pid>0)
   {
      //this is the parent
      exit(0);
   }
   else if (pid<0)
   {
      std::cout<<"could not fork()"<<std::endl;
      exit(0);
   }
   //we will only read/write to/from this socket in the child process
   int rawSocket=socket(AF_INET,SOCK_RAW,IPPROTO_ICMP);
   if (rawSocket==-1)
   {
      std::cout<<"could not create raw socket, root privileges required"<<std::endl;
      std::cout<<"take a look at the README for more information"<<std::endl;
      exit(0);
   }
   int bufferSize(60*1024);
   int on(1);
   setsockopt(rawSocket, SOL_SOCKET, SO_RCVBUF, (char*)&bufferSize,
	      sizeof(bufferSize));
   int result=setsockopt(rawSocket, SOL_SOCKET, SO_BROADCAST, (char*)&on,
			 sizeof(on));
   dcerr<<"setsockopt returns "<<result<<std::endl;
   //dropping root privileges
   //they will be regained once in the child process
   //for creating a raw socket

   //now dropping root privileges once and ever
   
   
   setuid(getuid());
   if (geteuid() != getuid())
     _exit(255);

   //according to R. Stevens the following three lines 
   //make daemons feel good :)
   setsid();
   chdir("/");
   umask(0);
   
   dcerr<<"starting, dropped root privileges"<<std::endl;
   dcerr<<"port: "<<portToUse<<" file: "<<configFile<<std::endl;
   NetManager netmanager(rawSocket,portToUse,configFile,configStyle,0);
   manager=&netmanager;
   dcerr<<"NetManager created"<<std::endl;
   setSignalHandler();

   netmanager.readConfig();
   if (netmanager.prepare())
   {
      if (!quiet)
      {
         printVersion();
         std::cout<<"\n\rrunning on port "<<portToUse<<"\n\rHave fun ! :-)"<<std::endl;
      }
      netmanager.run();
   };
   dcerr<<"server finished"<<std::endl;
}
