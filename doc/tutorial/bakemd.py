with open("tutorial.template.md") as f:
  for line in f:
    line = line.strip("\n")
    if line.find("    \\\\\\inputlisting{") == 0:
      epos = line.find("}")
      incfname = line[20:epos]
      with open(incfname) as incf:
        for incline in incf:
          incline = incline.strip("\n")
          print "    " + incline
    elif line.find("    \\\\\\inputlistingsnippet{") == 0:
      mpos = line.find(":")
      epos = line.find("}")
      incfname = line[27:mpos]
      incsname = line[mpos+1:epos]
      doprint = False
      didprint = False
      with open(incfname) as incf:
        for incline in incf:
          incline = incline.strip("\n")
          if incline.find("// snippet_begin:"+incsname) >= 0:
            doprint = True
          elif incline.find("// snippet_end") >= 0:
            doprint = False
          elif doprint:
            didprint = True
            print "    " + incline
      if not didprint:
        print "    WARNING: snippet "+incsname+" is empty"
    else:
      print line
