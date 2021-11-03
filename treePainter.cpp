#include "treePainter.h"
#include "ui_treePainter.h"

treePainter::treePainter(double radius, double height, double width, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::treePainter),
    radius_(radius),
    height_(height),
    width_(width),
    target_(-1), //-1 for the max (impossible index)
    timer_(new QTimer(this)),
    targetPath_(),
    points_(),
    lines_(),
    contents_(),
    oldInfo_(),
    oldTree_(nullptr),
    newInfo_(),
    newTree_(nullptr),
    oldLocator_(nullptr),
    newLocator_(nullptr)
{
    setFixedSize(width_, height_);
    ui->setupUi(this);
    ui->oldTreeInfo->setWindowTitle("oldTree's information");
    ui->newTreeInfo->setWindowTitle("newTree's information");
    timer_->stop();
    timer_->setInterval(1200); //1.2 second
    connect(timer_, &QTimer::timeout, this, [this]()
    {
        if (!targetPath_.empty()) {
            if (target_ == std::numeric_limits<size_type>::max())//initial status
                target_ = *targetPath_.cbegin();//the first one
            else {
                auto i = std::find(targetPath_.begin(), targetPath_.end(), target_);
                if (i != targetPath_.end())
                    target_ = *(i + 1);//move to the next
            }
            update();
        }
    });
}

QString treePainter::info(QString& s, const std::unique_ptr<avlTree<int>>& tree) noexcept
{
    s.append(tr("height: ") + QString::number(tree->height()) + '\n');
    s.append(tr("node count: ") + QString::number(tree->nodeCount()) + '\n');
    s.append(tr("level node count: "));
    for (const auto& i : tree->levelInfo()) {
        s.append(QString::number(i) + ' ');
    }
    s.append('\n');
    s.append(tr("preOrder: "));
    tree->preOrder([&s](const auto& p)
    {
        s.append(QString::number(p->element_) + ' ');
    });
    s.append('\n');
    s.append(tr("inOrder: "));
    tree->inOrder([&s](const auto& p)
    {
        s.append(QString::number(p->element_) + ' ');
    });
    s.append('\n');
    s.append(tr("postOrder: "));
    tree->postOrder([&s](const auto& p)
    {
        s.append(QString::number(p->element_) + ' ');
    });
    s.append('\n');
    s.append(tr("levelOrder: "));
    tree->levelOrder([&s](const auto& p)
    {
        s.append(QString::number(p->element_) + ' ');
    });
    s.append('\n');
    return s;
}

treePainter::~treePainter()
{
    delete ui;
    delete timer_;
    oldTree_ = nullptr;
    newTree_ = nullptr;
    oldLocator_ = nullptr;
    newLocator_ = nullptr;
}

void treePainter::on_insertNode_clicked()
{
    QString str = ui->insertContent->toPlainText().simplified();
    QStringList strList = str.split(" ");
    if (oldTree_ != nullptr) {
        oldTree_ = std::make_unique<avlTree<int>>(*newTree_);
        for (auto i = strList.cbegin(); i != strList.cend(); ++i)
            newTree_->insert((*i).toInt(), newTree_->root());
    } else { //initialize
        oldTree_ = std::make_unique<avlTree<int>>();
        newTree_ = std::make_unique<avlTree<int>>();
        for (auto i = strList.cbegin(); i != strList.cend(); ++i) {
            oldTree_->insert((*i).toInt(), oldTree_->root());
            newTree_->insert((*i).toInt(), newTree_->root());
        }
    }
    int diff = newTree_->height() - oldTree_->height();
    if (newTree_->height() > 5 && newTree_->height() < 8)
        radius_ -= diff * 5;
    oldLocator_ = std::make_unique<treeLocate<int>>(std::ref(*oldTree_), radius_, height_, width_/2);
    newLocator_ = std::make_unique<treeLocate<int>>(std::ref(*newTree_), radius_, height_, width_/2, width_/2); //begin from center
    //ui->insertContent->clear();
    update();
    oldInfo_.clear();
    newInfo_.clear();
    ui->oldTreeInfo->setPlainText(info(oldInfo_, oldTree_));
    ui->newTreeInfo->setPlainText(info(newInfo_, newTree_));
}

