#include <QGuiApplication>
#include <QFileDialog>
#include "GpgBackend.h"

#include <QProcess>
#include <QDateTime>
#include <QStringList>
#include <QVariantMap>
#include <QUrl>
#include <QSet>
#include <QDebug>

namespace
{

QString decodeGpgField(const QString &input)
{
    QString result;

    for (int i = 0; i < input.length(); ++i) {

        if (input[i] == '\\' &&
            i + 3 < input.length() &&
            input[i + 1] == 'x') {

            bool ok = false;
            int value = input.mid(i + 2, 2).toInt(&ok, 16);

            if (ok) {
                result.append(QChar(value));
                i += 3;
                continue;
            }
        }

        result.append(input[i]);
    }

    return result;
}

QString formatDate(const QString &timestamp)
{
    bool ok = false;
    qint64 seconds = timestamp.toLongLong(&ok);

    if (!ok || seconds == 0)
        return QString();

    return QDateTime::fromSecsSinceEpoch(seconds)
        .toLocalTime()
        .toString("dd.MM.yyyy");
}

}

GpgBackend::GpgBackend(QObject *parent)
    : QObject(parent)
{
}

QString GpgBackend::testGpg()
{
    qDebug() << "### TEST GPG START ###";

    QProcess process;

    process.setProgram("/usr/bin/gpg");
    process.setArguments(QStringList()
                         << "--version");

    qDebug() << "### GPG PROGRAM ###" << process.program();
    qDebug() << "### GPG ARGS ###" << process.arguments();

    process.start();

    if (!process.waitForStarted(5000)) {
        qDebug() << "### GPG START FAILED ###"
                 << process.errorString();

        return QString("GPG START FEHLER: %1")
            .arg(process.errorString());
    }

    qDebug() << "### GPG STARTED ###";

    if (!process.waitForFinished(10000)) {
        qDebug() << "### GPG TIMEOUT ###";

        process.kill();
        process.waitForFinished();

        return "GPG TIMEOUT";
    }

    const QByteArray stdoutData =
        process.readAllStandardOutput();

    const QByteArray stderrData =
        process.readAllStandardError();

    qDebug() << "### GPG EXIT CODE ###"
             << process.exitCode();

    qDebug() << "### GPG STDOUT ###"
             << stdoutData;

    qDebug() << "### GPG STDERR ###"
             << stderrData;

    if (process.exitCode() != 0) {
        return QString("GPG FEHLER (%1): %2")
            .arg(process.exitCode())
            .arg(QString::fromUtf8(stderrData).trimmed());
    }

    return QString::fromUtf8(stdoutData).trimmed();
}

QString GpgBackend::importKey(const QString &filePath,
                               const QString &passphrase)
{
    QString path = filePath;

    // QML liefert bei ContentTransfer file:///...
    if (path.startsWith("file://"))
        path = QUrl(path).toLocalFile();

    qDebug() << "### GPG IMPORT PATH ###" << path;
    qDebug() << "### GPG PASSPHRASE LENGTH ###" << passphrase.length();

    QProcess process;

    process.setProgram("/usr/bin/gpg");

    process.setArguments(QStringList()
                         << "--batch"
                         << "--yes"
                         << "--pinentry-mode"
                         << "loopback"
                         << "--passphrase-fd"
                         << "0"
                         << "--import"
                         << path);

    // Wichtig: keine interaktive Pinentry verwenden.
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert("GPG_TTY", "");
    process.setProcessEnvironment(env);

    qDebug() << "### IMPORT GPG ENV ###";
    qDebug() << "HOME =" << env.value("HOME");
    qDebug() << "GNUPGHOME =" << env.value("GNUPGHOME");
    qDebug() << "GPG_AGENT_INFO =" << env.value("GPG_AGENT_INFO");
    qDebug() << "GPG_TTY =" << env.value("GPG_TTY");
    qDebug() << "DBUS_SESSION_BUS_ADDRESS ="
             << env.value("DBUS_SESSION_BUS_ADDRESS");
    qDebug() << "DISPLAY =" << env.value("DISPLAY");
    qDebug() << "XDG_RUNTIME_DIR =" << env.value("XDG_RUNTIME_DIR");

    qDebug() << "### GPG PROGRAM ###" << process.program();
    qDebug() << "### GPG ARGS ###" << process.arguments();

    process.start();

    if (!process.waitForStarted(5000)) {
        qDebug() << "### GPG START FEHLER ###"
                 << process.errorString();

        return QString("GPG START FEHLER: %1")
            .arg(process.errorString());
    }

    qDebug() << "### GPG GESTARTET ###";

    // Passphrase über stdin an gpg übergeben.
    process.write(passphrase.toUtf8());
    process.write("\n");
    process.closeWriteChannel();

    if (!process.waitForFinished(15000)) {
        qDebug() << "### GPG TIMEOUT ###";

        process.kill();
        process.waitForFinished();

        return "GPG TIMEOUT beim Importieren des Schlüssels";
    }

    const QByteArray stdoutData =
        process.readAllStandardOutput();

    const QByteArray stderrData =
        process.readAllStandardError();

    qDebug() << "### GPG EXIT CODE ###"
             << process.exitCode();

    qDebug() << "### GPG STDOUT ###"
             << stdoutData;

    qDebug() << "### GPG STDERR ###"
             << stderrData;

    if (process.exitCode() != 0) {
        return stderrData.isEmpty()
            ? QString("GPG FEHLER (%1)")
                  .arg(process.exitCode())
            : QString::fromUtf8(stderrData).trimmed();
    }

    return stdoutData.isEmpty()
        ? "Schlüssel erfolgreich importiert."
        : QString::fromUtf8(stdoutData).trimmed();
}

