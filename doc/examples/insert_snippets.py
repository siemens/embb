import os
import glob
import shutil
import sys
import string

filenames = os.listdir(".")
# Work through all files and directories
for filename in filenames:
  # If it is a directory, just add content to filenames
  if os.path.isdir(filename):
    nestedfiles = os.listdir(filename + "/")
    for i in range(len(nestedfiles)):
      nestedfiles[i] = filename + "/" + nestedfiles[i]
    filenames.extend(nestedfiles)
  # Otherwise look into file
  else:
    #sys.stdout.write("Looking at " + filename)
    is_fragmented = string.find(filename, "-fragmented.cc") != -1
    if is_fragmented:
      in_file = open(filename, 'r+')
      out_filename = string.replace(filename, "-fragmented", "")
      sys.stdout.write(filename + " -> " + out_filename + "\n")
      out_file = open(out_filename, 'w')
      for line in in_file:
        stripped_line = string.lstrip(line)
        # Check if first characters after whitespaces are include
        is_include = string.find(stripped_line, "#include") == 0
        # Check if snippet keyword is contained in include
        includes_snippet = string.find(line, "-snippet.") != -1
        if is_include and includes_snippet:
          # Get number of whitespaces for indentation of snippet
          num_whitespaces = len(line) - len(stripped_line)
          # Create Whitespace-string for indentation
          whitespaces = ""
          for i in range(num_whitespaces):
          	whitespaces += " "
          # Get include filename
          first_quotes_pos = string.find(line, '"')
          last_quotes_pos = string.rfind(line, '"')
          snippet_filename = line[first_quotes_pos + 1 : last_quotes_pos]
          sys.stdout.write("  + " + snippet_filename + "\n")
          snippet_file = open(snippet_filename, 'r')
          for snippet_line in snippet_file:
            out_file.write(whitespaces + snippet_line)
          snippet_file.close()
          #out_file.write("\n")
        else:
          out_file.write(line)
      in_file.close()
      out_file.close()     