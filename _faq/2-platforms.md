---
title: Platforms
faq:
  - question: Is EMB² restricted to small embedded systems or can I also use it for server applications?
    answer: Although EMB² has been designed for embedded systems, it is not restricted to small controllers or the like. You can also use it to get the most out of &ldquo;big irons&rdquo;.
  - question: How can I port EMB² to a platform not yet supported out of the box?
    answer: Platform-specific code is contained in the base library and fenced using <code>EMBB_PLATFORM_*</code> defines. To port the code, add appropriate implementations for your platform. Please see <a href="https://github.com/siemens/embb/blob/master/CONTRIBUTING.md">CONTRIBUTING.md</a> for more details.
  - question: If you target embedded systems, why do you support Windows?
    answer: While Windows is not very common in embedded systems, it is often used for development, server applications, and human machine interfaces (e.g., panels).
---
