package org.mingyu.posix;

public class IPCException extends Exception {

	private static final long serialVersionUID = 1L;

	private int errno;

	public IPCException() {
		this(Errno.getErrno());
	}

	public IPCException(int errno) {
		super(Errno.getErrdesc(errno));
		this.errno = errno;
	}

	public IPCException(String tag, int errno) {
		super(tag + ": " + Errno.getErrdesc(errno));
		this.errno = errno;
	}

	public IPCException(String tag) {
		this(tag, Errno.getErrno());
	}

	public int getErrno() {
		return errno;
	}
}
