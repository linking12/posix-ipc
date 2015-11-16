package org.mingyu.posix;

public abstract class IPC {

	protected int id = -1;
	protected boolean owner;

	static final int IPC_RMID = 0;
	static final int IPC_SET = 1;
	static final int IPC_STAT = 2;

	static {
		System.loadLibrary("posix");
	}

	public static final int pid = init();

	public static final int euid = geteuid();
	public static final int egid = getegid();

	public int getId() {
		return id;
	}

	public static final int IPC_PRIVATE = 0, IPC_ALLOC = 0100000,
			IPC_CREAT = 0001000, IPC_EXCL = 0002000, IPC_NOWAIT = 0004000,
			IPC_CI = 0010000, IPC_NOCLEAR = 0020000, IPC_PHYS = 0040000;

	public static native int ftok(String path, int id);

	private static native int init();

	private static native int geteuid();

	private static native int getegid();

	public static class Perm {
		Perm(int id) {
			this.id = id;
		}

		final int id;
		public int uid;
		public int gid;
		public int cuid;
		public int cgid;
		public int mode;
		public int seq;
		public int key;
	}

	public abstract Perm getStatus() throws IPCException;

	public Perm getPerm() throws IPCException {
		return getStatus();
	}

	public abstract void setPerm(Perm st) throws IPCException;

	public void setPerm(int uid, int gid, int mode) throws IPCException {
		Perm st = getPerm();
		st.uid = uid;
		st.gid = gid;
		st.mode = mode;
		setPerm(st);
	}

	public abstract void remove();

	public synchronized void dispose() {
		if (owner)
			remove();
	}

	public void finalize() {
		dispose();
	}

}
