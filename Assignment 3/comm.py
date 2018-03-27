#!/usr/local/cs/bin/python3

import sys, locale, string
from argparse import ArgumentParser

class comm:
    def __init__(self, file1, file2, parser, opt1, opt2, opt3, u):
        self.sup_col_1 = opt1
        self.sup_col_2 = opt2
        self.sup_col_3 = opt3
        self.no_check_unsorted = u
        # Create the three column output lists if not suppressed
        if not self.sup_col_1:
            self.col1 = list()
        if not self.sup_col_2:
            self.col2 = list()
        if not self.sup_col_3:
            self.col3 = list()

        self.parser = parser
        if (file1 == "-" and file2 == "-"):
            parser.error("Both arguments cannot be '-', only one may be"
                          + "read from standard input")
        elif file1 == "-":
            self.f1Lines = sys.stdin.readlines()
            f2 = open(file2, 'r')
            self.f2Lines = f2.readlines()
            f2.close()
        elif file2 == "-":
            self.f2Lines = sys.stdin.readlines()
            f1 = open(file1, 'r')
            self.f1Lines = f1.readlines()
            f1.close()
        else:
            f1 = open(file1, 'r')
            f2 = open(file2, 'r')
            self.f1Lines = f1.readlines()
            self.f2Lines = f2.readlines()
            f1.close()
            f2.close()

    def stripTrailingNewlines(self, inpList):
        for i in range(len(inpList)):
            inpList[i] = inpList[i].rstrip('\n')

    def isSorted(self, inpList):
        for i in range(len(inpList)-1):
            if locale.strcoll(inpList[i], inpList[i+1]) > 0:
                return False
        return True

    def compareFiles(self):
        # Finding items unique to FILE1 and common between FILE1 & FILE2
        if not self.sup_col_1:
            for f1Line in self.f1Lines:
                flag = True
                for f2Line in self.f2Lines:
                    # Using the current locale settings to compare string
                    if locale.strcoll(f1Line, f2Line) == 0:
                        flag = False
                        if not self.sup_col_3:
                            self.col3.append(f1Line)
                        break
                if flag:
                    self.col1.append(f1Line)
            self.stripTrailingNewlines(self.col1)
            if not self.sup_col_3:
                self.stripTrailingNewlines(self.col3)

        # Finding items unique to FILE2
        if not self.sup_col_2:
            for f2Line in self.f2Lines:
                flag = True
                for f1Line in self.f1Lines:
                    # Using the current locale settings to compare string
                    if locale.strcoll(f2Line, f1Line) == 0:
                        flag = False
                        break
                if flag:
                    self.col2.append(f2Line)
            self.stripTrailingNewlines(self.col2)
        self.writeOutput()

    def getMaxNumberOfOutputLines(self):
        c1 = c2 = c3 = 0
        if not self.sup_col_1:
            c1 = len(self.col1)
        if not self.sup_col_2:
            c2 = len(self.col2)
        if not self.sup_col_3:
            c3 = len(self.col3)
        return max(c1, c2, c3)

    def writeOutput(self):
        output = list([''] * self.getMaxNumberOfOutputLines())
        if not self.sup_col_1:
            for i in range(len(self.col1)):
                output[i] = self.col1[i] + "\t"
            for i in range(len(self.col1), len(output)):
                output[i] = "\t"
        if not self.sup_col_2:
            for i in range(len(self.col2)):
                output[i] = output[i] + self.col2[i] + "\t"
            for i in range(len(self.col2), len(output)):
                output[i] = "\t"
        if not self.sup_col_3:
            for i in range(len(self.col3)):
                output[i] = output[i] + self.col3[i] + "\t"
        for i in range(len(output)):
            sys.stdout.write(output[i] + "\n")


    def execute(self):
        if not self.no_check_unsorted:
            # Checking if both files are sorted
            if not self.isSorted(self.f1Lines):
                self.parser.error("FILE1 is not in sorted order.")
            elif not self.isSorted(self.f2Lines):
                self.parser.error("FILE2 is not in sorted order.")
        self.compareFiles()

def main():
    version_msg = "%(prog)s 1.0"
    usage_msg = """%(prog)s [OPTION]... FILE1 FILE2

Compare sorted files FILE1 and FILE2 line by line.

When FILE1 or FILE2 (not both) is -, read standard input.

With no options, produce three-column output.  Column one contains
lines unique to FILE1, column two contains lines unique to FILE2,
and column three contains lines common to both files.

  -1              suppress column 1 (lines unique to FILE1)
  -2              suppress column 2 (lines unique to FILE2)
  -3              suppress column 3 (lines that appear in both files)
  -u              do not check that input is correctly sorted

Examples:
  comm -12 file1 file2
         Print only lines present in both file1 and file2.
  comm -3 file1 file2
         Print lines in file1 not in file2, and vice versa."""

    parser = ArgumentParser()
    parser.add_argument('-v', '--version', action='version',
                    version=version_msg,
                    help="Show program's version number and exit.")
    parser.add_argument("-1", action="store_true", dest="suppress1",
                      default=False,
                      help="Suppress column 1 (lines unique to FILE1)")
    parser.add_argument("-2", action="store_true", dest="suppress2",
                      default=False,
                      help="Suppress column 2 (lines unique to FILE2)")
    parser.add_argument("-3", action="store_true", dest="suppress3",
                      default=False,
                      help="Suppress column 3(lines that appear in both files)")
    parser.add_argument("-u", action="store_true", dest="unsorted",
                        default=False,
                        help="Do not check that input is correctly sorted.")
    parser.add_argument("file1", action="store", type=str,
                        help="Path to the 1st file to be compared.")
    parser.add_argument("file2", action="store", type=str,
                        help="Path to the 2nd file to be compared.")

    options = parser.parse_args()

    try:
        sup_col_1 = bool(options.suppress1)
        sup_col_2 = bool(options.suppress2)
        sup_col_3 = bool(options.suppress3)
        check_unsorted = bool(options.unsorted)
    except:
        parser.error("Invalid options.")

    file1 = options.file1
    file2 = options.file2
    generator = comm(file1, file2, parser,
                     sup_col_1, sup_col_2, sup_col_3,
                     check_unsorted)
    generator.execute()


if __name__ == "__main__":
    main()
