#!/usr/bin/python

import os, sys, getopt

class Global:
    CPU_USAGE_TIME_FACTOR                                           = 10000;
    TIMESTAMP_BROKE_DELTA                                           = 60 * CPU_USAGE_TIME_FACTOR;

    CPU_USAGE_OVERFLOW_MAX_LIMIT_DEFAULT                            = 90;                                   # >= 90%
    CPU_USAGE_OVERFLOW_MIN_TIME_DEFAULT                             = 30 * CPU_USAGE_TIME_FACTOR;           # lasted at least 30 seconds

    arg_inputCpuSummaryFilePath = '';
    arg_inputCpuUsageFilePath = '';
    arg_inputCpuUsageTimestampFilePath = '';
    arg_outputResultFilePath = '';
    arg_cpuUsageOverflowMaxLimit = CPU_USAGE_OVERFLOW_MAX_LIMIT_DEFAULT;
    arg_cpuUsageOverflowMinTime = CPU_USAGE_OVERFLOW_MIN_TIME_DEFAULT;
    arg_verbose = False;

def LogInfo(message):
    sys.stdout.write(str(message));

def LogError(message):
    sys.stderr.write("*** ERROR: " + str(message));

class CpuUsageOverflowChecker:
    def __init__(self):
        self.cpuSummaryFileContentLines = [];
        self.cpuUsageFileContentLines = [];
        self.cpuUsageTimestampFileContentLines = [];
        self.outputResultFileHandle = open(Global.arg_outputResultFilePath, "w+b");

    def __del__(self):
        if self.outputResultFileHandle != None:
            self.outputResultFileHandle.close();

    def loadCpuSummaryFileToLines(self):
        self.cpuSummaryFileContentLines = [];
        f = open(Global.arg_inputCpuSummaryFilePath, "rb");
        try:
            cpuSummaryFileContentLines = [];
            cpuSummaryFileContentLines = f.readlines();
            for line in cpuSummaryFileContentLines:
                sline = line.strip("\r\n");
                self.cpuSummaryFileContentLines.append(sline);
        except:
            LogError("CpuUsageOverflowChecker.loadCpuSummaryFileToLines(self): f.readlines() is failed\n");
        finally:
            f.close();

    def loadCpuUsageFileToLines(self):
        self.cpuUsageFileContentLines = [];
        f = open(Global.arg_inputCpuUsageFilePath, "rb");
        try:
            cpuUsageFileContentLines = [];
            cpuUsageFileContentLines = f.readlines();
            for line in cpuUsageFileContentLines:
                sline = line.strip("\r\n");
                self.cpuUsageFileContentLines.append(sline);
        except:
            LogError("CpuUsageOverflowChecker.loadCpuUsageFileToLines(self): f.readlines() is failed\n");
        finally:
            f.close();

    def loadCpuUsageTimestampFileToLines(self):
        self.cpuUsageTimestampFileContentLines = [];
        f = open(Global.arg_inputCpuUsageTimestampFilePath, "rb");
        try:
            cpuUsageTimestampFileContentLines = [];
            cpuUsageTimestampFileContentLines = f.readlines();
            for line in cpuUsageTimestampFileContentLines:
                sline = line.strip("\r\n");
                self.cpuUsageTimestampFileContentLines.append(sline);
        except:
            LogError("CpuUsageOverflowChecker.loadCpuUsageTimestampFileToLines(self): f.readlines() is failed\n");
        finally:
            f.close();

    def isOvertime(self, overflowRecordIndexes):
        if len(overflowRecordIndexes) < 2:
            return False;
        if overflowRecordIndexes[0] >= len(self.cpuUsageTimestampFileContentLines):
            return False;
        lastIndex = len(overflowRecordIndexes) - 1;
        if overflowRecordIndexes[lastIndex] >= len(self.cpuUsageTimestampFileContentLines):
            return False;
        firstTime = int(self.cpuUsageTimestampFileContentLines[overflowRecordIndexes[0]]);
        lastTime = int(self.cpuUsageTimestampFileContentLines[overflowRecordIndexes[lastIndex]]);
        deltaTime = lastTime - firstTime;
        if deltaTime >= Global.arg_cpuUsageOverflowMinTime:
            return True;
        return False;

    def getTimestamp(self, index):
        if index >= len(self.cpuUsageTimestampFileContentLines):
            return 0;
        else:
            return int(self.cpuUsageTimestampFileContentLines[index]);

    def isTimestampBroken(self, ts1, ts2):
        if ts1 == 0:
            return False;
        if ts2 < ts1:
            return True;
        deltaTime = ts2 - ts1;
        if deltaTime >= Global.TIMESTAMP_BROKE_DELTA:
            return True;
        return False;

    def checkCpuUsageOverflow(self):
        overflowRecordIndexes = [];
        lastTimestamp = 0;
        for i in range(len(self.cpuUsageFileContentLines)):
            cpuUsage = float(self.cpuUsageFileContentLines[i]);
            currentTimestamp = self.getTimestamp(i);
            if cpuUsage >= Global.arg_cpuUsageOverflowMaxLimit:
                if self.isTimestampBroken(lastTimestamp, currentTimestamp):
                    if self.isOvertime(overflowRecordIndexes):
                        if Global.arg_verbose:
                            LogInfo("%s\n" % (str(overflowRecordIndexes)));
                        self.outputResultToFile(overflowRecordIndexes);
                    overflowRecordIndexes = [];
                    lastTimestamp = 0;
                else:
                    lastTimestamp = currentTimestamp;
                overflowRecordIndexes.append(i);
            else:
                if self.isOvertime(overflowRecordIndexes):
                    if Global.arg_verbose:
                        LogInfo("%s\n" % (str(overflowRecordIndexes)));
                    self.outputResultToFile(overflowRecordIndexes);
                overflowRecordIndexes = [];
                lastTimestamp = 0;
        if self.isOvertime(overflowRecordIndexes):
            if Global.arg_verbose:
                LogInfo("%s\n" % (str(overflowRecordIndexes)));
            self.outputResultToFile(overflowRecordIndexes);
        overflowRecordIndexes = [];
        lastTimestamp = 0;

    def outputResultToFile(self, overflowRecordIndexes):
        for index in overflowRecordIndexes:
            if index < len(self.cpuSummaryFileContentLines):
                try:
                    self.outputResultFileHandle.write(self.cpuSummaryFileContentLines[index] + "\n");
                except:
                    LogError("CpuUsageOverflowChecker.outputResultToFile(self): self.outputResultFileHandle.write(self.cpuSummaryFileContentLines[index]) is failed\n");
            else:
                LogError("CpuUsageOverflowChecker.outputResultToFile(self): index=%d is too big\n" % (index));
        try:
            self.outputResultFileHandle.write("\n\n");
        except:
            LogError("CpuUsageOverflowChecker.outputResultToFile(self): self.outputResultFileHandle.write(\"\n\n\") is failed\n");

    def process(self):
        self.loadCpuSummaryFileToLines();
        self.loadCpuUsageFileToLines();
        self.loadCpuUsageTimestampFileToLines();
        self.checkCpuUsageOverflow();

