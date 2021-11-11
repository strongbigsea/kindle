#include "mywindow.h"
#include "ui_mywindow.h"
/*
    //弹出文件对话框
    //路径选择
    //内容显示
    //内容处理与显示
    //写入文件并保存
    //文件成功写入的提示
    //软件的关闭与误操作的判断
    //重复操作与判断
*/
#include<QFileDialog>
#include<QPushButton>
#include<QTextCodec>//编码格式的类
#include<QFile>
#include<QDebug>
#include<QVector>
#include<QMessageBox>
MyWindow::MyWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MyWindow)
{
    ui->setupUi(this);
    //弹出文件对话框并显示文件内容
    connect(ui->pushbtnFOpen,&QPushButton::clicked,[=](){
        QString path=QFileDialog::getOpenFileName(this,"文件选择","D:/file/books/book","(*.txt)");
        ui->lineEditOpen->setText(path);

        //QTextCodec *codec=QTextCodec::codecForName("gbk");
        QFile file(path);
        file.open(QIODevice::ReadOnly);  //设置文件的打开模式
        QByteArray text;
        while (!file.atEnd()) {
            int static line_num=1;
            QByteArray line=file.readLine();
            if((line_num+1)%5==0){  //5行是一个循环  笔记的内容刚好是5行中的第4行
                text+=line;
            }
            line_num++;
        }
        //text_out=codec->toUnicode(text);//转换为UTF-8格式编码的字符串
        text_out=GetCorrectUnicode(text);//转换为UTF-8格式编码的字符串
        ui->textIn->setText(text_out);//文件内容的显示
        file.close();
    });
    //对文件内容的处理
    connect(ui->pushbtnEdit,&QPushButton::clicked,[=](){
         list=text_out.split(QRegExp("[\r\n]"),QString::SkipEmptyParts);  //将字符串最后面的换行符去掉  否则这里始终判断为非子串
        QVector<QString> res ;//输出字符串
        qDebug()<<list.size()<<endl;
        for (int i=0;i<list.size();i++) {
            if(i>0&&(mykmp(res.last(),list[i])==1)){//如果当前行是子串，直接跳过当前行
                //qDebug()<<"当前是否为子串："<<mykmp(list[i-1],list[i])<<endl;
                continue;
            }
            if(i>0&&(mykmp(list[i],res.last())==1)){//如果上一行是子串 删掉上一行 并且加入当前行
                //qDebug()<<"上一字符串是否为子串"<<mykmp(list[i],list[i-1])<<endl;
                res.pop_back();//删尾
            }
            res.push_back(list[i]);//加入当前行
        }
        for(auto s:res){
            out_str+=s;
            out_str+="\r";//这里在每一行的内容后面加换行符
        }
        ui->textOut->setText(out_str);//此时文件已经成功处理
        flag_handle=true;
    });
    //写入文件并保存
    //这里做一个判断 文件处理完点点击文件保存才有效 否则直接弹出经警告对话框
    connect(ui->pushbtnSave,&QPushButton::clicked,[=](){
        if(flag_handle){
            QString filename=QFileDialog::getSaveFileName(this,"文件另存为","D:/file/books/book");  //返回值是新文件的路径
            filename+=".txt";//自动变为文本文件 这里直接输入文件名字就可以
            //qDebug()<<filename<<"<-新文件路径"<<endl;
            QFile file(filename);//文件名如果为空 就创建一个文件
            file.open(QIODevice::Append);
            file.write(out_str);  //这里类型为 QByteArray 查手册的重要性
            file.close();//关闭窗口以后直接显示文件保存成功
            if(!filename.isEmpty())
                QMessageBox::information(this,"保存成功","文件已成功保存");
            ui->lineEditClose->setText(filename);
        }
        else{
            QMessageBox::warning(this,"警告","文件未处理");
        }
    });
    //清空软件
    connect(ui->pushBtnClear,&QPushButton::clicked,[=](){
        if(flag_handle){
            ui->textIn->clear();
            ui->textOut->clear();
            ui->lineEditOpen->clear();
            ui->lineEditClose->close();
            flag_handle=false;
        }
        else{
            QMessageBox::warning(this,"警告","文件未处理");
        }
    });
    //关闭软件
    connect(ui->pushBtnClose,&QPushButton::clicked,[=](){
        this->close();//本窗口直接关闭
    });
}
MyWindow::~MyWindow()
{
    delete ui;
}
//编码转换
QString MyWindow::GetCorrectUnicode(const QByteArray &text)
{
    QTextCodec::ConverterState state;
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QString strtext = codec->toUnicode( text.constData(), text.size(), &state);
    if (state.invalidChars > 0)  //如果不是UTF-8格式编码 就会返回大于0 的值
    {
        strtext = QTextCodec::codecForName( "GBK" )->toUnicode(text);
    }
    else
    {
        strtext = text;
    }
    return strtext;
}

//对两行字符串执行KMP算法 返回布尔值
int MyWindow::mykmp(QString str1, QString str2)  //执行KMP算法
{
    int m=str1.size();
    int n=str2.size();
    if(n==0)
        return 0;//如果是0 直接返回值
    QVector<int>next(n);//构建Next数组 默认赋初值为0
    for(int i=1,j=0;i<n;i++){
        while (j>0&&str2[i]!=str2[j]) {//如果匹配字符串相等 则看上一位的值
            j=next[j-1];
        }
        if(str2[i]==str2[j]){
            j++;
        }
        next[i]=j;
    }
    for(int i=0,j=0;i<m;i++){
        while(j>0&&str1[i]!=str2[j]){
            j=next[j-1];  ///next 在不相等时告诉怎么做
        }
        if(str1[i]==str2[j]){
            j++;
        }
        if(j==n){
            return 1;  //返回值为1  代表是子串
        }
    }
    return -1;
}




