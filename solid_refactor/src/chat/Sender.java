package chat;

// ISP - Interface Segregation Principle
// Only sending functionality

public interface Sender {
    void send(String msg) throws Exception;
}