#include <string.h>
#include <errno.h>
#include <jni.h>

#include "org_mingyu_posix_Errno.h"

JNIEXPORT jint JNICALL Java_posix_Errno_getErrno
  (JNIEnv *, jclass) {
    return errno;
  }

JNIEXPORT jstring JNICALL Java_posix_Errno_strerror
  (JNIEnv *env, jclass, jint err) {
    char msg[512];
#if POSIX_STRERROR_R
    if (strerror_r(err,msg,sizeof msg) == 0)
      return env->NewStringUTF(msg);
#else
    // old glibc form of strerror_r
    char *s = strerror_r(err,msg,sizeof msg);
    if (s)
      return env->NewStringUTF(s);
#endif
    return NULL;
  }

JNIEXPORT jint JNICALL Java_posix_Errno_errno
  (JNIEnv *, jclass, jint eidx) {
    static int map[] = {
      EPERM, ENOENT, ESRCH, EINTR, EIO, EIDRM,
      ENOMSG, EFAULT, EINVAL, EACCES, E2BIG, EAGAIN, ENOMEM
    };
    if (eidx < 0 || eidx >= sizeof map / sizeof map[0])
      return -1;
    return map[eidx];
  }
