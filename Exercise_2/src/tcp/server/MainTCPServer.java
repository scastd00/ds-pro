package tcp.server;

import java.io.IOException;

public class MainTCPServer {
	public static void main(String[] args) throws IOException {
		Server server = new Server();

		Runtime.getRuntime().addShutdownHook(new Thread(() -> {
			try {
				server.closeConnection();
			} catch (IOException e) {
				e.printStackTrace();
			}
		}));

		while (true) {
			server.work();
		}
	}
}
