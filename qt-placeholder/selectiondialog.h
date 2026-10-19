#pragma once

#include <QDialog>
#include <QList>
#include <QString>
#include <QStringList>

#include <functional>

class QLabel;
class QListWidget;
class QPushButton;
class QWidget;

struct SelectionOption {
    QString value;
    QString label;
};

struct SelectionDialogConfig {
    QString title;
    QString instruction;
    QString searchPlaceholder;
    QString itemName;
    QString settingsKey;
    QString continueLabel;
    QList<SelectionOption> options;
    QString detailsLabel;
    std::function<void(const QString &, QWidget *)> showDetails;
};

class SelectionDialog : public QDialog {
public:
    explicit SelectionDialog(const SelectionDialogConfig &config, QWidget *parent = nullptr);
    QStringList selectedValues() const;

private:
    void updateCount();

    QString itemName_;
    QString settingsKey_;
    QListWidget *itemList_ = nullptr;
    QLabel *countLabel_ = nullptr;
    QPushButton *continueButton_ = nullptr;
};