void treePainter::on_eraseNode_clicked()
{
    QString str = ui->eraseContent->toPlainText().simplified();
    QStringList strList = str.split(" ");
    if (newTree_->root()) {
        oldTree_ = std::make_unique<avlTree<int>>(*newTree_);
        for (auto i = strList.cbegin(); i != strList.cend(); ++i)
            newTree_->erase((*i).toInt(), newTree_->root());
        int diff = oldTree_->height() - newTree_->height();
        if (newTree_->height() > 5 && newTree_->height() < 8)
            radius_ += diff * 5;
    } else
        oldTree_->root() = nullptr;
    if (newTree_->root()) {
        auto levelInfo = newTree_->levelInfo();
        oldLocator_ = std::make_unique<treeLocate<int>>(std::ref(*oldTree_), radius_, height_, width_/2);
        newLocator_ = std::make_unique<treeLocate<int>>(std::ref(*newTree_), radius_, height_, width_/2, width_/2);
    } else {
        oldTree_->root() = nullptr;
        newLocator_ = nullptr;
        oldLocator_ = nullptr;
    }

    //ui->eraseContent->clear();
    update();
    oldInfo_.clear();
    newInfo_.clear();
    ui->oldTreeInfo->setPlainText(info(oldInfo_, oldTree_));
    ui->newTreeInfo->setPlainText(info(newInfo_, newTree_));
}

void treePainter::paintEvent([[maybe_unused]]QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);//anti-aliasing
    painter.setRenderHint(QPainter::TextAntialiasing);//text anti-aliasing

    //draw divider
    painter.drawLine(width_/2, 100, width_ / 2, 1000);

    //set pen
    QPen pen;
    pen.setWidth(3);
    pen.setColor(Qt::black);
    pen.setStyle(Qt::SolidLine);
    pen.setCapStyle(Qt::SquareCap);
    pen.setJoinStyle(Qt::MiterJoin);
    painter.setPen(pen);

    //set font
    QFont font;
    font.setStyle(QFont::StyleItalic);
    font.setPointSizeF(radius_ - 10);
    painter.setFont(font);

    if (oldLocator_ && newLocator_) {
        points_ = oldLocator_->points();
        auto newPoints = newLocator_->points();
        for (const auto& i :newPoints)
            points_.emplace_back(i);

        lines_ = oldLocator_->lines();
        auto newLines = newLocator_->lines();
        for (const auto& i : newLines)
            lines_.emplace_back(i);

        contents_.clear();
        contents_.resize(0);
        oldTree_->levelOrder([this](const auto& p)
        {
            contents_.emplace_back(p->element_);
        });
        newTree_->levelOrder([this](const auto& p)
        {
            contents_.emplace_back(p->element_);
        });

        painter.save();
        painter.translate(0, 100);
        painter.translate(0, 2 * radius_);//painting in the center

        //draw ellipse
        if (!points_.empty())
            for (const auto& i : points_)
                painter.drawEllipse(i, radius_, radius_);

        //draw lines
        if (!lines_.empty())
            for (const auto& i : lines_)
                painter.drawLine(i);

        //coloring target node
        if (target_ != std::numeric_limits<size_type>::max()) {
            QBrush brush;
            if (target_ != *(targetPath_.end() - 1))
                brush.setColor(Qt::red);
            else
                brush.setColor(Qt::green);
            brush.setStyle(Qt::SolidPattern);
            painter.save();
            painter.setBrush(brush);
            //coloring red
            auto oldLength = oldLocator_->points().size();
            painter.drawEllipse(points_[oldLength + target_], radius_, radius_);
            painter.restore();
            /*
         * found!
         * coloring green
         */
            if (target_ == *(targetPath_.end() - 1)) {
                target_ = -1;
                targetPath_.clear();
                targetPath_.resize(0);
                timer_->stop();
            }
        }

        //draw contents
        if (!points_.empty() && !contents_.empty())
            for (decltype (points_.size()) i = 0; i != points_.size(); ++i){
                QPointF topLeft{static_cast<double>(points_[i].x()) - static_cast<double>(radius_) / std::sqrt(2),
                            static_cast<double>(points_[i].y()) - static_cast<double>(radius_) / std::sqrt(2)};
                QPointF bottomRight{static_cast<double>(points_[i].x()) + static_cast<double>(radius_) / std::sqrt(2),
                            static_cast<double>(points_[i].y()) + static_cast<double>(radius_) / std::sqrt(2)};
                painter.drawText(QRectF{topLeft, bottomRight}, Qt::AlignCenter|Qt::TextWordWrap, QString::number(contents_[i]));
            }

        painter.restore();
    }
}


void treePainter::on_findNode_clicked()
{
    QString str = ui->findContent->toPlainText().simplified();
    //ui->findContent->clear();
    QStringList strList = str.split(" ");
    QString element = strList[0];//only find the first element
    if (newTree_->find(element.toInt())) {//exist
        targetPath_ = newLocator_->findPath(element.toInt());
    }
    timer_->start();
    update();
}

