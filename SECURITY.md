# Security Policy

## Reporting Security Vulnerabilities

The security of vinjector is important to us. If you believe you have found a security vulnerability in this project, we encourage you to let us know right away.

### How to Report

**Please do not report security vulnerabilities through public GitHub issues or pull requests.**

Instead, please report security vulnerabilities by sending an email to the project maintainer:

- Contact: [GitHub Issues (Private)](https://github.com/iVyz3r/vinjector/security/advisories)
- Or submit a report through GitHub's Security Advisory system

Please include the following information in your report:

1. A clear description of the vulnerability
2. Steps to reproduce the issue (if applicable)
3. Potential impact and severity
4. Any suggested fixes or workarounds
5. Your contact information

## Security Considerations

### User Responsibility

- **Legal Compliance**: Users are solely responsible for ensuring their use of vinjector complies with all applicable laws and regulations
- **Authorized Use Only**: Only use this tool on systems and applications you own or have explicit permission to interact with
- **Terms of Service**: Respect the terms of service of applications and systems you interact with

### Known Limitations

- This tool requires administrative privileges to function properly
- Process injection may be detected by antivirus software or security monitoring systems
- Some applications may have protections against injection attempts
- Use on protected or DRM-protected software may violate applicable laws

## Security Best Practices

When using vinjector, follow these security best practices:

1. **Use in Safe Environments**: Only run in controlled, isolated testing environments
2. **Verify Permissions**: Always verify you have authorization before injecting into any process
3. **Monitor Activity**: Keep audit logs of injection activities
4. **Keep Updated**: Regularly update your system and dependencies
5. **Secure Access**: Restrict access to the compiled executable and source code
6. **Test Thoroughly**: Validate your injection implementations before deployment

## Vulnerability Response Timeline

Upon receiving a security report, we commit to:

1. **Initial Response**: Acknowledge receipt within 7 days
2. **Assessment**: Evaluate the vulnerability within 14 days
3. **Fix Development**: Work on a fix or mitigation strategy
4. **Advisory**: Publish a security advisory once a fix is available or the issue is addressed
5. **Update**: Release a patched version on the public repository

## Supported Versions

Security updates will be prioritized for:

- Current version (latest release)
- Previous major version (if applicable)

## Third-Party Dependencies

This project includes the following third-party libraries:

- **ImGui**: Licensed under MIT License
  - Website: https://github.com/ocornut/imgui
  - Security: Monitor the ImGui repository for security updates
  
- **DirectX 9 SDK**: Provided by Microsoft
  - Security: Follow Microsoft's security advisories for DirectX updates

For security updates to dependencies, regularly check their respective repositories and security advisories.

## Disclaimer

This project is provided for educational and authorized use only. The use of process injection technology may be restricted or prohibited in certain jurisdictions or contexts. Users assume all responsibility for legal and ethical use of this tool.

By using vinjector, you agree to use it only for lawful and authorized purposes.

---

*Last updated: April 2026*
