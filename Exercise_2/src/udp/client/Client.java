package udp.client;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.time.Instant;
import java.time.ZoneId;
import java.time.format.DateTimeFormatter;
import java.time.format.FormatStyle;
import java.util.Calendar;
import java.util.Locale;
import java.util.TimeZone;

public class Client {
	private static final byte[]         REQ_MSG = "GET_DATE\0".getBytes();
	private final        DatagramSocket socket;

	public Client() throws IOException {
		this.socket = new DatagramSocket();
	}

	public void work() throws IOException {
		DatagramPacket packet = new DatagramPacket(REQ_MSG, REQ_MSG.length, InetAddress.getLocalHost(), 37);

		this.socket.send(packet);

		byte[] response = new byte[Integer.SIZE];
		packet = new DatagramPacket(response, Integer.SIZE, InetAddress.getLocalHost(), 37);
		this.socket.receive(packet);

		String binStringNum = new String(packet.getData());
		long   secondsDate  = Long.parseLong(binStringNum, 2);

		Calendar cal = Calendar.getInstance(TimeZone.getTimeZone("GMT"));
		cal.clear();
		cal.set(1900, Calendar.JANUARY, 1, 0, 0, 0);
		Instant instant = cal.toInstant().plusSeconds(secondsDate);
		DateTimeFormatter formatter = DateTimeFormatter.ofLocalizedDateTime(FormatStyle.FULL, FormatStyle.FULL)
			.withLocale(Locale.getDefault())
			.withZone(ZoneId.of("GMT"));

		System.out.println(formatter.format(instant));
	}
}
