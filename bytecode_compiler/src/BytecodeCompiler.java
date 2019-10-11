import java.io.IOException;
import java.io.FileNotFoundException;

public class BytecodeCompiler {
    public static void main(String args[]) {
        String inputFile, outputFile;
        
        if (2 != args.length) {
            System.out.println("usage: [arg1:input-file] [arg2:output-file]");
        } else {
            inputFile = args[0];
            outputFile = args[1];

            /*
            String str = "M [3, I, I, L] [1, I]";
            Scanner scn = new Scanner(str);

            while(scn.hasNext()) {
                String type = scn.next();
                if ("M".equals(type)) {
                    System.out.println(type);
                    
                    scn.useDelimiter("\n");
                    String tillEnd = scn.next();
                    Pattern ptrn = Pattern.compile("\\[(.*?)\\]");
                    Matcher matcher = ptrn.matcher(tillEnd);
                    matcher.find();
                    String localsStr = matcher.group();
                    localsStr = localsStr.substring(1, localsStr.length() - 1);
                    matcher.find();
                    String argsStr = matcher.group();
                    System.out.println(localsStr);
                    System.out.println(argsStr);
                }
            }
            if (true) {
                return;
            }
            */

            try {
                Compiler2 compiler = new Compiler2(inputFile, outputFile);
                compiler.compile();
            } catch (FileNotFoundException e) {
                System.out.println("file not found: " + inputFile);
                System.exit(1);
            } catch (IOException e) {
                System.out.println("invalid file name: " + outputFile);
                System.exit(1);
            } catch (Compiler2.IllegalOpcodeException e) {
                System.out.println(e);
                System.exit(1);
            }
        }
    }
}