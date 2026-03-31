package chat;

import java.net.*;
import java.util.*;

// OCP - Server logic does not change if network changes

public class ChatServer {

    private List<ClientHandler> clients = new ArrayList<>();
    private int counter = 1;

    public void start() throws Exception {
        ServerSocket ss = new ServerSocket(27015);
        System.out.println("Server started...");

        while (true) {
            Socket socket = ss.accept();

            TcpNetworkAdapter adapter = new TcpNetworkAdapter(socket);

            ClientHandler handler = new ClientHandler(
                    adapter, adapter, this, "Client" + counter++
            );

            clients.add(handler);
            new Thread(handler).start();
        }
    }

    public void broadcast(String msg) {
        for (ClientHandler c : clients)
            c.send(msg);
    }

    public void removeClient(ClientHandler c) {
        clients.remove(c);
    }
}
