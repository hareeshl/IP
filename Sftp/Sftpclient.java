import java.io.*;
import java.net.*;

public class Sftpclient {

	public static void main(String[] args){
		ServerSocket s1;
		Socket s;
		ObjectInputStream ip;
		ObjectOutputStream op;
		try {
			s1 = new ServerSocket(7735);
			s = s1.accept();
			ip = new ObjectInputStream(s.getInputStream());
			op = new ObjectOutputStream(s.getOutputStream());
			
			op.writeObject("Hello world!");
			
			op.close();
			ip.close();
			s.close();
			
		} catch (UnknownHostException e) {
			e.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		}
			
	}
	
}
