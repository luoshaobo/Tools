import sys, getopt
from xlrd import open_workbook

def isEmptyStr(s):
    trimedStr = s.strip(" \t\r\n")
    return len(s) == 0

def stripAll(s):
    trimedStr = s.strip(" \t\r\n")
    return trimedStr
    
def parseValueRange(valueRangeStr):
    valueRangeStr = valueRangeStr.strip("()")
    ss = valueRangeStr.split(".")
    resultArr = []
    for s in ss:
        if len(s) > 0:
            resultArr.append(int(s))
    if len(resultArr) != 2:
        return None
    return resultArr
    
def getIntType(valueRangeArr):
    intType = "int"
    if 0 <= valueRangeArr[0] and valueRangeArr[1] <= 0xFF:
        intType = "uint8_t"
    elif 0 <= valueRangeArr[0] and valueRangeArr[1] <= 0xFFFF:
        intType = "uint16_t"
    elif 0 <= valueRangeArr[0] and valueRangeArr[1] <= 0xFFFFFFFF:
        intType = "uint32_t"
    elif 0 <= valueRangeArr[0] and valueRangeArr[1] <= 0xFFFFFFFFFFFFFFFF:
        intType = "uint64_t"
    elif -0xFF/2 <= valueRangeArr[0] and valueRangeArr[1] <= 0xFF/2:
        intType = "int8_t"
    elif -0xFFFF/2 <= valueRangeArr[0] and valueRangeArr[1] <= 0xFFFF/2:
        intType = "int16_t"
    elif -0xFFFFFFFF/2 <= valueRangeArr[0] and valueRangeArr[1] <= 0xFFFFFFFF/2:
        intType = "int32_t"
    elif -0xFFFFFFFFFFFFFFFF/2 <= valueRangeArr[0] and valueRangeArr[1] <= 0xFFFFFFFFFFFFFFFF/2:
        intType = "int64_t"
    return intType

class PortInexEnumGenerator:
    def __init__(self):
        self.firstime = True
        self.lastDatatypeName = "_invalidDatatypeName_"
        self.portIndex = 0
    def generateLine(self, datatypeName):
        if datatypeName != self.lastDatatypeName:
            print("        // port index: %u" % (self.portIndex))
            if self.firstime:
                print("        PCI_%s = 0," % (datatypeName.upper()))
                self.firstime = False
            else:
                print("        PCI_%s," % (datatypeName.upper()))
            self.lastDatatypeName = datatypeName
            self.portIndex += 1
    def generateLast(self):
        print("        PCI_MAX")
        
class PortInfoListGenerator:
    def __init__(self):
        self.firstime = True
        self.lastDatatypeName = "_invalidDatatypeName_"
        self.lastCollectingDuration = -1
        self.portIndex = 0
    def generateLine(self, datatypeName, collectingDuration, dataElement, valueOutputTypeStr, defaultValue):
        if datatypeName != self.lastDatatypeName:
            if self.lastCollectingDuration != -1:
                print("            },");
                print("            %d," % (self.lastCollectingDuration));
                print("        },");
            print("        // port index: %u" % (self.portIndex))
            print("        {")
            print("            \"%s\"," % (datatypeName))
            print("            {")
            print("                SidebandPayload::DataItem((%s)%s, \"%s\")," % (valueOutputTypeStr, defaultValue, dataElement))
            self.portIndex += 1
        else:
            print("                SidebandPayload::DataItem((%s)%s, \"%s\")," % (valueOutputTypeStr, defaultValue, dataElement))
        self.lastDatatypeName = datatypeName
        self.lastCollectingDuration = collectingDuration
    def generateLast(self):
        if self.lastCollectingDuration != -1:
            print("            },");
            print("           %d," % (self.lastCollectingDuration));
            print("        },");
class PortDataCollectionGenerator:
    def __init__(self):
        self.firstime = True
        self.lastDatatypeName = "_invalidDatatypeName_"
        self.lastCollectingDuration = -1
        self.portIndex = 0
        self.portItemIndex = 0
    def generateLine(self, datatypeName, collectingDuration, dataElement, valueOutputTypeStr, defaultValue):
        if datatypeName != self.lastDatatypeName:
            self.portItemIndex = 0
            if self.lastCollectingDuration != -1:
                print("                };");
                print("                valid = true;");
                print("            }");
                print("            break;");
            print("        // port index: %u" % (self.portIndex))
            print("        case PayloadConfig::PCI_%s:" % (datatypeName.upper()))
            print("            {")
            print("                items = {")
            print("                   SidebandPayload::DataItem((%s)%s, m_payloadConfig.portsInfo[index].defaultItems[%u].itemName)," % (valueOutputTypeStr, str(defaultValue), self.portItemIndex))
        else:
            print("                   SidebandPayload::DataItem((%s)%s, m_payloadConfig.portsInfo[index].defaultItems[%u].itemName)," % (valueOutputTypeStr, str(defaultValue), self.portItemIndex))
        self.portItemIndex +=1
        self.lastDatatypeName = datatypeName
        self.lastCollectingDuration = collectingDuration
    def generateLast(self):
        if self.lastCollectingDuration != -1:
            print("                };");
            print("                valid = true;");
            print("            }");
            print("            break;");
            
            print("        default:");
            print("            {");
            print("                return false;");
            print("            }");
            print("            break;");


