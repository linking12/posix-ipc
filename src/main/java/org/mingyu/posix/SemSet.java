package org.mingyu.posix;

public class SemSet extends IPC {

	private static final int GETNCNT = 3;
	private static final int GETPID = 4;
	private static final int GETVAL = 5;
	private static final int GETALL = 6;
	private static final int GETZCNT = 7;
	private static final int SETVAL = 8;
	private static final int SETALL = 9;
	public static final short SEM_UNDO = (short) init();
	private int nsems;

	public SemSet(int key, int nsem, int flag) throws IPCException {
		this.nsems = nsem;
		id = semget(key, nsem, flag);
		if (id < 0)
			throw new IPCException("semget", id & 0x7fffffff);
		owner = (flag & IPC_CREAT) != 0;
	}

	public void remove() {
		if (id >= 0) {
			semctl(id, 0, IPC_RMID, 0);
			id = -1;
		}
	}

	public SemSet(int qid) throws IPCException {
		id = qid;
		semid_ds st = getStatus();
		nsems = st.sem_nsems;
	}

	public SemSet(String path, int id) throws IPCException {
		this(ftok(path, id));
	}

	public static class semid_ds extends IPC.Perm {
		semid_ds(int id) throws IPCException {
			super(id);
			int rc = semctlb(id, 0, IPC_STAT, this);
			if (rc < 0)
				throw new IPCException("semctl", rc & 0x7fffffff);
		}

		public long sem_otime;
		public long sem_ctime;
		public int sem_nsems;
	}

	public static native int semget(int key, int num, int flag);

	private static native int semctlb(int id, int semnum, int cmd, semid_ds buf);

	private static native int semctl(int id, int semnum, int cmd, short[] buf);

	private static native int semctl(int id, int semnum, int cmd, int val);

	private static native int semop(int id, short[] sema);

	private static native int init();

	public semid_ds getStatus() throws IPCException {
		int id = this.id;
		if (id < 0)
			return null;
		return new semid_ds(id);
	}

	public void setPerm(Perm st) throws IPCException {
		if (!(st instanceof semid_ds) || st.id != id)
			throw new IllegalArgumentException("Current semid_ds required.");
		int rc = semctlb(id, 0, IPC_SET, (semid_ds) st);
		if (rc < 0)
			throw new IPCException("semctl", rc & 0x7fffffff);
	}

	public void semop(short[] sema) throws IPCException {
		if (sema.length % 3 != 0)
			throw new IllegalArgumentException("Wrong length for SemOp array: "
					+ sema.length);
		int rc = semop(id, sema);
		if (rc != 0)
			throw new IPCException("semop", rc & 0x7fffffff);
	}

	public int getPid(int semnum) throws IPCException {
		int pid = semctl(id, semnum, GETPID, 0);
		if (pid < 0)
			throw new IPCException("semGetPid", pid & 0x7fffffff);
		return pid;
	}

	public char getNCnt(int semnum) throws IPCException {
		int ncnt = semctl(id, semnum, GETNCNT, 0);
		if (ncnt < 0)
			throw new IPCException("semGetNCnt", ncnt & 0x7fffffff);
		return (char) ncnt;
	}

	public char getZCnt(int semnum) throws IPCException {
		int ncnt = semctl(id, semnum, GETZCNT, 0);
		if (ncnt < 0)
			throw new IPCException("semGetNCnt", ncnt & 0x7fffffff);
		return (char) ncnt;
	}

	public short getValue(int semnum) throws IPCException {
		int val = semctl(id, semnum, GETVAL, 0);
		if (val < 0)
			throw new IPCException("semGetValue", val & 0x7fffffff);
		return (short) val;
	}

	public void setValue(int semnum, int val) throws IPCException {
		int rc = semctl(id, semnum, SETVAL, val);
		if (rc < 0)
			throw new IPCException("semSetValue", rc & 0x7fffffff);
	}

	public short[] getValues() throws IPCException {
		short[] v = new short[nsems];
		int rc = semctl(id, nsems, GETALL, v);
		if (rc < 0)
			throw new IPCException("semGetValues", rc & 0x7fffffff);
		return v;
	}

	public void setValues(short[] vals) throws IPCException {
		if (vals.length != nsems)
			throw new IllegalArgumentException("Wrong length for SemVal array");
		int rc = semctl(id, nsems, SETALL, vals);
		if (rc < 0)
			throw new IPCException("semSetValues", rc & 0x7fffffff);
	}

}