QString GpgBackend::deleteKey(const QString &fingerprint)
{
    qDebug() << "### DELETE KEY START ###";
    qDebug() << "### DELETE FINGERPRINT ###" << fingerprint;

    if (fingerprint.trimmed().isEmpty()) {
        qDebug() << "### DELETE KEY FEHLER: KEIN FINGERPRINT ###";
        return "Kein Fingerprint angegeben.";
    }

    const QString fp = fingerprint.trimmed();

    // --------------------------------------------------------
    // Prüfen, ob ein geheimer Schlüssel vorhanden ist.
    // --------------------------------------------------------

    QProcess checkSecret;

    checkSecret.setProgram("/usr/bin/gpg");
    checkSecret.setArguments(QStringList()
                             << "--batch"
                             << "--list-secret-keys"
                             << "--with-colons"
                             << fp);

    qDebug() << "### DELETE SECRET CHECK PROGRAM ###"
             << checkSecret.program();
    qDebug() << "### DELETE SECRET CHECK ARGS ###"
             << checkSecret.arguments();

    checkSecret.start();

    if (!checkSecret.waitForStarted(5000)) {
        qDebug() << "### DELETE SECRET CHECK START FEHLER ###"
                 << checkSecret.errorString();

        return QString("GPG konnte nicht gestartet werden: %1")
            .arg(checkSecret.errorString());
    }

    if (!checkSecret.waitForFinished(10000)) {
        qDebug() << "### DELETE SECRET CHECK TIMEOUT ###";

        checkSecret.kill();
        checkSecret.waitForFinished();

        return "Timeout beim Prüfen des geheimen Schlüssels.";
    }

    const QByteArray secretStdout =
        checkSecret.readAllStandardOutput();

    const QByteArray secretStderr =
        checkSecret.readAllStandardError();

    qDebug() << "### DELETE SECRET CHECK EXIT ###"
             << checkSecret.exitCode();
    qDebug() << "### DELETE SECRET CHECK STDOUT ###"
             << secretStdout;
    qDebug() << "### DELETE SECRET CHECK STDERR ###"
             << secretStderr;

    const bool hasSecretKey =
        !QString::fromUtf8(secretStdout).trimmed().isEmpty();

    qDebug() << "### DELETE HAS SECRET KEY ###"
             << hasSecretKey;

    // --------------------------------------------------------
    // Geheimen Schlüssel löschen.
    // --------------------------------------------------------

    if (hasSecretKey) {

        qDebug() << "### DELETE SECRET KEY ###";

        QProcess secretDelete;

        secretDelete.setProgram("/usr/bin/gpg");
        secretDelete.setArguments(QStringList()
                                  << "--batch"
                                  << "--yes"
                                  << "--delete-secret-keys"
                                  << fp);

        qDebug() << "### DELETE SECRET PROGRAM ###"
                 << secretDelete.program();
        qDebug() << "### DELETE SECRET ARGS ###"
                 << secretDelete.arguments();

        secretDelete.start();

        if (!secretDelete.waitForStarted(5000)) {
            qDebug() << "### DELETE SECRET START FEHLER ###"
                     << secretDelete.errorString();

            return QString("GPG konnte nicht gestartet werden: %1")
                .arg(secretDelete.errorString());
        }

        if (!secretDelete.waitForFinished(15000)) {
            qDebug() << "### DELETE SECRET TIMEOUT ###";

            secretDelete.kill();
            secretDelete.waitForFinished();

            return "Timeout beim Löschen des geheimen Schlüssels.";
        }

        const QByteArray out =
            secretDelete.readAllStandardOutput();

        const QByteArray err =
            secretDelete.readAllStandardError();

        qDebug() << "### DELETE SECRET EXIT ###"
                 << secretDelete.exitCode();
        qDebug() << "### DELETE SECRET STDOUT ###"
                 << out;
        qDebug() << "### DELETE SECRET STDERR ###"
                 << err;

        if (secretDelete.exitCode() != 0) {
            qDebug() << "### DELETE SECRET FEHLER ###";

            return err.isEmpty()
                ? QString("Fehler beim Löschen des geheimen Schlüssels (%1)")
                      .arg(secretDelete.exitCode())
                : QString::fromUtf8(err).trimmed();
        }

        qDebug() << "### DELETE SECRET SUCCESS ###";
    }

    // --------------------------------------------------------
    // Öffentlichen Schlüssel löschen.
    // --------------------------------------------------------

    qDebug() << "### DELETE PUBLIC KEY ###";

    QProcess publicDelete;

    publicDelete.setProgram("/usr/bin/gpg");
    publicDelete.setArguments(QStringList()
                              << "--batch"
                              << "--yes"
                              << "--delete-keys"
                              << fp);

    qDebug() << "### DELETE PUBLIC PROGRAM ###"
             << publicDelete.program();
    qDebug() << "### DELETE PUBLIC ARGS ###"
             << publicDelete.arguments();

    publicDelete.start();

    if (!publicDelete.waitForStarted(5000)) {
        qDebug() << "### DELETE PUBLIC START FEHLER ###"
                 << publicDelete.errorString();

        return QString("GPG konnte nicht gestartet werden: %1")
            .arg(publicDelete.errorString());
    }

    if (!publicDelete.waitForFinished(15000)) {
        qDebug() << "### DELETE PUBLIC TIMEOUT ###";

        publicDelete.kill();
        publicDelete.waitForFinished();

        return "Timeout beim Löschen des öffentlichen Schlüssels.";
    }

    const QByteArray publicOut =
        publicDelete.readAllStandardOutput();

    const QByteArray publicErr =
        publicDelete.readAllStandardError();

    qDebug() << "### DELETE PUBLIC EXIT ###"
             << publicDelete.exitCode();
    qDebug() << "### DELETE PUBLIC STDOUT ###"
             << publicOut;
    qDebug() << "### DELETE PUBLIC STDERR ###"
             << publicErr;

    if (publicDelete.exitCode() != 0) {
        qDebug() << "### DELETE PUBLIC FEHLER ###";

        return publicErr.isEmpty()
            ? QString("Fehler beim Löschen des öffentlichen Schlüssels (%1)")
                  .arg(publicDelete.exitCode())
            : QString::fromUtf8(publicErr).trimmed();
    }

    qDebug() << "### DELETE KEY SUCCESS ###"
             << fp;

    return "Schlüssel erfolgreich gelöscht.";
}