class Global:
    GEN_MODE_FULL = 0
    GEN_MODE_PORT_INDEX_ENUM = 1
    GEN_MODE_PORT_INFO_LIST = 2
    GEN_MODE_PORT_DATA_COLLECTION = 3
    
def main(genMode):
    wb = open_workbook('simple.xls')
    
    datatypeName = "_invalidDatatypeName_"
    collectingDuration = -1
    dataElement = "_invalidDataElement_"
    valueType = "_invalidValueType_"
    valueRange = "_invalidRange_"
    defaultValue = "_invalidDefaultValue_"
    
    portInexEnumGenerator = PortInexEnumGenerator()
    portInfoListGenerator = PortInfoListGenerator()
    portDataCollectionGenerator = PortDataCollectionGenerator()

    for s in wb.sheets():
        if s.name == 'BigDataSrvForClimateCtrl':
            for row in range(s.nrows):
                if row == 0:
                    continue
                
                if not isEmptyStr(str(s.cell(row,1).value)):
                    datatypeName = stripAll(str(s.cell(row,1).value))
                if not isEmptyStr(str(s.cell(row,4).value)):
                    collectingDuration = int(float(stripAll(str(s.cell(row,4).value))))
                if not isEmptyStr(str(s.cell(row,5).value)):
                    dataElement = stripAll(str(s.cell(row,5).value))
                if not isEmptyStr(str(s.cell(row,6).value)):
                    valueType = stripAll(str(s.cell(row,6).value))
                if not isEmptyStr(str(s.cell(row,7).value)):
                    valueRange = stripAll(str(s.cell(row,7).value))
                if not isEmptyStr(str(s.cell(row,8).value)):
                    defaultValue = int(float(stripAll(str(s.cell(row,8).value))))
                
                if datatypeName == dataElement:
                    dataElement = "-"

                if valueType == "ENUMERATED":
                    valueRange = "0..255"
                elif valueType == "BOOLEAN":
                    valueRange = "0..1"
                elif valueType == "OCTET":
                    valueRange = "0..0"
                    
                valueRangeArr = parseValueRange(valueRange)
                if valueRangeArr != None:
                    valueRangeStr = "[%d, %d]" % (valueRangeArr[0], valueRangeArr[1])
                else:
                    valueRangeStr = "_invalidRange_"
                    
                valueOutputTypeStr = getIntType(valueRangeArr)
                if valueType == "BOOLEAN":
                    valueOutputTypeStr = "bool"
                elif valueType == "OCTET":
                    valueOutputTypeStr = "std::string"
                
                if genMode == Global.GEN_MODE_FULL:
                    print("%6d %35s %5d %35s %15s %30s %15s %7s" % (row+1, datatypeName, collectingDuration, dataElement, valueType, valueRangeStr, valueOutputTypeStr, defaultValue));
                elif genMode == Global.GEN_MODE_PORT_INDEX_ENUM:
                    portInexEnumGenerator.generateLine(datatypeName)
                elif genMode == Global.GEN_MODE_PORT_INFO_LIST:
                    portInfoListGenerator.generateLine(datatypeName, collectingDuration, dataElement, valueOutputTypeStr, defaultValue)
                elif genMode == Global.GEN_MODE_PORT_DATA_COLLECTION:
                    portDataCollectionGenerator.generateLine(datatypeName, collectingDuration, dataElement, valueOutputTypeStr, defaultValue)

                if row >= 30:
                    break
            if genMode == Global.GEN_MODE_FULL:
                print("")
            elif genMode == Global.GEN_MODE_PORT_INDEX_ENUM:
                portInexEnumGenerator.generateLast()
            elif genMode == Global.GEN_MODE_PORT_INFO_LIST:
                    portInfoListGenerator.generateLast()
            elif genMode == Global.GEN_MODE_PORT_DATA_COLLECTION:
                portDataCollectionGenerator.generateLast()

if __name__ == "__main__":
    if len(sys.argv) == 1:
        main(0)
    else:
        main(int(sys.argv[1]))
    