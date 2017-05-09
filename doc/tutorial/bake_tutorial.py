#
# Copyright (c) 2014-2017, Siemens AG. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice,
# this list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright notice,
# this list of conditions and the following disclaimer in the documentation
# and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#

with open("tutorial.template.md") as f:
  for line in f:
    line = line.strip("\n")
    if line.find("    \\\\\\inputlisting{") == 0:
      epos = line.find("}")
      incfname = line[20:epos]
      with open(incfname) as incf:
        print "```cpp"
        for incline in incf:
          incline = incline.strip("\n")
          print incline
        print "```"
    elif line.find("    \\\\\\inputlistingsnippet{") == 0:
      mpos = line.find(":")
      epos = line.find("}")
      incfname = line[27:mpos]
      incsname = line[mpos+1:epos]
      doprint = False
      didprint = False
      leadingspaces = 0
      with open(incfname) as incf:
        for incline in incf:
          incline = incline.strip("\n")
          if incline.find("// snippet_begin:"+incsname) >= 0:
            print "```cpp"
            doprint = True
          elif incline.find("// snippet_end") >= 0:
            if doprint:
              print "```"
            doprint = False
          elif doprint:
            if not didprint:
              leadingspaces = len(incline) - len(incline.lstrip())
            didprint = True
            if len(incline[:leadingspaces].lstrip()) != 0:
              print "    WARNING: Invalid indentation"
            print incline[leadingspaces:]
      if not didprint:
        print "    WARNING: snippet "+incsname+" is empty"
    else:
      print line
