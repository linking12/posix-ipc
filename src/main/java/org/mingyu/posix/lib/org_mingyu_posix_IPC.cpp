#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>
#include <errno.h>
#include <jni.h>
#include "org_mingyu_posix_IPC.h"
#include "org_mingyu_posix_SemSet.h"
#include "org_mingyu_posix_SharedMem.h"

class jIPC_perm {
  jfieldID uid,gid,cuid,cgid,mode; //,seq,key;
public:
  void init(JNIEnv *env) {
    jclass c = env->FindClass("org/mingyu/posix/IPC$Perm");
    uid = env->GetFieldID(c,"uid","I");
    gid = env->GetFieldID(c,"gid","I");
    cuid = env->GetFieldID(c,"cuid","I");
    cgid = env->GetFieldID(c,"cgid","I");
    mode = env->GetFieldID(c,"mode","I");
  }
  /** Get ipc perm data from java space */
  void getperm(JNIEnv *env,ipc_perm *p,jobject msg_perm) {
    p->uid = env->GetIntField(msg_perm,uid);
    p->gid = env->GetIntField(msg_perm,gid);
    p->cuid = env->GetIntField(msg_perm,cuid);
    p->cgid = env->GetIntField(msg_perm,cgid);
    p->mode = env->GetIntField(msg_perm,mode);
  }
  /** Set Java ipc perm data */
  void setperm(JNIEnv *env,const ipc_perm *p,jobject msg_perm) {
    env->SetIntField(msg_perm,uid,p->uid);
    env->SetIntField(msg_perm,gid,p->gid);
    env->SetIntField(msg_perm,cuid,p->cuid);
    env->SetIntField(msg_perm,cgid,p->cgid);
    env->SetIntField(msg_perm,mode,p->mode);
  }
};

static jIPC_perm perm;


JNIEXPORT jint JNICALL Java_org_mingyu_posix_IPC_ftok
  (JNIEnv *env, jclass, jstring s, jint id){
    if (s == 0) return id;
    const char *path = env->GetStringUTFChars(s,0);
    long key = ftok(path,id);
    env->ReleaseStringUTFChars(s,path);
    return key;
}

/*
 * Class:     org_mingyu_posix_IPC
 * Method:    init
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_org_mingyu_posix_IPC_init
  (JNIEnv *env, jclass){
    perm.init(env);
    return getpid();
}

/*
 * Class:     org_mingyu_posix_IPC
 * Method:    geteuid
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_org_mingyu_posix_IPC_geteuid
  (JNIEnv *, jclass){
  return geteuid();
}

/*
 * Class:     org_mingyu_posix_IPC
 * Method:    getegid
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_org_mingyu_posix_IPC_getegid
  (JNIEnv *, jclass){
   return getegid();
 }
 
 
 /**SemSet API****/
 
class jsemid_ds {
  jfieldID otime,ctime,nsems;
public:
  void init(JNIEnv *env) {
    jclass c = env->FindClass("org/mingyu/posix/SemSet$semid_ds");
    otime = env->GetFieldID(c,"sem_otime","J");
    ctime = env->GetFieldID(c,"sem_ctime","J");
    nsems = env->GetFieldID(c,"sem_nsems","I");
  }
  void getsemid(JNIEnv *env,semid_ds *mbuf,jobject obj) {
    ::perm.getperm(env,&mbuf->sem_perm,obj);
  }
  void setsemid(JNIEnv *env,const semid_ds *mbuf,jobject obj) {
    ::perm.setperm(env,&mbuf->sem_perm,obj);
    env->SetLongField(obj,otime,(jlong)mbuf->sem_otime * 1000);
    env->SetLongField(obj,ctime,(jlong)mbuf->sem_ctime * 1000);
    env->SetIntField(obj,nsems,mbuf->sem_nsems);
  }
};

static jsemid_ds semds;
 
 JNIEXPORT jint JNICALL Java_org_mingyu_posix_SemSet_semget
  (JNIEnv *env, jclass, jint key, jint size, jint flag){
  int rc = semget(key,size,flag);
  return (rc < 0) ? (errno | 0x80000000) : rc;
 }

/*
 * Class:     org_mingyu_posix_SemSet
 * Method:    semctlb
 * Signature: (IIILorg/mingyu/posix/SemSet/semid_ds;)I
 */
