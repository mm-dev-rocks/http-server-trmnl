#ifndef PLATFORM_H
#define PLATFORM_H

// OS
#if defined(__linux__)
#define PLATFORM_OS "linux"
#elif defined(__APPLE__) && defined(__MACH__)
#define PLATFORM_OS "macos"
#elif defined(_WIN32) || defined(_WIN64)
#define PLATFORM_OS "windows"
#elif defined(__FreeBSD__)
#define PLATFORM_OS "freebsd"
#else
#define PLATFORM_OS "unknown-os"
#endif

// Architecture
#if defined(__x86_64__) || defined(_M_X64)
#define PLATFORM_ARCH "x86_64"
#elif defined(__i386__) || defined(_M_IX86)
#define PLATFORM_ARCH "x86"
#elif defined(__aarch64__) || defined(_M_ARM64)
#define PLATFORM_ARCH "arm64"
#elif defined(__arm__) || defined(_M_ARM)
#define PLATFORM_ARCH "arm32"
#else
#define PLATFORM_ARCH "unknown-arch"
#endif

// Libc
#if defined(__GLIBC__)
#define PLATFORM_LIBC "glibc"
#elif defined(__BIONIC__)
#define PLATFORM_LIBC "bionic"
#elif defined(__APPLE__)
#define PLATFORM_LIBC "libSystem"
#elif defined(__FreeBSD__)
#define PLATFORM_LIBC "bsd-libc"
#elif defined(_WIN32)
#define PLATFORM_LIBC "msvcrt"
#else
#define PLATFORM_LIBC "unknown-libc"
#endif

// C standard
#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
#define PLATFORM_CSTD "c99"
#elif defined(__STDC__)
#define PLATFORM_CSTD "c89"
#else
#define PLATFORM_CSTD "unknown-std"
#endif

#define FILENAME OS "-" ARCH "-" LIBC "-" CSTD ".txt"

#endif // PLATFORM_H