QString GpgBackend::platformName() const
{
    QString name = QGuiApplication::platformName();

    qDebug() << "### PLATFORM NAME ###" << name;

    return name;
}


QString GpgBackend::chooseImportFile()
{
    qDebug() << "### CHOOSE IMPORT FILE START ###";

    QFileDialog dialog;

    // Nicht den nativen GTK-Dateidialog verwenden.
    dialog.setOption(QFileDialog::DontUseNativeDialog, true);

    dialog.setWindowTitle("GPG-Schlüssel auswählen");
    dialog.setDirectory("/home");
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setNameFilter(
        "GPG-Schlüssel (*.asc *.gpg *.pgp *.key);;Alle Dateien (*)"
    );

    if (dialog.exec() != QDialog::Accepted) {
        qDebug() << "### CHOOSE IMPORT FILE CANCELLED ###";
        return QString();
    }

    QStringList files = dialog.selectedFiles();

    if (files.isEmpty()) {
        qDebug() << "### CHOOSE IMPORT FILE: NO FILE ###";
        return QString();
    }

    qDebug() << "### CHOOSE IMPORT FILE RESULT ###" << files.first();

    return files.first();
}

QString GpgBackend::encryptText(const QString &text,
                                const QString &fingerprint)
{
    qDebug() << "### ENCRYPT TEXT START ###";
    qDebug() << "### ENCRYPT FINGERPRINT ###" << fingerprint;
    qDebug() << "### ENCRYPT TEXT LENGTH ###" << text.length();

    if (text.isEmpty()) {
        qDebug() << "### ENCRYPT FEHLER: TEXT LEER ###";
        return "FEHLER: Kein Text zum Verschlüsseln.";
    }

    if (fingerprint.isEmpty()) {
        qDebug() << "### ENCRYPT FEHLER: KEIN SCHLÜSSEL ###";
        return "FEHLER: Kein Schlüssel ausgewählt.";
    }

    QProcess process;

    process.setProgram("/usr/bin/gpg");

    process.setArguments(QStringList()
                         << "--batch"
                         << "--yes"
                         << "--armor"
                         << "--trust-model"
                         << "always"
                         << "--recipient"
                         << fingerprint
                         << "--encrypt");

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert("GPG_TTY", "");
    process.setProcessEnvironment(env);

    qDebug() << "### ENCRYPT PROGRAM ###" << process.program();
    qDebug() << "### ENCRYPT ARGS ###" << process.arguments();

    process.start();

    if (!process.waitForStarted(5000)) {
        qDebug() << "### ENCRYPT START FEHLER ###"
                 << process.errorString();

        return QString("FEHLER: GPG konnte nicht gestartet werden: %1")
            .arg(process.errorString());
    }

    qDebug() << "### ENCRYPT GPG GESTARTET ###";

    process.write(text.toUtf8());
    process.closeWriteChannel();

    if (!process.waitForFinished(15000)) {
        qDebug() << "### ENCRYPT TIMEOUT ###";

        process.kill();
        process.waitForFinished();

        return "FEHLER: Zeitüberschreitung beim Verschlüsseln.";
    }

    QByteArray stdoutData =
        process.readAllStandardOutput();

    QByteArray stderrData =
        process.readAllStandardError();

    qDebug() << "### ENCRYPT EXIT CODE ###"
             << process.exitCode();

    qDebug() << "### ENCRYPT STDOUT ###"
             << QString::fromUtf8(stdoutData);

    qDebug() << "### ENCRYPT STDERR ###"
             << QString::fromUtf8(stderrData);

    if (process.exitCode() != 0) {
        qDebug() << "### ENCRYPT FEHLER ###";

        QString error = QString::fromUtf8(stderrData).trimmed();

        if (error.isEmpty())
            error = "Unbekannter GPG-Fehler.";

        return "FEHLER beim Verschlüsseln:\n" + error;
    }

    qDebug() << "### ENCRYPT SUCCESS ###";
    qDebug() << "### ENCRYPT OUTPUT LENGTH ###"
             << stdoutData.size();

    return QString::fromUtf8(stdoutData);
}


