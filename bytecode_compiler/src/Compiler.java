import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.IOException;
import java.util.Map;
import java.util.Scanner;
import java.util.HashMap;

public class Compiler {
    private static final int MAGIC_NUMBER = 0xBABEFACE;
    private String inputFilename = null;
    private String outputFilename = null;
    private FileOutputStream output;
    private Map<String, Opcode> opcodes = new HashMap<>();
    private Map<String, Character> types = new HashMap<>();

    public Compiler(String inputFilename, String outputFilename) {
        this.inputFilename = inputFilename;
        this.outputFilename = outputFilename;
        
        initOpcodes();
        initTypes();
    }

    public void compile() 
    throws IOException, IllegalOpcodeException, FileNotFoundException {
        Scanner scn = null;
        output = new FileOutputStream(outputFilename);

        scn = new Scanner(new FileReader(inputFilename));
        output.write(getIntBytes(MAGIC_NUMBER)); // write magic number

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
        output.close();
        scn.close();
    }

    public void initOpcodes() {
        opcodes.put("@", new Opcode(0xFF, (scn, code) -> skip(scn)));
        
        opcodes.put("noop", new Opcode(0x00, (scn, code) -> writeNoArgOpcode(code)));
        
        opcodes.put("const", new Opcode(0x01, (scn, code) -> writeSingleIntOpcode(scn, code)));

        opcodes.put("local", new Opcode(0x02, (scn, code) -> writeSingleIntOpcode(scn, code)));

        opcodes.put("halt", new Opcode(0x03, (scn, code) -> writeSingleIntOpcode(scn, code)));

        opcodes.put("stop", new Opcode(0x04, (scn, code) -> writeNoArgOpcode(code)));

        opcodes.put("iconst", new Opcode(0x51, (scn, code) -> writeSingleIntOpcode(scn, code)));

        opcodes.put("sconst", new Opcode(0x53, (scn, code) -> {
            output.write((byte)code);
            String str = scn.nextLine().trim();
            char[] cStr = str.subSequence(1, str.length()).toString().toCharArray();
            cStr[cStr.length - 1] = (char)0x0;
            output.write(new String(cStr).getBytes());
        }));

        opcodes.put("ilocal", new Opcode(0x18, (scn, code) -> writeSingleIntOpcode(scn, code)));

        opcodes.put("slocal", new Opcode(0x34, (scn, code) -> writeSingleIntOpcode(scn, code)));

    }

    public void initTypes() {
        types.put("INTEGER_TYPE",   (char)0x01);
        types.put("FLOAT_TYPE",     (char)0x02);
        types.put("STRING_TYPE",    (char)0x03);
        types.put("REFERENCE_TYPE", (char)0x04);
    }

    public static class IllegalOpcodeException extends Exception {
        private static final long serialVersionUID = -5139052286656814625L;

        public IllegalOpcodeException(String msg) {
            super(msg);
        }
    }

    private byte[] getIntBytes(int val) {
        return new byte[] {
              (byte)(0xFF & val)
            , (byte)(((0xFF << 8) & val) >> 8)
            , (byte)(((0xFF << 16) & val) >> 16)
            , (byte)(((0xFF << 24) & val) >> 24)
        };
    }

    private void writeNoArgOpcode(int code) throws IOException {
        output.write((byte)code);
    }

    private void writeSingleIntOpcode(Scanner scn, int code) throws IOException {
        output.write((byte)code);
        int size = scn.nextInt();
        output.write(getIntBytes(size));
    }

    private void skip(Scanner scn) {
        scn.nextLine();
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