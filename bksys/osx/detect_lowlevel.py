# Thomas Nagy 2005

def detect(lenv,dest):
	import os

	content="""
/* Define where your dcop binary is */
#define DCOP_PATH "/sw/bin"

/* Define if getaddrinfo returns AF_UNIX sockets */
/* #undef GETADDRINFO_RETURNS_UNIX */

/* Define if you have the MIT Kerberos libraries */
/* #undef GSSAPI_MIT */

/* Define to 1 if you have `alloca', as a function or macro. */
#define HAVE_ALLOCA 1

/* Define to 1 if you have the <alloca.h> header file. */
#define HAVE_ALLOCA_H 1

/* Define to 1 if you have the <alsa/asoundlib.h> header file. */
/* #undef HAVE_ALSA_ASOUNDLIB_H */

/* Define to 1 if you have the <arpa/nameser8_compat.h> header file. */
#define HAVE_ARPA_NAMESER8_COMPAT_H 1

/* Define to 1 if you have the <awe_voice.h> header file. */
/* #undef HAVE_AWE_VOICE_H */

/* Define if execinfo.h exists and defines backtrace (GLIBC >= 2.1) */
/* #undef HAVE_BACKTRACE */

/* Define to 1 if you have the `bcopy' function. */
/* #undef HAVE_BCOPY */

/* You _must_ have bool */
#define HAVE_BOOL 1

/* Define if getaddrinfo is broken and should be replaced */
/* #undef HAVE_BROKEN_GETADDRINFO */

/* Defines if bzip2 is compiled */
#define HAVE_BZIP2_SUPPORT 1

/* Define to 1 if you have the <Carbon/Carbon.h> header file. */
#define HAVE_CARBON_CARBON_H 1

/* Define if you have the CoreAudio API */
#define HAVE_COREAUDIO 1

/* Define to 1 if you have the <crt_externs.h> header file. */
#define HAVE_CRT_EXTERNS_H 1

/* Defines if your system has the crypt function */
#define HAVE_CRYPT 1

/* Define to 1 if you have the <ctype.h> header file. */
#define HAVE_CTYPE_H 1

/* Defines if you have CUPS (Common UNIX Printing System) */
#define HAVE_CUPS 1

/* CUPS doesn't have password caching */
/* #undef HAVE_CUPS_NO_PWD_CACHE */

/* Define to 1 if you have the declaration of `getservbyname_r', and to 0 if
   you don't. */
/* #undef HAVE_DECL_GETSERVBYNAME_R */

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
#define HAVE_DNSSD 1

/* Define if you have OpenEXR */
#define HAVE_EXR 1

/* Define if your system has libfam */
/* #undef HAVE_FAM */

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
/* #undef HAVE_GETHOSTBYNAME2_R */

/* Define to 1 if you have the `gethostbyname_r' function. */
/* #undef HAVE_GETHOSTBYNAME_R */

/* Define to 1 if you have the `getmntinfo' function. */
#define HAVE_GETMNTINFO 1

/* Define to 1 if you have the `getnameinfo' function. */
#define HAVE_GETNAMEINFO 1

/* Define to 1 if you have the `getpagesize' function. */
#define HAVE_GETPAGESIZE 1

/* Define to 1 if you have the `getpeereid' function. */
#define HAVE_GETPEEREID 1

/* Define to 1 if you have the `getpeername' function. */
#define HAVE_GETPEERNAME 1

/* Define to 1 if you have the `getprotobyname_r' function. */
/* #undef HAVE_GETPROTOBYNAME_R */

/* Define to 1 if you have the `getpt' function. */
/* #undef HAVE_GETPT */

/* Define to 1 if you have the `getservbyname_r' function. */
/* #undef HAVE_GETSERVBYNAME_R */

/* Define to 1 if you have the `getservbyport_r' function. */
/* #undef HAVE_GETSERVBYPORT_R */

/* Define to 1 if you have the `getsockname' function. */
#define HAVE_GETSOCKNAME 1

/* Define to 1 if you have the `getsockopt' function. */
#define HAVE_GETSOCKOPT 1

/* Define to 1 if you have the `gettimeofday' function. */
#define HAVE_GETTIMEOFDAY 1

/* Define to 1 if you have the `grantpt' function. */
#define HAVE_GRANTPT 1

/* Define to 1 if you have the <idna.h> header file. */
#define HAVE_IDNA_H 1

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

/* Define if you have libasound.so.1 (required for ALSA 0.5.x support) */
/* #undef HAVE_LIBASOUND */

/* Define if you have libasound.so.2 (required for ALSA 0.9.x support) */
/* #undef HAVE_LIBASOUND2 */

/* Define if you have the libdl library or equivalent. */
#define HAVE_LIBDL 1

/* Define if you have GSSAPI libraries */
#define HAVE_LIBGSSAPI 1

/* Defined if you have libidn in your system */
#define HAVE_LIBIDN 1

/* Define if you have libjpeg */
#define HAVE_LIBJPEG 1

/* Define if you have libpng */
#define HAVE_LIBPNG 1

/* Define if you have a working libpthread (will enable threaded code) */
#define HAVE_LIBPTHREAD 1

/* Define to 1 if you have the `qt-mt' library (-lqt-mt). */
/* #undef HAVE_LIBQT_MT */

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

/* Define to 1 if you have the <linux/awe_voice.h> header file. */
/* #undef HAVE_LINUX_AWE_VOICE_H */

/* Define to 1 if you have the <locale.h> header file. */
#define HAVE_LOCALE_H 1

/* Define to 1 if you have the <machine/soundcard.h> header file. */
/* #undef HAVE_MACHINE_SOUNDCARD_H */

/* Define to 1 if you have the <malloc.h> header file. */
/* #undef HAVE_MALLOC_H */

/* Define to 1 if you have the `memcpy' function. */
#define HAVE_MEMCPY 1

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define if you want MIT-SHM support */
#define HAVE_MITSHM 1

/* Define to 1 if you have a working `mmap' system call. */
#define HAVE_MMAP 1

/* Define to 1 if you have the <mntent.h> header file. */
/* #undef HAVE_MNTENT_H */

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
#define HAVE_NSGETENVIRON 1

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
/* #undef HAVE_PTSNAME */

/* Define to 1 if you have the <pty.h> header file. */
/* #undef HAVE_PTY_H */

/* Define to 1 if you have the <punycode.h> header file. */
#define HAVE_PUNYCODE_H 1

/* Define to 1 if you have the `putenv' function. */
#define HAVE_PUTENV 1

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
/* #undef HAVE_SETMNTENT */

/* Define to 1 if you have the `setpriority' function. */
#define HAVE_SETPRIORITY 1

/* Define if you have a STL implementation by SGI */
#define HAVE_SGI_STL 1

/* Define if you have the shl_load function. */
/* #undef HAVE_SHL_LOAD */

/* if setgroups() takes short *as second arg */
/* #undef HAVE_SHORTSETGROUPS */

/* Define if libasound has snd_pcm_resume() */
/* #undef HAVE_SND_PCM_RESUME */

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
#define HAVE_STRUCT_SOCKADDR_SA_LEN 1

/* Define if struct ucred is present from sys/socket.h */
/* #undef HAVE_STRUCT_UCRED */

/* Define to 1 if you have the <sysent.h> header file. */
/* #undef HAVE_SYSENT_H */

/* Define to 1 if you have the <sys/asoundlib.h> header file. */
/* #undef HAVE_SYS_ASOUNDLIB_H */

/* Define to 1 if you have the <sys/bitypes.h> header file. */
/* #undef HAVE_SYS_BITYPES_H */

/* Define to 1 if you have the <sys/dir.h> header file, and it defines `DIR'.
   */
/* #undef HAVE_SYS_DIR_H */

/* Define to 1 if you have the <sys/filio.h> header file. */
#define HAVE_SYS_FILIO_H 1

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
/* #undef HAVE_SYS_SOUNDCARD_H */

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/stropts.h> header file. */
/* #undef HAVE_SYS_STROPTS_H */

/* Define to 1 if you have the <sys/time.h> header file. */
#define HAVE_SYS_TIME_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <sys/ucred.h> header file. */
#define HAVE_SYS_UCRED_H 1

/* Define if sys/stat.h declares S_ISSOCK. */
#define HAVE_S_ISSOCK 1

/* Define to 1 if you have the <termios.h> header file. */
#define HAVE_TERMIOS_H 1

/* Define to 1 if you have the <termio.h> header file. */
/* #undef HAVE_TERMIO_H */

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Define to 1 if you have the `unlockpt' function. */
/* #undef HAVE_UNLOCKPT */

/* Define if you have the utempter helper for utmp managment */
/* #undef HAVE_UTEMPTER */

/* Define to 1 if you have the <util.h> header file. */
#define HAVE_UTIL_H 1

/* Define to 1 if you have the <values.h> header file. */
/* #undef HAVE_VALUES_H */

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
/* #undef HAVE_X86_3DNOW */

/* Define to 1 if the assembler supports MMX instructions. */
/* #undef HAVE_X86_MMX */

/* Define to 1 if the assembler supports SSE instructions. */
/* #undef HAVE_X86_SSE */

/* Define to 1 if the assembler supports SSE2 instructions. */
/* #undef HAVE_X86_SSE2 */

/* Defined if your system has XRender support */
#define HAVE_XRENDER 1

/* Define to 1 if you have the `_getpty' function. */
/* #undef HAVE__GETPTY */

/* Define to 1 if you have the `_IceTransNoListen' function. */
#define HAVE__ICETRANSNOLISTEN 1

/* Define to 1 if you have the </usr/src/sys/gnu/i386/isa/sound/awe_voice.h>
   header file. */
/* #undef HAVE__USR_SRC_SYS_GNU_I386_ISA_SOUND_AWE_VOICE_H */

/* Define to 1 if you have the </usr/src/sys/i386/isa/sound/awe_voice.h>
   header file. */
/* #undef HAVE__USR_SRC_SYS_I386_ISA_SOUND_AWE_VOICE_H */

/* Define to 1 if you have the `__argz_count' function. */
/* #undef HAVE___ARGZ_COUNT */

/* Define to 1 if you have the `__argz_next' function. */
/* #undef HAVE___ARGZ_NEXT */

/* Define to 1 if you have the `__argz_stringify' function. */
/* #undef HAVE___ARGZ_STRINGIFY */

/* The prefix to use as fallback */
#define KDEDIR "/sw"

/* Use FontConfig in kdeinit */
#define KDEINIT_USE_FONTCONFIG 1

/* Use Xft preinitialization in kdeinit */
/* #undef KDEINIT_USE_XFT */

/* The compiled in system configuration prefix */
#define KDESYSCONFDIR "/sw/etc"

/* what OS used for compilation */
#define KDE_COMPILING_OS "Darwin 8.3.0 Power Macintosh"

/* Distribution Text to append to OS */
#define KDE_DISTRIBUTION_TEXT "Fink/Mac OS X"

/* Use own malloc implementation */
/* #undef KDE_MALLOC */

/* Enable debugging in fast malloc */
/* #undef KDE_MALLOC_DEBUG */

/* Make alloc as fast as possible */
/* #undef KDE_MALLOC_FULL */

/* The libc used is glibc */
/* #undef KDE_MALLOC_GLIBC */

/* The platform is x86 */
/* #undef KDE_MALLOC_X86 */

/* Define if we shall use KSSL */
#define KSSL_HAVE_SSL 1

/* Define if the OS needs help to load dependent libraries for dlopen(). */
/* #undef LTDL_DLOPEN_DEPLIBS */

/* Define to the sub-directory in which libtool stores uninstalled libraries.
   */
#define LTDL_OBJDIR ".libs/"

/* Define to the name of the environment variable that determines the dynamic
   library search path. */
#define LTDL_SHLIBPATH_VAR "DYLD_LIBRARY_PATH"

/* Define to the extension used for shared libraries, say, ".so". */
/* #undef LTDL_SHLIB_EXT */

/* Define to the system default library search path. */
#define LTDL_SYSSEARCHPATH "/usr/local/lib:/lib:/usr/lib"

/* Define the file for mount entries */
/* #undef MTAB_FILE */

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
/* #undef POSIX1B_SCHEDULING */

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
#define VERSION "3.4.2"

/* Defined if compiling without arts */
/* #undef WITHOUT_ARTS */

/* Define to 1 if your processor stores words with the most significant byte
   first (like Motorola and SPARC, unlike Intel and VAX). */
#define WORDS_BIGENDIAN 1

/* where rgb.txt is in */
#define X11_RGBFILE "/usr/X11R6/lib/X11/rgb.txt"

/* Defines the executable of xmllint */
#define XMLLINT "/sw/bin/xmllint"

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
/* #undef _GNU_SOURCE */


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
#define __KDE_BINDIR "/sw/bin"

/* execprefix or NONE if not set, for libloading */
#define __KDE_EXECPREFIX "NONE"

/* define to 1 if -fvisibility is supported */
/* #undef __KDE_HAVE_GCC_VISIBILITY */

/* path to su */
#define __PATH_SU "/sw/bin/su"


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
	## commented out for now; the real fix is not the above, but to write real tests in lowlevel.py
	### dest.write(content)