QString GpgBackend::decryptText(const QString &text, const QString &passphrase)
{
    qDebug() << "### DECRYPT TEXT START ###";
    qDebug() << "### DECRYPT TEXT LENGTH ###" << text.length();

    if (text.isEmpty()) {
        qDebug() << "### DECRYPT FEHLER: TEXT LEER ###";
        return "FEHLER: Kein Text zum Entschlüsseln.";
    }

    QProcess process;

    process.setProgram("/usr/bin/gpg");

    process.setArguments(QStringList()
                         << "--batch"
                         << "--yes"
                         << "--pinentry-mode"
                         << "loopback"
                         << "--passphrase-fd"
                         << "0"
                         << "--decrypt");

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert("GPG_TTY", "");
    process.setProcessEnvironment(env);

    qDebug() << "### DECRYPT PROGRAM ###" << process.program();
    qDebug() << "### DECRYPT ARGS ###" << process.arguments();

    process.start();

    if (!process.waitForStarted(5000)) {
        qDebug() << "### DECRYPT START FEHLER ###"
                 << process.errorString();

        return QString("FEHLER: GPG konnte nicht gestartet werden: %1")
            .arg(process.errorString());
    }

    qDebug() << "### DECRYPT GPG GESTARTET ###";

    process.write(passphrase.toUtf8());
    process.write("\n");
    process.write(text.toUtf8());
    process.closeWriteChannel();

    if (!process.waitForFinished(15000)) {
        qDebug() << "### DECRYPT TIMEOUT ###";

        process.kill();
        process.waitForFinished();

        return "FEHLER: Zeitüberschreitung beim Entschlüsseln.";
    }

    QByteArray stdoutData =
        process.readAllStandardOutput();

    QByteArray stderrData =
        process.readAllStandardError();

    qDebug() << "### DECRYPT EXIT CODE ###"
             << process.exitCode();

    qDebug() << "### DECRYPT STDOUT ###"
             << QString::fromUtf8(stdoutData);

    qDebug() << "### DECRYPT STDERR ###"
             << QString::fromUtf8(stderrData);

    if (process.exitCode() != 0) {
        qDebug() << "### DECRYPT FEHLER ###";

        QString error = QString::fromUtf8(stderrData).trimmed();

        if (error.isEmpty())
            error = "Unbekannter GPG-Fehler.";

        return "FEHLER beim Entschlüsseln:\n" + error;
    }

    qDebug() << "### DECRYPT SUCCESS ###";
    qDebug() << "### DECRYPT OUTPUT LENGTH ###"
             << stdoutData.size();

    return QString::fromUtf8(stdoutData);
}


