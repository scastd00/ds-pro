package tcp.server;

import java.io.IOException;
import java.io.OutputStream;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.Calendar;
import java.util.TimeZone;

public class Server {

	/**
	 * Timeserver default port <a href="https://datatracker.ietf.org/doc/html/rfc868">RFC868</a>.
	 */
	private static final int PORT = 37;

	/**
	 * Socket for the server.
	 */
	private final ServerSocket serverSocket;

	/**
	 * Class constructor that builds the {@link ServerSocket}.
	 *
	 * @throws IOException if an error occurs.
	 */
	public Server() throws IOException {
		this.serverSocket = new ServerSocket(PORT);
	}

	/**
	 * All the functionality of the server.
	 *
	 * @throws IOException if an error occurs.
	 */
	public void work() throws IOException {
		// Listen state and create a delegate socket on connection
		Socket s = this.serverSocket.accept();

		OutputStream out = s.getOutputStream();

		byte[] data = Long.toString(this.getTime()).getBytes();
		out.write(data); // Send the date

		System.out.printf("Sent %d bytes\n", data.length);

		// Close the sockets
		s.close();
		this.serverSocket.close();
	}

	/**
	 * Calculate the seconds since 01-01-1900 until now.
	 *
	 * @return the number of seconds since 01-01-1900.
	 */
	private long getTime() {
		Calendar cal = Calendar.getInstance(TimeZone.getTimeZone("GMT"));
		cal.clear();
		cal.set(1900, Calendar.JANUARY, 1, 0, 0, 0);

		/*
		 * 2 calculations
		 *   msFrom1900ToEpoch ==> 01-01-1900 -> 01-01-1970
		 *   msFrom1970ToToday ==> 01-01-1970 -> Today
		 */
		long msFrom1900ToEpoch = cal.getTimeInMillis() / 1000;
		long msFrom1970ToToday = Calendar.getInstance(TimeZone.getTimeZone("GMT")).getTimeInMillis() / 1000;

		return msFrom1970ToToday - msFrom1900ToEpoch;
	}
}
