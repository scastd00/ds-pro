package udp.client;

import java.io.IOException;

public class MainUDPClient {
	public static void main(String[] args) {
		try {
			new Client().work();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
}
