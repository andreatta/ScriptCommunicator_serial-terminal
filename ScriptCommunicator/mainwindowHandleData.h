#ifndef MAINWINDOWHANDLEDATA_H
#define MAINWINDOWHANDLEDATA_H

#include <QObject>
#include <QThread>
#include <QTime>
#include <QFile>
#include<QTextStream>
#include <QMessageBox>
#include <QXmlStreamWriter>
#include <QTimer>
#include <QScriptEngine>
#include "settingsdialog.h"


class MainWindow;
class CustomConsoleLogObject;

namespace Ui {
class MainWindow;
}

///The type of data in a StoredData struct.
typedef enum
{
    ///Received data.
    STORED_DATA_TYPE_RECEIVE,

    ///Send data.
    STORED_DATA_TYPE_SEND,

    ///Timestamp.
    STORED_DATA_TYPE_TIMESTAMP,

    ///User message (add message dialog).
    STORED_DATA_TYPE_USER_MESSAGE,

    ///New line.
    STORED_DATA_TYPE_NEW_LINE,

    ///Clear all standard consoles.
    STORED_DATA_CLEAR_ALL_STANDARD_CONSOLES,

    ///Invalid entry.
    STORED_DATA_TYPE_INVALID

}StoredDataType;

///The stored console/log data.
typedef struct
{
    StoredDataType type;
    QByteArray data;
    bool isFromCan;
    bool isSend;

}StoredData;

///The data which is needed for the mixed console.
typedef struct
{   ///The number of pixels per character.
    int pixelsWide;

    ///The divider for the bytes bytes per line calcualtion.
    double divider;

    ///True if only the type is in the mixed console (ascii, or hex...).
    bool onlyOneType;

    ///The bytes per decimal.
    int bytesPerDecimal;

    ///The max. number of bytes per line.
    int maxBytePerLine;

    ///The spaces for the ascii characters.
    QString asciiSpaces;

    ///The spaces for the hex characters.
    QString hexSpaces;

    ///The extra spaces for the hex characters.
    QString hexExtraSpaces;

    ///The spaces for the decimal characters.
    QString decimalSpaces;


}MixedConsoleData;


///Contains the MainWindow functions for handling the sent and recieved data.
class MainWindowHandleData : public QObject
{
    Q_OBJECT
    friend class MainWindow;
public:
    explicit MainWindowHandleData(MainWindow* mainWindow, SettingsDialog* settingsDialog,
                                  Ui::MainWindow* userInterface);

    ~MainWindowHandleData();

    ///Appends data to the m_storedData.
    void appendDataToStoredData(QByteArray &data, bool isSend, bool isUserMessage, bool isFromCan, bool forceTimeStamp);

    ///Creates the string for the mixed console.
    QString createMixedConsoleString(const QByteArray &data, bool hasCanMeta);

    ///Caclulates the mixed console data.
    void calculateMixedConsoleData();

    ///Appends data to the console strings (m_consoleDataBufferAscii, m_consoleDataBufferHex;
    ///m_consoleDataBufferDec)
    void appendDataToConsoleStrings(QByteArray& data, bool isSend, bool isUserMessage, bool isTimeStamp, bool isFromCan, bool isNewLine);

    ///Appends data the log file.
    void appendDataToLog(const QByteArray& data, bool isSend, bool isUserMessage, bool isTimeStamp, bool isFromCan, bool isNewLine);

    ///Clears all stored data.
    void clear(void);

    ///Reinserts the data into the consoles.
    void reInsertDataInConsole(void);

    ///Returns the number of bytes for a decimal type.
    qint32 bytesPerDecimalInConsole(DecimalType decimalType);

    ///Adds data to the send history.
    void addDataToSendHistory(const QByteArray* data);

    ///Send the history.
    void sendHistory(void);

    ///Cancel send the history.
    void cancelSendHistory(void);

signals:
    ///This signal is emitted for sending data with the main interface.
    void sendDataWithTheMainInterfaceSignal(const QByteArray data, uint id);

public slots:

    ///The history console timer slot.
    void historyConsoleTimerSlot();

    ///The send history timer slot.
    void sendHistoryTimerSlot();

    ///Slotfunction for m_checkDebugWindowsIsClosed.
    void checkDebugWindowsIsClosedSlot();


    ///The function append the received/send data to the consoles and the logs.
    ///This function is called periodically (log update interval).
    void updateConsoleAndLog(void);

    ///The slot is called if the main interface thread has received data.
    ///This slot is connected to the MainInterfaceThread::dataReceivedSignal signal.
    void dataReceivedSlot(QByteArray data);

    ///The slot is called if the main interface thread has received data.
    ///This slot is connected to the MainInterfaceThread::dataReceivedSignal signal.
    void canMessagesReceivedSlot(QVector<QByteArray> messages);

    ///The slot is called if the main interface thread has send data.
    ///This slot is connected to the MainInterfaceThread::sendingFinishedSignal signal.
    void dataHasBeenSendSlot(QByteArray data, bool success, uint id);

    ///Reinserts the data into the mixed consoles.
    void reInsertDataInMixecConsoleSlot(void);

private:

    ///Enables/disables the send history GUI elements.
    void enableHistoryGuiElements(bool enable);

    ///Append a time stamp to a stored data vector.
    void appendTimestamp(QVector<StoredData>* storedDataVector, bool isSend, bool isUserMessage,
                         bool isFromCan, QString timeStampFormat);

    ///Append a new line to a stored data vector.
    void appendNewLine(QVector<StoredData>* storedDataVector, bool isSend,
                                     bool isFromCan);

    ///Appends data to the unprocessed console data.
    void appendUnprocessConsoleData(QByteArray &data, bool isSend, bool isUserMessage, bool isFromCan, bool forceTimeStamp=false, bool isRecursivCall=false);

    ///Appends data to the unprocessed log data.
    void appendUnprocessLogData(const QByteArray &data, bool isSend, bool isUserMessage, bool isFromCan, bool forceTimeStamp=false, bool isRecursivCall=false);

    ///Processes the data in m_storedData (creates the log and the console strings).
    ///Note: m_storedData is cleared in this function.
   void processDataInStoredData();

    ///Pointer to the main window.
    MainWindow* m_mainWindow;

    ///Pointer to the settings dialog.
    SettingsDialog *m_settingsDialog;

    ///Pointer to the main windiw user interface.
    Ui::MainWindow *m_userInterface;

    ///Cyclic timer which call the function updateConsoleAndLog.
    QTimer *m_updateConsoleAndLogTimer ;

    ///The data buffer for the ascii console.
    QString m_consoleDataBufferAscii;

    ///The data buffer for the hex console.
    QString m_consoleDataBufferHex;

    ///The data buffer for the decimal console.
    QString m_consoleDataBufferDec;

    ///The data buffer for the mixed console.
    QString m_consoleDataBufferMixed;

    ///The data buffer for the binary console.
    QString m_consoleDataBufferBinary;

    ///Time stamp for the last console entry.
    QTime lastTimeInConsole;

    ///The number of received bytes.
    quint64 m_receivedBytes;

    ///The number of sent bytes.
    quint64 m_sentBytes;

    ///The html log file.
    QFile m_htmlLogFile;

    ///The html log file stream.
    QTextStream m_HtmlLogFileStream;

    ///The text log file.
    QFile m_textLogFile;

    ///The custom log file.
    QFile m_customLogFile;

    ///The text log file stream.
    QTextStream m_textLogFileStream;

    ///The text log file stream.
    QTextStream m_customLogFileStream;

    ///The unprocessed console data.
    QVector<StoredData> m_unprocessedConsoleData;

    ///The unprocessed log data.
    QVector<StoredData> m_unprocessedLogData;

    ///Bytes in m_unprocessedLogData;
    quint32 m_bytesInUnprocessedConsoleData;

    ///The stored console data.
    QVector<StoredData> m_storedConsoleData;

    ///Bytes in m_storedConsoleData;
    quint32 m_bytesInStoredConsoleData;

    ///The number of sent/received bytes after the last new line console
    quint32 m_bytesSinceLastNewLineInConsole;

    ///The number of sent/received bytes after the last new line log
    quint32 m_bytesSinceLastNewLineInLog;

    ///The custom log string.
    QString m_customLogString;

    ///The custom console object.
    CustomConsoleLogObject* m_customConsoleObject;

    ///The custom log object.
    CustomConsoleLogObject* m_customLogObject;

    ///The custom console strings.
    QStringList m_customConsoleStrings;

    ///The custom console stored strings.
    QStringList m_customConsoleStoredStrings;

    ///The number of bytes in m_customConsoleStrings.
    quint32 m_numberOfBytesInCustomConsoleStrings;

    ///The number of bytes in m_customConsoleStoredStrings
    quint32 m_numberOfBytesInCustomConsoleStoredStrings;

    ///The mixed console data.
    MixedConsoleData m_mixedConsoleData;

    ///The byte buffer for the decimal console.
    ///If insufficent number of bytes for a decimal are received then these a bytes are stored here.
    QByteArray m_decimalConsoleByteBuffer;

    ///The byte buffer for the mixed console.
    ///If insufficent number of bytes for a decimal are received then these a bytes are stored here.
    QByteArray m_mixedConsoleByteBuffer;

    ///The byte buffer for the decimal log.
    ///If insufficent number of bytes for a decimal are received then these a bytes are stored here.
    QByteArray m_decimalLogByteBuffer;

    ///The send history buffer.
    QVector<QByteArray> m_sendHistory;

    ///The history data which must be sent.
    QVector<QByteArray> m_sendHistorySendData;

    ///The max. number of elements in m_sendHistory.
    static const qint32 MAX_SEND_HISTORY_ENTRIES = 30;

    ///The history console timer.
    QTimer m_historyConsoleTimer;

    ///The send history timer.
    QTimer m_sendHistoryTimer;

    ///True if the history is currently sent.
    bool m_historySendIsInProgress;

    ///Checks if the custom console/log script debug window has been closed.
    QTimer m_checkDebugWindowsIsClosed;

};

#endif // MAINWINDOWHANDLEDATA_H
