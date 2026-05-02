#include "CliManager.h"
#include <QMap>
#include <QString>
// #include <format>
#include <iostream>

namespace {
static const QString BOLD = "\033[1m";
static const QString DIM = "\033[2m";
static const QString CYAN = "\033[36m";
static const QString YELLOW = "\033[33m";
static const QString GREEN = "\033[32m";
static const QString RED = "\033[31m";
static const QString RESET = "\033[0m";

QStringList tokenize(const QString& line, QString& err)
{
    QStringList tokens;
    QString current;
    bool inDouble = false;
    bool inSingle = false;

    for (int i = 0; i < line.size(); ++i) {
        QChar c = line[i];

        if (inSingle) {
            if (c == '\'')
                inSingle = false;
            else
                current += c;
        } else if (inDouble) {
            if (c == '"')
                inDouble = false;
            else if (c == '\\' && i + 1 < line.size()) {
                // inside double quotes only \" and \\ are escape sequences
                QChar next = line[i + 1];
                if (next == '"' || next == '\\') {
                    current += next;
                    ++i;
                } else {
                    current += c;
                }
            } else
                current += c;
        } else {
            // unquoted
            if (c == '"') {
                inDouble = true;
            } else if (c == '\'') {
                inSingle = true;
            } else if (c == '\\' && i + 1 < line.size()) {
                current += line[++i]; // escape next char literally
            } else if (c.isSpace()) {
                if (!current.isEmpty()) {
                    tokens << current;
                    current.clear();
                }
            } else {
                current += c;
            }
        }
    }

    if (inDouble || inSingle) {
        err = inDouble ? "Unterminated double quote" : "Unterminated single quote";
        return {};
    }

    if (!current.isEmpty())
        tokens << current;

    return tokens;
}

QMap<QString, QString> parseFlags(const QStringList& parts)
{
    QMap<QString, QString> flags;
    for (int i = 0; i < parts.size(); ++i) {
        QString part = parts[i];
        if (part.startsWith("--")) {
            QString key = part.sliced(2); // remove "--"
            if (i + 1 < parts.size() && !parts[i + 1].startsWith("-")) {
                flags.insert(key, parts[++i]);
            } else {
                flags.insert(key, "true"); // boolean flag with no value
            }
        } else if (part.startsWith("-")) {
            QString key = part.sliced(1); // remove "-"
            if (i + 1 < parts.size() && !parts[i + 1].startsWith("-")) {
                flags.insert(key, parts[++i]);
            } else {
                flags.insert(key, "true");
            }
        }
    }
    return flags;
}

void printPort(QTextStream& out, Port* port, int portGlobalIdx)
{
    QString dir = (port->type == PortType::Input) ? "in " : "out";
    QString label = QString("[%1 %2]").arg(dir).arg(portGlobalIdx);
    QString item = port->item ? port->item->itemName : "?";
    QString qty = QString::number(port->amount, 'f', 1); // strip trailing ".000"

    out << QString("    %1  %2x %3")
               .arg(label, -8) // left-align in 8 chars
               .arg(qty, 4) // right-align qty in 4 chars
               .arg(item);

    if (port->connectedTo.isEmpty()) {
        out << DIM << "  (unconnected)" << RESET << "\n";
    } else {
        out << "  →";
        for (Port* peer : port->connectedTo) {
            int peerNodeIdx = peer->owner.index();
            int peerPortIdx = peer->owner.getPortIndex(*peer);
            QString peerItem = peer->item ? peer->item->itemName : "?";
            QString peerQty = QString::number(peer->amount, 'f', 0);
            out << RED << QString("  [%1:%2] %3x %4").arg(peerNodeIdx).arg(peerPortIdx).arg(peerQty, 4).arg(peerItem) << RESET;
        }
        out << "\n";
    }
}

void printProductionNode(QTextStream& out, ProductionNode* node, Factory* factory)
{
    int idx = factory->subNodes().indexOf(node);

    QString recipeName = "none";
    QString machineName = "";
    QString cycleInfo = "";
    if (const Recipe* r = node->currentRecipe()) {
        recipeName = r->recipeClass;
        if (r->producedIn)
            machineName = r->producedIn->machineName;
        cycleInfo = QString("  (%1s, %2MW)")
                        .arg(r->recipeTime, 0, 'f', 1)
                        .arg(r->producedIn ? r->producedIn->powerConsumption : 0.f, 0, 'f', 0);
    }

    QString limitStr = (node->machineLimit() >= 0)
        ? QString("  [limit: %1]").arg(node->machineLimit(), 0, 'f', 2)
        : QString();
    out << CYAN << BOLD
        << QString("┌─ [%1] %2  \"%3\"  x%4%5\n")
               .arg(idx)
               .arg(machineName)
               .arg(node->name())
               .arg(node->machineCount(), 0, 'f', 2)
               .arg(limitStr)
        << RESET;
    out << CYAN << "│  " << RESET
        << "recipe : " << recipeName << cycleInfo << "\n";

    // inputs
    int portIdx = 0;
    for (Port* p : node->inputs()) {
        out << CYAN << "│  " << RESET;
        printPort(out, p, portIdx++);
    }
    // outputs (portIdx continues from inputs count — matches getPortFromIndex)
    for (Port* p : node->outputs()) {
        out << CYAN << "│  " << RESET;
        printPort(out, p, portIdx++);
    }
    out << CYAN << "└" << QString("─").repeated(50) << RESET << "\n";
}

void printExtractionNode(QTextStream& out, ExtractionNode* node, Factory* factory)
{
    int idx = factory->subNodes().indexOf(node);

    QString extractorName = node->getExtractorName();
    QString resourceName = node->currentRecipe() ? node->currentRecipe()->resource->itemName : "none";

    QString limitStr = (node->machineLimit() >= 0)
        ? QString("  [limit: %1]").arg(node->machineLimit(), 0, 'f', 2)
        : QString();
    out << GREEN << BOLD
        << QString("┌─ [%1] %2  \"%3\"  x%4%5\n")
               .arg(idx).arg(extractorName).arg(node->name())
               .arg(node->machineCount(), 0, 'f', 2).arg(limitStr)
        << RESET;
    static const QMap<NodePurity, QString> purityNames = {
        { NodePurity::Impure, "Impure" },
        { NodePurity::Normal, "Normal" },
        { NodePurity::Pure,   "Pure"   },
    };
    out << GREEN << "│  " << RESET
        << "resource : " << resourceName
        << "  purity: " << purityNames.value(node->purity())
        << "\n";

    int portIdx = 0;
    for (Port* p : node->outputs()) {
        out << GREEN << "│  " << RESET;
        printPort(out, p, portIdx++);
    }
    out << GREEN << "└" << QString("─").repeated(50) << RESET << "\n";
}

void printFactoryNode(QTextStream& out, FactoryNode* node, Factory* factory)
{
    int idx = factory->subNodes().indexOf(node);

    out << YELLOW << BOLD
        << QString("┌─ [%1] Factory  \"%2\"  (%3 nodes inside)\n")
               .arg(idx)
               .arg(node->name())
               .arg(node->factory().subNodes().count())
        << RESET;

    int portIdx = 0;
    for (Port* p : node->inputs()) {
        out << YELLOW << "│  " << RESET;
        printPort(out, p, portIdx++);
    }
    for (Port* p : node->outputs()) {
        out << YELLOW << "│  " << RESET;
        printPort(out, p, portIdx++);
    }
    out << YELLOW << "└" << QString("─").repeated(50) << RESET << "\n";
}

} // namespace

