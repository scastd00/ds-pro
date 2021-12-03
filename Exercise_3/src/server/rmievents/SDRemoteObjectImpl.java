package rmievents;

import java.rmi.RemoteException;
import java.rmi.server.UnicastRemoteObject;

public class SDRemoteObjectImpl extends UnicastRemoteObject implements SDRemoteObject {

	private long counter = 0;

	/*
	 * This constructor along with the throws' clause is required so that
	 * objects are appropriately created as remote objects.
	 */
	public SDRemoteObjectImpl() throws RemoteException {
		super(); // Todo: add port??
	}

	/*
	Se añade el puerto en el host o en otro sitio?
	 */

	@Override
	public long increment() throws RemoteException {
		printThread();
		return this.counter++;
	}

	@Override
	public long decrement() throws RemoteException {
		printThread();
		return this.counter--;
	}

	private void printThread() {
		System.out.println("Thread name:  " + Thread.currentThread().getName());
		System.out.flush();
	}
}
