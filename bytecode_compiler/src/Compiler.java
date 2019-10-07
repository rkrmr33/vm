import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.util.Map;
import java.util.Scanner;
import java.util.function.Consumer;
import java.util.HashMap;

public class Compiler {
    private FileReader input;
    private FileWriter output;
    private Map<String, InputProcessor> opcodes = new HashMap<>();
    private Map<String, Integer> types = new HashMap<>();

    private interface InputProcessor {
        void process(Scanner input) throws IOException, UnknownTypeException;
    }

    public Compiler(String inputFilename, String outputFilename) 
    throws FileNotFoundException, IOException {
        input = new FileReader(inputFilename);
        output = new FileWriter(outputFilename);

        initOpcodes();
        initTypes();
    }

    public void compile() 
    throws IOException, UnknownTypeException, IllegalOpcodeException {
        int curByte = 0;
        Scanner scn = new Scanner(input);

        output.write(""); // clear file

        while (scn.hasNextLine()) {
            String nextOpcode = scn.next();
            InputProcessor inputProcessor = opcodes.get(nextOpcode);

            if (null == inputProcessor) {
                throw new IllegalOpcodeException("unknown opcode: " + nextOpcode);
            } else {
                inputProcessor.process(scn);
            }
        }

        System.out.println("Compilation finished!");
        output.flush();
    }

    public void initOpcodes() {
        opcodes.put("@", (scn) -> scn.nextLine());
        
        opcodes.put("noop",   (scn) -> output.append((char)0x00));
        
        opcodes.put("const",  (scn) -> { 
            output.append((char)0x01);
            String typeName = scn.next();
            try {
                int type = types.get(typeName);
                switch (type) {
                    case 0x01: // INTEGER_TYPE
                        int val = scn.nextInt();
                        output.append(new String(getIntBytes(val))); 
                        break;
                    case 0x02: // FLOAT_TYPE
                        break;
                    case 0x03: // STRING_TYPE
                        String str = scn.nextLine().trim();
                        char[] cStr = str.subSequence(1, str.length()).toString().toCharArray();
                        cStr[cStr.length - 1] = (char)0x0;
                        output.append(new String(cStr));
                        break;
                }
            } catch (NullPointerException e) {
                throw new UnknownTypeException("unknown type: " + typeName);
            }
        });
        // opcodes.put("local",  0x02);

        // opcodes.put("iload",  0x10);
        // opcodes.put("istore", 0x11);
        // opcodes.put("iadd",   0x12);
        // opcodes.put("isub",   0x13);
        // opcodes.put("imult",  0x14);
        // opcodes.put("idiv",   0x15);
        // opcodes.put("ineg",   0x16);
        // opcodes.put("iprint", 0x17);

        // opcodes.put("cload",  0x20);
    }

    public void initTypes() {
        types.put("INTEGER_TYPE",   0x01);
        types.put("FLOAT_TYPE",     0x02);
        types.put("STRING_TYPE",    0x03);
        types.put("REFERENCE_TYPE", 0x04);
    }

    public static class UnknownTypeException extends Exception {
        public UnknownTypeException(String msg) {
            super(msg);
        }
    }

    public static class IllegalOpcodeException extends Exception {
        public IllegalOpcodeException(String msg) {
            super(msg);
        }
    }

    private char[] getIntBytes(int val) {
        return new char[] {
            (char)(0xFF & val)
            , (char)(((0xFF << 8) & val) >> 8)
            , (char)(((0xFF << 16) & val) >> 16)
            , (char)(((0xFF << 24) & val) >> 24)
        };
    }
}