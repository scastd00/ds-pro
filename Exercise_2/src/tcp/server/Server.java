package tcp.server;

import java.io.IOException;
import java.io.OutputStream;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.Calendar;

public class Server {
	private static final int          PORT = 37;
	private final        ServerSocket serverSocket;
	private              Socket       s;

	public Server() throws IOException {
		this.serverSocket = new ServerSocket(PORT);
	}

	public void work() throws IOException {
		s = this.serverSocket.accept();

		OutputStream out = s.getOutputStream();

		byte[] data = Long.toString(this.getTime()).getBytes();
		out.write(data);

		System.out.printf("Sent %d bytes\n", data.length);
	}

	public void closeConnection() throws IOException {
		s.close();
		this.serverSocket.close();
	}

	private long getTime() {
		Calendar cal = Calendar.getInstance();
		cal.clear();
		cal.set(1900, Calendar.JANUARY, 1, 0, 0, 0);

		long msFrom1900ToEpoch = cal.getTimeInMillis() / 1000;
		long msFrom1970ToToday = Calendar.getInstance().getTimeInMillis() / 1000;

		/* 2 calculations
			- 01-01-1900 -> 01-01-1970
			- 01-01-1970 -> Today
		*/
		return -msFrom1900ToEpoch + msFrom1970ToToday;
	}
}