CliManager::CliManager(QObject* parent)
    : QObject(parent)
    , m_out(stdout)
    , m_session(&SessionManager::get())
{
    m_notifier = new QSocketNotifier(0, QSocketNotifier::Read, this);
    connect(m_notifier, &QSocketNotifier::activated, this, &CliManager::onInputReady);

    m_out << "\n=== Factory CLI Console Initialized ===" << Qt::endl;
    m_out << "Type 'help' for a list of commands." << Qt::endl;
    printPrompt();
}

CliManager::~CliManager()
{
    // std::cout.rdbuf(m_oldBuf);
    // endwin();
    // delete
    // delete m_logBuf;
}

void CliManager::printPrompt()
{
    const QString GREEN = "\033[32m";
    const QString RESET_COLOR = "\033[0m";
    auto active = SessionManager::get().activeFactory();
    QString path = active ? active->name() : "Root";
    m_out << GREEN << "[" << path << "]> " << RESET_COLOR << Qt::flush;
}

void CliManager::processCommand(const QString& line)
{
    QString err;
    QStringList parts = tokenize(line, err);

    if (!err.isEmpty()) {
        m_out << "Parse error: " << err << "\n";
        return;
    }
    if (parts.isEmpty())
        return;

    QString cmd = parts.takeFirst().toLower();

    static const QMap<QString, std::function<void(const QStringList&)>> commands = {
        { "add", [this](const auto& p) { handleAdd(p); } },
        { "rm", [this](const auto& p) { handleRm(p); } },
        { "ls", [this](const auto& p) { handleLs(); } },
        // { "cd", [this](const auto& p) { handleCd(p); } },
        // { "cnct", [this](const auto& p) { handleConnect(p); } },
        // { "limit", [this](const auto& p) { handleLimit(p); } },
        // { "purity", [this](const auto& p) { handlePurity(p); } },
        // { "tier", [this](const auto& p) { handleTier(p); } },
        // { "solve", [this](const auto&) { handleSolve(); } },
        // { "help", [this](const auto& p) { handleHelp(); } },
        { "exit", [this](const auto&) { emit quitRequested(); } },
        { "quit", [this](const auto&) { emit quitRequested(); } },
    };

    auto it = commands.find(cmd);
    if (it != commands.end())
        it.value()(parts);
    else
        m_out << "Unknown command: " << cmd << ". Type 'help' for available commands.\n";
}

