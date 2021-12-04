package jrmp.client;

import java.rmi.Remote;
import java.rmi.RemoteException;

public interface JRMPRemoteObject extends Remote {
	String getTime() throws RemoteException;
}
