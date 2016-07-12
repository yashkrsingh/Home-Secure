import java.io.*;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.Date;
import java.util.StringTokenizer;

/**
 * Created by manav on 5/5/16.
 */
public class ServerClass extends Thread{


    private static Socket socket = null;
    private static ServerSocket serverSocket = null;
    private DataOutputStream output;
    private BufferedReader input;
    private String request;
    private static int security;
    public ServerClass(){

    }

    @Override
    public void run() {
        try{
            input = new BufferedReader(new InputStreamReader(socket.getInputStream()));
            output = new DataOutputStream(socket.getOutputStream());
            request = input.readLine();
            System.out.println(request);

            StringTokenizer tokenizer = new StringTokenizer(request);
            String httpMethod = tokenizer.nextToken();
            String httpQueryString = tokenizer.nextToken();
            String responseString = "";

            if(httpQueryString.contains("sensorData")){
                valueHolder.last_request=request;
            }
            else if(httpQueryString.contains("security=1")){
                security = 1;
                responseString = valueHolder.last_request;
                output.writeBytes(responseString);
                output.writeBytes("\r\n");
            }
            else if(httpQueryString.contains("security=0")) {
                security = 0;
                responseString = valueHolder.last_request;
                output.writeBytes(responseString);
                output.writeBytes("\r\n");
            }else if(httpQueryString.contains("securityToggle")){
                output.writeChar((char)(security+48));
            }


            output.close();
        }catch (Exception e){
            e.printStackTrace();
        }

    }

    public static void main(String[] args) throws IOException {

        serverSocket = new ServerSocket(5000,10);
        System.out.println("Server Socket");

        while(true){
            socket = serverSocket.accept();
            new ServerClass().run();
        }
    }
}
class valueHolder{
    static  String last_request="";
}
