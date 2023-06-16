package me.Ult1;

import org.yaml.snakeyaml.DumperOptions;
import org.yaml.snakeyaml.Yaml;

import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.nio.file.StandardCopyOption;
import java.util.HashMap;

public class Config {

    private static boolean configEdited = false;

    private static HashMap<String, Object> config = new HashMap<>(10);

    private static String configPath;

    public static void init() throws IOException {
        configPath = Main.DATA_PATH + "/config.yml";
        if(!new File(configPath).exists())
            Files.createFile(Paths.get(configPath));

        Yaml yaml = new Yaml();
        config = yaml.load(new FileReader(configPath));
    }

    public static void update() throws IOException {

        if(!configEdited)
            return;
        configEdited = false;

        Files.copy (
            Paths.get(configPath),
            Paths.get(Main.DATA_PATH + "config.yml.bak"),
            StandardCopyOption.REPLACE_EXISTING
        );

        FileWriter writer = new FileWriter(configPath);

        DumperOptions dumperOpts = new DumperOptions();
        dumperOpts.setPrettyFlow(true);
        dumperOpts.setDefaultFlowStyle(DumperOptions.FlowStyle.BLOCK);

        Yaml yaml = new Yaml(dumperOpts);
        yaml.dump(config, writer);

    }

    public static void forceUpdate() throws IOException {
        boolean temp = configEdited;
        configEdited = true;
        update();
        configEdited = temp;
    } // I forgot why I made this function, and I am scared to delete it. Please help!


    // I made this whole elaborate pathing system, and then I just never used it... Lol...
    public static Object get(String path, Object _default){
        String[] paths = path.split("\\.");

        Object current = config;
        for (int i = 0; i < paths.length; i++) {
            current = ((HashMap<String, Object>) current).get(paths[i]);

            if(current == null){
                put(path, _default);
                if(i == paths.length - 1)
                    return _default;
                else
                    current = new HashMap<String, Object>();
            }

            if(i == paths.length - 1)
                return current;
        }

        return _default;
    }

    public static void put(String path, Object object){
        configEdited = true;

        String[] paths = path.split("\\.");

        HashMap<String, Object> previous = config;
        for (int i = 0; i < paths.length; i++) {
            if(i < paths.length - 1){
                HashMap<String, Object> current = (HashMap<String, Object>) previous.get(paths[i]);
                if(current == null) {
                    previous.put(paths[i], new HashMap<>());
                    i --; // retry    this loop
                } else
                    previous = current;

            } else
                previous.put(paths[i], object);
        }

    }

}