QStringList GpgBackend::listSecretKeyIds()
{
    QStringList result;

    QProcess process;

    process.start("/usr/bin/gpg",
                  QStringList()
                  << "--list-secret-keys"
                  << "--with-colons");

    if (!process.waitForStarted(3000))
        return result;

    if (!process.waitForFinished(5000)) {
        process.kill();
        process.waitForFinished();
        return result;
    }

    QString output =
        QString::fromUtf8(process.readAllStandardOutput());

    const QStringList lines = output.split('\n');

    for (const QString &line : lines) {

        const QStringList fields = line.split(':');

        if (fields.isEmpty())
            continue;

        // "sec" = Secret Key / privater Hauptschlüssel
        if (fields[0] == "sec") {

            const QString keyId = fields.value(4);

            if (!keyId.isEmpty())
                result.append(keyId);
        }
    }

    return result;
}


QVariantList GpgBackend::listPublicKeys()
{
    QVariantList result;

    // Vorhandene private Hauptschlüssel ermitteln
    const QSet<QString> secretKeyIds =
        QSet<QString>::fromList(listSecretKeyIds());

    QProcess process;

    process.start("/usr/bin/gpg",
                  QStringList()
                  << "--list-keys"
                  << "--with-colons");

    if (!process.waitForStarted(3000)) {

        QVariantMap error;

        error["error"] =
            QString("Fehler beim Starten von gpg: %1")
                .arg(process.errorString());

        result.append(error);

        return result;
    }

    if (!process.waitForFinished(5000)) {

        process.kill();
        process.waitForFinished();

        QVariantMap error;

        error["error"] =
            "Timeout beim Auslesen der Schlüssel";

        result.append(error);

        return result;
    }

    QString output =
        QString::fromUtf8(process.readAllStandardOutput());

    const QStringList lines = output.split('\n');

    QVariantMap currentKey;

    bool haveKey = false;

    for (const QString &line : lines) {

        if (line.isEmpty())
            continue;

        const QStringList fields = line.split(':');

        if (fields.isEmpty())
            continue;

        const QString type = fields[0];

        // -------------------------------------------------
        // Öffentlicher Hauptschlüssel
        // -------------------------------------------------

        if (type == "pub") {

            if (haveKey)
                result.append(currentKey);

            currentKey.clear();
            haveKey = true;

            const QString keyId =
                fields.value(4);

            currentKey["keyId"] = keyId;

            currentKey["created"] =
                formatDate(fields.value(5));

            currentKey["expires"] =
                formatDate(fields.value(6));

            currentKey["algorithm"] =
                fields.value(3);

            currentKey["length"] =
                fields.value(2);

            currentKey["capabilities"] =
                fields.value(11);

            currentKey["name"] = "";
            currentKey["email"] = "";
            currentKey["fingerprint"] = "";

            // Ist der private Hauptschlüssel vorhanden?
            currentKey["hasSecretKey"] =
                secretKeyIds.contains(keyId);
        }

        // -------------------------------------------------
        // Fingerprint des Hauptschlüssels
        // -------------------------------------------------

        else if (type == "fpr" && haveKey) {

            if (currentKey["fingerprint"]
                    .toString()
                    .isEmpty()) {

                currentKey["fingerprint"] =
                    fields.value(9);
            }
        }

        // -------------------------------------------------
        // User ID
        // -------------------------------------------------

        else if (type == "uid" && haveKey) {

            QString uid =
                decodeGpgField(fields.value(9));

            const int start =
                uid.lastIndexOf('<');

            const int end =
                uid.lastIndexOf('>');

            if (start >= 0 && end > start) {

                const QString name =
                    uid.left(start).trimmed();

                const QString email =
                    uid.mid(
                        start + 1,
                        end - start - 1
                    ).trimmed();

                currentKey["name"] = name;
                currentKey["email"] = email;

            } else {

                currentKey["name"] =
                    uid;
            }
        }
    }

    // Letzten Schlüssel speichern
    if (haveKey)
        result.append(currentKey);

    return result;
}
