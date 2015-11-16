package org.mingyu.posix;

import java.nio.charset.Charset;

public class TestMain {

	public static void main(String[] args) throws Exception {

		try {
			SharedMem sha1 = new SharedMem(123, 1024, 0666);
			try {
				sha1.lock();
				sha1.put(new String("testtesttest").getBytes(Charset
						.forName("UTF-8")));
			} catch (Exception e) {
				e.printStackTrace();
			} finally {
				sha1.unlock();
			}
			SharedMem sha2 = new SharedMem(123, 1024, 0666);
			String test = sha2.get();
			System.out.println(test);

		} catch (IPCException e) {
			e.printStackTrace();
		}
	}

}
