#!/usr/bin/python

import os, sys, getopt

class Global:
    CPU_USAGE_TIME_FACTOR                                           = 10000;
    TIMESTAMP_BROKE_DELTA                                           = 60 * CPU_USAGE_TIME_FACTOR;

    CPU_USAGE_OVERFLOW_MAX_LIMIT_DEFAULT                            = 90;                                   # >= 90%
    CPU_USAGE_OVERFLOW_MIN_TIME_DEFAULT                             = 30 * CPU_USAGE_TIME_FACTOR;           # lasted at least 30 seconds

    arg_inputCpuSummaryFilePath = 'stdin';
    arg_outputResultFilePath = 'stdout';
    arg_cpuUsageOverflowMaxLimit = CPU_USAGE_OVERFLOW_MAX_LIMIT_DEFAULT;
    arg_cpuUsageOverflowMinTime = CPU_USAGE_OVERFLOW_MIN_TIME_DEFAULT;
    arg_verbose = False;

def logInfo(message):
    sys.stdout.write(str(message));

def logError(message):
    sys.stderr.write("*** ERROR: " + str(message));

class CpuUsageOverflowChecker:
    def __init__(self):
        self.cpuSummaryInfoLines = [];
        self.cpuUsageInfoLines = [];
        self.cpuUsageTimestampInfoLines = [];
        if Global.arg_outputResultFilePath != "stdout":
            self.outputResultFileHandle = open(Global.arg_outputResultFilePath, "w+b");
        else:
            self.outputResultFileHandle = sys.stdout;
        self.currentIndexBase = 0;

    def __del__(self):
        self.checkCpuUsageOverflowLastPart();
        if self.outputResultFileHandle != None:
            if Global.arg_outputResultFilePath != "stdout":
                if self.outputResultFileHandle != None:
                    self.outputResultFileHandle.close();

    def removeConsumedRecords(self, newBeginningindex):
        self.cpuSummaryInfoLines = self.cpuSummaryInfoLines[newBeginningindex:];
        self.cpuUsageInfoLines = self.cpuUsageInfoLines[newBeginningindex:];
        self.cpuUsageTimestampInfoLines = self.cpuUsageTimestampInfoLines[newBeginningindex:];

    def isOvertime(self, overtimeRecordIndexes):
        if len(overtimeRecordIndexes) < 2:
            return False;
        if overtimeRecordIndexes[0] >= len(self.cpuUsageTimestampInfoLines):
            return False;
        if overtimeRecordIndexes[-1] >= len(self.cpuUsageTimestampInfoLines):
            return False;
        firstTime = int(self.cpuUsageTimestampInfoLines[overtimeRecordIndexes[0]]);
        lastTime = int(self.cpuUsageTimestampInfoLines[overtimeRecordIndexes[-1]]);
        deltaTime = lastTime - firstTime;
        if deltaTime >= Global.arg_cpuUsageOverflowMinTime:
            return True;
        return False;

    def getTimestamp(self, index):
        if index >= len(self.cpuUsageTimestampInfoLines):
            return 0;
        else:
            return int(self.cpuUsageTimestampInfoLines[index]);

    def isTimestampBroken(self, ts1, ts2):
        if ts1 == 0:
            return False;
        if ts2 < ts1:
            return True;
        deltaTime = ts2 - ts1;
        if deltaTime >= Global.TIMESTAMP_BROKE_DELTA:
            return True;
        return False;

    def printOvertimeRecordIndexes(self, overtimeRecordIndexes):
        overtimeRecordIndexesTmp = [];
        for index in overtimeRecordIndexes:
            overtimeRecordIndexesTmp.append(self.currentIndexBase + index);
        logInfo("%s\n" % (str(overtimeRecordIndexesTmp)));

    def checkCpuUsageOverflow(self):
        newBeginningindex = 0;
        overtimeRecordIndexes = [];
        lastTimestamp = 0;
        for i in range(len(self.cpuUsageInfoLines)):
            cpuUsage = float(self.cpuUsageInfoLines[i]);
            currentTimestamp = self.getTimestamp(i);
            if cpuUsage >= Global.arg_cpuUsageOverflowMaxLimit:
                if self.isTimestampBroken(lastTimestamp, currentTimestamp):
                    if self.isOvertime(overtimeRecordIndexes):
                        if Global.arg_verbose:
                            self.printOvertimeRecordIndexes(overtimeRecordIndexes);
                        self.outputResultToFile(overtimeRecordIndexes);
                    overtimeRecordIndexes = [];
                    lastTimestamp = 0;
                    newBeginningindex = i;
                else:
                    lastTimestamp = currentTimestamp;
                overtimeRecordIndexes.append(i);
            else:
                if self.isOvertime(overtimeRecordIndexes):
                    if Global.arg_verbose:
                        self.printOvertimeRecordIndexes(overtimeRecordIndexes);
                    self.outputResultToFile(overtimeRecordIndexes);
                overtimeRecordIndexes = [];
                lastTimestamp = 0;
                newBeginningindex = i + 1;
        self.removeConsumedRecords(newBeginningindex);
        self.currentIndexBase += newBeginningindex;

    def checkCpuUsageOverflowLastPart(self):
        overtimeRecordIndexes = [];
        lastTimestamp = 0;
        for i in range(len(self.cpuUsageInfoLines)):
            overtimeRecordIndexes.append(i);
        if self.isOvertime(overtimeRecordIndexes):
            if Global.arg_verbose:
                self.printOvertimeRecordIndexes(overtimeRecordIndexes);
            self.outputResultToFile(overtimeRecordIndexes);
        overtimeRecordIndexes = [];
        lastTimestamp = 0;

    def outputResultToFile(self, overtimeRecordIndexes):
        for index in overtimeRecordIndexes:
            if index < len(self.cpuSummaryInfoLines):
                try:
                    self.outputResultFileHandle.write(self.cpuSummaryInfoLines[index] + "\n");
                except:
                    logError("CpuUsageOverflowChecker.outputResultToFile(self): self.outputResultFileHandle.write(self.cpuSummaryInfoLines[index]) is failed\n");
            else:
                logError("CpuUsageOverflowChecker.outputResultToFile(self): index=%d is too big\n" % (index));
        try:
            self.outputResultFileHandle.write("\n\n");
        except:
            logError("CpuUsageOverflowChecker.outputResultToFile(self): self.outputResultFileHandle.write(\"\n\n\") is failed\n");

    def parseLine(self, line):
        lineSubstrArr = [];
        strippedline = line.strip("\r\n");
        strs = strippedline.split(" ");
        for s in strs:
            strippedStr = s.strip(" \t\r\n");
            if len(strippedStr) > 0:
                lineSubstrArr.append(strippedStr);

        if len(lineSubstrArr) == 28:
            lineSubstrArr = lineSubstrArr[1:];
        if len(lineSubstrArr) != 27:
            return False;
        if lineSubstrArr[11] != "[CPU:":
            return False;
        if lineSubstrArr[19] != "idle":
            return False;
        idleNumStr = lineSubstrArr[18];
        if idleNumStr[-1] != "%":
            return False;

        cpuUsageNum = 100.0 - float(idleNumStr[:-1]);
        timestampStr = lineSubstrArr[2];

        self.cpuSummaryInfoLines.append(strippedline);
        self.cpuUsageInfoLines.append(str(cpuUsageNum));
        self.cpuUsageTimestampInfoLines.append(timestampStr);

        return True;

    def processLine(self, line):
        if self.parseLine(line):
            self.checkCpuUsageOverflow();

