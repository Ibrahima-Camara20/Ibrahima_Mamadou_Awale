import java.io.*;
import java.util.concurrent.*;

public class Arbitre {
    private static final int TIMEOUT_SECONDS = 3;

    public static void main(String[] args) throws Exception {
        // Process A = Runtime.getRuntime().exec("");
        Process A = Runtime.getRuntime().exec("super_bot.exe");
        Process B = Runtime.getRuntime().exec("super_bot.exe");
        // Process B = new ProcessBuilder("./B").start();

        Joueur joueurA = new Joueur("A", A);
        Joueur joueurB = new Joueur("B", B);

        Joueur courant = joueurA;
        Joueur autre = joueurB;

        String coup = "START";
        int nbCoups = 0;
        while (true) {
            // Reception du coup de l'adversaire
            courant.receive(coup);
            // reponse avec TIMEOUT
            coup = courant.response(TIMEOUT_SECONDS);
            if (coup == null) {
                disqualifier(courant, "timeout");
                break;
            }
            nbCoups++;
            if (nbCoups == 400) {
                System.out.println("RESULT LIMIT");
            }
            // Validation du coup
            if (!coupValide(coup)) {
                disqualifier(courant, "coup invalide : " + coup);
                break;
            }

            System.out.println(courant.nom + " -> " + coup);

            // Skip OK acknowledgments (not a real move)
            if (coup.equals("OK")) {
                continue; // Don't change player, wait for actual move
            }

            // Fin de partie
            if (coup.contains("RESULT")) {
                System.out.println(coup);
                break;
            }
            // Changement de joueur
            Joueur tmp = courant;
            courant = autre;
            autre = tmp;
        }
        joueurA.destroy();
        joueurB.destroy();
        System.out.println("Fin.");
    }

    // ===============================
    // Validation du coup (Awale)
    // ===============================
    private static boolean coupValide(String coup) {
        // Accept RESULT messages
        if (coup.startsWith("RESULT"))
            return true;

        // Accept OK (acknowledgment for START)
        if (coup.equals("OK"))
            return true;

        // New format: "1R", "5TR", "12B", etc.
        // Extract digits and type
        try {
            int i = 0;
            // Skip whitespace
            while (i < coup.length() && Character.isWhitespace(coup.charAt(i))) {
                i++;
            }

            // Get digits
            String numStr = "";
            while (i < coup.length() && Character.isDigit(coup.charAt(i))) {
                numStr += coup.charAt(i);
                i++;
            }

            if (numStr.isEmpty()) {
                return false;
            }

            // Get type
            String type = coup.substring(i).trim();

            int src = Integer.parseInt(numStr);

            // Valid src: 1-16 (user format)
            if (src < 1 || src > 16)
                return false;

            // Valid types: R, B, TR, TB
            return type.equals("R") || type.equals("B") ||
                    type.equals("TR") || type.equals("TB");
        } catch (Exception e) {
            return false;
        }
    }

    private static void disqualifier(Joueur j, String raison) {
        System.out.println("RESULT Joueur " + j.nom + " disqualifié (" + raison + ")");
    }

    // ===============================
    // Classe Joueur
    // ===============================
    static class Joueur {
        String nom;
        Process process;
        BufferedWriter in;
        BufferedReader out;
        ExecutorService executor = Executors.newSingleThreadExecutor();

        Joueur(String nom, Process p) {
            this.nom = nom;
            this.process = p;
            this.in = new BufferedWriter(new OutputStreamWriter(p.getOutputStream()));
            this.out = new BufferedReader(new InputStreamReader(p.getInputStream()));
        }

        void receive(String msg) throws IOException {
            in.write(msg);
            in.newLine();
            in.flush();
        }

        String response(int timeoutSeconds) throws IOException {
            Future<String> future = executor.submit(() -> out.readLine());
            try {
                return future.get(timeoutSeconds, TimeUnit.SECONDS);
            } catch (TimeoutException e) {
                future.cancel(true);
                return null;
            } catch (Exception e) {
                return null;
            }
        }

        void destroy() {
            executor.shutdownNow();
            process.destroy();
        }
    }
}
