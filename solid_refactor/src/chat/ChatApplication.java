package chat;

// Entry point of application

public class ChatApplication {

    public static void main(String[] args) throws Exception {

        if (args.length > 0 && args[0].equals("server")) {
            new ChatServer().start();
        } else {
            new ChatClient().start();
        }
    }
}

//Main Class


// cd .. 
// javac chat/*.java
// java chat.ChatApplication server


// For another new terminal
// cd solid_refactor\src
// java chat.ChatApplication