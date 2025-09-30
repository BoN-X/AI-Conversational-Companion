#include "ChatWindow.h"
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QScrollBar>

#include "../core/persona_engine.h"

ChatWindow::ChatWindow(QWidget* parent)
    : QMainWindow(parent),
    chat_(new QTextEdit(this)),
    input_(new QLineEdit(this)),
    send_(new QPushButton("发送", this)),
    net_(new QNetworkAccessManager(this)) {

    chat_->setReadOnly(true);
    chat_->setMinimumSize(600, 480);

    auto *w = new QWidget(this);
    auto *lay = new QVBoxLayout(w);
    lay->addWidget(chat_);
    lay->addWidget(input_);
    lay->addWidget(send_);
    setCentralWidget(w);
    setWindowTitle(QStringLiteral("Luna - 虚拟女友 (本地LLM)"));

    connect(send_, &QPushButton::clicked, this, &ChatWindow::onSend);
    connect(input_, &QLineEdit::returnPressed, this, &ChatWindow::onSend);

    // 初始化 system 提示
    PersonaEngine pe;
    UiMessage sys{ "system", QString::fromStdString(pe.buildSystemPrompt("assets")) };
    history_.push_back(sys);
    appendBubble("System", "(已加载人设)");
}

ChatWindow::~ChatWindow() = default;

void ChatWindow::appendBubble(const QString& who, const QString& text) {
    chat_->append(QString("<b>%1:</b> %2").arg(who, text.toHtmlEscaped()));
    chat_->verticalScrollBar()->setValue(chat_->verticalScrollBar()->maximum());
}

void ChatWindow::onSend() {
    const QString userText = input_->text().trimmed();
    if (userText.isEmpty()) return;

    input_->clear();
    appendBubble("You", userText);

    history_.push_back(UiMessage{ "user", userText });
    postChat(history_);
    send_->setEnabled(false);
    input_->setEnabled(false);
}

void ChatWindow::postChat(const QList<UiMessage>& messages) {
    QJsonArray arr;
    for (const auto& m : messages) {
        QJsonObject o;
        o["role"] = m.role;
        o["content"] = m.content;
        arr.push_back(o);
    }

    QJsonObject body;
    body["model"] = modelName_;
    body["messages"] = arr;
    body["stream"] = false;        // 简化：先不用流式；想要流式可改为 SSE
    body["temperature"] = 0.7;     // 口语更自然

    QNetworkRequest req((QUrl(apiUrl_)));
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    auto* reply = net_->post(req, QJsonDocument(body).toJson(QJsonDocument::Compact));
    connect(reply, &QNetworkReply::finished, this, &ChatWindow::onReplyFinished);
}

void ChatWindow::onReplyFinished() {
    auto* reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) return;

    send_->setEnabled(true);
    input_->setEnabled(true);

    if (reply->error() != QNetworkReply::NoError) {
        appendBubble("Error", reply->errorString());
        reply->deleteLater();
        return;
    }

    const auto raw = reply->readAll();
    reply->deleteLater();

    // llama.cpp server（--api）是 OpenAI 兼容格式：
    // choices[0].message.content
    const auto doc = QJsonDocument::fromJson(raw);
    const auto obj = doc.object();
    const auto choices = obj.value("choices").toArray();
    if (choices.isEmpty()) {
        appendBubble("Luna", "(空回复)");
        return;
    }
    const auto msg = choices[0].toObject().value("message").toObject();
    const auto content = msg.value("content").toString();

    history_.push_back(UiMessage{ "assistant", content });
    appendBubble("Luna", content);
}
