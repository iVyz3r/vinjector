# Contributing to vinjector

Thank you for considering contributing to vinjector! We welcome contributions from the community. This document provides guidelines and instructions for contributing to the project.

## Code of Conduct

We are committed to providing a welcoming and inspiring community for all. Please read and adhere to the following principles:

- **Respectful Communication**: Treat all contributors and users with respect
- **Constructive Feedback**: Provide feedback in a constructive and helpful manner
- **Inclusive Environment**: Welcome contributors of all backgrounds and experience levels
- **Ethical Use**: Ensure all contributions align with legal and ethical standards

## How to Contribute

### Reporting Bugs

Before submitting a bug report, please check the issue tracker to avoid duplicates.

When reporting a bug, include:

1. **Clear Title**: A descriptive title for the issue
2. **Environment Details**:
   - Windows version
   - Visual Studio version used
   - DirectX version
3. **Reproduction Steps**: Step-by-step instructions to reproduce the issue
4. **Expected Behavior**: What you expected to happen
5. **Actual Behavior**: What actually happened
6. **Screenshots/Logs**: If applicable, include error messages or logs
7. **Additional Context**: Any other relevant information

### Suggesting Enhancements

We appreciate feature suggestions! When proposing an enhancement:

1. **Use a Clear Title**: Clearly describe the enhancement
2. **Provide a Detailed Description**: Explain the use case and benefits
3. **Include Examples**: Show how the feature would be used
4. **List any Alternatives**: Mention existing workarounds or alternative approaches
5. **Add Context**: Explain why this enhancement would be valuable

### Pull Requests

We welcome pull requests! Here's the process:

#### Before You Start

1. **Fork the Repository**: Create a personal fork of the project
2. **Create a Branch**: Create a feature branch with a descriptive name
   ```
   git checkout -b feature/add-new-feature
   git checkout -b fix/resolve-issue-description
   ```

#### Development Guidelines

1. **Code Style**:
   - Follow existing code style and formatting conventions
   - Use meaningful variable and function names
   - Add comments for complex logic
   - Keep functions focused and modular

2. **C++ Standards**:
   - Use C++11 or later features appropriately
   - Follow RAII principles for resource management
   - Avoid raw pointers where smart pointers can be used
   - Use const correctness

3. **Windows API Practices**:
   - Handle Windows API errors appropriately
   - Use HANDLE and other Windows types correctly
   - Consider platform compatibility (Windows 7+)
   - Document any OS-specific code

4. **ImGui Integration**:
   - Follow ImGui conventions and patterns
   - Test UI changes in the ImGui demo before implementing
   - Ensure UI responsiveness and stability
   - Update ImGui versions carefully

5. **Testing**:
   - Test changes on Windows 7, Windows 10, and Windows 11
   - Verify compatibility with supported DirectX versions
   - Test with various target processes
   - Include edge case testing

#### Submitting a Pull Request

1. **Update Your Branch**:
   ```
   git fetch origin
   git rebase origin/main
   ```

2. **Push Your Changes**:
   ```
   git push origin feature/your-feature-name
   ```

3. **Create a Pull Request**:
   - Use a clear, descriptive title
   - Reference any related issues (#issue-number)
   - Provide a detailed description of changes
   - Explain the motivation and rationale
   - Include any breaking changes or migration notes

4. **Code Review**:
   - Be open to feedback and suggestions
   - Respond promptly to reviewer questions
   - Make requested changes in follow-up commits
   - Discuss disagreements respectfully

#### PR Requirements

- [ ] Code follows the project's style guidelines
- [ ] Changes are well-commented and documented
- [ ] New features include appropriate documentation updates
- [ ] Testing has been performed on supported Windows versions
- [ ] No new compiler warnings are introduced
- [ ] Changes do not break existing functionality

## Development Setup

### Prerequisites

1. **Visual Studio 2017 or Later**
   - Desktop development with C++
   - Windows 10 SDK or later

2. **DirectX 9 SDK**
   - Download from Microsoft
   - Add SDK paths to Visual Studio

3. **Git**
   - For version control and collaboration

### Building Locally

1. Clone your fork:
   ```
   git clone https://github.com/your-username/vinjector.git
   cd vinjector
   ```

2. Build using the provided script:
   ```powershell
   .\build.ps1
   ```

3. Or build in Visual Studio:
   - Open the solution file
   - Select Debug/Release configuration
   - Build the project

## Project Structure

```
├── Main.cpp                # Application entry point
├── ImGui/                  # ImGui framework
├── Inject/                 # Injection module
├── ProcessCheck/           # Process monitoring
└── build.ps1              # Build script
```

### Module Responsibilities

- **Inject Module**: Handle injection logic, payload delivery, and process manipulation
- **ProcessCheck Module**: Monitor process status, validation, and verification
- **ImGui Integration**: UI rendering, user input, and interface logic
- **Main**: Application initialization and orchestration

## Documentation

### Code Documentation

- Add meaningful comments for complex algorithms
- Document function parameters and return values
- Explain non-obvious implementation decisions
- Keep documentation up-to-date with code changes

### Commit Messages

Write clear, descriptive commit messages:

```
Brief summary (50 chars or less)

More detailed explanation of the changes and why they were made.
This can span multiple lines and should explain the context and
rationale for the changes.

- Bullet points for multiple changes
- Use past tense
- Reference related issues: Fixes #123
```

## Legal

- All contributions must be provided under the MIT License
- By contributing, you agree that your contributions will be licensed under the MIT License
- Ensure your contributions do not violate any copyright or intellectual property rights
- Do not include code that violates any laws or licensing agreements

## Recognition

Contributors will be recognized in the README and release notes. Thank you for your contributions!

## Questions or Need Help?

- Check existing documentation and issues first
- Create a GitHub discussion for questions
- Open an issue for bugs or features
- Review existing PRs to avoid duplicate work

## Thank You!

We appreciate your interest in contributing to vinjector. Your efforts help make this project better for everyone!

---

*Last updated: April 2026*
