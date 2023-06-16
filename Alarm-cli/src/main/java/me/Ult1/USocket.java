package me.Ult1;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.SocketException;
import java.nio.charset.StandardCharsets;
import java.rmi.ServerException;

public class USocket {

    private static int processesLeft = 0;
    private static boolean processSent = false;

    public static Thread serverThread;

    enum InformationType {
        ERROR,
        SUCCESS,
        INFO,
        HIDDEN
    }

    /**
     * Internal method, which reads whatever comes from the alarms' back-end, parses it and prints it. <br>
     * Incoming data may be annotated & categorized in a <b>state machine</b> fashion.  <br><br>
     *
     * Current annotations: `[ERROR]`, `[SUCCESS]`, `[INFO]`, `[END]`.  <br>
     * ERROR prints the text to `System.error`, END is obvious, other two -> default out. <br><br>
     *
     * Sent data is expected to look like this(# is comment):  <br>
     * <pre style="font-family: Consolas">
     * Started
     *
     * [INFO]                           # list command sent
     * table listing the alarms
     * table listing the alarms
     * table listing the alarms
     * [END]
     * [SUCCESS]                        # add command sent
     * Alarm by name: "example" was added and will be executed after: 6 weeks 3 days
     * [END]
     * [ERROR]                          # remove command sent
     * Alarm by that name does not exist.
     * [END]
     *
     *
     * </pre>
     */
    public static void read() {
        serverThread = new Thread(){
            @Override
            public void run() {
                super.run();

                DatagramSocket socket = null;

                try {
                    socket = new DatagramSocket((int) Config.get("client_port", 347));
                } catch (SocketException e) {
                    System.err.println("Problem establishing connection with the server! \nUnable to listen!");
                    e.printStackTrace();
                }


                InformationType currentInfo = InformationType.INFO;

                byte[] cb = new byte[512];

                while(/*!processSent || */processesLeft > 0){

                    DatagramPacket packet = new DatagramPacket(cb, cb.length);
                    try {
                        socket.receive(packet);
                    } catch (IOException e) {
                        e.printStackTrace();
                    }

                    cb[packet.getLength()] = '\0';

                    String[] message = new String(cb, 0, packet.getLength()).split("\n");
                    for(String line : message) {
                        if (line.contains("[ERROR]")) currentInfo = InformationType.ERROR;
                        else if (line.contains("[SUCCESS]")) currentInfo = InformationType.SUCCESS;
                        else if (line.contains("[INFO]")) currentInfo = InformationType.INFO; // enums in Java always seemed like a strange case to me
                        else if(line.contains("[END]")) {
                            currentInfo = InformationType.HIDDEN;
                            processesLeft --;
                        } // In case the line doesn't have annotation/category. e.g.: "[ERROR]\n" shouldn't be printed
                        if (currentInfo == InformationType.ERROR)
                            System.err.println(line);
                        else if (currentInfo != InformationType.HIDDEN)
                            System.out.println(line);
                    }
                }
            }
        };
        serverThread.start();
    }

    private static boolean serverInitialized = false;

    private static InetAddress address = null;
    private static DatagramSocket clientSocket = null;

    public static void send(String message) throws ServerException {
        System.out.println("[INFO] Sent out: \"" + message + '"');

        if(!serverInitialized){

            address = InetAddress.getLoopbackAddress();

            try {
                clientSocket = new DatagramSocket();
            } catch (SocketException e) {
                System.err.println("Problem establishing connection with the server! \nUnable to listen!\n");
                e.printStackTrace();
            }
            serverInitialized = true;
        } // end if

        DatagramPacket packet = new DatagramPacket(
            message.getBytes(StandardCharsets.US_ASCII), 0,
            message.length(), address, (int) Config.get("server_port", 348)
        );


        try {

            processesLeft ++;
            USocket.read();

            clientSocket.send(packet);

            processSent = true;


        } catch (IOException e){
            System.err.println("Could not send message to server!");
            throw new ServerException(e.getLocalizedMessage());
        }

    }

}






















