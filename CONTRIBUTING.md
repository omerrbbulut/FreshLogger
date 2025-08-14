# Contributing to FreshLogger

First off, thank you for considering contributing to FreshLogger! It's people like you who make FreshLogger such a great tool.

## 🚀 Quick Start

1. **Fork** the repository
2. **Clone** your fork: `git clone git@github.com:YOUR_USERNAME/FreshLogger.git`
3. **Create** a feature branch: `git checkout -b feature/amazing-feature`
4. **Make** your changes
5. **Test** your changes: `make enterprise-test`
6. **Commit** your changes: `git commit -m 'feat: add amazing feature'`
7. **Push** to your branch: `git push origin feature/amazing-feature`
8. **Open** a Pull Request

## 📋 Code of Conduct

### Our Pledge
We as members, contributors, and leaders pledge to make participation in our community a harassment-free experience for everyone.

### Our Standards
Examples of behavior that contributes to a positive environment:
- Using welcoming and inclusive language
- Being respectful of differing viewpoints and experiences
- Gracefully accepting constructive criticism
- Focusing on what is best for the community
- Showing empathy towards other community members

## 🛠️ Development Setup

### Prerequisites
- C++17 compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)
- CMake 3.10+ or Make
- spdlog library
- Google Test (for testing)

### Build Commands
```bash
# Using Make
make all                    # Build everything
make test                   # Run basic tests
make enterprise-test        # Run all tests

# Using CMake
mkdir build && cd build
cmake ..
make
make test
```

### Testing
```bash
# Run specific test suites
make simple-test           # Basic functionality tests
make unit-tests            # Unit tests
make performance-tests     # Performance benchmarks
make stress-tests          # Stress and stability tests
make edge-case-tests       # Edge case tests

# Run complete test suite
make enterprise-test
```

## 📝 Commit Message Format

We use [Conventional Commits](https://www.conventionalcommits.org/) for commit messages:

```
<type>[optional scope]: <description>

[optional body]

[optional footer(s)]
```

### Types
- **feat**: A new feature
- **fix**: A bug fix
- **docs**: Documentation only changes
- **style**: Changes that do not affect the meaning of the code
- **refactor**: A code change that neither fixes a bug nor adds a feature
- **perf**: A code change that improves performance
- **test**: Adding missing tests or correcting existing tests
- **chore**: Changes to the build process or auxiliary tools

### Examples
```
feat: add async logging support
fix: resolve memory leak in file rotation
docs: update README with new examples
test: add performance benchmarks
chore: bump version to 1.1.0
```

## 🔍 Code Quality Standards

### C++ Standards
- **C++17** minimum requirement
- Use modern C++ features where appropriate
- Prefer RAII and smart pointers
- Avoid raw pointers when possible

### Performance Requirements
- **Throughput**: >1M messages/second
- **Latency**: <1 μs average
- **Memory**: Minimal overhead
- **Thread Safety**: 100% thread-safe operations

### Testing Requirements
- **Coverage**: 100% code coverage
- **Performance**: All performance tests must pass
- **Stress**: All stress tests must pass
- **Edge Cases**: All edge case tests must pass

## 🧪 Adding New Tests

### Test Structure
```cpp
TEST_F(TestClassName, TestMethodName) {
    // Arrange
    Logger::Config config;
    config.logFilePath = "test.log";
    
    // Act
    Logger logger(config);
    logger.info("test message");
    
    // Assert
    EXPECT_TRUE(std::filesystem::exists("test.log"));
}
```

### Test Categories
1. **Unit Tests**: Test individual functions
2. **Integration Tests**: Test component interactions
3. **Performance Tests**: Test performance metrics
4. **Stress Tests**: Test under extreme conditions
5. **Edge Case Tests**: Test boundary conditions

## 📚 Documentation

### Code Documentation
- Use Doxygen-style comments for public APIs
- Include examples in comments
- Document all configuration options
- Explain complex algorithms

### User Documentation
- Keep README.md up to date
- Document breaking changes in CHANGELOG.md
- Provide usage examples
- Include troubleshooting guides

## 🚀 Release Process

### Version Bumping
- **Patch** (1.0.x): Bug fixes, no breaking changes
- **Minor** (1.x.0): New features, backward compatible
- **Major** (x.0.0): Breaking changes

### Release Checklist
- [ ] All tests pass
- [ ] Performance benchmarks meet requirements
- [ ] Documentation is up to date
- [ ] CHANGELOG.md is updated
- [ ] Version numbers are updated
- [ ] Release notes are written

## 🤝 Getting Help

- **Issues**: Use GitHub Issues for bug reports and feature requests
- **Discussions**: Use GitHub Discussions for questions and ideas
- **Wiki**: Check the wiki for detailed guides and examples

## 📄 License

By contributing to FreshLogger, you agree that your contributions will be licensed under the MIT License.

---

**Thank you for contributing to FreshLogger!** 🌱✨ 