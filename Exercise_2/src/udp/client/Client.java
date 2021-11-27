package udp.client;

import java.io.IOException;
import java.io.InputStream;
import java.net.Socket;
import java.nio.ByteBuffer;
import java.time.Instant;
import java.time.LocalTime;
import java.time.OffsetTime;
import java.time.ZoneId;
import java.util.Arrays;

public class Client {
	private final Socket socket;

	public Client() throws IOException {
		this.socket = new Socket("127.0.0.1", 37);
	}

	public void work() throws IOException {
		InputStream         in      = this.socket.getInputStream();
		byte[]              buffer  = new byte[32];

		if (in.read(buffer) < 0) {
			System.out.println("Error while reading");
			System.exit(1);
		}

//		byte[] buffer2 = new byte[32];
//		for (int i = 0; i < buffer.length; i++) {
//			buffer2[buffer.length - i - 1] = buffer[i];
//		}

		ByteBuffer wrapped = ByteBuffer.wrap(buffer);
		System.out.println(Arrays.toString(buffer));
		long date = wrapped.getLong();
		System.out.println("Date: " + date);
//		long      l    = Long.getLong(new String(buffer));
		LocalTime time = LocalTime.from(OffsetTime.ofInstant(Instant.ofEpochSecond(date), ZoneId.of("UTC")));

		System.out.println(time.toString());
	}
}
