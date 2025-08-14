---
name: 🐛 Bug Report
about: Create a report to help us improve FreshLogger
title: '[BUG] '
labels: ['bug', 'needs-triage']
assignees: ['omerrbbulut']
---

## 🐛 Bug Description
A clear and concise description of what the bug is.

## 🔍 Steps to Reproduce
1. Go to '...'
2. Click on '....'
3. Scroll down to '....'
4. See error

## ✅ Expected Behavior
A clear and concise description of what you expected to happen.

## ❌ Actual Behavior
A clear and concise description of what actually happened.

## 📱 Environment
- **OS**: [e.g. Ubuntu 22.04, Windows 11, macOS 13]
- **Compiler**: [e.g. GCC 11, Clang 14, MSVC 2019]
- **C++ Standard**: [e.g. C++17, C++20]
- **FreshLogger Version**: [e.g. 1.0.0]
- **spdlog Version**: [e.g. 1.12.0]

## 📋 Code Example
```cpp
#include "Logger.hpp"

int main() {
    Logger::Config config;
    config.logFilePath = "test.log";
    
    Logger logger(config);
    logger.info("This causes the bug");
    
    return 0;
}
```

## 📊 Error Messages
```
Paste any error messages or logs here
```

## 🔧 Additional Context
Add any other context about the problem here.

## 📝 Checklist
- [ ] I have searched existing issues for duplicates
- [ ] I have provided a minimal reproduction example
- [ ] I have included environment details
- [ ] I have included error messages/logs

---

**Thank you for helping improve FreshLogger!** 🌱✨ 