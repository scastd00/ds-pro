package udp.client;

import java.io.DataInputStream;
import java.io.IOException;
import java.math.BigDecimal;
import java.math.BigInteger;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.UnknownHostException;
import java.time.Instant;
import java.time.ZoneId;
import java.time.format.DateTimeFormatter;
import java.util.Arrays;
import java.util.Calendar;
import java.util.Locale;
import java.util.TimeZone;

public class Client {

	/**
	 * Message sent to the server to get the date.
	 */
	private static final byte[] REQ_MSG = "GET_DATE\0".getBytes();

	/**
	 * UDP socket used for the connection
	 */
	private final DatagramSocket socket;

	/**
	 * Class constructor. Creates the {@link DatagramSocket}.
	 *
	 * @throws IOException if an error occurs while creating the socket.
	 */
	public Client() throws IOException {
		this.socket = new DatagramSocket();
	}

	/**
	 * All the functionality of the client
	 *
	 * @throws IOException if an error occurs.
	 */
	public void work() throws IOException {
		DatagramPacket packet = createDatagramPacket(REQ_MSG, REQ_MSG.length);

		// Send the packet to the server
		this.socket.send(packet);

		// Create a new packet to store the byte stream that the server will send
		packet = createDatagramPacket(new byte[Integer.BYTES], Integer.BYTES);
		this.socket.receive(packet); // Store the server's response into packet.buf attribute

		/*
		 * The data contains the number of seconds since 01-01-1900
		 * in a binary representation of the number. It is parsed to a long
		 * because an exception is thrown if used an int.
		 */
//		String binStringNum = new String(packet.getData());
//		long   secondsDate  = Long.parseLong(binStringNum, 2);
		System.out.println(Arrays.toString(packet.getData()));
		long value = new BigInteger(packet.getData()).longValueExact();
		System.out.println(value);

		// Get a new empty calendar
		Calendar cal = Calendar.getInstance(TimeZone.getTimeZone("GMT"));
		cal.clear();
		cal.set(1900, Calendar.JANUARY, 1, 0, 0, 0); // Setting the base date of 01-01-1900

		/*
		 * Add the seconds received from the server to the calendar to obtain the current date.
		 * The output format is: day, dd mm yyyy hh:mm:ss TZ
		 */
//		Instant instant = cal.toInstant().plusSeconds(secondsDate);
//		DateTimeFormatter formatter = DateTimeFormatter.RFC_1123_DATE_TIME
//			.withLocale(Locale.getDefault())
//			.withZone(ZoneId.of("GMT"));
//
//		System.out.println(binStringNum);
//		System.out.println(formatter.format(instant));
	}

	/**
	 * Generates a new {@link DatagramPacket} to store data.
	 *
	 * @param data the data being sent or received.
	 * @param size size of the data to be read or written.
	 *
	 * @return a new {@link DatagramPacket}.
	 *
	 * @throws UnknownHostException if an error occurs.
	 */
	private DatagramPacket createDatagramPacket(byte[] data, int size) throws UnknownHostException {
		return new DatagramPacket(data, size, InetAddress.getLocalHost(), 37);
	}
}
