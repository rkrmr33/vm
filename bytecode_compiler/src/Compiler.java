import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.util.Map;
import java.util.HashMap;

public class Compiler {
    private FileReader input;
    private FileWriter output;
    private Map<String, Integer> opcodes = new HashMap<>();
    private Map<String, Integer> types = new HashMap<>();

    public Compiler(String inputFilename, String outputFilename) 
    throws FileNotFoundException, IOException {
        input = new FileReader(inputFilename);
        output = new FileWriter(outputFilename);

        initOpcodes();
        initTypes();
    }

    public void compile() throws IOException {
        int curByte = 0;

        while (-1 != (curByte = input.read())) {
            System.out.println(curByte);
        }
    }

    public void initOpcodes() {
        opcodes.put("noop", 0x00);
        opcodes.put("print", 0x01);

        opcodes.put("iload", 0x10);
        opcodes.put("istore", 0x11);
        opcodes.put("iadd", 0x12);
        opcodes.put("isub", 0x13);
        opcodes.put("imult", 0x14);
        opcodes.put("idiv", 0x15);
        opcodes.put("ineg", 0x16);

        opcodes.put("cload", 0x20);
    }

    public void initTypes() {
        types.put("INTEGER_TYPE", 0x01);
        types.put("FLOAT_TYPE", 0x02);
        types.put("STRING_TYPE", 0x03);
        types.put("REFERENCE_TYPE", 0x04);
    }
}