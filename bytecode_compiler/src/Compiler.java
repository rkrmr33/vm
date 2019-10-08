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
    private Map<String, Opcode> opcodes = new HashMap<>();
    private Map<String, Character> types = new HashMap<>();

    public Compiler(String inputFilename, String outputFilename) 
    throws FileNotFoundException, IOException {
        input = new FileReader(inputFilename);
        output = new FileWriter(outputFilename);

        initOpcodes();
        initTypes();
    }

    public void compile() 
    throws IOException, IllegalOpcodeException {
        int curByte = 0;
        Scanner scn = new Scanner(input);

        output.write(""); // clear file

        while (scn.hasNextLine()) {
            String opcodeName = scn.next();
            Opcode curOpcode = opcodes.get(opcodeName);

            if (null == curOpcode) {
                throw new IllegalOpcodeException("unknown opcode: " + opcodeName);
            } else {
                try {
                    curOpcode.process(scn);
                } catch (IOException e) {
                    System.out.println(e);
                }
            }
        }

        System.out.println("Compilation finished!");
        output.flush();
    }

    public void initOpcodes() {
        opcodes.put("@", new Opcode(0xFF, (scn, code) -> scn.nextLine()));
        
        opcodes.put("noop", new Opcode(0x00, (scn, code) -> output.append((char)code)));
        
        opcodes.put("const", new Opcode(0x01, (scn, code) -> {
            output.append((char)code);
            int size = scn.nextInt();
            output.append(new String(getIntBytes(size)));
        }));

        opcodes.put("local", new Opcode(0x02, (scn, code) -> {
            output.append((char)code);
            int size = scn.nextInt();
            output.append(new String(getIntBytes(size)));
        }));

        opcodes.put("iconst", new Opcode(0x51, (scn, code) -> { 
            output.append((char)code);
            int val = scn.nextInt();
            output.append(new String(getIntBytes(val))); 
        }));

        opcodes.put("sconst", new Opcode(0x53, (scn, code) -> {
            output.append((char)code);
            String str = scn.nextLine().trim();
            char[] cStr = str.subSequence(1, str.length()).toString().toCharArray();
            cStr[cStr.length - 1] = (char)0x0;
            output.append(new String(cStr));
        }));

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
        types.put("INTEGER_TYPE",   (char)0x01);
        types.put("FLOAT_TYPE",     (char)0x02);
        types.put("STRING_TYPE",    (char)0x03);
        types.put("REFERENCE_TYPE", (char)0x04);
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

    private static class Opcode {
        private int code;
        private InputProcessor inputProcessor;

        public Opcode(int code, InputProcessor inputProcessor) {
            this.code = code;
            this.inputProcessor = inputProcessor;
        }

        public int getCode() {
            return code;
        }

        public void process(Scanner scn) throws IOException {
            inputProcessor.process(scn, getCode());
        }
    }

    private interface InputProcessor {
        void process(Scanner input, int code) throws IOException;
    }
}