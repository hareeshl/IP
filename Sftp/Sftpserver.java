import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.UnknownHostException;


public class Sftpserver {
	public static void main(String[] args){
		Socket s;
		ObjectInputStream ip;
		ObjectOutputStream op;
		String message;
		try {
			s = new Socket("localhost", 7735);
			ip = new ObjectInputStream(s.getInputStream());
			op = new ObjectOutputStream(s.getOutputStream());
			
			message = (String) ip.readObject();
			System.out.println("Server sent"+message);
			
			op.close();
			ip.close();
			s.close();
			
		} catch (UnknownHostException e) {
			e.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		} catch (ClassNotFoundException e) {
			e.printStackTrace();
		}
			
	}

}
