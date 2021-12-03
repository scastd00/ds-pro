/**
 * All rights reserved (C) 2014-2021 by José María Foces Morán and José María
 * Foces Vivancos from textbook "Conceptual Computer Networks"
 * <p>
 * SDRemoteObject.java
 * <p>
 * This is the java interface file corresponding to the remote interface of
 * RemoteMethodsImpl objects, the compiled form of this file (.class) is
 * the only local file needed by the client plus the client class itself.
 * <p>
 * java.rmi.Remote is a marker interface. Every method declared in this interface
 * file will become a remote method for objects that implement it.
 ******************************************************************************/
package client.rmievents;

import java.rmi.Remote;
import java.rmi.RemoteException;

public interface SDRemoteObject extends Remote {

	int longStringHash(String s) throws RemoteException;

	long factorial(int x) throws RemoteException;
}
