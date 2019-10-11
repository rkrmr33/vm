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
            } catch (Compiler2.ConstantPoolException e) {
                System.out.println(e);
                System.exit(1);
            }
        }
    }
}