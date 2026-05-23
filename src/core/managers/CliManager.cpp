#include "CliManager.h"
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QMap>
#include <QString>
#include "core/nodes/Connection.h"
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

    if (port->connections.isEmpty()) {
        out << DIM << "  (unconnected)" << RESET << "\n";
    } else {
        out << "  →";
        for (Connection* conn : port->connections) {
            Port* peer = conn->getPeer(*port);
            int peerNodeIdx = peer->owner.index();
            int peerPortIdx = peer->owner.getPortIndex(*peer);
            QString peerItem = peer->item ? peer->item->itemName : "?";
            QString peerQty = QString::number(peer->amount, 'f', 0);
            out << RED << QString("  [%1:%2] %3x %4").arg(peerNodeIdx).arg(peerPortIdx).arg(peerQty, 4).arg(peerItem) << RESET;
        }
        out << "\n";
    }
}

void printNode(QTextStream& out, AbstractNode* node, Factory* factory)
{
    QString color;
    switch (node->type()) {
    case NodeType::Production:  color = CYAN;   break;
    case NodeType::Extraction:  color = GREEN;  break;
    case NodeType::Factory:
    case NodeType::FactoryEdge:
    case NodeType::Splitter:    color = YELLOW; break;
    default:
        out << "WARNING: UNHANDLED PRINT\n";
        return;
    }

    out << color << BOLD << node->getHeaderInfo() << "\n" << RESET;

    int portIdx = 0;
    for (auto& p : node->inputs()) {
        out << color << "│  " << RESET;
        printPort(out, p.get(), portIdx++);
    }
    for (auto& p : node->outputs()) {
        out << color << "│  " << RESET;
        printPort(out, p.get(), portIdx++);
    }
    out << color << "└" << QString("─").repeated(50) << RESET << "\n";
}
} // namespace

CliManager::CliManager(QObject* parent)
    : QObject(parent)
    , m_out(stdout)
    , m_session(&SessionManager::get())
{
    s_instance = this;
    qInstallMessageHandler(messageHandler);
    m_notifier = new QSocketNotifier(0, QSocketNotifier::Read, this);
    connect(m_notifier, &QSocketNotifier::activated, this, &CliManager::onInputReady);

    connect(m_session, &SessionManager::operationFailed,
        this, [this](const QString msg) { m_out << "Error: " << msg << "\n"; });

    m_out << "\n=== Factory CLI Console Initialized ===" << Qt::endl;
    m_out << "Type 'help' for a list of commands." << Qt::endl;
    printPrompt();
}

CliManager::~CliManager()
{
    qInstallMessageHandler(nullptr);
    s_instance = nullptr;
}

CliManager* CliManager::s_instance = nullptr;

void CliManager::messageHandler(QtMsgType type, const QMessageLogContext& ctx, const QString& msg)
{
    if (!s_instance)
        return;
    QString prefix;
    switch (type) {
    case QtDebugMsg:
        prefix = DIM + "[log] " + RESET;
        break;
    case QtWarningMsg:
        prefix = YELLOW + "[warn] " + RESET;
        break;
    case QtCriticalMsg:
        prefix = RED + "[crit] " + RESET;
        break;
    case QtFatalMsg:
        prefix = RED + "[fatal] " + RESET;
        break;
    default:
        break;
    }
    s_instance->m_out << "\r\033[2k" << prefix << msg << "\n";
    s_instance->printPrompt();
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
        { "cd", [this](const auto& p) { handleCd(p); } },
        { "cnct", [this](const auto& p) { handleConnect(p); } },
        { "discnct", [this](const auto& p) { handleDisconnect(p); } },
        { "limit", [this](const auto& p) { handleLimit(p); } },
        { "purity", [this](const auto& p) { handlePurity(p); } },
        { "tier", [this](const auto& p) { handleTier(p); } },
        { "solve", [this](const auto&) { handleSolve(); } },
        { "report", [this](const auto&) { handleReport(); } },
        { "rename", [this](const auto& p) { handleRename(p); } },
        { "save", [this](const auto& p) { handleSave(p); } },
        { "load", [this](const auto& p) { handleLoad(p); } },
        // FIX: fix help
        //
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
                 "  add fact [--name <name>]\n"
                 "  add splitter [w1 w2 ...] [--name <name>]\n";
        return;
    }

    QString sub = parts[0].toLower();
    QStringList rest = parts.mid(1);
    if (sub == "prod")
        handleAddProd(rest);
    else if (sub == "extractor" || sub == "extr")
        handleAddExtractor(rest);
    else if (sub == "fact")
        handleAddFact(rest);
    else if (sub == "edge")
        handleAddEdge(rest);
    else if (sub == "splitter" || sub == "split")
        handleAddSplitter(rest);
    else
        m_out << "Unknown add subcommand: " << sub << "\n";
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
        m_out << "Added Extraction node"
              << (name.isEmpty() ? "" : " \"" + name + "\"")
              << " (" << klass << ")\n";
}

