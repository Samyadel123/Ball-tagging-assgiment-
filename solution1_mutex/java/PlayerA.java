import java.io.*;
import java.net.*;
import java.util.*;
import org.json.*;

public class PlayerA {
    private static final double CAPTURE_THRESHOLD = 5.0;
    private static final int MY_COLOR = 1;

    public static void main(String[] args) throws Exception {
        System.out.println("[JAVA] Player A starting...");
        Socket socket = new Socket("localhost", 5000);
        BufferedReader in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
        PrintWriter out = new PrintWriter(socket.getOutputStream(), true);

        // Receive initial state
        String initStr = in.readLine();
        JSONObject init = new JSONObject(initStr);
        int numBalls = init.getInt("num_balls");
        JSONArray positions = init.getJSONArray("positions");
        double[][] ballPos = new double[numBalls][2];
        for (int i = 0; i < numBalls; i++) {
            ballPos[i][0] = positions.getJSONArray(i).getDouble(0);
            ballPos[i][1] = positions.getJSONArray(i).getDouble(1);
        }

        Random rand = new Random();
        double currX = rand.nextDouble() * 100;
        double currY = rand.nextDouble() * 100;

        long startTime = System.currentTimeMillis();
        long duration = init.getInt("duration") * 1000;

        while (System.currentTimeMillis() - startTime < duration) {
            // Movement simulation
            currX = Math.max(0, Math.min(100, currX + (rand.nextDouble() * 6 - 3)));
            currY = Math.max(0, Math.min(100, currY + (rand.nextDouble() * 6 - 3)));

            for (int i = 0; i < numBalls; i++) {
                double dist = Math.sqrt(Math.pow(currX - ballPos[i][0], 2) + Math.pow(currY - ballPos[i][1], 2));
                if (dist < CAPTURE_THRESHOLD) {
                    // Try to tag
                    out.println("TAG " + i + " " + MY_COLOR);
                    String response = in.readLine();
                    // if ("SUCCESS".equals(response)) System.out.println("[JAVA] Tagged " + i);
                }
            }
            Thread.sleep(100);
        }
        socket.close();
    }
}
