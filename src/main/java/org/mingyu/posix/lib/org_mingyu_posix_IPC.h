/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class org_mingyu_posix_IPC */

#ifndef _Included_org_mingyu_posix_IPC
#define _Included_org_mingyu_posix_IPC
#ifdef __cplusplus
extern "C" {
#endif
#undef org_mingyu_posix_IPC_IPC_RMID
#define org_mingyu_posix_IPC_IPC_RMID 0L
#undef org_mingyu_posix_IPC_IPC_SET
#define org_mingyu_posix_IPC_IPC_SET 1L
#undef org_mingyu_posix_IPC_IPC_STAT
#define org_mingyu_posix_IPC_IPC_STAT 2L
#undef org_mingyu_posix_IPC_IPC_PRIVATE
#define org_mingyu_posix_IPC_IPC_PRIVATE 0L
#undef org_mingyu_posix_IPC_IPC_ALLOC
#define org_mingyu_posix_IPC_IPC_ALLOC 32768L
#undef org_mingyu_posix_IPC_IPC_CREAT
#define org_mingyu_posix_IPC_IPC_CREAT 512L
#undef org_mingyu_posix_IPC_IPC_EXCL
#define org_mingyu_posix_IPC_IPC_EXCL 1024L
#undef org_mingyu_posix_IPC_IPC_NOWAIT
#define org_mingyu_posix_IPC_IPC_NOWAIT 2048L
#undef org_mingyu_posix_IPC_IPC_CI
#define org_mingyu_posix_IPC_IPC_CI 4096L
#undef org_mingyu_posix_IPC_IPC_NOCLEAR
#define org_mingyu_posix_IPC_IPC_NOCLEAR 8192L
#undef org_mingyu_posix_IPC_IPC_PHYS
#define org_mingyu_posix_IPC_IPC_PHYS 16384L
/*
 * Class:     org_mingyu_posix_IPC
 * Method:    ftok
 * Signature: (Ljava/lang/String;I)I
 */
JNIEXPORT jint JNICALL Java_org_mingyu_posix_IPC_ftok
  (JNIEnv *, jclass, jstring, jint);

/*
 * Class:     org_mingyu_posix_IPC
 * Method:    init
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_org_mingyu_posix_IPC_init
  (JNIEnv *, jclass);

/*
 * Class:     org_mingyu_posix_IPC
 * Method:    geteuid
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_org_mingyu_posix_IPC_geteuid
  (JNIEnv *, jclass);

/*
 * Class:     org_mingyu_posix_IPC
 * Method:    getegid
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_org_mingyu_posix_IPC_getegid
  (JNIEnv *, jclass);

#ifdef __cplusplus
}
#endif
#endif
