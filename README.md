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
- Suitable for use together with mail applications such as Dekko

## Ubuntu Touch integration

GPG Text uses the Ubuntu Touch Content Hub for file exchange and key selection.

On Ubuntu Touch, the application uses the Lomiri Content Hub picker rather than the desktop Qt file dialog.

The intended key-import workflow is:

1. Open the "Import Key" function.
2. Select an OpenPGP key file (`.asc`, `.gpg`, etc.) through Content Hub.
3. Import the key into the local GPG keyring.
4. Refresh the key list.

This approach is important because desktop Qt file dialogs are not the appropriate mechanism for normal Ubuntu Touch application integration.

## Dekko integration

GPG Text was developed partly to investigate and provide a possible GPG backend/workflow for Ubuntu Touch mail applications such as Dekko.

The application demonstrates that GPG operations can be performed from a native Ubuntu Touch application and that files can be exchanged through Content Hub.

During development, several Content Hub API operations were tested. Content Hub itself is available, but some operations require additional AppArmor permissions. In particular, the following operations were found to be restricted by the application security policy:

- `DefaultSourceForType`
- `RegisterImportExportHandler`
- `CreateImportFromPeer`
- `HasPending`

`ContentTransfer` was also found not to be a valid solution for the tested workflow.

These findings may be useful for developers implementing Content Hub based mail integration on Ubuntu Touch.

## Security / AppArmor

The current application uses an AppArmor configuration based on the Ubuntu Touch `unconfined` template.

The `unconfined` configuration is intentional for the current development version because the application needs to perform local GPG operations and access the relevant user files.

As a consequence, the standard Click review reports:

    (NEEDS REVIEW) "unconfined" not allowed

This is a known limitation of the current package and requires manual review for OpenStore publication.

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
