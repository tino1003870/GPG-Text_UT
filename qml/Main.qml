import QtQuick 2.12
import QtQuick.Controls 2.5
import Lomiri.Content 1.3

Rectangle {
    anchors.fill: parent
    color: "white"

    Text {
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.margins: 10
        text: "ROOT: " + parent.width + " x " + parent.height
        color: "red"
        font.pixelSize: 16
        z: 9999
    }


    Timer {
        id: pickerDebugTimer
        interval: 2000
        repeat: false
        onTriggered: {
            console.log("### PICKER DEBUG ###")
            console.log("visible:", keyPicker.visible)
            console.log("width:", keyPicker.width)
            console.log("height:", keyPicker.height)
            console.log("peer:", keyPicker.peer)
        }
    }

    ContentPeerPicker {
        id: keyPicker
        anchors.fill: parent
        visible: false
        handler: ContentHandler.Source
        contentType: ContentType.All
        showTitle: true

        onPeerSelected: {
            console.log("### CONTENT PEER AUSGEWÄHLT ###")
            console.log("### PEER ###", peer)
            console.log("### PEER APP ID ###", peer.appId)
            console.log("### PEER NAME ###", peer.name)

            var transfer = peer.request()

            if (!transfer) {
                console.log("### CONTENT TRANSFER FEHLER: KEIN TRANSFER ###")
                return
            }

            console.log("### CONTENT TRANSFER ERZEUGT ###")
            console.log("### TRANSFER STATE ###", transfer.state)
            console.log("### TRANSFER DIRECTION ###", transfer.direction)
            console.log("### TRANSFER STORE ###", transfer.store)
            console.log("### TRANSFER SOURCE ###", transfer.source)
            console.log("### TRANSFER DESTINATION ###", transfer.destination)
            console.log("### TRANSFER ITEMS ###", transfer.items.length)

            transfer.stateChanged.connect(function() {
                console.log("### TRANSFER STATE CHANGE ###", transfer.state)
                console.log("### ITEMS ###", transfer.items.length)

                for (var i = 0; i < transfer.items.length; i++) {
                    console.log("### ITEM ###", i)
                    console.log("### ITEM NAME ###", transfer.items[i].name)
                    console.log("### ITEM URL ###", transfer.items[i].url)
                    console.log("### ITEM TEXT ###", transfer.items[i].text)
                }

                if (transfer.state === ContentTransfer.Collected &&
                    transfer.items.length > 0) {
                    var importUrl = transfer.items[0].url
                    console.log("### IMPORT DATEI ###", importUrl)

                    pendingImportUrl = importUrl
                    passphraseField.text = ""
                    passphraseDialog.open()

                    Qt.callLater(function() {
                        passphraseField.forceActiveFocus()
                        passphraseField.openSoftwareInputPanel()
                    })
                }
            })

            if (!transfer.start()) {
                console.log("### CONTENT TRANSFER START FEHLER ###")
            } else {
                console.log("### CONTENT TRANSFER GESTARTET ###")
            }
        }

        onCancelPressed: {
            console.log("### CONTENT HUB ABGEBROCHEN ###")
            visible = false
        }
    }

    function loadKeys() {
    console.log("### LOAD KEYS START ###")

    var keys = gpgBackend.listPublicKeys()

    console.log("### LOAD KEYS RAW ###",
    JSON.stringify(keys))

    keyList = keys

    console.log("### LOAD KEYS COUNT ###",
    keyList.length)

    for (var i = 0; i < keyList.length; i++) {
    console.log("### KEY ###", i)
    console.log("### KEY NAME ###", keyList[i].name)
    console.log("### KEY EMAIL ###", keyList[i].email)
    console.log("### KEY ID ###", keyList[i].keyId)
    console.log("### KEY FINGERPRINT ###",
    keyList[i].fingerprint)
    console.log("### KEY SECRET ###",
    keyList[i].hasSecretKey)
    console.log("### KEY CREATED ###",
    keyList[i].created)
    console.log("### KEY EXPIRES ###",
    keyList[i].expires)
    }

    console.log("### LOAD KEYS END ###")
    }

    property string pendingImportUrl: ""
    property string selectedFingerprint: ""

    function encodeText() {
        console.log("### ENCODE BUTTON CLICKED ###")
        console.log("### ENCODE SELECTED FINGERPRINT ###",
                    selectedFingerprint)
        console.log("### ENCODE INPUT LENGTH ###",
                    inputText.text.length)

        if (selectedFingerprint === "") {
            console.log("### ENCODE FEHLER: KEIN SCHLÜSSEL AUSGEWÄHLT ###")
            output.text = "FEHLER: Bitte zuerst einen Schlüssel auswählen."
            return
        }

        if (inputText.text === "") {
            console.log("### ENCODE FEHLER: EINGABE LEER ###")
            output.text = "FEHLER: Bitte Text eingeben."
            return
        }

        console.log("### ENCODE GPG AUFRUF ###")

        var result = gpgBackend.encryptText(
            inputText.text,
            selectedFingerprint
        )

        console.log("### ENCODE RESULT LENGTH ###",
                    result.length)
        console.log("### ENCODE RESULT ###", result)

        output.text = result

        console.log("### ENCODE DONE ###")
    }

    function decodeText() {
        console.log("### DECODE BUTTON CLICKED ###")
        console.log("### DECODE INPUT LENGTH ###",
                    inputText.text.length)

        if (inputText.text === "") {
            console.log("### DECODE FEHLER: EINGABE LEER ###")
            output.text = "FEHLER: Bitte verschlüsselten Text eingeben."
            return
        }

        console.log("### DECODE GPG AUFRUF ###")

        var result = gpgBackend.decryptText(
            inputText.text
        )

        console.log("### DECODE RESULT LENGTH ###",
                    result.length)
        console.log("### DECODE RESULT ###", result)

        output.text = result

        console.log("### DECODE DONE ###")
    }

    property var keyList: []

    Popup {
        id: passphraseDialog

        width: parent.width - 60
        height: 230

        x: 30
        y: (parent.height - height) / 2

        modal: true
        focus: true

        background: Rectangle {
            color: "white"
            border.color: "gray"
            border.width: 1
            radius: 8
        }

        Column {
            anchors.fill: parent
            anchors.margins: 30
            spacing: 20

            Text {
                text: "Passphrase eingeben"
                font.pixelSize: 24
                color: "black"
            }

            TextField {
                id: passphraseField

                width: parent.width
                height: 58

                placeholderText: "Passphrase"
                echoMode: TextInput.Password
                inputMethodHints: Qt.ImhHiddenText

                onAccepted: {
                    importWithPassphrase()
                }

    Component.onCompleted: {
                    forceActiveFocus()
                }
            }

            Row {
                width: parent.width
                spacing: 10

                Button {
                    width: (parent.width - 10) / 2
                    height: 50
                    text: "Abbrechen"

                    onClicked: {
                        passphraseField.text = ""
                        pendingImportUrl = ""
                        passphraseDialog.close()
                    }
                }

                Button {
                    width: (parent.width - 10) / 2
                    height: 50
                    text: "Importieren"

                    onClicked: {
                        importWithPassphrase()
                    }
                }
            }
        }

        onOpened: {
            passphraseField.forceActiveFocus()
        }
    }

    function importKeyDesktopOrTouch() {
        console.log("### IMPORT KEY BUTTON ###")

        var platform = gpgBackend.platformName()

        console.log("### PLATFORM ###", platform)

        // Desktop: normaler Qt-Dateidialog
        if (platform === "xcb" ||
            platform === "wayland" ||
            platform === "windows" ||
            platform === "cocoa") {

            console.log("### DESKTOP FILE DIALOG ###")

            var filePath = gpgBackend.chooseImportFile()

            console.log("### SELECTED FILE ###", filePath)

            if (filePath === "") {
                console.log("### FILE SELECTION CANCELLED ###")
                return
            }

            pendingImportUrl = filePath

            passphraseField.text = ""
            passphraseDialog.open()

            Qt.callLater(function() {
                passphraseField.forceActiveFocus()
                passphraseField.openSoftwareInputPanel()
            })

            return
        }

        // Ubuntu Touch: Content Hub
        console.log("### LOMIRI CONTENT HUB ###")

        keyPicker.visible = true
    }


    function importWithPassphrase() {
        if (pendingImportUrl === "")
            return

        console.log("### GPG IMPORT MIT PASSPHRASE ###")

        var result = gpgBackend.importKey(
            pendingImportUrl,
            passphraseField.text
        )

        console.log("### GPG IMPORT ERGEBNIS ###", result)
        output.text = result

        passphraseField.text = ""
        pendingImportUrl = ""
        passphraseDialog.close()
    }

    Popup {
        id: keyManagerPopup

        width: parent.width
        height: parent.height

        x: 0
        y: 0

        modal: true
        focus: true

        background: Rectangle {
            color: "white"
            border.color: "gray"
            border.width: 1
            radius: 8
        }

        Column {
            anchors.fill: parent
            anchors.margins: 20
            spacing: 15

            Text {
                text: "Installierte Schlüssel"
                font.pixelSize: 36
                color: "black"
            }

            Text {
                text: "Anzahl: " + keyList.length
                font.pixelSize: 24
                color: "gray"
            }

            ListView {
                id: keyListView

                width: parent.width
                height: parent.height - 150

                clip: true
                spacing: 16

                model: keyList

                delegate: Rectangle {
                    width: keyListView.width
                    height: 175

                    color: "#f3f3f3"
                    border.color: "#cccccc"
                    border.width: 1
                    radius: 6

                    Column {
                        anchors.left: parent.left
                        anchors.right: deleteButton.left
                        anchors.top: parent.top
                        anchors.bottom: parent.bottom

                        anchors.margins: 16
                        spacing: 8

                        Text {
                            width: parent.width
                            text: modelData.name !== ""
                                  ? modelData.name
                                  : "(kein Name)"
                            font.pixelSize: 25
                            color: "black"
                            elide: Text.ElideRight
                        }

                        Text {
                            width: parent.width
                            text: modelData.email !== ""
                                  ? modelData.email
                                  : "(keine E-Mail)"
                            font.pixelSize: 21
                            color: "gray"
                            elide: Text.ElideRight
                        }

                        Text {
                            width: parent.width
                            text: "Fingerprint: " + modelData.fingerprint
                            font.pixelSize: 16
                            color: "gray"
                            elide: Text.ElideMiddle
                        }

                        Text {
                            text: modelData.hasSecretKey
                                  ? "Privater Schlüssel vorhanden"
                                  : "Nur öffentlicher Schlüssel"
                            font.pixelSize: 18
                            color: modelData.hasSecretKey
                                   ? "black"
                                   : "gray"
                        }
                    }

                    Button {
                        id: deleteButton

                        width: 125
                        height: 50

                        anchors.right: parent.right
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.rightMargin: 14

                        text: "Löschen"

                        onClicked: {
                            console.log("### DELETE BUTTON CLICKED ###")
                            console.log("### DELETE NAME ###",
                                        modelData.name)
                            console.log("### DELETE EMAIL ###",
                                        modelData.email)
                            console.log("### DELETE FINGERPRINT ###",
                                        modelData.fingerprint)
                            console.log("### DELETE HAS SECRET ###",
                                        modelData.hasSecretKey)

                            var result =
                                gpgBackend.deleteKey(
                                    modelData.fingerprint
                                )

                            console.log(
                                "### DELETE RESULT ###",
                                result
                            )

                            output.text = result

                            loadKeys()
                        }
                    }
                }

                Text {
                    anchors.centerIn: parent
                    visible: keyList.length === 0

                    text: "Keine Schlüssel installiert."
                    font.pixelSize: 20
                    color: "gray"
                }
            }

            Button {
                width: parent.width
                height: 55

                text: "Schließen"

                onClicked: {
                    console.log("### KEY MANAGER CLOSE ###")
                    keyManagerPopup.close()
                }
            }
        }

        onOpened: {
            console.log("### KEY MANAGER OPENED ###")
            loadKeys()
        }
    }


    ScrollView {
        id: cryptoScrollView

        visible: !keyPicker.visible
        z: 100

        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
            bottom: parent.bottom
            margins: 0
        }

        clip: true

        ScrollBar.vertical.policy: ScrollBar.AsNeeded

        Column {
            id: cryptoColumn

            width: cryptoScrollView.width - 20
            x: 10

            spacing: 12

            topPadding: 15
            bottomPadding: 30

            Button {
                width: parent.width
                height: 50

                text: "Beenden"
                font.pixelSize: 19

                onClicked: {
                    console.log("### EXIT BUTTON CLICKED ###")
                    Qt.quit()
                }
            }


        Text {
            text: "Verschlüsseln / Entschlüsseln"
            font.pixelSize: 28
            color: "black"
        }

        Row {
            width: parent.width
            spacing: 12

            Button {
                width: (parent.width - parent.spacing) / 2
                height: 60

                text: "Schlüssel importieren"
                font.pixelSize: 19

                onClicked: {
                    console.log("### IMPORT KEY BUTTON ###")
                    importKeyDesktopOrTouch()
                }
            }

            Button {
                width: (parent.width - parent.spacing) / 2
                height: 60

                text: "Installierte Schlüssel"
                font.pixelSize: 19

                onClicked: {
                    console.log("### KEY MANAGER BUTTON ###")
                    keyManagerPopup.open()
                }
            }
        }

        Text {
            text: "Schlüssel für Verschlüsselung"
            font.pixelSize: 22
            font.bold: true
            color: "black"
        }

        ComboBox {
            id: keyComboBox

            width: parent.width
            height: 70

            z: 200

            model: keyList
            textRole: "name"

            currentIndex: selectedFingerprint === ""
                          ? -1
                          : keyList.findIndex(function(k) {
                                return k.fingerprint === selectedFingerprint
                            })

            displayText: currentIndex >= 0 && currentIndex < keyList.length
                        ? keyList[currentIndex].name + " <" +
                          keyList[currentIndex].email + ">"
                        : "Bitte Schlüssel auswählen ..."

            font.pixelSize: 21

            delegate: ItemDelegate {
                width: keyComboBox.width
                height: 82

                contentItem: Column {
                    anchors.fill: parent
                    anchors.margins: 10
                    spacing: 3

                    Text {
                        text: modelData.name !== ""
                              ? modelData.name
                              : "(kein Name)"

                        font.pixelSize: 20
                        font.bold: true
                        color: "black"
                        elide: Text.ElideRight
                    }

                    Text {
                        text: modelData.email !== ""
                              ? modelData.email
                              : "(keine E-Mail)"

                        font.pixelSize: 17
                        color: "gray"
                        elide: Text.ElideRight
                    }
                }

                onClicked: {
                    console.log("### KEY SELECTED ###")
                    console.log("### KEY NAME ###", modelData.name)
                    console.log("### KEY EMAIL ###", modelData.email)
                    console.log("### KEY FINGERPRINT ###",
                                modelData.fingerprint)
                    console.log("### KEY SECRET ###",
                                modelData.hasSecretKey)

                    selectedFingerprint = modelData.fingerprint
                    keyComboBox.currentIndex = index

                    console.log("### SELECTED FINGERPRINT ###",
                                selectedFingerprint)
                }
            }

            onCurrentIndexChanged: {
                if (currentIndex >= 0 &&
                    currentIndex < keyList.length) {

                    selectedFingerprint =
                        keyList[currentIndex].fingerprint

                    console.log("### KEY COMBO INDEX ###",
                                currentIndex)
                    console.log("### SELECTED FINGERPRINT ###",
                                selectedFingerprint)
                }
            }
        }

        Text {
            width: parent.width

            text: selectedFingerprint === ""
                  ? "⚠ Kein Schlüssel ausgewählt"
                  : "✓ Verwendeter Schlüssel: " +
                    keyList[keyComboBox.currentIndex].name +
                    " <" +
                    keyList[keyComboBox.currentIndex].email +
                    ">"

            font.pixelSize: 19
            font.bold: true

            color: selectedFingerprint === ""
                   ? "red"
                   : "black"

            elide: Text.ElideRight
        }

        Text {
            width: parent.width

            visible: selectedFingerprint !== ""

            text: selectedFingerprint === ""
                  ? ""
                  : "Fingerprint: " + selectedFingerprint

            font.pixelSize: 15
            color: "gray"
            elide: Text.ElideMiddle
        }

        Text {
            text: "Eingabe"
            font.pixelSize: 22
            font.bold: true
            color: "black"
        }

        Rectangle {
            width: parent.width
            height: 180

            color: "#f5f5f5"
            border.color: "#888888"
            border.width: 2
            radius: 8

            TextArea {
                id: inputText

                anchors.fill: parent
                anchors.margins: 10

                font.pixelSize: 23

                wrapMode: TextArea.Wrap

                placeholderText:
                    "Klartext oder verschlüsselten Text eingeben ..."

                color: "black"
                background: null

                selectByMouse: true

                Keys.onPressed: {
                    if ((event.modifiers & Qt.ControlModifier) &&
                        event.key === Qt.Key_C) {

                        copy()
                        event.accepted = true

                        console.log("### INPUT COPIED ###")
                    }
                }
            }
        }

        Row {
            width: parent.width
            spacing: 12

            Button {
                width: (parent.width - parent.spacing) / 2
                height: 65

                text: "Encode"
                font.pixelSize: 22

                onClicked: {
                    console.log("### ENCODE BUTTON ###")
                    encodeText()
                }
            }

            Button {
                width: (parent.width - parent.spacing) / 2
                height: 65

                text: "Decode"
                font.pixelSize: 22

                onClicked: {
                    console.log("### DECODE BUTTON ###")
                    decodeText()
                }
            }
        }

        Row {
            width: parent.width
            spacing: 12

            Button {
                width: (parent.width - parent.spacing) / 2
                height: 55

                text: "Eingabe kopieren"
                font.pixelSize: 18

                onClicked: {
                    inputText.selectAll()
                    inputText.copy()
                    inputText.deselect()

                    console.log("### INPUT COPY BUTTON ###")
                    console.log("### INPUT COPY LENGTH ###",
                                inputText.text.length)
                }
            }

            Button {
                width: (parent.width - parent.spacing) / 2
                height: 55

                text: "Eingabe einfügen"
                font.pixelSize: 18

                onClicked: {
                    console.log("### INPUT PASTE BUTTON ###")

                    inputText.paste()

                    console.log("### INPUT AFTER PASTE LENGTH ###",
                                inputText.text.length)
                }
            }
        }

        Row {
            width: parent.width
            spacing: 12

            Button {
                width: (parent.width - parent.spacing) / 2
                height: 55

                text: "Eingabe löschen"
                font.pixelSize: 18

                onClicked: {
                    inputText.clear()

                    console.log("### INPUT CLEARED ###")
                }
            }

            Button {
                width: (parent.width - parent.spacing) / 2
                height: 55

                text: "Eingabe leeren"
                font.pixelSize: 18

                onClicked: {
                    inputText.clear()

                    console.log("### INPUT CLEAR BUTTON ###")
                }
            }
        }

        Text {
            text: "Ausgabe"
            font.pixelSize: 20
            color: "black"
        }

        Rectangle {
            width: parent.width
            height: 180

            color: "#eeeeee"
            border.color: "#aaaaaa"
            border.width: 1
            radius: 6

            TextArea {
                id: output

                anchors.fill: parent
                anchors.margins: 8

                font.pixelSize: 23

                wrapMode: TextArea.Wrap

                readOnly: true

                selectByMouse: true

                color: "black"
                background: null

                Keys.onPressed: {
                    if ((event.modifiers & Qt.ControlModifier) &&
                        event.key === Qt.Key_C) {

                        copy()
                        event.accepted = true

                        console.log("### OUTPUT COPIED ###")
                    }
                }
            }
        }

        Row {
            width: parent.width
            spacing: 12

            Button {
                width: (parent.width - parent.spacing) / 2
                height: 55

                text: "Ausgabe kopieren"
                font.pixelSize: 18

                onClicked: {
                    output.selectAll()
                    output.copy()
                    output.deselect()

                    console.log("### OUTPUT COPY BUTTON ###")
                    console.log("### OUTPUT COPY LENGTH ###",
                                output.text.length)
                }
            }

            Button {
                width: (parent.width - parent.spacing) / 2
                height: 55

                text: "Ausgabe einfügen"
                font.pixelSize: 18

                onClicked: {
                    console.log("### OUTPUT PASTE BUTTON ###")

                    output.paste()

                    console.log("### OUTPUT AFTER PASTE LENGTH ###",
                                output.text.length)
                }
            }
        }

        Row {
            width: parent.width
            spacing: 12

            Button {
                width: (parent.width - parent.spacing) / 2
                height: 55

                text: "Ausgabe löschen"
                font.pixelSize: 18

                onClicked: {
                    output.clear()

                    console.log("### OUTPUT CLEARED ###")
                }
            }
        }
    }

    }

    Column {
        id: mainColumn
        visible: false

        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
            margins: 30
        }

        spacing: 20

        Button {
            width: parent.width
            height: 60

            text: "Schlüssel importieren"

            onClicked: {
                console.log("### SCHLÜSSEL IMPORT BUTTON ###")
                importKeyDesktopOrTouch()
            }
        }

        Button {
            width: parent.width
            height: 60

            text: "Öffentliche Schlüssel"

            onClicked: {
                console.log("### ÖFFENTLICHE SCHLÜSSEL BUTTON ###")

                var keys = gpgBackend.listPublicKeys()

                console.log("### LIST PUBLIC KEYS ERGEBNIS ###", JSON.stringify(keys))

                output.text = JSON.stringify(keys, null, 2)
            }
        }

        Button {
            width: parent.width
            height: 60

            text: "Schlüssel verwalten"

            onClicked: {
                console.log("### SCHLÜSSEL VERWALTEN BUTTON ###")
                keyManagerPopup.open()
            }
        }
    }

    Component.onCompleted: {
        console.log("### GPG BACKEND UI START ###")
    }
}
