# ChangeLog

## Unreleased
### Added
- Support libipset version 7.
- Add MQTT TLS support.
- Add MQTT username and password option.

### Removed
- EL6 support.

### Fixed
- Show program name and PID in syslog on musl systems.

## 0.1.2 - 2017-01-18
### Added
- Validate IP addresses before adding them to an IPset.

### Changed
- Don't overwrite /etc/default/vallumd when updating DEB package.
- Reset timeout when adding IP that is already in set.

## 0.1.1 - 2017-01-15
### Added
- Add ChangeLog.

### Fixed
- Fix crash on reconnect.
- Fix tweak version and use it in DEB/RPM packages.

## 0.1.0 - 2017-01-14
Initial release.
