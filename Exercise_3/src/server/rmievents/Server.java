package rmievents;

import java.net.MalformedURLException;
import java.rmi.Naming;
import java.rmi.RemoteException;
import java.util.logging.Level;
import java.util.logging.Logger;

public class Server {
	public static void main(String[] args) {
		if (args.length != 2) {
			System.err.println("rmievents.Server <Remote object registry name> <Remote object port>");
			System.exit(-1);
		}

		int port = Integer.parseInt(args[1]);

		if (!checkPortRange(port)) {
			System.err.println("Port must be greater than 0 and lower than 65535");
			System.exit(-2);
		}

		try {
			SDRemoteObjectImpl cc = new SDRemoteObjectImpl(port);
			Naming.rebind(args[0], cc);

			System.out.println("-------------------------------------------------");
			System.out.println("- Remote objects instantiated                   -");
			System.out.println("- Remote objects registered (Java RMI registry) -");
			System.out.println("- Server running                                -");
			System.out.println("-------------------------------------------------");
		} catch (RemoteException | MalformedURLException ex) {
			Logger.getLogger(Server.class.getName()).log(Level.SEVERE, null, ex);
		}
	}

	private static boolean checkPortRange(int port) {
		return port > 0 && port < 65535;
	}
}