def usage(arg0):
    print '%s \\\n' \
          '    [-H|--help] \\\n' \
          '    [-i|--input-cpu-summary-file-path=<input_cpu_summary_file_path=stdin>] \\\n' \
          '    [-o|--output-result-file-path=<output_result_file_path=stdout>] \\\n'\
          '    [-l|--cpu-usage-overflow-max-limit=<cpu_usage_overflow_max_limit_percents=90>] \\\n' \
          '    [-t|--cpu-usage-overflow-min-time=<cpu_usage_overflow_min_time_seconds=30>]\n' \
          '    [-v|--verbose]\n' \
          'Note:\n' \
          '    -H|--help: output this help information.\n' \
          '    -i|--input-cpu-summary-file-path=: specify the file path of the CPU summary information. default: stdin\n' \
          '    -o|--output-result-file-path=: specify the file path of the CPU usage overflow result. default: stdout\n' \
          '    -l|--cpu-usage-overflow-max-limit=: specify the high limitation of CPU usage (%%). default: 90\n' \
          '    -t|--cpu-usage-overflow-min-time=: specify the minimum time during which CPU is high (seconds). default: 30\n' \
          '    -v|--verbose: output more information, for debug purpose.\n' \
          % (os.path.basename(arg0));

def printCmdline(argv):
    for arg in sys.argv:
        logInfo(arg + " ");
    logInfo("\n");

def openInputCpuSummaryFile():
    fd = None;
    if Global.arg_inputCpuSummaryFilePath == "stdin":
        fd = sys.stdin;
    else:
        fd = open(Global.arg_inputCpuSummaryFilePath);
    return fd;

def closeInputCpuSummaryFile(fd):
    if Global.arg_inputCpuSummaryFilePath != "stdin":
        if fd != None:
            fd.close();

def main(argv):
    arg0 = argv[0];
    args = argv[1:];

    try:
        opts, args = getopt.getopt(args, "Hi:o:l:t:v", [
            "help",
            "input-cpu-summary-file-path=",
            "output-result-file-path=",
            "cpu-usage-overflow-max-limit=",
            "cpu-usage-overflow-min-time=",
            "verbose"])
    except getopt.GetoptError:
        printCmdline(argv);
        logError("main(): failed to parse arguments\n");
        usage(arg0);
        sys.exit(-1);
    for opt, arg in opts:
        if opt in ("-H", "--help"):
            usage(arg0);
            sys.exit(0);
        elif opt in ("-i", "--input-cpu-summary-file-path"):
            Global.arg_inputCpuSummaryFilePath = arg;
        elif opt in ("-o", "--output-result-file-path"):
            Global.arg_outputResultFilePath = arg;
        elif opt in ("-l", "--cpu-usage-overflow-max-limit"):
            Global.arg_cpuUsageOverflowMaxLimit = int(arg);
        elif opt in ("-t", "--cpu-usage-overflow-min-time"):
            Global.arg_cpuUsageOverflowMinTime = int(arg) * Global.CPU_USAGE_TIME_FACTOR;
        elif opt in ("-v", "--verbose"):
            Global.arg_verbose = True;
        else:
            printCmdline(argv);
            logError("main(): argument not supported: %s\n" % (arg));
            usage(arg0);
            sys.exit(-1);

    if Global.arg_verbose:
        printCmdline(argv);

    if len(Global.arg_outputResultFilePath) == 0:
        usage(arg0);
        sys.exit(-1);

    cpuUsageOverflowChecker = CpuUsageOverflowChecker();
    fd = openInputCpuSummaryFile();
    while True:
        line = "";
        try:
            line = fd.readline();
        except:
            logError("main(): sys.stdin.readline() is failed\n");
            break;
        if len(line) == 0:
            break;
        cpuUsageOverflowChecker.processLine(line);
    closeInputCpuSummaryFile(fd);

if __name__ == "__main__":
    main(sys.argv)
