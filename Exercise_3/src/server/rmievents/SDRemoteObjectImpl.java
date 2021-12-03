/**
 * All rights reserved (C) 2014-2021 by José María Foces Morán and José María
 * Foces Vivancos from textbook "Conceptual Computer Networks"
 * <p>
 * RemoteMethodsImpl.java
 * <p>
 * This class offers two example public methods to be exported. Objects of this
 * class become RMI remote objects since this class implements a java.rmi.Remote
 * subinterface (SDRemoteObject is the subinterface)
 * <p>
 * ****************************************************************************
 */
package rmievents;

import java.rmi.RemoteException;
import java.rmi.server.UnicastRemoteObject;

public class SDRemoteObjectImpl extends UnicastRemoteObject implements SDRemoteObject {

	/* This constructor along with the throws' clause is required so that
	 * objects are appropriately created as remote objects.
	 */
	public SDRemoteObjectImpl() throws RemoteException {
		super();
	}

	/*
	 * This public method will become a remote method since it has been included
	 * in the remote interface ExampleMethods. In classes like this one, obviously
	 * one can include other non-remote methods, public or otherwise, the only
	 * requirement for a method to become remote is that its full signature is
	 * declared in the remote interface, in this case, ExampleMethods.java
	 *
	 * This example method calculates a long string hash (Taken from Sedgewick
	 * "Algorithms in Java" parts 1-4)
	 *
	 */
	public int longStringHash(String s) throws RemoteException {
		int m = s.length();
		int h = 0, a = 127;

		for (int i = 0; i < s.length(); i++) {
			h = (a * h + s.charAt(i)) % m;
		}

		printThread();
		return h;
	}

	/*
	 * Returns the factorial of x
	 */
	public long factorial(int x) throws RemoteException {
		long f = 1;

		for (int i = x; i > 1; i--) {
			f = f * i;
		}

		printThread();
		return f;
	}

	/*
	 * This utility method is not public, therefore it could not be a remote
	 * method in any case
	 */
	private void printThread() {
		System.out.println("Thread name:  " + Thread.currentThread().getName());
		System.out.flush();
	}
}
