package chat;

// ISP - Interface Segregation Principle
// Only receiving functionality

public interface Receiver {
    String receive() throws Exception;
}