void CliManager::handleAddFact(const QStringList& parts)
{
    auto flags = parseFlags(parts);
    QString name = flags.value("name");

    Factory* node = m_session->createFactory(m_session->activeFactory(), name);

    if (!node)
        m_out << "Failed to create factory.\n";
    else
        m_out << "Added factory"
              << (name.isEmpty() ? "" : " \"" + name + "\"")
              << "\n";
}

void CliManager::handleAddEdge(const QStringList& parts)
{
    // positional: add edge <in|out|input|output> [--name <name>]
    // flag-based: add edge --type <in|out|input|output> [--name <name>]
    QString edgeType_raw;
    QStringList rest = parts;
    if (!rest.isEmpty() && !rest[0].startsWith('-'))
        edgeType_raw = rest.takeFirst();
    auto flags = parseFlags(rest);
    QString name = flags.value("name");
    if (edgeType_raw.isEmpty())
        edgeType_raw = flags.value("type");
    PortType edgeType = portTypeFromString(edgeType_raw);
    FactoryEdgeNode* node = m_session->createFactoryEdgeNode(edgeType, nullptr, name);

    if (!node)
        m_out << "Failed to create factory.\n";
    else
        m_out << "Added Edge"
              << (name.isEmpty() ? "" : " \"" + name + "\"")
              << (" " + stringFromPortType(edgeType))
              << "\n";
}

void CliManager::handleAddSplitter(const QStringList& parts)
{
    QList<Frac> weights;
    QString name;

    for (int i = 0; i < parts.size(); ++i) {
        if (parts[i] == "--name" && i + 1 < parts.size()) {
            name = parts[++i];
        } else {
            bool ok;
            int w = parts[i].toInt(&ok);
            if (ok && w > 0)
                weights << Frac(w);
        }
    }

    if (weights.size() < 2)
        weights = { Frac(1), Frac(1) };

    SplitterNode* node = m_session->createSplitterNode(weights, m_session->activeFactory(), name);
    if (!node)
        m_out << "Failed to create splitter.\n";
    else
        m_out << "Added splitter" << (name.isEmpty() ? "" : " \"" + name + "\"")
              << " (" << weights.size() << " outputs)\n";
}

void CliManager::handleRm(const QStringList& args)
{
    if (args.size() == 0) {
        m_out << "Usage: rm <idx>\n";
        return;
    }
    bool ok;
    int index = args[0].toInt(&ok);
    if (!ok)
        return;

    auto factory = SessionManager::get().activeFactory();
    auto& nodes = factory->nodes();

    if (index >= 0 && index < nodes.size()) {
        AbstractNode* target = nodes.at(index);
        SessionManager::get().deleteNode(target);
        return;
    }
}

void CliManager::handleLs()
{
    auto* factory = SessionManager::get().activeFactory();
    if (!factory || factory->nodes().empty()) {
        m_out << DIM << "(empty factory)\n"
              << RESET;
        return;
    }
    for (const auto& node : factory->nodes()) {
        printNode(m_out, node, factory);
        m_out << "\n";
    }
}

