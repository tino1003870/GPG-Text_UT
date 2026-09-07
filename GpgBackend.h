#ifndef GPGBACKEND_H
#define GPGBACKEND_H

#include <QObject>
#include <QString>
#include <QVariantList>

class GpgBackend : public QObject
{
    Q_OBJECT

public:
    explicit GpgBackend(QObject *parent = nullptr);

    Q_INVOKABLE QString testGpg();
    Q_INVOKABLE QVariantList listPublicKeys();
    Q_INVOKABLE QString deleteKey(const QString &fingerprint);
    Q_INVOKABLE QString importKey(const QString &filePath, const QString &passphrase);
    Q_INVOKABLE QString platformName() const;
    Q_INVOKABLE QString chooseImportFile();

    Q_INVOKABLE QString encryptText(const QString &text,
                                     const QString &fingerprint);

    Q_INVOKABLE QString decryptText(const QString &text);

private:
    QStringList listSecretKeyIds();
};

#endif // GPGBACKEND_H
