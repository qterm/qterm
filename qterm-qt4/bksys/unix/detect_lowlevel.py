# Thomas Nagy 2005

def detect(lenv,dest):
	import os


	#### Don't fix the stuff below by hand, write proper tests in lowlevel.py
	content="""
/* #undef CRAY_STACKSEG_END */

/* Define to 1 if using `alloca.c'. */
/* #undef C_ALLOCA */

/* Define where your dcop binary is */
#define DCOP_PATH "/usr/bin"

/* Define if getaddrinfo returns AF_UNIX sockets */
/* #undef GETADDRINFO_RETURNS_UNIX */

/* Define if you have the MIT Kerberos libraries */
#define GSSAPI_MIT 1

/* define if you have aKodelib installed */
/* #undef HAVE_AKODE */

/* Define to 1 if you have `alloca', as a function or macro. */
#define HAVE_ALLOCA 1

/* Define to 1 if you have the <alloca.h> header file. */
#define HAVE_ALLOCA_H 1

/* Define to 1 if you have the <arpa/nameser8_compat.h> header file. */
/* #undef HAVE_ARPA_NAMESER8_COMPAT_H */

/* Define if execinfo.h exists and defines backtrace (GLIBC >= 2.1) */
#define HAVE_BACKTRACE 1

/* Define to 1 if you have the `bcopy' function. */
/* #undef HAVE_BCOPY */

/* You _must_ have bool */
#define HAVE_BOOL 1

/* Define if getaddrinfo is broken and should be replaced */
/* #undef HAVE_BROKEN_GETADDRINFO */

/* Defines if bzip2 is compiled */
#define HAVE_BZIP2_SUPPORT 1

/* Define to 1 if you have the <Carbon/Carbon.h> header file. */
/* #undef HAVE_CARBON_CARBON_H */

/* Define if you have the CoreAudio API */
/* #undef HAVE_COREAUDIO */

/* Define to 1 if you have the <crt_externs.h> header file. */
/* #undef HAVE_CRT_EXTERNS_H */

/* Defines if your system has the crypt function */
#define HAVE_CRYPT 1

/* Define to 1 if you have the <ctype.h> header file. */
#define HAVE_CTYPE_H 1

/* Defines if you have CUPS (Common UNIX Printing System) */
#define HAVE_CUPS 1

/* CUPS doesn't have password caching */
#define HAVE_CUPS_NO_PWD_CACHE 1

/* Define to 1 if you have the declaration of `getservbyname_r', and to 0 if
   you don't. */
#define HAVE_DECL_GETSERVBYNAME_R 1

/* Define to 1 if you have the <dirent.h> header file, and it defines `DIR'.
   */
#define HAVE_DIRENT_H 1

/* Define if you have the GNU dld library. */
/* #undef HAVE_DLD */

/* Define to 1 if you have the <dld.h> header file. */
/* #undef HAVE_DLD_H */

/* Define to 1 if you have the `dlerror' function. */
#define HAVE_DLERROR 1

/* Define to 1 if you have the <dlfcn.h> header file. */
#define HAVE_DLFCN_H 1

/* Define to 1 if you have the <dl.h> header file. */
/* #undef HAVE_DL_H */

/* Define if your system has Linux Directory Notification */
/* #undef HAVE_DNOTIFY */

/* Define if dns-sd is available */
/* #undef HAVE_DNSSD */

/* Define if you have OpenEXR */
/* #undef HAVE_EXR */

/* Define is posix_fadvise is supported */
#define HAVE_FADVISE 1

/* Define to 1 if you have the <float.h> header file. */
#define HAVE_FLOAT_H 1

/* Define to 1 if you have the `freeaddrinfo' function. */
#define HAVE_FREEADDRINFO 1

/* Define to 1 if you have the <fstab.h> header file. */
#define HAVE_FSTAB_H 1

/* Define if you have finite */
#define HAVE_FUNC_FINITE 1

/* Define if you have isinf */
#define HAVE_FUNC_ISINF 1

/* Define if you have isnan */
#define HAVE_FUNC_ISNAN 1

/* Define if you have _finite */
/* #undef HAVE_FUNC__FINITE */

/* Define to 1 if you have the `gai_strerror' function. */
#define HAVE_GAI_STRERROR 1

/* Define to 1 if you have the `getaddrinfo' function. */
#define HAVE_GETADDRINFO 1

/* Define to 1 if you have the `getcwd' function. */
#define HAVE_GETCWD 1

/* Define to 1 if you have the `getgroups' function. */
#define HAVE_GETGROUPS 1

/* Define to 1 if you have the `gethostbyname2' function. */
#define HAVE_GETHOSTBYNAME2 1

/* Define to 1 if you have the `gethostbyname2_r' function. */
#define HAVE_GETHOSTBYNAME2_R 1

/* Define to 1 if you have the `gethostbyname_r' function. */
#define HAVE_GETHOSTBYNAME_R 1

/* Define to 1 if you have the `getmntinfo' function. */
/* #undef HAVE_GETMNTINFO */

/* Define to 1 if you have the `getnameinfo' function. */
#define HAVE_GETNAMEINFO 1

/* Define to 1 if you have the `getpagesize' function. */
#define HAVE_GETPAGESIZE 1

/* Define to 1 if you have the `getpeereid' function. */
/* #undef HAVE_GETPEEREID */

/* Define to 1 if you have the `getpeername' function. */
#define HAVE_GETPEERNAME 1

/* Define to 1 if you have the `getprotobyname_r' function. */
#define HAVE_GETPROTOBYNAME_R 1

/* Define to 1 if you have the `getpt' function. */
#define HAVE_GETPT 1

/* Define to 1 if you have the `getservbyname_r' function. */
#define HAVE_GETSERVBYNAME_R 1

/* Define to 1 if you have the `getservbyport_r' function. */
#define HAVE_GETSERVBYPORT_R 1

/* Define to 1 if you have the `getsockname' function. */
#define HAVE_GETSOCKNAME 1

/* Define to 1 if you have the `getsockopt' function. */
#define HAVE_GETSOCKOPT 1

/* Define to 1 if you have the `gettimeofday' function. */
#define HAVE_GETTIMEOFDAY 1

/* Define to 1 if you have the `grantpt' function. */
#define HAVE_GRANTPT 1

/* Define to 1 if you have the <ieeefp.h> header file. */
/* #undef HAVE_IEEEFP_H */

/* Define to 1 if you have the `if_nametoindex' function. */
#define HAVE_IF_NAMETOINDEX 1

/* Define to 1 if you have the `index' function. */
/* #undef HAVE_INDEX */

/* Define to 1 if you have the `inet_ntop' function. */
#define HAVE_INET_NTOP 1

/* Define to 1 if you have the `inet_pton' function. */
#define HAVE_INET_PTON 1


/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define if you have jasper */
/* #undef HAVE_JASPER */

/* Define if your locale.h file contains LC_MESSAGES */
#define HAVE_LC_MESSAGES 1

/* Defines if your system has the libart library */
#define HAVE_LIBART 1

/* Define if you have the libdl library or equivalent. */
#define HAVE_LIBDL 1

/* Define if you have GSSAPI libraries */
#define HAVE_LIBGSSAPI 1

/* Defined if you have libidn in your system */
#define HAVE_LIBIDN 1

/* Define if you have libjpeg */
#define HAVE_LIBJPEG 1

/* Define to 1 if you have the `libQtGui_debug' library (-llibQtGui_debug). */
/* #undef HAVE_LIBLIBQTGUI_DEBUG */

/* Define if you have libpng */
#define HAVE_LIBPNG 1

/* Define if you have a working libpthread (will enable threaded code) */
#define HAVE_LIBPTHREAD 1

/* Defined if you have libthai and want to have it compiled in */
/* #undef HAVE_LIBTHAI */

/* Define if you have libtiff */
#define HAVE_LIBTIFF 1

/* Define to 1 if you have the <libutil.h> header file. */
/* #undef HAVE_LIBUTIL_H */

/* Define if you have libz */
#define HAVE_LIBZ 1

/* Define to 1 if you have the <limits.h> header file. */
#define HAVE_LIMITS_H 1

/* Define to 1 if you have the <locale.h> header file. */
#define HAVE_LOCALE_H 1

/* Define if you have LUA > 5.0 */
/* #undef HAVE_LUA */

/* Define to 1 if you have the <malloc.h> header file. */
#define HAVE_MALLOC_H 1

/* Define to 1 if you have the `memcpy' function. */
#define HAVE_MEMCPY 1

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define if you want MIT-SHM support */
#define HAVE_MITSHM 1

/* Define to 1 if you have a working `mmap' system call. */
#define HAVE_MMAP 1

/* Define to 1 if you have the <mntent.h> header file. */
#define HAVE_MNTENT_H 1

/* Define to 1 if you have the `munmap' function. */
#define HAVE_MUNMAP 1

/* Define to 1 if you have the <ndir.h> header file, and it defines `DIR'. */
/* #undef HAVE_NDIR_H */

/* Define to 1 if you have the <netinet/in.h> header file. */
#define HAVE_NETINET_IN_H 1

/* Define to 1 if you have the <net/if.h> header file. */
#define HAVE_NET_IF_H 1

/* Define to 1 if you have the <nl_types.h> header file. */
#define HAVE_NL_TYPES_H 1

/* Define if your system needs _NSGetEnviron to set up the environment */
/* #undef HAVE_NSGETENVIRON */

/* Define if you have OpenSSL < 0.9.6 */
/* #undef HAVE_OLD_SSL_API */

/* Define to 1 if you have the `openpty' function. */
#define HAVE_OPENPTY 1

/* Define to 1 if you have the <paths.h> header file. */
#define HAVE_PATHS_H 1

/* Define if you have pcreposix libraries and header files. */
#define HAVE_PCREPOSIX 1

/* Define to 1 if you have the `poll' function. */
#define HAVE_POLL 1

/* Define to 1 if the assembler supports AltiVec instructions. */
/* #undef HAVE_PPC_ALTIVEC */

/* Define if libtool can extract symbol lists from object files. */
#define HAVE_PRELOADED_SYMBOLS 1

/* Define to 1 if you have the `ptsname' function. */
#define HAVE_PTSNAME 1

/* Define to 1 if you have the <pty.h> header file. */
#define HAVE_PTY_H 1

/* Define to 1 if you have the <punycode.h> header file. */
#define HAVE_PUNYCODE_H 1

/* Define to 1 if you have the `putenv' function. */
#define HAVE_PUTENV 1

/* Define to 1 if you have the `readdir_r' function. */
#define HAVE_READDIR_R 1

/* Define if you have res_init */
#define HAVE_RES_INIT 1

/* Define if you have the res_init prototype */
#define HAVE_RES_INIT_PROTO 1

/* Define if revoke(tty) is present in unistd.h */
#define HAVE_REVOKE 1

/* Define to 1 if you have the `rindex' function. */
/* #undef HAVE_RINDEX */

/* Define if you want sendfile() support */
/* #undef HAVE_SENDFILE */

/* Define to 1 if you have the `setegid' function. */
#define HAVE_SETEGID 1

/* Define to 1 if you have the `seteuid' function. */
#define HAVE_SETEUID 1

/* Define to 1 if you have the `setfsent' function. */
#define HAVE_SETFSENT 1

/* Define to 1 if you have the `setgroups' function. */
#define HAVE_SETGROUPS 1

/* Define to 1 if you have the `setlocale' function. */
#define HAVE_SETLOCALE 1

/* Define to 1 if you have the `setmntent' function. */
#define HAVE_SETMNTENT 1

/* Define to 1 if you have the `setpriority' function. */
#define HAVE_SETPRIORITY 1

/* Define if you have a STL implementation by SGI */
#define HAVE_SGI_STL 1

/* Define if you have the shl_load function. */
/* #undef HAVE_SHL_LOAD */

/* if setgroups() takes short *as second arg */
/* #undef HAVE_SHORTSETGROUPS */

/* Define to 1 if you have the `snprintf' function. */
#define HAVE_SNPRINTF 1

/* Define to 1 if you have the `socket' function. */
#define HAVE_SOCKET 1

/* If we are going to use OpenSSL */
#define HAVE_SSL 1

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdio.h> header file. */
#define HAVE_STDIO_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the `stpcpy' function. */
#define HAVE_STPCPY 1

/* Define to 1 if you have the `strcasecmp' function. */
#define HAVE_STRCASECMP 1

/* Define to 1 if you have the `strchr' function. */
#define HAVE_STRCHR 1

/* Define to 1 if you have the `strcmp' function. */
#define HAVE_STRCMP 1

/* Define to 1 if you have the `strfmon' function. */
#define HAVE_STRFMON 1

/* Define to 1 if you have the <stringprep.h> header file. */
#define HAVE_STRINGPREP_H 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the `strrchr' function. */
#define HAVE_STRRCHR 1

/* Define to 1 if you have the `strtoll' function. */
#define HAVE_STRTOLL 1

/* Define to 1 if the system has the type `struct addrinfo'. */
#define HAVE_STRUCT_ADDRINFO 1

/* Define to 1 if the system has the type `struct sockaddr_in6'. */
#define HAVE_STRUCT_SOCKADDR_IN6 1

/* Define to 1 if `sin6_scope_id' is member of `struct sockaddr_in6'. */
#define HAVE_STRUCT_SOCKADDR_IN6_SIN6_SCOPE_ID 1

/* Define to 1 if `sa_len' is member of `struct sockaddr'. */
/* #undef HAVE_STRUCT_SOCKADDR_SA_LEN */

/* Define if struct ucred is present from sys/socket.h */
#define HAVE_STRUCT_UCRED 1

/* Define to 1 if you have the <sysent.h> header file. */
/* #undef HAVE_SYSENT_H */

/* Define to 1 if you have the <sys/bitypes.h> header file. */
#define HAVE_SYS_BITYPES_H 1

/* Define to 1 if you have the <sys/dir.h> header file, and it defines `DIR'.
   */
/* #undef HAVE_SYS_DIR_H */

/* Define to 1 if you have the <sys/filio.h> header file. */
/* #undef HAVE_SYS_FILIO_H */

/* Define to 1 if you have the <sys/mman.h> header file. */
#define HAVE_SYS_MMAN_H 1

/* Define to 1 if you have the <sys/mntent.h> header file. */
/* #undef HAVE_SYS_MNTENT_H */

/* Define to 1 if you have the <sys/mnttab.h> header file. */
/* #undef HAVE_SYS_MNTTAB_H */

/* Define to 1 if you have the <sys/mount.h> header file. */
#define HAVE_SYS_MOUNT_H 1

/* Define to 1 if you have the <sys/ndir.h> header file, and it defines `DIR'.
   */
/* #undef HAVE_SYS_NDIR_H */

/* Define to 1 if you have the <sys/param.h> header file. */
#define HAVE_SYS_PARAM_H 1

/* Define to 1 if you have the <sys/select.h> header file. */
#define HAVE_SYS_SELECT_H 1

/* Define to 1 if you have the <sys/soundcard.h> header file. */
#define HAVE_SYS_SOUNDCARD_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/stropts.h> header file. */
#define HAVE_SYS_STROPTS_H 1

/* Define to 1 if you have the <sys/time.h> header file. */
#define HAVE_SYS_TIME_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <sys/ucred.h> header file. */
/* #undef HAVE_SYS_UCRED_H */

/* Define if sys/stat.h declares S_ISSOCK. */
#define HAVE_S_ISSOCK 1

/* Define to 1 if you have the <termios.h> header file. */
#define HAVE_TERMIOS_H 1

/* Define to 1 if you have the <termio.h> header file. */
#define HAVE_TERMIO_H 1

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Define to 1 if you have the `unlockpt' function. */
#define HAVE_UNLOCKPT 1

/* Define if you have the utempter helper for utmp managment */
#define HAVE_UTEMPTER 1

/* Define to 1 if you have the <util.h> header file. */
/* #undef HAVE_UTIL_H */

/* Define to 1 if you have the <values.h> header file. */
#define HAVE_VALUES_H 1

/* Define, to enable volume management (Solaris 2.x), if you have -lvolmgt */
/* #undef HAVE_VOLMGT */

/* Define to 1 if you have the `vsnprintf' function. */
#define HAVE_VSNPRINTF 1

/* Define to 1 if you have the <X11/extensions/shape.h> header file. */
#define HAVE_X11_EXTENSIONS_SHAPE_H 1

/* Define to 1 if you have the <X11/extensions/XShm.h> header file. */
#define HAVE_X11_EXTENSIONS_XSHM_H 1

/* Define to 1 if you have the <X11/ICE/ICElib.h> header file. */
#define HAVE_X11_ICE_ICELIB_H 1

/* Define to 1 if the assembler supports 3DNOW instructions. */
#define HAVE_X86_3DNOW 1

/* Define to 1 if the assembler supports MMX instructions. */
#define HAVE_X86_MMX 1

/* Define to 1 if the assembler supports SSE instructions. */
#define HAVE_X86_SSE 1

/* Define to 1 if the assembler supports SSE2 instructions. */
#define HAVE_X86_SSE2 1

/* Defined if your system has XRender support */
#define HAVE_XRENDER 1

/* Define to 1 if you have the `_getpty' function. */
/* #undef HAVE__GETPTY */

/* Define to 1 if you have the `_IceTransNoListen' function. */
#define HAVE__ICETRANSNOLISTEN 1

/* Define to 1 if you have the `__argz_count' function. */
#define HAVE___ARGZ_COUNT 1

/* Define to 1 if you have the `__argz_next' function. */
#define HAVE___ARGZ_NEXT 1

/* Define to 1 if you have the `__argz_stringify' function. */
#define HAVE___ARGZ_STRINGIFY 1

/* The prefix to use as fallback */
#define KDEDIR "/usr"

/* Use FontConfig in kdeinit */
#define KDEINIT_USE_FONTCONFIG 1

/* Use Xft preinitialization in kdeinit */
/* #undef KDEINIT_USE_XFT */

/* The compiled in system configuration prefix */
#define KDESYSCONFDIR "/usr/etc"

/* what OS used for compilation */
#define KDE_COMPILING_OS "Linux 2.6.12-1.1398_FC4 i686"

/* Distribution Text to append to OS */
#define KDE_DISTRIBUTION_TEXT "compiled sources"

/* Use own malloc implementation */
/* #undef KDE_MALLOC */

/* Enable debugging in fast malloc */
/* #undef KDE_MALLOC_DEBUG */

/* Make alloc as fast as possible */
/* #undef KDE_MALLOC_FULL */

/* The libc used is glibc */
#define KDE_MALLOC_GLIBC 1

/* The platform is x86 */
#define KDE_MALLOC_X86 1

/* Define if we shall use KSSL */
#define KSSL_HAVE_SSL 1

/* Define if the OS needs help to load dependent libraries for dlopen(). */
/* #undef LTDL_DLOPEN_DEPLIBS */

/* Define to the sub-directory in which libtool stores uninstalled libraries.
   */
#define LTDL_OBJDIR ".libs/"

/* Define to the name of the environment variable that determines the dynamic
   library search path. */
#define LTDL_SHLIBPATH_VAR "LD_LIBRARY_PATH"

/* Define to the extension used for shared libraries, say, ".so". */
/* #undef LTDL_SHLIB_EXT */

/* Define to the system default library search path. */
#define LTDL_SYSSEARCHPATH "/lib:/usr/lib"

/* Define the file for mount entries */
#define MTAB_FILE "/etc/mtab"

/* Define if the libbz2 functions need the BZ2_ prefix */
#define NEED_BZ2_PREFIX 1

/* Define if dlsym() requires a leading underscode in symbol names. */
/* #undef NEED_USCORE */

/* Name of package */
#define PACKAGE "kdelibs"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT ""

/* Define to the full name of this package. */
#define PACKAGE_NAME ""

/* Define to the full name and version of this package. */
#define PACKAGE_STRING ""

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME ""

/* Define to the version of this package. */
#define PACKAGE_VERSION ""

/* Define if you have POSIX.1b scheduling */
#define POSIX1B_SCHEDULING 1

/* The size of a `char *', as computed by sizeof. */
#define SIZEOF_CHAR_P 4

/* The size of a `int', as computed by sizeof. */
#define SIZEOF_INT 4

/* The size of a `long', as computed by sizeof. */
#define SIZEOF_LONG 4

/* The size of a `short', as computed by sizeof. */
#define SIZEOF_SHORT 2

/* The size of a `size_t', as computed by sizeof. */
#define SIZEOF_SIZE_T 4

/* The size of a `unsigned long', as computed by sizeof. */
#define SIZEOF_UNSIGNED_LONG 4

/* If using the C implementation of alloca, define if you know the
   direction of stack growth for your system; otherwise it will be
   automatically deduced at run-time.
	STACK_DIRECTION > 0 => grows toward higher addresses
	STACK_DIRECTION < 0 => grows toward lower addresses
	STACK_DIRECTION = 0 => direction of growth unknown */
/* #undef STACK_DIRECTION */

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Define to 1 if you can safely include both <sys/time.h> and <time.h>. */
#define TIME_WITH_SYS_TIME 1

/* Version number of package */
#define VERSION "3.4.89"

/* Defined if compiling without arts */
#define WITHOUT_ARTS 1

/* Define to 1 if your processor stores words with the most significant byte
   first (like Motorola and SPARC, unlike Intel and VAX). */
/* #undef WORDS_BIGENDIAN */

/* where rgb.txt is in */
#define X11_RGBFILE "/usr/X11R6/lib/X11/rgb.txt"

/* Defines the executable of xmllint */
#define XMLLINT "/usr/bin/xmllint"

/*
 * jpeg.h needs HAVE_BOOLEAN, when the system uses boolean in system
 * headers and I'm too lazy to write a configure test as long as only
 * unixware is related
 */
#ifdef _UNIXWARE
#define HAVE_BOOLEAN
#endif



/*
 * AIX defines FD_SET in terms of bzero, but fails to include <strings.h>
 * that defines bzero.
 */

#if defined(_AIX)
#include <strings.h>
#endif



#if defined(HAVE_NSGETENVIRON) && defined(HAVE_CRT_EXTERNS_H)
# include <sys/time.h>
# include <crt_externs.h>
# define environ (*_NSGetEnviron())
#endif



#ifdef __osf__
#ifdef __cplusplus
extern "C" {
#endif
#include <sys/mount.h>
int getmntinfo(struct statfs **mntbufp, int flags);
#include <sys/fs_types.h>    /* for mnt_names[] */
#ifdef __cplusplus
}
#endif
#endif


/* Define if you need to use the GNU extensions */
#define _GNU_SOURCE 1


#if !defined(HAVE_RES_INIT_PROTO)
#ifdef __cplusplus
extern "C" {
#endif
int res_init(void);
#ifdef __cplusplus
}
#endif
#endif


#ifndef HAVE_S_ISSOCK
#define HAVE_S_ISSOCK
#define S_ISSOCK(mode) (1==0)
#endif


/*
 * Steven Schultz <sms at to.gd-es.com> tells us :
 * BSD/OS 4.2 doesn't have a prototype for openpty in its system header files
 */
#ifdef __bsdi__
__BEGIN_DECLS
int openpty(int *, int *, char *, struct termios *, struct winsize *);
__END_DECLS
#endif



/*
 * On HP-UX, the declaration of vsnprintf() is needed every time !
 */

#if !defined(HAVE_VSNPRINTF) || defined(hpux)
#if __STDC__
#include <stdarg.h>
#include <stdlib.h>
#else
#include <varargs.h>
#endif
#ifdef __cplusplus
extern "C"
#endif
int vsnprintf(char *str, size_t n, char const *fmt, va_list ap);
#ifdef __cplusplus
extern "C"
#endif
int snprintf(char *str, size_t n, char const *fmt, ...);
#endif


/* KDE bindir */
#define __KDE_BINDIR "/usr/bin"

/* execprefix or NONE if not set, for libloading */
#define __KDE_EXECPREFIX "NONE"

/* define to 1 if -fvisibility is supported */
#define __KDE_HAVE_GCC_VISIBILITY "1"

/* path to su */
#define __PATH_SU "/bin/su"


#if defined(__SVR4) && !defined(__svr4__)
#define __svr4__ 1
#endif


/* type to use in place of socklen_t if not defined */
#define kde_socklen_t socklen_t

/* type to use in place of socklen_t if not defined (deprecated, use
   kde_socklen_t) */
#define ksize_t socklen_t

/* Define to `long' if <sys/types.h> does not define. */
/* #undef off_t */

/* Define to `unsigned' if <sys/types.h> does not define. */
/* #undef size_t */


/* provide a definition for a 32 bit entity, usable as a typedef, possibly
   extended by "unsigned" */
/* #undef INT32_BASETYPE */
#ifdef SIZEOF_INT
#if SIZEOF_INT == 4
#define INT32_BASETYPE int
#endif
#endif
#if !defined(INT32_BASETYPE) && defined(SIZEOF_LONG)
#if SIZEOF_LONG == 4
#define INT32_BASETYPE long
#endif
#endif
#ifndef INT32_BASETYPE
#define INT32_BASETYPE int
#endif

#ifndef HAVE_SETEUID
#define HAVE_SETEUID
#define HAVE_SETEUID_FAKE
#ifdef __cplusplus
extern "C"
#endif
int seteuid(INT32_BASETYPE euid); /* defined in fakes.c */
#endif
"""

	dest.write(content)
