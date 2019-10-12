import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.IOException;
import java.util.*;

public class Compiler {
    private static final int MAGIC_NUMBER = 0xBABEFACE;
    private String inputFilename = null;
    private String outputFilename = null;
    private FileOutputStream output = null;
    private List<Byte> outputBytes = new ArrayList<>();
    private Map<String, Opcode> opcodes = new HashMap<>();
    private Map<String, VMType> types = new HashMap<>();
    private Map<String, List<Byte>> methods = new HashMap<>();
    private List<List<Byte>> constantPool = new ArrayList<>();

    public Compiler(String inputFilename, String outputFilename) {
        this.inputFilename = inputFilename;
        this.outputFilename = outputFilename;
        
        initOpcodes();
        initTypes();
    }

    public void compile() 
    throws IOException, IllegalOpcodeException, FileNotFoundException, ConstantPoolException {
        Scanner scn = null;
        int opcodeIndex = 0;

        output = new FileOutputStream(outputFilename);
        scn = new Scanner(new FileReader(inputFilename));

        // write magic number
        output.write(getIntBytes(MAGIC_NUMBER));

        // build constant pool
        buildConstantPool(scn);

        while (scn.hasNext()) {
            String opcodeName = scn.next();
            Opcode curOpcode = opcodes.get(opcodeName);
            
            if (null == curOpcode) {
                // check if it's a method label, if it's not, it's an unknown opcode
                if (-1 == opcodeName.indexOf(':', 0)) { 
                    throw new IllegalOpcodeException("unknown opcode: " + opcodeName);
                }
                
                addMethodLocation(opcodeName, opcodeIndex);
            } else {
                curOpcode.process(scn);
                
                if (0xFF != curOpcode.getOpcode()) {
                    ++opcodeIndex;
                }
            }
        }

        output.write((byte)constantPool.size());

        for (List<Byte> entry : constantPool) {
            for (Byte b : entry) {
                output.write(b.byteValue());
            }
        }

        for (Byte b : outputBytes) {
            output.write(b);
        }

        output.flush();
        output.close();
        scn.close();
    }

    private void addMethodLocation(String methodName, int opcodeIndex) throws ConstantPoolException {
        List<Byte> output = null;

        methodName = methodName.substring(0, methodName.length() - 1);
        output = methods.get(methodName);
        
        if (null == output) {
            throw new ConstantPoolException("method: '" + methodName + "' not found in constant pool, file: "
                + inputFilename);
        }

        for (Byte b : getIntBytes(opcodeIndex)) {
            output.add(b);
        }
    }

    private void buildConstantPool(Scanner scn) throws ConstantPoolException, IOException {
        int constantPoolSize = 0;

        if (!scn.next().contentEquals("const")) {
            throw new ConstantPoolException("no constant pool was found in: " + inputFilename);
        }
        
        if (!scn.hasNextInt()) {
            throw new ConstantPoolException("missing constant pool size in: " + inputFilename);
        }

        constantPoolSize = scn.nextInt();
        
        for (int i = 0; i < constantPoolSize; ++i) {
            List<Byte> entry = new ArrayList<>();

            String typeIdentifier = scn.next();
            VMType type = resolveType(typeIdentifier);

            type.process(scn, entry);
            constantPool.add(entry);
        }
    }

    private void initOpcodes() {
        /* special operations */
        opcodes.put("@", new Opcode(0xFF, (scn, code) -> skip(scn)));
        opcodes.put("noop", new Opcode(0x00, (scn, code) -> writeNoArgOpcode(code)));
        opcodes.put("halt", new Opcode(0x01, (scn, code) -> writeSingleIntOpcode(scn, code)));
        opcodes.put("stop", new Opcode(0x02, (scn, code) -> writeNoArgOpcode(code)));
        opcodes.put("pop", new Opcode(0x03, (scn, code) -> writeNoArgOpcode(code)));
        opcodes.put("call", new Opcode(0x04, (scn, code) -> writeSingleIntOpcode(scn, code)));
        opcodes.put("ret", new Opcode(0x05, (scn, code) -> writeNoArgOpcode(code)));

        /* integer operations */
        opcodes.put("iload", new Opcode(0x10, (scn, code) -> writeSingleIntOpcode(scn, code)));
        opcodes.put("istore", new Opcode(0x11, (scn, code) -> writeSingleIntOpcode(scn, code)));
        opcodes.put("ipush", new Opcode(0x12, (scn, code) -> writeSingleIntOpcode(scn, code)));
        opcodes.put("iadd", new Opcode(0x13, (scn, code) -> writeNoArgOpcode(code)));
        opcodes.put("isub", new Opcode(0x14, (scn, code) -> writeNoArgOpcode(code)));
        opcodes.put("imult", new Opcode(0x15, (scn, code) -> writeNoArgOpcode(code)));
        opcodes.put("idiv", new Opcode(0x16, (scn, code) -> writeNoArgOpcode(code)));
        opcodes.put("ineg", new Opcode(0x17, (scn, code) -> writeNoArgOpcode(code)));
        opcodes.put("iprint", new Opcode(0x18, (scn, code) -> writeNoArgOpcode(code)));
        opcodes.put("iret", new Opcode(0x19, (scn, code) -> writeNoArgOpcode(code)));       
        
        /* string operations */
        opcodes.put("sload", new Opcode(0x30, (scn, code) -> writeSingleIntOpcode(scn, code)));
        opcodes.put("sstore", new Opcode(0x31, (scn, code) -> writeSingleIntOpcode(scn, code)));
        opcodes.put("sprint", new Opcode(0x32, (scn, code) -> writeNoArgOpcode(code)));
        opcodes.put("sret", new Opcode(0x33, (scn, code) -> writeNoArgOpcode(code)));
        
        /* constant pool operations */
        opcodes.put("cload", new Opcode(0x50, (scn, code) -> writeSingleIntOpcode(scn, code)));
    }

