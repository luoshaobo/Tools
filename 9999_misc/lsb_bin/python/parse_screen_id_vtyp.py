import xml.dom.minidom;
import string;

FILE_PATH__ScreenID_vtyp = """D:\casdev\WinCE\public\MMP_PROD\_HS\HMI\Infrastructure\DataTypes\ScreenID.vtyp""";

def main():
    DoParsing();
    return 0;
    
def DoParsing():
    print "delta^priority^hibyte^lobyte^value^name"
    lastValue = 0;
    dom = xml.dom.minidom.parse(FILE_PATH__ScreenID_vtyp);
    nodes = dom.getElementsByTagName("named_value");
    for node in nodes:
        attributes = node.attributes;
        value = string.atol(attributes["value"].value);
        hibyte = value >> 8;
        lobyte = value & 0xFF;
        print "%d^%u^%u^%u^%u^%s" % (value - lastValue, 250 - hibyte, hibyte, lobyte, value, attributes["name"].value);
        lastValue = value;
        
    
if __name__ == "__main__":
     main();
