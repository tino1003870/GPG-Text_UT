# GPG Text

GPG Text is a small Ubuntu Touch application for working with OpenPGP/GPG encrypted text and OpenPGP keys.

The application is designed specifically for Ubuntu Touch and provides a simple graphical interface for common GPG operations without requiring a terminal.

## Features

- Encrypt text using OpenPGP/GPG
- Decrypt OpenPGP/GPG encrypted text
- Import OpenPGP keys
- Display available GPG keys
- Work with public and private keys
- Integration with Ubuntu Touch Content Hub

## Ubuntu Touch integration

GPG Text uses the Ubuntu Touch Content Hub for file exchange and key selection.

On Ubuntu Touch, the application uses the Lomiri Content Hub picker rather than the desktop Qt file dialog.

The intended key-import workflow is:

1. Open the "Import Key" function.
2. Select an OpenPGP key file (`.asc`, `.gpg`, etc.) through Content Hub.
3. Import the key into the local GPG keyring.
4. Refresh the key list.

This approach is important because desktop Qt file dialogs are not the appropriate mechanism for normal Ubuntu Touch application integration.

## GPG Runtime

GPG Text uses a bundled GPG executable that is included in the Click package.

The GPG package is declared as a Click build dependency and the `gpg` executable is installed into the application package. The application therefore does not depend on a system-wide GPG installation on the Ubuntu Touch device.

At runtime, GPG is started as `gpg` and resolved through the application's runtime `PATH`.

This makes the GPG functionality self-contained within the application package and avoids relying on the availability or location of a system-installed GPG executable.

## Building

The project is built with Clickable and CMake.

For an ARM64 Ubuntu Touch package:

    clickable build --arch arm64

The resulting Click package is generated in:

    build/aarch64-linux-gnu/app/

For desktop testing:

    clickable desktop

## Project structure

    GPG-Text_UT/
    ├── assets/
    │   └── gpg-text.svg
    ├── qml/
    │   ├── Main.qml
    │   ├── ContentPeerPicker13.qml
    │   └── TestContent/
    ├── CMakeLists.txt
    ├── GpgBackend.cpp
    ├── GpgBackend.h
    ├── clickable.yaml
    ├── gpg-text.apparmor
    ├── gpg-text-contenthub.json
    ├── gpg-text.desktop
    ├── gpg.rules
    ├── main.cpp
    ├── manifest.json
    └── qml.qrc

## Version

Current version: **1.0.0**

Target platform: **Ubuntu Touch / Ubuntu SDK 20.04**

Primary architecture for Ubuntu Touch release:

**ARM64**

## License

See the `LICENSE` file included in this repository.