void CliManager::handleAdd(const QStringList& parts)
{
    if (parts.isEmpty()) {
        m_out << "Usage:\n"
                 "  add prod <RecipeClass> [--name <name>]\n"
                 "  add fact [--name <name>]\n";
        return;
    }

    QString sub = parts[0].toLower();
    QStringList rest = parts.mid(1);
    //
    if (sub == "prod")
        handleAddProd(rest);
    else if (sub == "extractor" || sub == "extr")
        handleAddExtractor(rest);
    else if (sub == "fact")
        handleAddFact(rest);
    else
        m_out << "Unknown add subcommand: " << sub << ". Expected 'prod' or 'fact'.\n";
}

void CliManager::handleAddProd(const QStringList& parts)
{
    if (parts.isEmpty()) {
        m_out << "Usage: add prod <RecipeClass> [--name <name>]\n";
        return;
    }

    QString klass = parts[0]; // first positional = recipe class
    auto flags = parseFlags(parts.mid(1)); // remaining = optional flags
    QString name = flags.value("name");

    auto* factory = SessionManager::get().activeFactory();
    ProductionNode* node = m_session->createProductionNodeByClass(klass, factory, name);

    if (!node)
        m_out << "Unknown recipe class: " << klass << "\n";
    else
        m_out << "Added production node"
              << (name.isEmpty() ? "" : " \"" + name + "\"")
              << " (" << klass << ")\n";
}

void CliManager::handleAddExtractor(const QStringList& parts)
{
    if (parts.isEmpty()) {
        m_out << "Usage: add extractor <RecipeClass> [--name <name>]\n";
        return;
    }

    QString klass = parts[0]; // first positional = recipe class
    auto flags = parseFlags(parts.mid(1)); // remaining = optional flags
    QString name = flags.value("name");

    auto* factory = SessionManager::get().activeFactory();
    ExtractionNode* node = m_session->createExtractionNodeByName(klass, 1, factory, name);

    if (!node)
        m_out << "Unknown recipe class: " << klass << "\n";
    else
        m_out << "Added production node"
              << (name.isEmpty() ? "" : " \"" + name + "\"")
              << " (" << klass << ")\n";
}

void CliManager::handleAddFact(const QStringList& parts)
{
    auto flags = parseFlags(parts);
    QString name = flags.value("name");

    // m_out << "Name is: " << name << "\n";
    Factory* node = m_session->createFactory(m_session->activeFactory(), name);

    if (!node)
        m_out << "Failed to create factory.\n";
    else
        m_out << "Added factory"
              << (name.isEmpty() ? "" : " \"" + name + "\"")
              << "\n";
}

void CliManager::handleRm(const QStringList& args)
{
    // bool ok;
    // int index = args[0].toInt(&ok);
    // if (!ok)
    //     return;
    //
    // auto factory = SessionManager::get().activeFactory();
    // auto nodes = factory->nodes();
    //
    // if (index >= 0 && index < nodes.size()) {
    //     AbstractNode* target = nodes.at(index);
    //     SessionManager::get().deleteNode(target);
    //     return;
    // }
}

void CliManager::handleLs()
{
    auto* factory = SessionManager::get().activeFactory();
    if (!factory || factory->subNodes().isEmpty()) {
        m_out << DIM << "(empty factory)\n"
              << RESET;
        return;
    }
    for (AbstractNode* node : factory->subNodes()) {
        if (auto* p = dynamic_cast<ProductionNode*>(node))
            printProductionNode(m_out, p, factory);
        else if (auto* e = dynamic_cast<ExtractionNode*>(node))
            printExtractionNode(m_out, e, factory);
        else if (auto* f = dynamic_cast<FactoryNode*>(node))
            printFactoryNode(m_out, f, factory);
        m_out << "\n";
    }
}

// -------------------- SLOTS --------------------------
void CliManager::onInputReady()
{
    std::string line;
    if (!std::getline(std::cin, line)) {
        emit quitRequested();
        return;
    }
    QString qline = QString::fromStdString(line).trimmed();

    if (!qline.isEmpty()) {
        processCommand(qline);
    }
    printPrompt();
}
