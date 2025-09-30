#pragma once
#include <QMainWindow>
#include <QVector>
#include <QPair>

class QTextEdit;
class QLineEdit;
class QPushButton;
class QNetworkAccessManager;
class QNetworkReply;

struct UiMessage {
    QString role;    // "system"|"user"|"assistant"
    QString content;
};

class ChatWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit ChatWindow(QWidget* parent = nullptr);
    ~ChatWindow() override;

private slots:
    void onSend();
    void onReplyFinished();

private:
    void appendBubble(const QString& who, const QString& text);
    void postChat(const QList<UiMessage>& messages);

private:
    QTextEdit* chat_;
    QLineEdit* input_;
    QPushButton* send_;

    QNetworkAccessManager* net_;
    QList<UiMessage> history_;

    // 你可在 UI 里做成可配置；这里先写死默认地址
    QString apiUrl_ = "http://127.0.0.1:8080/v1/chat/completions";
    QString modelName_ = "local"; // llama.cpp server 会忽略或接受任意占位
};

