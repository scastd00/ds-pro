package rmievents;

import java.rmi.NotBoundException;
import java.rmi.RemoteException;
import java.rmi.registry.LocateRegistry;
import java.rmi.registry.Registry;
import java.util.logging.Level;
import java.util.logging.Logger;

public class Client {
	public static void main(String[] args) {

		if (args.length != 3) {
			System.err.println("rmievents.Client <IP address of RMI server's registry> <Registry external port> <RMI name>");
			System.exit(-1);
		}

		/*
		 * The security manager will use the policy file
		 */
		if (System.getSecurityManager() == null) {
			System.setSecurityManager(new SecurityManager());
		}

		try {
			/*
			 * Locate a rmi registry at the IP address specified in the command line
			 */
			Registry r = LocateRegistry.getRegistry(args[0], Integer.parseInt(args[1]));
			System.out.println("Registry found");
			System.out.flush();

			SDRemoteObject cc = (SDRemoteObject) r.lookup(args[2]);

			System.out.println("Stub lookup done");
			System.out.flush();

			System.out.println("1st increment of the counter. Current value: " + cc.increment());
			System.out.println("2nd increment of the counter. Current value: " + cc.increment());

			System.out.println("1st decrement of the counter. Current value: " + cc.decrement());
			System.out.println("2nd decrement of the counter. Current value: " + cc.decrement());

			System.out.flush();
		} catch (RemoteException | NotBoundException ex) {
			Logger.getLogger(Client.class.getName()).log(Level.SEVERE, null, ex);
		}
	}
}