    private void initTypes() {
        // byte
        types.put("B", new VMType(0x01, (scn, type, output) -> {
            output.add((byte)type);
            int intVal = scn.nextInt();
            output.add(getIntBytes(intVal)[0]);
        })); 
        
        // integer
        types.put("I", new VMType(0x02, (scn, type, output) -> {
            output.add((byte)type);
            int intVal = scn.nextInt();
            for (Byte b : getIntBytes(intVal)) {
                output.add(b);
            }
        })); 
        
        // float
        types.put("F", new VMType(0x03, (scn, type, output) -> {
            output.add((byte)type);
        })); 
        
        // long
        types.put("L", new VMType(0x04, (scn, type, output) -> {
            output.add((byte)type);
        })); 
        
        // double
        types.put("D", new VMType(0x05, (scn, type, output) -> {
            output.add((byte)type);
        })); 
        
        // string
        types.put("S", new VMType(0x06, (scn, type, output) -> {
            output.add((byte)type);
            for (Byte b : getStringBytes(scn.nextLine())) {
                output.add(b);
            }
        })); 
        
        // reference
        types.put("R", new VMType(0x07, (scn, type, output) -> {
            output.add((byte)type);
        })); 
        
        // method
        types.put("M", new VMType(0x08, (scn, type, output) -> {
            output.add((byte)type);

            String name = scn.next();
            byte[] nameBytes = getStringBytes(name);

            for (byte b : nameBytes) {
                output.add(b);
            }

            name = name.substring(1, name.length() - 1); // clear ""
            methods.put(name, output);

            VMType retType = resolveType(scn.next());

            output.add((byte)retType.getType());

            String locals = scn.next();
            int numLocals = Integer.parseInt(String.valueOf(locals.charAt(0)));

            output.add((byte)numLocals);

            for (int i = 1; i <= numLocals; ++i) {
                VMType cur = resolveType(String.valueOf(locals.charAt(i)));
                output.add((byte)cur.getType());
            }

            String args = scn.next();
            int numArgs = Integer.parseInt(String.valueOf(args.charAt(0)));

            output.add((byte)numArgs);
            for (int i = 1; i <= numArgs; ++i) {
                VMType cur = resolveType(String.valueOf(args.charAt(i)));
                output.add((byte)cur.getType());
            }
        })); 
    }


    public static class IllegalOpcodeException extends Exception {
        private static final long serialVersionUID = -5139052286656814625L;

        public IllegalOpcodeException(String msg) {
            super(msg);
        }
    }

    public static class ConstantPoolException extends Exception {
        private static final long serialVersionUID = -5139052286656814625L;

        public ConstantPoolException(String msg) {
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

    private byte[] getStringBytes(String str) {
        str = str.trim();
        char[] cStr = str.subSequence(1, str.length()).toString().toCharArray();
        cStr[cStr.length - 1] = (char)0x0;
       
        return new String(cStr).getBytes();
    }

    private VMType resolveType(String typeName) throws ConstantPoolException {
        VMType type = types.get(typeName);

        if (null == type) {
            throw new ConstantPoolException("unknown type: '" + typeName + "' in file: " + inputFilename);
        }

        return type;
    }

    private void writeNoArgOpcode(int code) throws IOException {
        for (Byte b : getIntBytes(code)) {
            outputBytes.add(b);
        }
        
        for (Byte b : getIntBytes(0x0)) {
            outputBytes.add(b);
        }
    }

    private void writeSingleIntOpcode(Scanner scn, int code) throws IOException {
        for (Byte b : getIntBytes(code)) {
            outputBytes.add(b);
        }

        for (Byte b : getIntBytes(scn.nextInt())) {
            outputBytes.add(b);
        }
    }

    private void skip(Scanner scn) {
        scn.nextLine();
    }

    private static class Opcode {
        private int opcode;
        private OpcodeProcessor opcodeProcessor;

        public Opcode(int opcode, OpcodeProcessor opcodeProcessor) {
            this.opcode = opcode;
            this.opcodeProcessor = opcodeProcessor;
        }

        public int getOpcode() {
            return opcode;
        }

        public void process(Scanner scn) throws IOException {
            opcodeProcessor.process(scn, getOpcode());
        }
    }

    private interface OpcodeProcessor {
        void process(Scanner input, int code) throws IOException;
    }

    private static class VMType {
        private int type;
        private VMTypeProcessor typeProcessor;

        public VMType(int type, VMTypeProcessor typeProcessor) {
            this.type = type;
            this.typeProcessor = typeProcessor;
        }

        public int getType() {
            return type;
        }

        public void process(Scanner scn, List<Byte> output) throws IOException, ConstantPoolException {
            typeProcessor.process(scn, getType(), output);
        }
    }

    private interface VMTypeProcessor {
        void process(Scanner input, int type, List<Byte> output) throws IOException, ConstantPoolException;
    }
}
