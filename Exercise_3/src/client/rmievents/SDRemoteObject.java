package rmievents;

import java.rmi.Remote;
import java.rmi.RemoteException;

public interface SDRemoteObject extends Remote {
	long increment() throws RemoteException;

	long decrement() throws RemoteException;
}
