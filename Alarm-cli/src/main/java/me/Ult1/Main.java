package me.Ult1;

import net.harawata.appdirs.AppDirs;
import net.harawata.appdirs.AppDirsFactory;

import java.io.File;
import java.io.IOException;
import java.rmi.ServerException;


public class Main {

    public static String DATA_PATH;

//    HashMap<State, Character> commandControlValues = new HashMap<State, Character>(9);

    public static void main(String[] args) throws ServerException {
        AppDirs dirs = AppDirsFactory.getInstance();
        DATA_PATH = dirs.getUserDataDir("Alarm", null, "Ult1");

        new File(DATA_PATH).mkdirs();

        try {
            Config.init();
        } catch(Exception e){ e.printStackTrace();}


        try {
            Parser.parse(args);
//            Parser.parse("-a 1y 1d 1h 1m 1s -n test0".split(" "));
//            Parser.parse("-c".split(" "));
//            Parser.parse("-c port".split(" "));

//            Parser.parse("-a 2m test1".split(" "));
//            Parser.parse("-d name_collision".split(" "));
//            Parser.parse("-a 1y 1d".split(" "));
//            Parser.parse("-d -n test2".split(" "));
//            Parser.parse("-a 1y 1d test4 -l -d test4".split(" ")); // ! NEEDS TESTING!
//            Parser.parse(new String[]{"-h"});
//            Parser.parse("-h list".split(" "));
//            Parser.parse(new String[0]);
//            Parser.parse("-l".split(" "));
//            Parser.parse(new String[]{"-s"});

        } catch(IOException e) { e.printStackTrace(); return; }


        Parser.printHelpIfShould();

//        System.out.println(Config.get("help_file", "some_path"));
//        System.out.println(Config.get("a.b.c.d", "a3"));
//        System.out.println(Config.get("a.b.c.g", "a4"));

        Runtime.getRuntime().addShutdownHook(new Thread (){
            @Override
            public void run() {
                super.run();
                try {
                    Config.update(); // Has to be at the end!
                    if(USocket.serverThread != null && USocket.serverThread.isAlive())
                        USocket.serverThread.join();
                } catch (IOException | InterruptedException e) {
                    e.printStackTrace();
                }

            }
        }); // This seems highly unsafe...
    }


}

// alarm -a [-t] 7y 7d 7s [-n] name2
// alarm -d [-n] name2
// alarm -l (list)  // which I could actually also read here...
// alarm -h         // help
// alarm -c [-h] [--port, --lang EN/LT...]( // config, maybe open, maybe let configure through console
// alarm -u         // update???

/*

    TODO: --examples ????

 * Test commands:
 *
 * alarm -ahlt 7s -n test0              # expect: adds, prints help & lists active alarms.
 * alarm -athl 7s -n test1              # expect: same as previous.
 * alarm -a -t 7s -n test2              # expect: adds an alarm.
 * alarm -a 7s -n test3 -c --open       # expect: adds an alarm, opens config.
 * alarm -a 7s -n test4 -d -n test4     # expect: adds an alarm, removes an alarm.
 * alarm --list
 * alarm --add 7s --name test5
 */


/*
* EN.man:
*
* alarm -c [--config] [key] [value]
  "alarm -c" lists options in config.
  "alarm -c key" reads the key's value.
  "alarm -c key value" sets the key's value.
*
*/