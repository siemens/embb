---
title: Development
faq:
  - question: How can I report bugs and contribute to EMB²?
    answer: Please report bugs, feature requests, etc. via <a href="https://github.com/siemens/embb/issues">GitHub</a>. Alternatively, e.g. in case of vulnerabilities, send an email to <a href="mailto:embb.info@gmail.com">embb.info@gmail.com</a>. Bug fixes, extensions, etc. can be contributed as pull requests via GitHub or as patches via mail. If possible, refer to a current snapshot of the master branch and create pull requests against the <em>development</em> branch. More detailed information can be found in <a href="https://github.com/siemens/embb/blob/master/CONTRIBUTING.md">CONTRIBUTING.md</a>.
  - question: Where is your CI server?
    answer: Due to a complex configuration supporting different compilers and operating systems, specialized tools for verification, various hardware platforms for testing (from small ARM boards to x86-based servers), and CI runtimes of more than 10 hours per night, we use a customized Jenkins server running in our internal network for most of the work. Additionally, basic builds and tests are done using <a href="https://travis-ci.org/siemens/embb">Travis CI</a>. On the long run, we would love to 'outsource' all CI jobs but this will take some time.
  - question: Which tools do you use for verification?
    answer: Besides static analysis, selected parts of the code have been formally verified using the <a href="https://divine.fi.muni.cz/">Divine</a> model checker. Moreover, we employ a <a href="https://github.com/ahorn/linearizability-checker">linearizability checker</a> to verify that our concurrent data structures behave in the same way as their sequential counterparts. However, verifying the complete source code of EMB² is not feasible with the given tools.
---
