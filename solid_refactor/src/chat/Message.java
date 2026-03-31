package chat;

// SRP - Single Responsibility Principle
// This class only stores message data

public class Message {
    private final String sender;
    private final String content;

    public Message(String sender, String content) {
        this.sender = sender;
        this.content = content;
    }

    public String format() {
        return sender + ": " + content;
    }
}