void CliManager::handleCd(const QStringList& args)
{
    if (args.empty()) {
        m_session->enterRootFactory();
        return;
    }


    if (args[0] == "..") {
        m_session->enterParentFactory();
        return;
    }

    bool ok;
    int index = args[0].toInt(&ok);
    if (!ok) {
        m_out << "Usage: cd <index>  (or cd with no args to go to root)\n";
        return;
    }

    auto* activeFactory = SessionManager::get().activeFactory();
    auto& nodes = activeFactory->nodes();

    if (index < 0 || index >= nodes.size()) {
        m_out << "No node at index " << index << "\n";
        return;
    }

    auto* target = nodes.at(index);
    if (target->type() != NodeType::Factory) {
        m_out << "Node " << index << " is not a factory node\n";
        return;
    }

    m_session->enterFactory(&static_cast<FactoryNode*>(target)->factory());
}

void CliManager::handleConnect(const QStringList& args)
{
    if (args.size() != 2) {
        m_out << "2 Arguements Are needed for connection ";
        return;
    }
    QString arg1 = args[0];
    int xPos1 = arg1.indexOf('x');
    if (xPos1 == -1) {
        m_out << "Error parsing first index" << arg1;
    }
    int sourceNodeIdx = arg1.left(xPos1).toInt();
    int sourcePortIdx = arg1.mid(xPos1 + 1, arg1.length() - xPos1 - 1).toInt();

    QString arg2 = args[1];
    int xPos2 = arg2.indexOf('x');
    if (xPos2 == -1) {
        m_out << "Error parsing second index" << arg2;
    }
    int destNodeIdx = arg2.left(xPos2).toInt();
    int destPortIdx = arg2.mid(xPos2 + 1, arg2.length() - xPos2 - 1).toInt();

    SessionManager::get().connectNode(sourceNodeIdx, sourcePortIdx, destNodeIdx, destPortIdx);
}

void CliManager::handleDisconnect(const QStringList& args)
{
    if (args.size() == 0) {
        m_out << "Usage: disconnect <idx>\n";
        return;
    }
    QString arg1 = args.value(0);
    int xPos1 = arg1.indexOf('x');
    if (xPos1 == -1) {
        m_out << "Error parsing second index" << arg1;
        return;
    }
    int sourceNodeIdx = arg1.left(xPos1).toInt();
    int sourcePortIdx = arg1.mid(xPos1 + 1, arg1.length() - xPos1 - 1).toInt();

    QString arg2 = args.value(1);
    int destNodeIdx = -1;
    int destPortIdx = -1;

    if (!arg2.isEmpty()) {
        int xPos2 = arg2.indexOf('x');
        destNodeIdx = arg2.left(xPos2).toInt();
        destPortIdx = arg2.mid(xPos2 + 1, arg2.length() - xPos2 - 1).toInt();
    }

    SessionManager::get().disconnectNode(sourceNodeIdx, sourcePortIdx, destNodeIdx, destPortIdx);
}

void CliManager::handleLimit(const QStringList& args)
{
    if (args.size() < 2) {
        m_out << "Usage: limit <nodeIndex> <value>  (use -1 to remove limit)\n";
        return;
    }
    bool okIdx, okVal;
    int index = args[0].toInt(&okIdx);
    float value = args[1].toFloat(&okVal);
    if (!okIdx || !okVal) {
        m_out << "Usage: limit <nodeIndex> <value>\n";
        return;
    }
    auto* factory = SessionManager::get().activeFactory();
    if (index < 0 || index >= factory->nodes().size()) {
        m_out << "No node at index " << index << "\n";
        return;
    }
    AbstractNode* node = factory->nodes().at(index);
    SessionManager::get().setMachineLimit(node, value);
    m_out << "Limit set to " << value << " on node " << index << "\n";
}