JNIEXPORT jint JNICALL Java_org_mingyu_posix_SemSet_semctlb
  (JNIEnv *env, jclass, jint id, jint sem, jint cmd, jobject obj){
  semid_ds buf;
  int rc = -1;
  if (obj != 0) {
    if (cmd == org_mingyu_posix_SemSet_IPC_SET) {
      semds.getsemid(env,&buf,obj);
      rc = semctl(id,sem,IPC_SET,&buf);
      if (rc < 0) rc = errno | 0x80000000;
    }
    else if (cmd == org_mingyu_posix_SemSet_IPC_STAT) {
      rc = semctl(id,sem,IPC_STAT,&buf);
      if (rc >= 0)
	     semds.setsemid(env,&buf,obj);
      else rc = errno | 0x80000000;
    }
  }
  else if (cmd == org_mingyu_posix_SemSet_IPC_RMID)
    rc = semctl(id,IPC_RMID,0);
    if (rc < 0) rc = errno | 0x80000000;
  return rc;  
}

/*
 * Class:     org_mingyu_posix_SemSet
 * Method:    semctl
 * Signature: (III[S)I
 */
JNIEXPORT jint JNICALL Java_org_mingyu_posix_SemSet_semctl__III_3S
  (JNIEnv *env, jclass, jint id, jint sem, jint cmd, jshortArray v){
  int n = env->GetArrayLength(v);
  short *sa = (short *)alloca(n * sizeof *sa);
  int rc;
  switch (cmd) {
  case org_mingyu_posix_SemSet_GETALL:
    rc = semctl(id,sem,GETALL,sa);
    if (rc == 0)
      env->SetShortArrayRegion(v,0,n,sa);
    break;
  case org_mingyu_posix_SemSet_SETALL:
    env->GetShortArrayRegion(v,0,n,sa);
    rc = semctl(id,sem,SETALL,sa);
    break;
  default:
    return -1;
  }
  return (rc < 0) ? (errno | 0x80000000) : rc; 
 }

/*
 * Class:     org_mingyu_posix_SemSet
 * Method:    semctl
 * Signature: (IIII)I
 */
JNIEXPORT jint JNICALL Java_org_mingyu_posix_SemSet_semctl__IIII
  (JNIEnv *env, jclass, jint id, jint sem, jint cmd, jint val){
 int rc;
  switch (cmd) {
  case org_mingyu_posix_SemSet_IPC_RMID:
    rc = semctl(id,sem,IPC_RMID,val); break;
  case org_mingyu_posix_SemSet_GETNCNT:
    rc = semctl(id,sem,GETNCNT,val); break;
  case org_mingyu_posix_SemSet_GETPID:
    rc = semctl(id,sem,GETPID,val); break;
  case org_mingyu_posix_SemSet_GETVAL:
    rc = semctl(id,sem,GETVAL,val); break;
  case org_mingyu_posix_SemSet_GETZCNT:
    rc = semctl(id,sem,GETZCNT,val); break;
  case org_mingyu_posix_SemSet_SETVAL:
    rc = semctl(id,sem,SETVAL,val); break;
  default:
    return -1;
  }
  return (rc < 0) ? (errno | 0x80000000) : rc;  
}

/*
 * Class:     org_mingyu_posix_SemSet
 * Method:    semop
 * Signature: (I[S)I
 */
JNIEXPORT jint JNICALL Java_org_mingyu_posix_SemSet_semop
  (JNIEnv *env, jclass, jint id, jshortArray sema){
  int n = env->GetArrayLength(sema);
  short *sa = (short *)alloca(n * sizeof *sa);
  env->GetShortArrayRegion(sema,0,n,sa);
  int nop = n / 3;
  struct sembuf *buf = (struct sembuf *)alloca(nop * sizeof *buf);
  for (int i = 0; i < nop; ++i) {
    struct sembuf *p = buf + i;
    p->sem_num = *sa++;
    p->sem_op = *sa++;
    p->sem_flg = *sa++;
  }
  int rc = semop(id,buf,nop);
  return (rc < 0) ? errno : rc;  
}

