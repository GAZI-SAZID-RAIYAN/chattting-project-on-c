package chat;

import java.io.*;
import java.net.Socket;

// DIP - depends on abstraction
// LSP - Can substitute Sender/Receiver

public class TcpNetworkAdapter implements Sender, Receiver {

    private BufferedReader in;
    private BufferedWriter out;

    public TcpNetworkAdapter(Socket socket) throws Exception {
        in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
        out = new BufferedWriter(new OutputStreamWriter(socket.getOutputStream()));
    }

    @Override
    public void send(String msg) throws Exception {
        out.write(msg);
        out.newLine();
        out.flush();
    }

    @Override
    public String receive() throws Exception {
        return in.readLine();
    }
}