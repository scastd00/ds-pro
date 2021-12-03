/**
 * ***************************************************************************
 * All rights reserved (C) 2014-2021 by José María Foces Morán and José María
 * Foces Vivancos from textbook "Conceptual Computer Networks"
 * <p>
 * Server.java
 * <p>
 * Creates an instance of ExampleMethodsImpl() and registers it with an running
 * rmi registry.
 * <p>
 * PRECONDITIONS FOR EXECUTION OF THIS PROGRAM
 * -------------------------------------------
 * <p>
 * 1. rmiregistry must be running on TCP port 1099
 * <p>
 * Stub classes should be uploaded to a web server and its address specified on
 * the command line (See the next example)
 * <p>
 * $ java -Djava.rmi.server.codebase=http://192.168.1.88/\
 * sdrmiexample.Server\
 * <Remote object registry name>
 ******************************************************************************/
package rmievents;

import java.net.MalformedURLException;
import java.rmi.Naming;
import java.rmi.RemoteException;
import java.util.logging.Level;
import java.util.logging.Logger;

public class Server {
	public static void main(String[] args) {
		if (args.length != 1) {
			System.err.println("dsrmipract.Server <Remote object registry name>");
			System.exit(-1);
		}

		try {

			SDRemoteObjectImpl cc = new SDRemoteObjectImpl();

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
}