/*
 * Class:     org_mingyu_posix_SemSet
 * Method:    init
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_org_mingyu_posix_SemSet_init
  (JNIEnv *env, jclass){
  semds.init(env);
  return SEM_UNDO;
 }
 
 
 /***SharedMem API ***/
 
 class jshmid_ds {
  jfieldID segsz,lpid,cpid,nattch,atime,dtime,ctime;
public:
  void init(JNIEnv *env) {
    jclass c = env->FindClass("org/mingyu/posix/SharedMem$shmid_ds");
    segsz = env->GetFieldID(c,"shm_segsz","I");
    lpid = env->GetFieldID(c,"shm_lpid","I");
    cpid = env->GetFieldID(c,"shm_cpid","I");
    nattch = env->GetFieldID(c,"shm_nattch","I");
    atime = env->GetFieldID(c,"shm_atime","J");
    dtime = env->GetFieldID(c,"shm_dtime","J");
    ctime = env->GetFieldID(c,"shm_ctime","J");
  }
  void getshmid(JNIEnv *env,shmid_ds *mbuf,jobject obj) {
    ::perm.getperm(env,&mbuf->shm_perm,obj);
  }
  void setshmid(JNIEnv *env,const shmid_ds *mbuf,jobject obj) {
    ::perm.setperm(env,&mbuf->shm_perm,obj);
    env->SetIntField(obj,segsz,mbuf->shm_segsz);
    env->SetIntField(obj,lpid,mbuf->shm_lpid);
    env->SetIntField(obj,cpid,mbuf->shm_cpid);
    env->SetIntField(obj,nattch,mbuf->shm_nattch);
    env->SetLongField(obj,atime,(jlong)mbuf->shm_atime * 1000);
    env->SetLongField(obj,dtime,(jlong)mbuf->shm_dtime * 1000);
    env->SetLongField(obj,ctime,(jlong)mbuf->shm_ctime * 1000);
  }
};

static jshmid_ds shmds;


JNIEXPORT jlong JNICALL Java_org_mingyu_posix_SharedMem_getLBA
  (JNIEnv *env, jclass){
   shmds.init(env);
   return SHMLBA;
}
 
JNIEXPORT jint JNICALL Java_org_mingyu_posix_SharedMem_shmctl
  (JNIEnv *env, jobject obj1, jint id, jint cmd, jobject obj){
    shmid_ds buf;
    int rc = -1;
    if (obj != 0) {
      if (cmd == org_mingyu_posix_SharedMem_IPC_SET) {
	    shmds.getshmid(env,&buf,obj);
	    rc = shmctl(id,IPC_SET,&buf);
      }
      else if (cmd == org_mingyu_posix_SharedMem_IPC_STAT) {
	    rc = shmctl(id,IPC_STAT,&buf);
	    shmds.setshmid(env,&buf,obj);
      }
    }
    else if (cmd == org_mingyu_posix_SharedMem_IPC_RMID)
      rc = shmctl(id,IPC_RMID,0);
    return (rc < 0) ? errno : rc;
 }

/*
 * Class:     org_mingyu_posix_SharedMem
 * Method:    shmat
 * Signature: (IJI)J
 */
JNIEXPORT jlong JNICALL Java_org_mingyu_posix_SharedMem_shmat
  (JNIEnv *env, jobject obj1, jint id, jlong caddr, jint flag){
     return (jlong)shmat(id,(void *)caddr,flag); 
 }

/*
 * Class:     org_mingyu_posix_SharedMem
 * Method:    shmdt
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL Java_org_mingyu_posix_SharedMem_shmdt
  (JNIEnv *env, jobject obj1, jlong caddr){
    return shmdt((void *)caddr);
}

/*
 * Class:     org_mingyu_posix_SharedMem
 * Method:    shmget
 * Signature: (III)I
 */
JNIEXPORT jint JNICALL Java_org_mingyu_posix_SharedMem_shmget
  (JNIEnv *env, jobject obj1, jint key, jint size, jint flag){
  return shmget(key,size,flag); 
 }

/*
 * Class:     org_mingyu_posix_SharedMem
 * Method:    mapping_bytebuffer
 * Signature: (JI)V
 */
JNIEXPORT void JNICALL Java_org_mingyu_posix_SharedMem_mapping_1bytebuffer
  (JNIEnv *env, jobject thisObj, jlong caddr, jint size){
    jobject ret = NULL;
    ret = env->NewDirectByteBuffer((void *)caddr, size);
    jclass thisClass = env->GetObjectClass(thisObj);
    jfieldID fidNumber = env->GetFieldID(thisClass, "buffer", "Ljava/nio/ByteBuffer;");
    env->SetObjectField(thisObj, fidNumber, ret);
}