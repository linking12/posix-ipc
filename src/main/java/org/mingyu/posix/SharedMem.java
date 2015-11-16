package org.mingyu.posix;

import java.nio.ByteBuffer;
import java.nio.CharBuffer;
import java.nio.charset.Charset;
import java.nio.charset.CharsetDecoder;

public class SharedMem extends IPC {

	private final long addr;

	private final SemSet semSet;

	public static final long SHMLBA = getLBA();

	public SharedMem(int key, int flag) throws IPCException {
		this(key, 0, flag);
	}

	public SharedMem(int key, int size, int flag) throws IPCException {
		id = shmget(key, size, flag);
		if (id < 0)
			throw new IPCException("shmget, key=0x" + Integer.toHexString(key));
		long addr = shmat(id, 0, 0);
		if (addr == -1L)
			throw new IPCException("shmat");
		this.mapping_bytebuffer(addr, size);
		this.addr = addr;
		semSet = new SemSet(key, 1, flag);
		semSet.setValue(0, 1);
		owner = (flag & IPC_CREAT) != 0 || key == IPC_PRIVATE;
	}

	public SharedMem(String path, int type) throws IPCException {
		this(ftok(path, type), 0);
	}

	public class shmid_ds extends IPC.Perm {
		shmid_ds(int id) throws IPCException {
			super(id);
			int rc = shmctl(id, IPC_STAT, this);
			if (rc != 0)
				throw new IPCException(
						String.format("shmctl(%d,IPC_STAT)", id), rc);
		}

		public int shm_segsz;
		public int shm_lpid;
		public int shm_cpid;
		public int shm_nattch;
		public int shm_cnattch;
		public long shm_atime;
		public long shm_dtime;
		public long shm_ctime;
	}

	private static native long getLBA();

	private native int shmctl(int id, int cmd, shmid_ds buf);

	private native long shmat(int id, long addr, int flag);

	private native int shmdt(long addr);

	private native int shmget(int key, int size, int flag);

	private ByteBuffer buffer;

	private native void mapping_bytebuffer(long addr, int size);

	public synchronized void lock() throws Exception {
		try {
			short[] sema = new short[] { 0, -1, SemSet.SEM_UNDO };
			semSet.semop(sema);
		} catch (IPCException e) {
			throw e;
		}
	}

	public synchronized void unlock() throws Exception {
		try {
			short[] sema = new short[] { 0, 1, SemSet.SEM_UNDO };
			semSet.semop(sema);
		} catch (IPCException e) {
			throw e;
		}
	}

	public synchronized ByteBuffer put(byte[] data) {
		return buffer.put(data);
	}

	public synchronized String get() {
		Charset charset = null;
		CharsetDecoder decoder = null;
		CharBuffer charBuffer = null;
		try {
			charset = Charset.forName("UTF-8");
			decoder = charset.newDecoder();
			charBuffer = decoder.decode(buffer);
			return charBuffer.toString();
		} catch (Exception e) {
			return "";
		}
	}

	public shmid_ds getStatus() throws IPCException {
		int id = this.id;
		if (id < 0)
			return null;
		return new shmid_ds(id);
	}

	public void setPerm(Perm st) throws IPCException {
		if (!(st instanceof shmid_ds) || st.id != this.id)
			throw new IllegalArgumentException("Current shmid_ds required.");
		int rc = shmctl(id, IPC_SET, (shmid_ds) st);
		if (rc < 0)
			throw new IPCException("shmctl", rc & 0x7fffffff);
	}

	public void remove() {
		if (id >= 0) {
			shmctl(id, IPC_RMID, null);
			id = -1;
		}
	}

	public synchronized void dispose() {
		shmdt(addr);
	}

}