def usage(arg0):
    print '%s\n' \
          '  -I|--input-cpu-summary-file-path=<input_cpu_summary_file_path>\n' \
          '  -i|--input-cpu-usage-file-path=<input_cpu_usage_file_path>\n' \
          '  -T|--input-cpu-usage-timestamp-file-path=<input_cpu_usage_timestamp_file_path>\n' \
          '  -o|--output-result-file-path=<output_result_file_path>\n'\
          '  [-l|--cpu-usage-overflow-max-limit=<cpu_usage_overflow_max_limit_percents=90>]\n' \
          '  [-t|--cpu-usage-overflow-min-times=<cpu_usage_overflow_min_time_seconds=30>]\n' \
          % (os.path.basename(arg0));

def outputCmdline(argv):
    for arg in sys.argv:
        LogInfo(arg + " ");
    LogInfo("\n");

def main(argv):
    arg0 = argv[0];
    args = argv[1:];

    try:
        opts, args = getopt.getopt(args, "hI:i:T:o:l:t:V", [
            "help",
            "input-cpu-summary-file-path=",
            "input-cpu-usage-file-path=",
            "input-cpu-usage-timestamp-file-path=",
            "output-result-file-path=",
            "cpu-usage-overflow-max-limit=",
            "cpu-usage-overflow-min-time=",
            "verbose"])
    except getopt.GetoptError:
        outputCmdline(argv);
        LogError("main(): failed to parse arguments\n");
        usage(arg0);
        sys.exit(-1);
    for opt, arg in opts:
        if opt in ("-h", "--help"):
            usage(arg0);
            sys.exit(0);
        elif opt in ("-I", "--input-cpu-summary-file-path"):
            Global.arg_inputCpuSummaryFilePath = arg;
        elif opt in ("-i", "--input-cpu-usage-file-path"):
            Global.arg_inputCpuUsageFilePath = arg;
        elif opt in ("-T", "--input-cpu-usage-timestamp-file-path"):
            Global.arg_inputCpuUsageTimestampFilePath = arg;
        elif opt in ("-o", "--output-result-file-path"):
            Global.arg_outputResultFilePath = arg;
        elif opt in ("-l", "--cpu-usage-overflow-max-limit"):
            Global.arg_cpuUsageOverflowMaxLimit = int(arg);
        elif opt in ("-t", "--cpu-usage-overflow-min-time"):
            Global.arg_cpuUsageOverflowMinTime = int(arg) * Global.CPU_USAGE_TIME_FACTOR;
        elif opt in ("-V", "--verbose"):
            Global.arg_verbose = True;
        else:
            outputCmdline(argv);
            LogError("main(): argument not supported: %s\n" % (arg));
            usage(arg0);
            sys.exit(-1);

    if Global.arg_verbose:
        outputCmdline(argv);

    if len(Global.arg_inputCpuSummaryFilePath) == 0 \
        or len(Global.arg_inputCpuUsageFilePath) == 0 \
        or len(Global.arg_inputCpuUsageTimestampFilePath) == 0 \
        or len(Global.arg_outputResultFilePath) == 0:
        usage(arg0);
        sys.exit(-1);

    cpuUsageOverflowChecker = CpuUsageOverflowChecker();
    cpuUsageOverflowChecker.process();

if __name__ == "__main__":
    main(sys.argv)
