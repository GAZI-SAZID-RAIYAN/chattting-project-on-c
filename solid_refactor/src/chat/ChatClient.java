package chat;

import java.awt.*;
import java.io.*;
import java.net.Socket;
import javax.swing.*;

public class ChatClient {

    private JTextArea chatArea;
    private JTextField inputField;
    private BufferedWriter out;

    public void start() throws Exception {
        // ১. গ্রাফিকাল ইউজার ইন্টারফেস (GUI) সেটআপ
        JFrame frame = new JFrame("Chat Application");
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame.setSize(400, 500);

        chatArea = new JTextArea();
        chatArea.setEditable(false);
        chatArea.setLineWrap(true);
        JScrollPane scrollPane = new JScrollPane(chatArea);

        inputField = new JTextField();
        JButton sendButton = new JButton("Send");

        // মেসেজ পাঠানোর প্যানেল
        JPanel panel = new JPanel(new BorderLayout());
        panel.add(inputField, BorderLayout.CENTER);
        panel.add(sendButton, BorderLayout.EAST);

        frame.getContentPane().add(scrollPane, BorderLayout.CENTER);
        frame.getContentPane().add(panel, BorderLayout.SOUTH);

        // ২. নেটওয়ার্ক কানেকশন (Socket)
        Socket socket = new Socket("localhost", 27015);
        
        BufferedReader in = new BufferedReader(
                new InputStreamReader(socket.getInputStream()));

        out = new BufferedWriter(
                new OutputStreamWriter(socket.getOutputStream()));

        // ৩. মেসেজ পাঠানোর লজিক (Action Listener)
        sendButton.addActionListener(e -> sendMessage());
        inputField.addActionListener(e -> sendMessage());

        // ৪. ইনকামিং মেসেজ রিসিভ করার জন্য আলাদা থ্রেড
        new Thread(() -> {
            try {
                String line;
                while ((line = in.readLine()) != null) {
                    chatArea.append(line + "\n");
                    // অটো স্ক্রোল ডাউন
                    chatArea.setCaretPosition(chatArea.getDocument().getLength());
                }
            } catch (Exception e) {
                chatArea.append("Connection lost...\n");
            }
        }).start();

        frame.setVisible(true);
    }

    private void sendMessage() {
        try {
            String msg = inputField.getText().trim();
            if (!msg.isEmpty()) {
                out.write(msg);
                out.newLine();
                out.flush();
                inputField.setText("");
            }
        } catch (Exception e) {
            chatArea.append("Error sending message.\n");
        }
    }
}