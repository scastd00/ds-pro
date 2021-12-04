package jrmp.client;

import java.rmi.NotBoundException;
import java.rmi.RemoteException;
import java.rmi.registry.LocateRegistry;
import java.rmi.registry.Registry;
import java.util.logging.Level;
import java.util.logging.Logger;

public class Client {
	public static void main(String[] args) {
		if (args.length != 2) {
			System.err.println("dsrmipract.Client <ip address of rmi server's registry> <rmi name>");
			System.exit(-1);
		}

		/*
		 * The security manager will use the policy file mentioned above
		 */
		if (System.getSecurityManager() == null) {
			System.setSecurityManager(new SecurityManager());
		}

		try {
			/*
			 * Locate a rmi registry at the IP address specified in the command
			 * line
			 */
			Registry r = LocateRegistry.getRegistry(args[0]);
			System.out.println("registry found");
			System.out.flush();


			/*
			 * Here we use the the same key name used in the server program
			 * args[1], provided in the command line, the remote registry r will
			 * return us a proxy java class (It is called stub conventionally),
			 * it will return us a stub which will be cast to an ExampleMethods
			 * instance, note that we do not need the implementation, it
			 * suffices for us to have the interface ExampleMethods. From this
			 * moment on we have cc, an instance of ExampleMethods that has been
			 * unmarshalled by the rmi protocol and we know that it has two
			 * remote methods that we can call for our good.
			 */
			JRMPRemoteObject remoteObj = (JRMPRemoteObject) r.lookup(args[1]);

			System.out.println("stub lookup done");
			System.out.flush();

			/*
			 * Now we can invoke the remote methods as though they were local
			 * objects methods, but, keep on mind that in each invocation we
			 * are passing parameters and receiving results by way of stub objects
			 * that are like stand-ins for the remote object and that, eventually,
			 * all the computations will be carried out in the remote server.
			 */
			System.out.println("Time from server " + " = " + remoteObj.getTime());
			System.out.flush();

		} catch (RemoteException | NotBoundException ex) {
			Logger.getLogger(Client.class.getName()).log(Level.SEVERE, "Remote error", ex);
		}
	}
}
