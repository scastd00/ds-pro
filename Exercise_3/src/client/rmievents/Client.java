/**
 * ***************************************************************************
 * All rights reserved (C) 2014-2021 by José María Foces Morán and José María
 * Foces Vivancos from textbook "Conceptual Computer Networks"
 * <p>
 * Client.java
 * <p>
 * This class is the client that will invoke the example remote methods.
 * <p>
 * PRECONDITIONS FOR EXECUTION OF THIS PROGRAM
 * -------------------------------------------
 * <p>
 * This program needs that a security policy file be specified in the java command
 * line, that file should contain a java security policy file that allows the
 * run time to dynamically load java code from anywhere, specifically, it needs
 * to download and execute the proxy classes from the remote server by way of
 * the remote registry. In order for the user to express this permission, a
 * command line like the following one would be fine:
 * <p>
 * $ java -Djava.security.policy="all.policy"\
 * sdrmiexample.Client\
 * 192.168.2.33\
 * <Remote object registry name>
 * <p>
 * The policy file (all.policy) contents required in this case would be:
 * <p>
 * grant{
 * permission java.security.AllPermission;
 * };
 ******************************************************************************/

package client.rmievents;

import java.rmi.NotBoundException;
import java.rmi.RemoteException;
import java.rmi.registry.LocateRegistry;
import java.rmi.registry.Registry;
import java.util.logging.Level;
import java.util.logging.Logger;

public class Client {
	public static void main(String args[]) {

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
			SDRemoteObject cc = (SDRemoteObject) r.lookup(args[1]);

			System.out.println("stub lookup done");
			System.out.flush();

			String ts = "Hello world in Java using Java RMI!!!";

			/*
			 * Now we can invoke the remote methods as though they were local
			 * objects methods, but, keep on mind that in each invocation we
			 * are passing parameters and receiving results by way of stub objects
			 * that are like stand-ins for the remote object and that, eventually,
			 * all the computations will be carried out in the remote server.
			 */
			System.out.println("Hash of " + ts + " = " + cc.longStringHash(ts));
			System.out.println("Computing 7!  = " + cc.factorial(7));
			System.out.flush();

		} catch (RemoteException | NotBoundException ex) {
			Logger.getLogger(Client.class.getName()).log(Level.SEVERE, null, ex);
		}
    }
}
