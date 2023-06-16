package me.Ult1;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.rmi.ServerException;
import java.util.Arrays;
import java.util.LinkedList;
import java.util.Locale;
import java.util.Queue;

public class Parser {

    private enum ErrorType {
        UNEXPECTED_FLAG,
        UNEXPECTED_ARG,
        REDUNDANT_ARG,
        REDUNDANT_FLAG,
        MISSING_ARG
    }

    private static boolean errorDetected = false;

    public static void parse(String[] arguments) throws IOException, ServerException {

        if(arguments.length == 0){
            arguments = new String[1];
            arguments[0] = "-h"; // yes.
        }

        Queue<String> args = new LinkedList<>(Arrays.asList(arguments));

        StringBuilder time = new StringBuilder(18);
        String name = null;

        String flag = null;

        while(!args.isEmpty()){
            String current = args.poll();

            if(is(current, "a", "-add")) {
                while (!args.isEmpty()) {
                    if (is(args.peek(), "n", "-name")) {
                        flag = args.poll();
                        if (args.isEmpty())
                            error(ErrorType.MISSING_ARG, flag);
                        else if (isFlag(args.peek()))
                            error(ErrorType.UNEXPECTED_FLAG, args.peek());
                        else
                            name = args.poll();
                        break;
                        // If current argument has normal date format e.g. 31d, 9999h...
                    } else if (args.peek().matches("\\d+[ydhms]")) {
                        time.append(args.poll()).append(' ');
                        // If it's neither a flag nor a date format, it's name
                    } else if (!isFlag(args.peek())) {
                        name = args.poll();
                        break;
                        // Else if it is an unrelated flag
                    } else
                        break;
                }
                if (time.length() > 2) {
                    time.deleteCharAt(time.length() - 1);
                    USocket.send("a " + time + ((name != null) ? " " + name : ""));
                } else
                    error(ErrorType.MISSING_ARG, "time");

            } else if(is(current, "d", "-delete")){
                if(args.isEmpty())
                    error(ErrorType.MISSING_ARG, "--delete");
                else if(is(args.peek(), "n", "-name")){
                    flag = args.poll();
                    if(args.isEmpty())
                        error(ErrorType.MISSING_ARG, flag);
                    else if(isFlag(args.peek()))
                        error(ErrorType.UNEXPECTED_FLAG, args.peek());
                    else {
                        name = args.poll();
                        USocket.send("r " + name);
                    }
                } else if(args.isEmpty())
                    error(ErrorType.MISSING_ARG, flag);
                else if(isFlag(args.peek()))
                    error(ErrorType.UNEXPECTED_FLAG, args.peek());
                else {
                    name = args.poll();
                    USocket.send("r " + name);
                }

            } else if(is(current, "l", "-list")) {
                if (!args.isEmpty()) {
                    if (is(args.peek(), "r", "-relative")) {
                        args.poll();
                        USocket.send("l r");
                    }
                } else
                    USocket.send("l");

            } else if(is(current, "h", "-help")) {
                File file;
                if(!args.isEmpty() && !isFlag(args.peek()))
                    file = new File(Main.DATA_PATH + "/lang/help/" + args.poll().toLowerCase(Locale.ROOT) + ".man");
                else
                    file = new File(Main.DATA_PATH + '/' + Config.get("help_file", "lang/help/main.man"));
                if (file.exists()) {
                    BufferedReader reader = new BufferedReader(new FileReader(file));
                    String line;
                    while ((line = reader.readLine()) != null)
                        System.out.println(line);
                } else {
                    System.err.println("[ERROR] CONFIG_NOT_FOUND: Help file not found at: \"" + file.getAbsolutePath() + "\"! Falling back to default text.");
                    System.out.println(
                        "alarm -a [--add] [-t] 7y 7d 7s [-n, --name] <name>\n" +
                            "alarm -d [--delete] [-n, --name] <name>\n" +
                            "alarm -l [--list] [-r, --relative]\n" +
                            "alarm -h [--help]");
                    System.err.println("You can likely find the help file at path: \"" + file.getAbsolutePath() + '"');
                }
            } else if (is(current, "s", "-show")){
                USocket.send("s");
            } else {
                if(isFlag(current))
                    error(ErrorType.UNEXPECTED_FLAG, current);
                else
                    error(ErrorType.UNEXPECTED_ARG, current);
//                    System.err.println("The argument: '" + current + "' isn't wanted (and loved)!");
            }
            time = new StringBuilder(18);
            name = null;
        }

    }




    private static boolean is(String string, String... strs){
        if(string.charAt(0) != '-') return false;
        for(String str : strs){
            if(string.toLowerCase(Locale.ROOT).contains(str.toLowerCase(Locale.ROOT)))
                return true;
        }
        return false;
    }

    private static boolean isFlag(String string){
        try {
            return string.charAt(0) == '-' || string.charAt(1) == '-';
        } catch(Exception ignored){ // Null Dereference, IndexOutOfBounds
            return false;
        }
    }

    private static void error(ErrorType type, String contextClue){
        errorDetected = true;
        System.err.print("[ERROR] " + type.toString() + ": ");
        switch(type){
            case UNEXPECTED_FLAG:
                System.err.println("The flag/cmd: '" + contextClue + "' is unexpected!");
                break;
            case UNEXPECTED_ARG:
                System.err.println("The argument: '" + contextClue + "' is unexpected!");
                break;
            case REDUNDANT_FLAG:
                System.err.println("The flag/cmd: '" + contextClue + "' is redundant!");
                break;
            case REDUNDANT_ARG:
                System.err.println("The argument: '" + contextClue + "' is redundant!");
                break;
            case MISSING_ARG:
                System.err.println("The argument for: '" + contextClue + "' is missing!");
                break;
        }

    }

    public static void printHelpIfShould(){
        if(errorDetected)
            System.err.println("        Please use -h or --help for commands");
        errorDetected = false;


    }


}
