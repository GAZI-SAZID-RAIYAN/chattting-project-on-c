// package chat;

// // Handles only one client connection

// public class ClientHandler implements Runnable {

//     private final Sender sender;
//     private final Receiver receiver;
//     private final ChatServer server;
//     private final String name;

//     public ClientHandler(Sender sender, Receiver receiver,
//                          ChatServer server, String name) {
//         this.sender = sender;
//         this.receiver = receiver;
//         this.server = server;
//         this.name = name;
//     }
// }








package chat;

// Handles only one client connection
public class ClientHandler implements Runnable {

    private final Sender sender;
    private final Receiver receiver;
    private final ChatServer server;
    private final String name;

    public ClientHandler(Sender sender, Receiver receiver,
                         ChatServer server, String name) {
        this.sender = sender;
        this.receiver = receiver;
        this.server = server;
        this.name = name;
    }

    // এই মেথডটি না থাকায় ২য় এররটি আসছিল
    @Override
    public void run() {
        try {
            server.broadcast(name + " joined the chat.");
            String input;
            while ((input = receiver.receive()) != null) {
                Message msg = new Message(name, input);
                server.broadcast(msg.format());
            }
        } catch (Exception e) {
            System.out.println(name + " disconnected.");
        } finally {
            server.removeClient(this);
            server.broadcast(name + " left the chat.");
        }
    }

    // এই মেথডটি না থাকায় ১ম এররটি আসছিল
    public void send(String msg) {
        try {
            sender.send(msg);
        } catch (Exception ignored) {}
    }
}