package org.mingyu.posix;

public class Errno {
	public static native int getErrno();

	public static native String strerror(int errno);

	private static native int errno(int eidx);

	static {
		System.loadLibrary("posix");
	}

	public final static int EPERM = errno(0);
	public final static int ENOENT = errno(1);
	public final static int ESRCH = errno(2);
	public final static int EINTR = errno(3);
	public final static int EIO = errno(4);
	public final static int EIDRM = errno(5);
	public final static int ENOMSG = errno(6);
	public final static int EFAULT = errno(7);
	public final static int EINVAL = errno(8);
	public final static int EACCES = errno(9);
	public final static int E2BIG = errno(10);
	public final static int EAGAIN = errno(11);
	public final static int ENOMEM = errno(12);

	public static String getErrdesc(int errno) {
		String msg = strerror(errno);
		if (msg == null)
			msg = "Unknown error: " + errno;
		return msg;
	}

	public static String getErrdesc() {
		return getErrdesc(getErrno());
	}

	public static void main(String[] argv) {
		for (int i = 0; i < argv.length; ++i) {
			int err = Integer.parseInt(argv[i]);
			System.out.println(Errno.getErrdesc(err));
		}
	}
}