void CliManager::handlePurity(const QStringList& args)
{
    if (args.size() < 2) {
        m_out << "Usage: purity <nodeIndex> <impure|normal|pure>\n";
        return;
    }
    bool ok;
    int index = args[0].toInt(&ok);
    if (!ok) {
        m_out << "Usage: purity <nodeIndex> <impure|normal|pure>\n";
        return;
    }
    QString val = args[1].toLower();
    NodePurity purity;
    if (val == "impure")
        purity = NodePurity::Impure;
    else if (val == "normal")
        purity = NodePurity::Normal;
    else if (val == "pure")
        purity = NodePurity::Pure;
    else {
        m_out << "Unknown purity: " << args[1] << ". Expected: impure, normal, pure\n";
        return;
    }
    auto* factory = SessionManager::get().activeFactory();
    if (index < 0 || index >= factory->nodes().size()) {
        m_out << "No node at index " << index << "\n";
        return;
    }
    SessionManager::get().setExtractionPurity(factory->nodes().at(index), purity);
    m_out << "Purity set to " << args[1] << " on node " << index << "\n";
}

void CliManager::handleTier(const QStringList& args)
{
    if (args.size() < 2) {
        m_out << "Usage: tier <nodeIndex> <tierIndex>\n";
        return;
    }
    bool okIdx, okTier;
    int index = args[0].toInt(&okIdx);
    int tier = args[1].toInt(&okTier);
    if (!okIdx || !okTier) {
        m_out << "Usage: tier <nodeIndex> <tierIndex>\n";
        return;
    }
    auto* factory = SessionManager::get().activeFactory();
    if (index < 0 || index >= factory->nodes().size()) {
        m_out << "No node at index " << index << "\n";
        return;
    }
    SessionManager::get().setExtractionTier(factory->nodes().at(index), tier);
    m_out << "Tier set to " << tier << " on node " << index << "\n";
}

void CliManager::handleSolve()
{
    m_session->cleanSolve();
    m_out << "Solved.\n";
}

void CliManager::handleReport()
{
    m_out << m_session->solverReport();
}

void CliManager::handleRename(const QStringList& args)
{
    if (args.size() != 2) {
        m_out << "Rename need 2 arguements: ";
        return;
    }

    bool ok = false;
    int index = args[0].toInt(&ok);
    if (!ok) {
        m_out << "Invalid index: " << args[0];
        return;
    }
    QString newName = args[1];
    if (newName.isEmpty()) {
        m_out << "Invalid Name: " << newName;
        return;
    }

    SessionManager::get().renameNode(index, newName);
}

void CliManager::handleSave(const QStringList& args)
{
    if (args.isEmpty()) {
        m_out << "Usage: save <path|name>  (name saves to current directory)\n";
        return;
    }
    QString path = args[0];
    if (!path.contains('/') && !path.contains('\\')) {
        if (!path.endsWith(".json"))
            path += ".json";
        path = QCoreApplication::applicationDirPath() + "/" + path;
    }
    m_session->save(path);
    m_out << "Saved to " << path << "\n";
}

void CliManager::handleLoad(const QStringList& args)
{
    if (args.isEmpty()) {
        m_out << "Usage: load <path|name>  (name saves to current directory)\n";
        return;
    }
    QString path = args[0];
    if (!path.contains('/') && !path.contains('\\')) {
        if (!path.endsWith(".json"))
            path += ".json";
        path = QCoreApplication::applicationDirPath() + "/" + path;
    }
    m_session->load(path);
    m_out << "Loaded From: " << path << "\n";
}
// -------------------- SLOTS --------------------------
void CliManager::onInputReady()
{
    QFile stdinFile;
    if (!stdinFile.open(stdin, QIODevice::ReadOnly)) {
    }
    QByteArray data = stdinFile.readLine();
    stdinFile.close();

    if (data.isEmpty()) {
        emit quitRequested();
        return;
    }
    QString qline = QString::fromUtf8(data).trimmed();

    if (!qline.isEmpty()) {
        processCommand(qline);
    }
    printPrompt();
}
